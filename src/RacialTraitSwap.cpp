/*
 *MIT License
 *
 *Copyright (c) 2023 Azerothcore
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 *
 * Racial Traits Swap NPC For Azerothcore Custom Changes Branch
 * Written by SPP DEV MDIC
 * SQL Assistance by Nemesis Dev Jinnai
 * Modified for Azerothcore
 *
 * Tofu Worldview fork (2026-11-18): the per-race learn blocks are replaced by TofuRacials::DoSwap, which stores the
 * choice and learns the racial variants for the character's class from SkillLineAbility (see TofuRacials.cpp).
 */

#include "Chat.h"
#include "Config.h"
#include "Creature.h"
#include "GameEventMgr.h"
#include "Item.h"
#include "ItemEnchantmentMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "WorldSession.h"
#include "TofuRacials.h"

#define LOCALE_RACESWAP_0 "Racial Trait Swap"
#define LOCALE_RACESWAP_1 "인종 특성 교환"
#define LOCALE_RACESWAP_2 "Échange de traits raciaux"
#define LOCALE_RACESWAP_3 "Rassenmerkmalswechsel"
#define LOCALE_RACESWAP_4 "種族特質交換"
#define LOCALE_RACESWAP_5 "種族特質交換"
#define LOCALE_RACESWAP_6 "Intercambio de rasgos raciales"
#define LOCALE_RACESWAP_7 "Intercambio de rasgos raciales"
#define LOCALE_RACESWAP_8 "Обмен расовыми особенностями"

#define LOCALE_EXIT_0 "[EXIT]"
#define LOCALE_EXIT_1 "[출구]"
#define LOCALE_EXIT_2 "[SORTIR]"
#define LOCALE_EXIT_3 "[AUSFAHRT]"
#define LOCALE_EXIT_4 "[出口]"
#define LOCALE_EXIT_5 "[出口]"
#define LOCALE_EXIT_6 "[SALIDA]"
#define LOCALE_EXIT_7 "[SALIDA]"
#define LOCALE_EXIT_8 "[ВЫХОД]"

#define LOCALE_BACK_0 "[BACK]"
#define LOCALE_BACK_1 "[출구]"
#define LOCALE_BACK_2 "[RETOUR]"
#define LOCALE_BACK_3 "[ZURÜCK]"
#define LOCALE_BACK_4 "[背部]"
#define LOCALE_BACK_5 "[背部]"
#define LOCALE_BACK_6 "[ESPALDA]"
#define LOCALE_BACK_7 "[ESPALDA]"
#define LOCALE_BACK_8 "[НАЗАД]"

#define LOCALE_GOLD_0 " Gold."
#define LOCALE_GOLD_1 " 골드."
#define LOCALE_GOLD_2 " L' or."
#define LOCALE_GOLD_3 " Gold."
#define LOCALE_GOLD_4 " 金."
#define LOCALE_GOLD_5 " 金."
#define LOCALE_GOLD_6 " Oro."
#define LOCALE_GOLD_7 " Oro."
#define LOCALE_GOLD_8 " Золото."

// female icons
#define LOCALE_BES_0 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tBlood Elf"
#define LOCALE_BES_1 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|t블러드 엘프 인종 특성 스왑 -"
#define LOCALE_BES_2 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tÉchange de traits raciaux des elfes de sang -"
#define LOCALE_BES_3 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tRassenwechsel der Blutelfen -"
#define LOCALE_BES_4 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|t血精靈種族特質交換 -"
#define LOCALE_BES_5 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|t血精靈種族特質交換 -"
#define LOCALE_BES_6 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tIntercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_7 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tIntercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_8 "|TInterface\\icons\\Achievement_Character_Bloodelf_Female:25|tОбмен расовой особенностью эльфов крови -"

// male icons
#define LOCALE_BES_9  "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tBlood Elf"
#define LOCALE_BES_10 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|t블러드 엘프 인종 특성 스왑 -"
#define LOCALE_BES_11 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tÉchange de traits raciaux des elfes de sang -"
#define LOCALE_BES_12 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tRassenwechsel der Blutelfen -"
#define LOCALE_BES_13 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|t血精靈種族特質交換 -"
#define LOCALE_BES_14 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|t血精靈種族特質交換 -"
#define LOCALE_BES_15 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tIntercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_16 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tIntercambio de rasgos raciales de elfos de sangre -"
#define LOCALE_BES_17 "|TInterface\\icons\\Achievement_Character_Bloodelf_Male:25|tОбмен расовой особенностью эльфов крови -"

// female icons
#define LOCALE_DRS_0 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tDraenei"
#define LOCALE_DRS_1 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|t드레나이 인종 특성 스왑 -"
#define LOCALE_DRS_2 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tÉchange de traits raciaux draeneï -"
#define LOCALE_DRS_3 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tDraenei Rasseneigenschaftstausch -"
#define LOCALE_DRS_4 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|t德萊尼人種特質交換 -"
#define LOCALE_DRS_5 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|t德萊尼人種特質交換 -"
#define LOCALE_DRS_6 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tCambio de rasgo racial Draenei -"
#define LOCALE_DRS_7 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tCambio de rasgo racial Draenei -"
#define LOCALE_DRS_8 "|TInterface\\icons\\Achievement_Character_Draenei_Female:25|tОбмен расовыми особенностями дренеев -"

