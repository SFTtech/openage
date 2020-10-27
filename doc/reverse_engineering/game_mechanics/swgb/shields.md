# Shields mechanics in SWGB

## Intro

Shields are another unique feature of the SWGB franchise. A shielded unit has
double the number of hit points that of an unshielded one, and those extra hit
points regenerate.

## Mechanism

A shielded unit gradually fills a shield bar (originally yellow and placed
under the health bar), that doubles its health points. Aditionally, shielded
units have a yellow glow effect, and [the UI](user-interface.md#cap) lights the
leftmost indicator in yellow when a shielded unit is selected, as visible
[here]([https://www.youtube.com/watch?v=pwAtMv_eiM4&feature=youtu.be&t=1786]).

Shields are depleted first when taking hits, before the regular health bar.
Regenerating shields do not affect the health bar.

### Modifiers

When losing the shield ability (like when exiting a shield generator), the
shield points will gradually deplete. They will gradually replenish when
regaining the ability.
The depletion rate is reportedly set for the full depletion to happen in 40s.
This still needs confirmation.

Shielded units always spawn with an empty shield.

## Applicable Units

([Source](https://swgb.fandom.com/wiki/Shield))

### Shield generators (always self-shielded when active)

* Shield Generator (when powered)
* Fambaa Shield Generator (Gungans' mobile shield generator)

### Self-shielded units

* Heavy Destroyer Droid (Trade Federation)
* Royal Crusader (Royal Naboo)
  * Requires the shielding technology in vanilla SWGB, which is unavailable in
    SWGBCC
  * Shielding automatically granted when upgraded to Elite Royal crusader in
    SWGBCC
* All aircraft except transport with the *Shield modifications* technology
  (Rebel Alliance, Republic, Wookies, Royal Naboo)

### Self-shielded buildings

* Power Core (with *Power Core Shielding* technology)
* Shield Wall (with necessary upgrade)

## Future research

Confirm depletion and regeneration rates.
