/*
 * Tofu Worldview racial trait swap fork.
 *
 * Racial choice storage, per-class racial selection, and the login enforcement that fixes the native-racial relearn bug
 * (upstream azerothcore/mod-racial-trait-swap issues #13 and #25). See TofuRacials.cpp.
 */

#ifndef TOFU_RACIALS_H
#define TOFU_RACIALS_H

#include "Define.h"

class Player;

namespace TofuRacials
{
    // Charges goldCost gold, records race as the character's racial choice (or clears the choice when race is the
    // character's own race), and makes the character's racial spells match that race and the character's class.
    void DoSwap(Player* player, uint8 race, int32 goldCost);
}

#endif