// male icons
#define LOCALE_DRS_9 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tDraenei"
#define LOCALE_DRS_10 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|t드레나이 인종 특성 스왑 -"
#define LOCALE_DRS_11 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tÉchange de traits raciaux draeneï -"
#define LOCALE_DRS_12 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tDraenei Rasseneigenschaftstausch -"
#define LOCALE_DRS_13 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|t德萊尼人種特質交換 -"
#define LOCALE_DRS_14 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|t德萊尼人種特質交換 -"
#define LOCALE_DRS_15 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tCambio de rasgo racial Draenei -"
#define LOCALE_DRS_16 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tCambio de rasgo racial Draenei -"
#define LOCALE_DRS_17 "|TInterface\\icons\\Achievement_Character_Draenei_Male:25|tОбмен расовыми особенностями дренеев -"

// female icons
#define LOCALE_DWS_0 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tDwarf"
#define LOCALE_DWS_1 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|t드워프 종족 특성 전환 -"
#define LOCALE_DWS_2 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tÉchange de traits raciaux nains -"
#define LOCALE_DWS_3 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tTausch der Rasseneigenschaften der Zwerge -"
#define LOCALE_DWS_4 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|t矮人種族特質交換 -"
#define LOCALE_DWS_5 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|t矮人種族特質交換 -"
#define LOCALE_DWS_6 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tIntercambio de rasgos raciales enanos -"
#define LOCALE_DWS_7 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tIntercambio de rasgos raciales enanos -"
#define LOCALE_DWS_8 "|TInterface\\icons\\Achievement_Character_Dwarf_Female:25|tОбмен расовыми особенностями гномов -"

// male icons
#define LOCALE_DWS_9 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tDwarf"
#define LOCALE_DWS_10 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|t드워프 종족 특성 전환 -"
#define LOCALE_DWS_11 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tÉchange de traits raciaux nains -"
#define LOCALE_DWS_12 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tTausch der Rasseneigenschaften der Zwerge -"
#define LOCALE_DWS_13 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|t矮人種族特質交換 -"
#define LOCALE_DWS_14 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|t矮人種族特質交換 -"
#define LOCALE_DWS_15 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tIntercambio de rasgos raciales enanos -"
#define LOCALE_DWS_16 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tIntercambio de rasgos raciales enanos -"
#define LOCALE_DWS_17 "|TInterface\\icons\\Achievement_Character_Dwarf_Male:25|tОбмен расовыми особенностями гномов -"

// female icons
#define LOCALE_GNS_0 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tGnome"
#define LOCALE_GNS_1 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|t그놈 인종 특성 스왑 -"
#define LOCALE_GNS_2 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tÉchange de traits raciaux de gnome -"
#define LOCALE_GNS_3 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tTausch der Rasseneigenschaft Gnom -"
#define LOCALE_GNS_4 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|t侏儒種族特質交換 -"
#define LOCALE_GNS_5 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|t侏儒種族特質交換 -"
#define LOCALE_GNS_6 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tCambio de rasgo racial de gnomo -"
#define LOCALE_GNS_7 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tCambio de rasgo racial de gnomo -"
#define LOCALE_GNS_8 "|TInterface\\icons\\Achievement_Character_Gnome_Female:25|tСмена расовой особенности гномов -"

// male icons
#define LOCALE_GNS_9 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tGnome"
#define LOCALE_GNS_10 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|t그놈 인종 특성 스왑 -"
#define LOCALE_GNS_11 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tÉchange de traits raciaux de gnome -"
#define LOCALE_GNS_12 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tTausch der Rasseneigenschaft Gnom -"
#define LOCALE_GNS_13 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|t侏儒種族特質交換 -"
#define LOCALE_GNS_14 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|t侏儒種族特質交換 -"
#define LOCALE_GNS_15 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tCambio de rasgo racial de gnomo -"
#define LOCALE_GNS_16 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tCambio de rasgo racial de gnomo -"
#define LOCALE_GNS_17 "|TInterface\\icons\\Achievement_Character_Gnome_Male:25|tСмена расовой особенности гномов -"

// female icons
#define LOCALE_HUS_0 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tHuman"
#define LOCALE_HUS_1 "|TInterface\\icons\\Achievement_Character_Human_Female:25|t인간 인종 특성 교환 -"
#define LOCALE_HUS_2 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tÉchange de traits raciaux humains -"
#define LOCALE_HUS_3 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tAustausch von menschlichen Rassenmerkmalen -"
#define LOCALE_HUS_4 "|TInterface\\icons\\Achievement_Character_Human_Female:25|t人類種族特質交換 -"
#define LOCALE_HUS_5 "|TInterface\\icons\\Achievement_Character_Human_Female:25|t人類種族特質交換 -"
#define LOCALE_HUS_6 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tIntercambio de rasgos raciales humanos -"
#define LOCALE_HUS_7 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tIntercambio de rasgos raciales humanos -"
#define LOCALE_HUS_8 "|TInterface\\icons\\Achievement_Character_Human_Female:25|tОбмен расовыми особенностями человека -"

