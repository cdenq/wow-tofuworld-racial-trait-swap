/*
 * Tofu Worldview racial trait swap fork.
 *
 * Why this exists: racial skills are default skills (playercreateinfo_skills), and Player::LoadFromDB re-adds every
 * default skill and re-teaches its spells (learnSkillRewardedSpells, as temporary spells) at every login. So a swap
 * that only removes and learns spells loses its removals at the next login, and the character ends up with both sets.
 *
 * What it does:
 * - A swap stores the chosen race in acore_characters.tofu_racial_swap (no row means the character's own race).
 * - The racial set for a race and class is read from SkillLineAbility (the DBC plus skilllineability_dbc rows, so
 *   stock, mod-uac, and Tofu rows all count): every spell on that race's racial skill line whose race and class masks
 *   allow it.
 * - At every login, for every character (bots included), racial spells outside that set are removed and missing ones
 *   are learned. In-world relearning of a racial outside the set is refused (OnPlayerCanLearnSpell).
 * - A character that swapped before this fork (another race's racials saved, no row) is detected at its first login
 *   and its choice is stored.
 * - Custom spells: Arcane Torrent for druids (900001) energizes by form; Command (900002) puts a 5% damage aura
 *   (900003) on a Water Elemental, Shadowfiend, or Treant summoned by its owner.
 */

