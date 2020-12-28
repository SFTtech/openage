# Stealth and detection mechanics in SWGB

## Intro

A few units have a "stealth" ability.
This feature is unique to the SWGB franchise among games that run on the Genie
Engine. Unfortunately, it is somewhat underutilized in the game.

A lot more units have the "detection" ability, and can detect stealthed units
within their line of sight.

## Mechanism

A stealthed unit cannot be seen by the enemy.

A Detector enemy can reveal stealth units within its line of sight.

## User interface

These capabilities are displayed in the [game UI](user-interface.md#cap) by
lighting up the corresponding indicator when the unit is selected.

A stealthed unit will lose the stealth indicator when it becomes unconcealed
([reference video](https://youtu.be/S-SL-G7KMuE?t=2256)).

### Modifiers

A stealthed unit is unconcealed when it either:

* Attacks
* Enters the line of sight of a detector unit

## Applicable Units

([Source](https://swgb.fandom.com/wiki/Stealth/Detection))

### Stealth

#### Always active

* Frigate (Gungans)
* Underwater Prefab Shelter (Gungans)

#### Available trough the *Jedi Mind Trick* technology

* Jedi/Sith Master (Rebel Alliance, Royal Naboo and Republic)
* Jedi Starfighter (Republic)

### Detection

#### Always active

* Scout
* Bounty Hunter
* Sentry Post
* Sensor Buoy
* Dark Trooper (Galactic Empire)
* Probot (Galactic Empire)
* Airspeeder (Rebel Alliance)
* Royal Crusader (Royal Naboo)

#### Available trough the *Jedi/Sith Perception* technology

* Jedi/Sith Knight
* Jedi/Sith Master

## Future research

Do detector units unconceal an enemy unit for every player, or only for
themselves? How does this affect the indicator?