// male icons
#define LOCALE_HUS_9 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tHuman"
#define LOCALE_HUS_10 "|TInterface\\icons\\Achievement_Character_Human_Male:25|t인간 인종 특성 교환 -"
#define LOCALE_HUS_11 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tÉchange de traits raciaux humains -"
#define LOCALE_HUS_12 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tAustausch von menschlichen Rassenmerkmalen -"
#define LOCALE_HUS_13 "|TInterface\\icons\\Achievement_Character_Human_Male:25|t人類種族特質交換 -"
#define LOCALE_HUS_14 "|TInterface\\icons\\Achievement_Character_Human_Male:25|t人類種族特質交換 -"
#define LOCALE_HUS_15 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tIntercambio de rasgos raciales humanos -"
#define LOCALE_HUS_16 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tIntercambio de rasgos raciales humanos -"
#define LOCALE_HUS_17 "|TInterface\\icons\\Achievement_Character_Human_Male:25|tОбмен расовыми особенностями человека -"

// female icons
#define LOCALE_NES_0 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tNight Elf"
#define LOCALE_NES_1 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|t나이트 엘프 인종 특성 스왑 -"
#define LOCALE_NES_2 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tÉchange de traits raciaux des elfes de la nuit -"
#define LOCALE_NES_3 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tTausch der Nachtelfen-Rasseneigenschaft -"
#define LOCALE_NES_4 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|t暗夜精靈種族特質交換 -"
#define LOCALE_NES_5 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|t暗夜精靈種族特質交換 -"
#define LOCALE_NES_6 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tCambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_7 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tCambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_8 "|TInterface\\icons\\Achievement_Character_Nightelf_Female:25|tОбмен расовыми особенностями ночных эльфов -"

// male icons
#define LOCALE_NES_9 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tNight Elf"
#define LOCALE_NES_10 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|t나이트 엘프 인종 특성 스왑 -"
#define LOCALE_NES_11 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tÉchange de traits raciaux des elfes de la nuit -"
#define LOCALE_NES_12 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tTausch der Nachtelfen-Rasseneigenschaft -"
#define LOCALE_NES_13 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|t暗夜精靈種族特質交換 -"
#define LOCALE_NES_14 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|t暗夜精靈種族特質交換 -"
#define LOCALE_NES_15 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tCambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_16 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tCambio de rasgo racial elfo de la noche -"
#define LOCALE_NES_17 "|TInterface\\icons\\Achievement_Character_Nightelf_Male:25|tОбмен расовыми особенностями ночных эльфов -"

// female icons
#define LOCALE_ORC_0 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tOrc"
#define LOCALE_ORC_1 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|t오크 종족 특성 교환 -"
#define LOCALE_ORC_2 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tÉchange de traits raciaux orques -"
#define LOCALE_ORC_3 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tAustausch der Ork-Rassenmerkmale -"
#define LOCALE_ORC_4 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|t獸人種族特質交換 -"
#define LOCALE_ORC_5 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|t獸人種族特質交換 -"
#define LOCALE_ORC_6 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tIntercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_7 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tIntercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_8 "|TInterface\\icons\\Achievement_Character_Orc_Female:25|tСмена расовой особенности орков -"

// male icons
#define LOCALE_ORC_9 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tOrc"
#define LOCALE_ORC_10 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|t오크 종족 특성 교환 -"
#define LOCALE_ORC_11 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tÉchange de traits raciaux orques -"
#define LOCALE_ORC_12 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tAustausch der Ork-Rassenmerkmale -"
#define LOCALE_ORC_13 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|t獸人種族特質交換 -"
#define LOCALE_ORC_14 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|t獸人種族特質交換 -"
#define LOCALE_ORC_15 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tIntercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_16 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tIntercambio de rasgos raciales de orcos -"
#define LOCALE_ORC_17 "|TInterface\\icons\\Achievement_Character_Orc_Male:25|tСмена расовой особенности орков -"

// female icons
#define LOCALE_TUR_0 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tTauren"
#define LOCALE_TUR_1 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|t타우렌 인종 특성 교환 -"
#define LOCALE_TUR_2 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tÉchange de traits raciaux Tauren -"
#define LOCALE_TUR_3 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tTauren Rasseneigenschaftstausch -"
#define LOCALE_TUR_4 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|t牛頭人種族特質交換 -"
#define LOCALE_TUR_5 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|t牛頭人種族特質交換 -"
#define LOCALE_TUR_6 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tIntercambio de rasgos raciales tauren -"
#define LOCALE_TUR_7 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tIntercambio de rasgos raciales tauren -"
#define LOCALE_TUR_8 "|TInterface\\icons\\Achievement_Character_Tauren_Female:25|tОбмен расовыми особенностями тауренов -"