#include "TofuRacials.h"
#include "Chat.h"
#include "Creature.h"
#include "DBCStores.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include <map>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace
{
    // SkillLineAbility.AcquireMethod values that the core learns with the skill (see Player::learnSkillRewardedSpells).
    constexpr uint32 ACQUIRE_ON_SKILL_VALUE = 1;
    constexpr uint32 ACQUIRE_ON_SKILL_LEARN = 2;

    constexpr uint32 SPELL_VISUAL_LEVEL_UP = 47292;
    constexpr uint32 SPELL_TOFU_COMMAND = 900002;
    constexpr uint32 SPELL_TOFU_COMMAND_BUFF = 900003;

    constexpr uint32 NPC_WATER_ELEMENTAL = 510;
    constexpr uint32 NPC_WATER_ELEMENTAL_GLYPHED = 37994;
    constexpr uint32 NPC_SHADOWFIEND = 19668;
    constexpr uint32 NPC_TREANT = 1964;

    // Racial skill line of each playable race (SkillLine.dbc category 9).
    uint32 RacialSkillLine(uint8 race)
    {
        switch (race)
        {
            case RACE_HUMAN:         return 754;
            case RACE_ORC:           return 125;
            case RACE_DWARF:         return 101;
            case RACE_NIGHTELF:      return 126;
            case RACE_UNDEAD_PLAYER: return 220;
            case RACE_TAUREN:        return 124;
            case RACE_GNOME:         return 753;
            case RACE_TROLL:         return 733;
            case RACE_BLOODELF:      return 756;
            case RACE_DRAENEI:       return 760;
            default:                 return 0;
        }
    }

    constexpr uint8 PLAYABLE_RACES[] = { RACE_HUMAN, RACE_ORC, RACE_DWARF, RACE_NIGHTELF, RACE_UNDEAD_PLAYER, RACE_TAUREN, RACE_GNOME, RACE_TROLL, RACE_BLOODELF, RACE_DRAENEI };

    bool IsLearnedWithSkill(SkillLineAbilityEntry const* ability)
    {
        return ability->AcquireMethod == ACQUIRE_ON_SKILL_VALUE || ability->AcquireMethod == ACQUIRE_ON_SKILL_LEARN;
    }

    // Every racial spell and the race whose racial line it is on. Built once, on first use (after the DBC stores and
    // the skilllineability_dbc rows are loaded at startup). No spell is on two racial lines (checked 2026-11-18).
    std::unordered_map<uint32, uint8> const& RacialSpellRaces()
    {
        static std::unordered_map<uint32, uint8> const racialSpells = []()
        {
            std::unordered_map<uint32, uint8> spells;
            for (uint8 race : PLAYABLE_RACES)
                for (SkillLineAbilityEntry const* ability : GetSkillLineAbilitiesBySkillLine(RacialSkillLine(race)))
                    if (IsLearnedWithSkill(ability))
                        spells.emplace(ability->Spell, race);
            return spells;
        }();

        return racialSpells;
    }

    bool IsRacialSpell(uint32 spellId)
    {
        return RacialSpellRaces().count(spellId) != 0;
    }

    // The racial spells a character of this class gets from this race's racial line.
    std::unordered_set<uint32> DesiredRacials(uint8 race, uint8 classId)
    {
        std::unordered_set<uint32> spells;
        uint32 const skillLine = RacialSkillLine(race);
        if (!skillLine || !classId)
            return spells;

        uint32 const raceMask = 1 << (race - 1);
        uint32 const classMask = 1 << (classId - 1);
        for (SkillLineAbilityEntry const* ability : GetSkillLineAbilitiesBySkillLine(skillLine))
        {
            if (!IsLearnedWithSkill(ability))
                continue;
            if (ability->RaceMask && !(ability->RaceMask & raceMask))
                continue;
            if (ability->ClassMask && !(ability->ClassMask & classMask))
                continue;
            spells.insert(ability->Spell);
        }

        return spells;
    }

    // Cache of stored choices for characters that are logged in: guid (low part) to race.
    std::mutex choiceLock;
    std::unordered_map<uint32, uint8> choiceCache;

    uint8 CachedChoice(uint32 guidLow)
    {
        std::lock_guard<std::mutex> guard(choiceLock);
        auto itr = choiceCache.find(guidLow);
        return itr != choiceCache.end() ? itr->second : 0;
    }

    void CacheChoice(uint32 guidLow, uint8 race)
    {
        std::lock_guard<std::mutex> guard(choiceLock);
        if (race)
            choiceCache[guidLow] = race;
        else
            choiceCache.erase(guidLow);
    }

    uint8 NativeRace(Player const* player)
    {
        return player->getRace(true);
    }

    // The race whose racials this character uses: its stored choice, or its own race.
    uint8 EffectiveRace(Player const* player)
    {
        uint8 const chosen = CachedChoice(player->GetGUID().GetCounter());
        return chosen ? chosen : NativeRace(player);
    }

    void StoreChoice(Player* player, uint8 race)
    {
        uint32 const guidLow = player->GetGUID().GetCounter();
        if (!race || race == NativeRace(player))
        {
            CharacterDatabase.Execute("DELETE FROM `tofu_racial_swap` WHERE `guid` = {}", guidLow);
            CacheChoice(guidLow, 0);
        }
        else
        {
            CharacterDatabase.Execute("REPLACE INTO `tofu_racial_swap` (`guid`, `race`) VALUES ({}, {})", guidLow, uint32(race));
            CacheChoice(guidLow, race);
        }
    }

    // Loads the stored choice into the cache. Returns false when the character has no row.
    bool LoadChoice(Player* player)
    {
        uint32 const guidLow = player->GetGUID().GetCounter();
        QueryResult result = CharacterDatabase.Query("SELECT `race` FROM `tofu_racial_swap` WHERE `guid` = {}", guidLow);
        if (!result)
        {
            CacheChoice(guidLow, 0);
            return false;
        }

        uint8 const race = result->Fetch()[0].Get<uint8>();
        if (!RacialSkillLine(race))
        {
            LOG_ERROR("module", "TofuRacials: character {} has invalid stored race {}; ignoring it.", guidLow, uint32(race));
            CacheChoice(guidLow, 0);
            return true;
        }

        CacheChoice(guidLow, race == NativeRace(player) ? 0 : race);
        return true;
    }

    // For a character with no stored choice: if it knows another race's racial spells (a swap made before this
    // fork), returns that race (the one with the most known spells), otherwise 0.
    uint8 DetectLegacySwap(Player const* player)
    {
        uint8 const native = NativeRace(player);
        std::map<uint8, uint32> counts;
        for (auto const& [spellId, race] : RacialSpellRaces())
            if (race != native && player->HasSpell(spellId))
                ++counts[race];

        uint8 bestRace = 0;
        uint32 bestCount = 0;
        for (auto const& [race, count] : counts)
        {
            if (count > bestCount)
            {
                bestRace = race;
                bestCount = count;
            }
        }

        return bestRace;
    }

    // Makes the character's racial spells exactly the set for its effective race and its class.
    void EnforceRacials(Player* player)
    {
        uint8 const native = NativeRace(player);
        uint8 const effective = EffectiveRace(player);
        std::unordered_set<uint32> const desired = DesiredRacials(effective, player->getClass());

        for (auto const& [spellId, race] : RacialSpellRaces())
            if (!desired.count(spellId) && player->HasSpell(spellId))
                player->removeSpell(spellId, SPEC_MASK_ALL, false);

        // Own race: learned as temporary, the same way the core grants them from the skill, so nothing extra is saved.
        // Another race: learned permanently, so they load with the character's spells.
        bool const temporary = effective == native;
        for (uint32 spellId : desired)
            if (!player->HasSpell(spellId))
                player->learnSpell(spellId, temporary, temporary);
    }
}

