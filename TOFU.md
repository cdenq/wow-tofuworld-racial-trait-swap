# Tofu Worldview fork

Fork of azerothcore/mod-racial-trait-swap for the Tofu Worldview server (mod-playerbots fork of AzerothCore, 3.3.5a). Plan and decisions: project doc `claude/racial-swap-fork-plan.md`.

## What changed from upstream

- Fixes the native racials returning at login (upstream issues #13 and #25). Racial skills are default skills, and the core re-teaches their spells at every login, so the module stores each swap in `acore_characters.tofu_racial_swap` and, at every login, removes racial spells that do not belong to the chosen race and class and learns the missing ones. In-world relearning of a racial outside that set is refused.
- Characters that swapped before this fork are detected at their first login (they know another race's racials) and their choice is stored.
- Racial variants come from SkillLineAbility (the DBC plus `skilllineability_dbc` rows) for the character's class, so a swap and a native character of the same race and class (including mod-uac combinations) get the same spells. The ten hardcoded per-race learn blocks are gone.
- Custom spells (module SQL `data/sql/db-world/tofu_rts_01_spells.sql`):
  - 900000 Arcane Torrent for warriors: 15 rage.
  - 900001 Arcane Torrent for druids: 15 rage in bear or dire bear form, 15 energy in cat form, 6% mana otherwise (`spell_tofu_arcane_torrent_druid`).
  - 900002 Command for mages, priests, and druids, and 900003, the 5% damage aura it puts on a Water Elemental (510, 37994), Shadowfiend (19668), or Treant (1964).
  - Skill-line rows 900000 to 900004: the two Arcane Torrents on the Blood Elf line, Gift of the Naaru 59548 (warlock) and 59547 (druid) on the Draenei line, and Command 900002 on the Orc line.
- The custom spells need the Tofu client patch (Spell.dbc rows 900000 to 900003) to show their names, icons, and tooltips.
- Swirl's menu picks male or female race labels from the character's own gender (`PLAYER_BYTES_3`), not the display gender, which a shapeshift or morph sets from the model (Tree of Life is GENDER_NONE). Upstream left every label null in that case and crashed the worldserver on any option click.
- Overrides of mod-uac and stock rows (12041, 21984, 21987) live in the server's custom SQL (wow-tofuworld-internal, `tofu_2026_11_18_04_racial_variant_overrides.sql`), not here.

## Requirements

- Deploy this repo under the upstream folder name `mod-racial-trait-swap` (in `WoW Projects\modules` and `C:\azerothcore\modules`): the script loader function is the upstream `Addmod_racial_trait_swapScripts`, and the core derives the loader it calls from the folder name. The dev clone's own folder name (`wow-tofuworld-racial-trait-swap`) does not matter.
- Conf: copy `conf/RacialTraitSwap.conf.dist` to `configs/modules/RacialTraitSwap.conf` (keys unchanged from upstream).
- NPC 98888 (Swirl) is created by `data/sql/db-world/base/world.RTS.sql` but not spawned; spawn it with `.npc add 98888`. Add 98888 to `Creatures.CustomIDs` in worldserver.conf to silence the gossip validation message.