// male icons
#define LOCALE_TUR_9 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tTauren"
#define LOCALE_TUR_10 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|t타우렌 인종 특성 교환 -"
#define LOCALE_TUR_11 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tÉchange de traits raciaux Tauren -"
#define LOCALE_TUR_12 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tTauren Rasseneigenschaftstausch -"
#define LOCALE_TUR_13 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|t牛頭人種族特質交換 -"
#define LOCALE_TUR_14 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|t牛頭人種族特質交換 -"
#define LOCALE_TUR_15 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tIntercambio de rasgos raciales tauren -"
#define LOCALE_TUR_16 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tIntercambio de rasgos raciales tauren -"
#define LOCALE_TUR_17 "|TInterface\\icons\\Achievement_Character_Tauren_Male:25|tОбмен расовыми особенностями тауренов -"

// female icons
#define LOCALE_TRL_0 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tTroll"
#define LOCALE_TRL_1 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|t트롤 인종 특성 교환 -"
#define LOCALE_TRL_2 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tÉchange de traits raciaux de troll -"
#define LOCALE_TRL_3 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tTroll-Rasseneigenschaftstausch -"
#define LOCALE_TRL_4 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|t巨魔種族特質交換 -"
#define LOCALE_TRL_5 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|t巨魔種族特質交換 -"
#define LOCALE_TRL_6 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tCambio de rasgo racial de trol -"
#define LOCALE_TRL_7 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tCambio de rasgo racial de trol -"
#define LOCALE_TRL_8 "|TInterface\\icons\\Achievement_Character_Troll_Female:25|tОбмен расовой особенности троллей -"

// male icons
#define LOCALE_TRL_9 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tTroll"
#define LOCALE_TRL_10 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|t트롤 인종 특성 교환 -"
#define LOCALE_TRL_11 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tÉchange de traits raciaux de troll -"
#define LOCALE_TRL_12 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tTroll-Rasseneigenschaftstausch -"
#define LOCALE_TRL_13 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|t巨魔種族特質交換 -"
#define LOCALE_TRL_14 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|t巨魔種族特質交換 -"
#define LOCALE_TRL_15 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tCambio de rasgo racial de trol -"
#define LOCALE_TRL_16 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tCambio de rasgo racial de trol -"
#define LOCALE_TRL_17 "|TInterface\\icons\\Achievement_Character_Troll_Male:25|tОбмен расовой особенности троллей -"

// female icons
#define LOCALE_UND_0 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tUndead"
#define LOCALE_UND_1 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|t언데드 인종 특성 스왑 -"
#define LOCALE_UND_2 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tÉchange de traits raciaux morts-vivants -"
#define LOCALE_UND_3 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tUntoter Rassenmerkmalentausch -"
#define LOCALE_UND_4 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|t亡靈種族特質交換 -"
#define LOCALE_UND_5 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|t亡靈種族特質交換 -"
#define LOCALE_UND_6 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tIntercambio de rasgos raciales no muertos -"
#define LOCALE_UND_7 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tIntercambio de rasgos raciales no muertos -"
#define LOCALE_UND_8 "|TInterface\\icons\\Achievement_Character_Undead_Female:25|tСмена расовой особенности нежити -"

// male icons
#define LOCALE_UND_9 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tUndead"
#define LOCALE_UND_10 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|t언데드 인종 특성 스왑 -"
#define LOCALE_UND_11 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tÉchange de traits raciaux morts-vivants -"
#define LOCALE_UND_12 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tUntoter Rassenmerkmalentausch -"
#define LOCALE_UND_13 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|t亡靈種族特質交換 -"
#define LOCALE_UND_14 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|t亡靈種族特質交換 -"
#define LOCALE_UND_15 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tIntercambio de rasgos raciales no muertos -"
#define LOCALE_UND_16 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tIntercambio de rasgos raciales no muertos -"
#define LOCALE_UND_17 "|TInterface\\icons\\Achievement_Character_Undead_Male:25|tСмена расовой особенности нежити -"

class Azerothcore_Race_Trait_announce : public PlayerScript
{
public:
    Azerothcore_Race_Trait_announce() : PlayerScript("Azerothcore_Race_Trait_announce", {
        PLAYERHOOK_ON_LOGIN
    }) { }

    void OnPlayerLogin(Player* Player)
    {
        if (sConfigMgr->GetOption<bool>("Azerothcore.Racial.Trait.Swap.Announce.enable", true))
            ChatHandler(Player->GetSession()).SendSysMessage("This server is running the |cff4CFF00Azerothcore Racial Trait Swap NPC |rmodule.");
    }
};