void TofuRacials::DoSwap(Player* player, uint8 race, int32 goldCost)
{
    if (!player || !RacialSkillLine(race))
        return;

    if (goldCost > 0)
    {
        uint32 const cost = uint32(goldCost) * GOLD;
        if (!player->HasEnoughMoney(cost))
        {
            ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold for a racial trait swap.");
            return;
        }

        player->ModifyMoney(-int32(cost));
    }

    player->CastSpell(player, SPELL_VISUAL_LEVEL_UP, true);
    StoreChoice(player, race);
    EnforceRacials(player);
}

class TofuRacialsPlayerScript : public PlayerScript
{
public:
    TofuRacialsPlayerScript() : PlayerScript("TofuRacialsPlayerScript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LOGOUT,
        PLAYERHOOK_ON_DELETE_FROM_DB,
        PLAYERHOOK_CAN_LEARN_SPELL
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (!LoadChoice(player))
        {
            if (uint8 const legacyRace = DetectLegacySwap(player))
            {
                LOG_INFO("module", "TofuRacials: {} (guid {}) knows race {} racials from an earlier swap; storing that choice.", player->GetName(), player->GetGUID().GetCounter(), uint32(legacyRace));
                StoreChoice(player, legacyRace);
            }
        }

        EnforceRacials(player);
    }

    void OnPlayerLogout(Player* player) override
    {
        CacheChoice(player->GetGUID().GetCounter(), 0);
    }

    void OnPlayerDeleteFromDB(CharacterDatabaseTransaction trans, uint32 guid) override
    {
        trans->Append("DELETE FROM `tofu_racial_swap` WHERE `guid` = {}", guid);
        CacheChoice(guid, 0);
    }

    bool OnPlayerCanLearnSpell(Player* player, uint32 spellId) override
    {
        if (!IsRacialSpell(spellId))
            return true;

        return DesiredRacials(EffectiveRace(player), player->getClass()).count(spellId) != 0;
    }
};

// Command (900002) for mage, priest, and druid: their summons get a 5% damage done aura (900003).
class TofuCommandSummonScript : public AllCreatureScript
{
public:
    TofuCommandSummonScript() : AllCreatureScript("TofuCommandSummonScript") { }

    void OnCreatureAddWorld(Creature* creature) override
    {
        switch (creature->GetEntry())
        {
            case NPC_WATER_ELEMENTAL:
            case NPC_WATER_ELEMENTAL_GLYPHED:
            case NPC_SHADOWFIEND:
            case NPC_TREANT:
                break;
            default:
                return;
        }

        Player* owner = creature->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner || !owner->HasAura(SPELL_TOFU_COMMAND))
            return;

        owner->AddAura(SPELL_TOFU_COMMAND_BUFF, creature);
    }
};

// Arcane Torrent for druids (900001, a copy of 28730): 15 rage in bear or dire bear form, 15 energy in cat form,
// otherwise the copied effect (6% of maximum mana).
class spell_tofu_arcane_torrent_druid : public SpellScript
{
    PrepareSpellScript(spell_tofu_arcane_torrent_druid);

    void HandleEnergize(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        switch (caster->GetShapeshiftForm())
        {
            case FORM_BEAR:
            case FORM_DIREBEAR:
                PreventHitDefaultEffect(effIndex);
                caster->EnergizeBySpell(caster, GetSpellInfo()->Id, 150, POWER_RAGE); // rage is stored x10
                break;
            case FORM_CAT:
                PreventHitDefaultEffect(effIndex);
                caster->EnergizeBySpell(caster, GetSpellInfo()->Id, 15, POWER_ENERGY);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_tofu_arcane_torrent_druid::HandleEnergize, EFFECT_1, SPELL_EFFECT_ENERGIZE_PCT);
    }
};

void AddSC_tofu_racials()
{
    new TofuRacialsPlayerScript();
    new TofuCommandSummonScript();
    RegisterSpellScript(spell_tofu_arcane_torrent_druid);
}