class npc_race_trait_swap : public CreatureScript
{
public:
    npc_race_trait_swap() : CreatureScript("npc_race_trait_swap") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        char const* localizedEntry;
        switch (player->GetSession()->GetSessionDbcLocale())
        {
            case LOCALE_koKR:
                localizedEntry = LOCALE_RACESWAP_1;
                break;
            case LOCALE_frFR:
                localizedEntry = LOCALE_RACESWAP_2;
                break;
            case LOCALE_deDE:
                localizedEntry = LOCALE_RACESWAP_3;
                break;
            case LOCALE_zhCN:
                localizedEntry = LOCALE_RACESWAP_4;
                break;
            case LOCALE_zhTW:
                localizedEntry = LOCALE_RACESWAP_5;
                break;
            case LOCALE_esES:
                localizedEntry = LOCALE_RACESWAP_6;
                break;
            case LOCALE_esMX:
                localizedEntry = LOCALE_RACESWAP_7;
                break;
            case LOCALE_ruRU:
                localizedEntry = LOCALE_RACESWAP_8;
                break;
            case LOCALE_enUS:
            default:
                localizedEntry = LOCALE_RACESWAP_0;
        }
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, localizedEntry, GOSSIP_SENDER_MAIN, 11);
        SendGossipMenuFor(player, 98888, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*menuId*/, uint32 gossipListId) override
    {
        int32 const RTS1 = sConfigMgr->GetOption<int32>("Racial.Traits.Swap.Gold", 100);

        char const* localizedExit = {};
        char const* localizedBack = {};
        char const* localizedGold = {};
        char const* localizedBES = {};
        char const* localizedDRS = {};
        char const* localizedDWS = {};
        char const* localizedGNS = {};
        char const* localizedHUS = {};
        char const* localizedNES = {};
        char const* localizedORC = {};
        char const* localizedTUR = {};
        char const* localizedTRL = {};
        char const* localizedUND = {};

        switch (player->GetSession()->GetSessionDbcLocale())
        {
            case LOCALE_koKR:
                localizedExit = LOCALE_EXIT_1;
                localizedBack = LOCALE_BACK_1;
                localizedGold = LOCALE_GOLD_1;
                break;
            case LOCALE_frFR:
                localizedExit = LOCALE_EXIT_2;
                localizedBack = LOCALE_BACK_2;
                localizedGold = LOCALE_GOLD_2;
                break;
            case LOCALE_deDE:
                localizedExit = LOCALE_EXIT_3;
                localizedBack = LOCALE_BACK_3;
                localizedGold = LOCALE_GOLD_3;
                break;
            case LOCALE_zhCN:
                localizedExit = LOCALE_EXIT_4;
                localizedBack = LOCALE_BACK_4;
                localizedGold = LOCALE_GOLD_4;
                break;
            case LOCALE_zhTW:
                localizedExit = LOCALE_EXIT_5;
                localizedBack = LOCALE_BACK_5;
                localizedGold = LOCALE_GOLD_5;
                break;
            case LOCALE_esES:
                localizedExit = LOCALE_EXIT_6;
                localizedBack = LOCALE_BACK_6;
                localizedGold = LOCALE_GOLD_6;
                break;
            case LOCALE_esMX:
                localizedExit = LOCALE_EXIT_7;
                localizedBack = LOCALE_BACK_7;
                localizedGold = LOCALE_GOLD_7;
                break;
            case LOCALE_ruRU:
                localizedExit = LOCALE_EXIT_8;
                localizedBack = LOCALE_BACK_8;
                localizedGold = LOCALE_GOLD_8;
                break;
            case LOCALE_enUS:
            default:
                localizedExit = LOCALE_EXIT_0;
                localizedBack = LOCALE_BACK_0;
                localizedGold = LOCALE_GOLD_0;
        }

        // Use the character's own gender: a shapeshift or morph sets the display gender (UNIT_FIELD_BYTES_0) from the model, which can be GENDER_NONE and left every label null (crash in tree form, 2026-11-23).
        uint8 const nativeGender = player->GetByteValue(PLAYER_BYTES_3, PLAYER_BYTES_3_OFFSET_GENDER);

        if (nativeGender == GENDER_FEMALE)
        {
            switch (player->GetSession()->GetSessionDbcLocale())
            {
                case LOCALE_koKR:
                    localizedBES = LOCALE_BES_1;
                    localizedDRS = LOCALE_DRS_1;
                    localizedDWS = LOCALE_DWS_1;
                    localizedGNS = LOCALE_GNS_1;
                    localizedHUS = LOCALE_HUS_1;
                    localizedNES = LOCALE_NES_1;
                    localizedORC = LOCALE_ORC_1;
                    localizedTUR = LOCALE_TUR_1;
                    localizedTRL = LOCALE_TRL_1;
                    localizedUND = LOCALE_UND_1;
                    break;
                case LOCALE_frFR:
                    localizedBES = LOCALE_BES_2;
                    localizedDRS = LOCALE_DRS_2;
                    localizedDWS = LOCALE_DWS_2;
                    localizedGNS = LOCALE_GNS_2;
                    localizedHUS = LOCALE_HUS_2;
                    localizedNES = LOCALE_NES_2;
                    localizedORC = LOCALE_ORC_2;
                    localizedTUR = LOCALE_TUR_2;
                    localizedTRL = LOCALE_TRL_2;
                    localizedUND = LOCALE_UND_2;
                    break;
                case LOCALE_deDE:
                    localizedBES = LOCALE_BES_3;
                    localizedDRS = LOCALE_DRS_3;
                    localizedDWS = LOCALE_DWS_3;
                    localizedGNS = LOCALE_GNS_3;
                    localizedHUS = LOCALE_HUS_3;
                    localizedNES = LOCALE_NES_3;
                    localizedORC = LOCALE_ORC_3;
                    localizedTUR = LOCALE_TUR_3;
                    localizedTRL = LOCALE_TRL_3;
                    localizedUND = LOCALE_UND_3;
                    break;
                case LOCALE_zhCN:
                    localizedBES = LOCALE_BES_4;
                    localizedDRS = LOCALE_DRS_4;
                    localizedDWS = LOCALE_DWS_4;
                    localizedGNS = LOCALE_GNS_4;
                    localizedHUS = LOCALE_HUS_4;
                    localizedNES = LOCALE_NES_4;
                    localizedORC = LOCALE_ORC_4;
                    localizedTUR = LOCALE_TUR_4;
                    localizedTRL = LOCALE_TRL_4;
                    localizedUND = LOCALE_UND_4;
                    break;
                case LOCALE_zhTW:
                    localizedBES = LOCALE_BES_5;
                    localizedDRS = LOCALE_DRS_5;
                    localizedDWS = LOCALE_DWS_5;
                    localizedGNS = LOCALE_GNS_5;
                    localizedHUS = LOCALE_HUS_5;
                    localizedNES = LOCALE_NES_5;
                    localizedORC = LOCALE_ORC_5;
                    localizedTUR = LOCALE_TUR_5;
                    localizedTRL = LOCALE_TRL_5;
                    localizedUND = LOCALE_UND_5;
                    break;
                case LOCALE_esES:
                    localizedBES = LOCALE_BES_6;
                    localizedDRS = LOCALE_DRS_6;
                    localizedDWS = LOCALE_DWS_6;
                    localizedGNS = LOCALE_GNS_6;
                    localizedHUS = LOCALE_HUS_6;
                    localizedNES = LOCALE_NES_6;
                    localizedORC = LOCALE_ORC_6;
                    localizedTUR = LOCALE_TUR_6;
                    localizedTRL = LOCALE_TRL_6;
                    localizedUND = LOCALE_UND_6;
                    break;
                case LOCALE_esMX:
                    localizedBES = LOCALE_BES_7;
                    localizedDRS = LOCALE_DRS_7;
                    localizedDWS = LOCALE_DWS_7;
                    localizedGNS = LOCALE_GNS_7;
                    localizedHUS = LOCALE_HUS_7;
                    localizedNES = LOCALE_NES_7;
                    localizedORC = LOCALE_ORC_7;
                    localizedTUR = LOCALE_TUR_7;
                    localizedTRL = LOCALE_TRL_7;
                    localizedUND = LOCALE_UND_7;
                    break;
                case LOCALE_ruRU:
                    localizedBES = LOCALE_BES_8;
                    localizedDRS = LOCALE_DRS_8;
                    localizedDWS = LOCALE_DWS_8;
                    localizedGNS = LOCALE_GNS_8;
                    localizedHUS = LOCALE_HUS_8;
                    localizedNES = LOCALE_NES_8;
                    localizedORC = LOCALE_ORC_8;
                    localizedTUR = LOCALE_TUR_8;
                    localizedTRL = LOCALE_TRL_8;
                    localizedUND = LOCALE_UND_8;
                    break;
                case LOCALE_enUS:
                default:
                    localizedBES = LOCALE_BES_0;
                    localizedDRS = LOCALE_DRS_0;
                    localizedDWS = LOCALE_DWS_0;
                    localizedGNS = LOCALE_GNS_0;
                    localizedHUS = LOCALE_HUS_0;
                    localizedNES = LOCALE_NES_0;
                    localizedORC = LOCALE_ORC_0;
                    localizedTUR = LOCALE_TUR_0;
                    localizedTRL = LOCALE_TRL_0;
                    localizedUND = LOCALE_UND_0;
            }
        }
        else // GENDER_MALE, and the fallback so the labels are never null
        {
            switch (player->GetSession()->GetSessionDbcLocale())
            {
                case LOCALE_koKR:
                    localizedBES = LOCALE_BES_10;
                    localizedDRS = LOCALE_DRS_10;
                    localizedDWS = LOCALE_DWS_10;
                    localizedGNS = LOCALE_GNS_10;
                    localizedHUS = LOCALE_HUS_10;
                    localizedNES = LOCALE_NES_10;
                    localizedORC = LOCALE_ORC_10;
                    localizedTUR = LOCALE_TUR_10;
                    localizedTRL = LOCALE_TRL_10;
                    localizedUND = LOCALE_UND_10;
                    break;
                case LOCALE_frFR:
                    localizedBES = LOCALE_BES_11;
                    localizedDRS = LOCALE_DRS_11;
                    localizedDWS = LOCALE_DWS_11;
                    localizedGNS = LOCALE_GNS_11;
                    localizedHUS = LOCALE_HUS_11;
                    localizedNES = LOCALE_NES_11;
                    localizedORC = LOCALE_ORC_11;
                    localizedTUR = LOCALE_TUR_11;
                    localizedTRL = LOCALE_TRL_11;
                    localizedUND = LOCALE_UND_11;
                    break;
                case LOCALE_deDE:
                    localizedBES = LOCALE_BES_12;
                    localizedDRS = LOCALE_DRS_12;
                    localizedDWS = LOCALE_DWS_12;
                    localizedGNS = LOCALE_GNS_12;
                    localizedHUS = LOCALE_HUS_12;
                    localizedNES = LOCALE_NES_12;
                    localizedORC = LOCALE_ORC_12;
                    localizedTUR = LOCALE_TUR_12;
                    localizedTRL = LOCALE_TRL_12;
                    localizedUND = LOCALE_UND_12;
                    break;
                case LOCALE_zhCN:
                    localizedBES = LOCALE_BES_13;
                    localizedDRS = LOCALE_DRS_13;
                    localizedDWS = LOCALE_DWS_13;
                    localizedGNS = LOCALE_GNS_13;
                    localizedHUS = LOCALE_HUS_13;
                    localizedNES = LOCALE_NES_13;
                    localizedORC = LOCALE_ORC_13;
                    localizedTUR = LOCALE_TUR_13;
                    localizedTRL = LOCALE_TRL_13;
                    localizedUND = LOCALE_UND_13;
                    break;
                case LOCALE_zhTW:
                    localizedBES = LOCALE_BES_14;
                    localizedDRS = LOCALE_DRS_14;
                    localizedDWS = LOCALE_DWS_14;
                    localizedGNS = LOCALE_GNS_14;
                    localizedHUS = LOCALE_HUS_14;
                    localizedNES = LOCALE_NES_14;
                    localizedORC = LOCALE_ORC_14;
                    localizedTUR = LOCALE_TUR_14;
                    localizedTRL = LOCALE_TRL_14;
                    localizedUND = LOCALE_UND_14;
                    break;
                case LOCALE_esES:
                    localizedBES = LOCALE_BES_15;
                    localizedDRS = LOCALE_DRS_15;
                    localizedDWS = LOCALE_DWS_15;
                    localizedGNS = LOCALE_GNS_15;
                    localizedHUS = LOCALE_HUS_15;
                    localizedNES = LOCALE_NES_15;
                    localizedORC = LOCALE_ORC_15;
                    localizedTUR = LOCALE_TUR_15;
                    localizedTRL = LOCALE_TRL_15;
                    localizedUND = LOCALE_UND_15;
                    break;
                case LOCALE_esMX:
                    localizedBES = LOCALE_BES_16;
                    localizedDRS = LOCALE_DRS_16;
                    localizedDWS = LOCALE_DWS_16;
                    localizedGNS = LOCALE_GNS_16;
                    localizedHUS = LOCALE_HUS_16;
                    localizedNES = LOCALE_NES_16;
                    localizedORC = LOCALE_ORC_16;
                    localizedTUR = LOCALE_TUR_16;
                    localizedTRL = LOCALE_TRL_16;
                    localizedUND = LOCALE_UND_16;
                    break;
                case LOCALE_ruRU:
                    localizedBES = LOCALE_BES_17;
                    localizedDRS = LOCALE_DRS_17;
                    localizedDWS = LOCALE_DWS_17;
                    localizedGNS = LOCALE_GNS_17;
                    localizedHUS = LOCALE_HUS_17;
                    localizedNES = LOCALE_NES_17;
                    localizedORC = LOCALE_ORC_17;
                    localizedTUR = LOCALE_TUR_17;
                    localizedTRL = LOCALE_TRL_17;
                    localizedUND = LOCALE_UND_17;
                    break;
                case LOCALE_enUS:
                default:
                    localizedBES = LOCALE_BES_9;
                    localizedDRS = LOCALE_DRS_9;
                    localizedDWS = LOCALE_DWS_9;
                    localizedGNS = LOCALE_GNS_9;
                    localizedHUS = LOCALE_HUS_9;
                    localizedNES = LOCALE_NES_9;
                    localizedORC = LOCALE_ORC_9;
                    localizedTUR = LOCALE_TUR_9;
                    localizedTRL = LOCALE_TRL_9;
                    localizedUND = LOCALE_UND_9;
            }
        }

        std::ostringstream messageBE;
        messageBE << localizedBES;
        if (RTS1 > 0)
            messageBE << " - " << RTS1 << localizedGold;
        std::ostringstream messageDR;
        messageDR << localizedDRS;
        if (RTS1 > 0)
            messageDR << " - " << RTS1 << localizedGold;
        std::ostringstream messageDW;
        messageDW << localizedDWS;
        if (RTS1 > 0)
            messageDW << " - " << RTS1 << localizedGold;
        std::ostringstream messageGN;
        messageGN << localizedGNS;
        if (RTS1 > 0)
            messageGN << " - " << RTS1 << localizedGold;
        std::ostringstream messageHU;
        messageHU << localizedHUS;
        if (RTS1 > 0)
            messageHU << " - " << RTS1 << localizedGold;
        std::ostringstream messageNE;
        messageNE << localizedNES;
        if (RTS1 > 0)
            messageNE << " - " << RTS1 << localizedGold;
        std::ostringstream messageOR;
        messageOR << localizedORC;
        if (RTS1 > 0)
            messageOR << " - " << RTS1 << localizedGold;
        std::ostringstream messageTA;
        messageTA << localizedTUR;
        if (RTS1 > 0)
            messageTA << " - " << RTS1 << localizedGold;
        std::ostringstream messageTR;
        messageTR << localizedTRL;
        if (RTS1 > 0)
            messageTR << " - " << RTS1 << localizedGold;
        std::ostringstream messageUN;
        messageUN << localizedUND;
        if (RTS1 > 0)
            messageUN << " - " << RTS1 << localizedGold;

        ClearGossipMenuFor(player);

        switch (gossipListId)
        {
            case 11:
                if (player)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageBE.str(), GOSSIP_SENDER_MAIN, 1); // Blood Elf Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageDR.str(), GOSSIP_SENDER_MAIN, 2); // Draenei Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageDW.str(), GOSSIP_SENDER_MAIN, 3); // Dwarves Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageGN.str(), GOSSIP_SENDER_MAIN, 4); // Gnome Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageHU.str(), GOSSIP_SENDER_MAIN, 5); // Human Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageNE.str(), GOSSIP_SENDER_MAIN, 6); // Night Elf Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageOR.str(), GOSSIP_SENDER_MAIN, 7); // Orc Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageTA.str(), GOSSIP_SENDER_MAIN, 8); // Tauren Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageTR.str(), GOSSIP_SENDER_MAIN, 9); // Troll Selection
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, messageUN.str(), GOSSIP_SENDER_MAIN, 10); // Undead
                    AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                    SendGossipMenuFor(player, 98888, creature->GetGUID());
                }
                break;

            case 1111: // close
                CloseGossipMenuFor(player);
                break;

            case 1: // Blood Elf Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Pride of the Blood Elves", GOSSIP_SENDER_MAIN, 112);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988881, creature->GetGUID());
                break;

            case 112: // Blood Elf
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_BLOODELF, RTS1);
                break;

            case 2: // Draenei Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Light of the Draenei", GOSSIP_SENDER_MAIN, 212);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988882, creature->GetGUID());
                break;

            case 212: // Draenei
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_DRAENEI, RTS1);
                break;

            case 3: // Dwarves Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Iron Will of the Dwarves", GOSSIP_SENDER_MAIN, 31);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988883, creature->GetGUID());
                break;

            case 31: // Dwarves
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_DWARF, RTS1);
                break;

            case 4: // Gnome Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Technological Might of the Gnomes", GOSSIP_SENDER_MAIN, 41);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988884, creature->GetGUID());
                break;

            case 41: // Gnome
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_GNOME, RTS1);
                break;

            case 5: // Human Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Human Spirit", GOSSIP_SENDER_MAIN, 51);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988885, creature->GetGUID());
                break;

            case 51: // Human
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_HUMAN, RTS1);
                break;

            case 6: // Night Elf Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Ancient Night Elves", GOSSIP_SENDER_MAIN, 61);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988886, creature->GetGUID());
                break;

            case 61: // Night Elf
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_NIGHTELF, RTS1);
                break;

            case 7: // Orc Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Strength of the Orc", GOSSIP_SENDER_MAIN, 71);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988887, creature->GetGUID());
                break;

            case 71: // Orc
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_ORC, RTS1);
                break;

            case 8: // Tauren Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Endurance of the Taurens", GOSSIP_SENDER_MAIN, 81);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988888, creature->GetGUID());
                break;

            case 81: // Tauren
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_TAUREN, RTS1);
                break;

            case 9: // Troll Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Vodoo of the Trolls", GOSSIP_SENDER_MAIN, 91);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988889, creature->GetGUID());
                break;

            case 91: // Troll
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_TROLL, RTS1);
                break;

            case 10: // Undead Selection
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Commit to the Will of the Forsaken", GOSSIP_SENDER_MAIN, 101);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedBack, GOSSIP_SENDER_MAIN, 11);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, localizedExit, GOSSIP_SENDER_MAIN, 1111);
                SendGossipMenuFor(player, 988880, creature->GetGUID());
                break;

            case 101: // Undead
                CloseGossipMenuFor(player);
                TofuRacials::DoSwap(player, RACE_UNDEAD_PLAYER, RTS1);
                break;

            default:
                break;
        }
        return true;
    }
};

void AddSC_racial_traits_npc()
{
    new Azerothcore_Race_Trait_announce();
    new npc_race_trait_swap();
}
