# Attacking Alarms

[Source](https://www.youtube.com/watch?v=jmgD8WMlZtk)

## The two different alarms/alerts

There are two alarms in the game files that are triggered when the player is attacked. One of them also has two variations that are played for special occasions.

The first one (`50315`) sounds like a horn and is triggered when one of your units is attacked by an enemy. `50316` has a bell sound and plays when a villager or building gets damaged by an enemy. Variations on the first sound are `50313` (warns that a unit is being converted) and `50366` (warns that a wild animal attacks a unit).

    50313.wav  (Horn + monk) One of your units is being converted
    50315.wav  (Horn) Your military units are attacked
    50316.wav  (Bell) A villager or building of yours is attacked
    50366.wav  (Horn + wolf) Wild animal attacks your units (not played for boar)

All the alarms only trigger if you are attacked and not if you kill enemy units that have not hit you yet.

## Battles

There is only one alarm played per "battle". Battles start when the player is attacked, i.e. when the alarm sound plays. Their size is always 20x20 tiles with the center being at the place where the players' units were attacked first (i.e. a 10 tile radius). Units fighting in this area can not trigger more alarms until they either leave the battle field (they would have to move outside of the 20x20 area) or the battle is over (see **Cooldown**).

Multiple battles can happen simultaineously. For example, units that start to fight outside of the 10 tile radius will create a new battle field and thus a seperate alarm sound will be played.

## Cooldown

A battle is over when

* fighting in the 20x20 area has stopped
* a 10 second cooldown has passed

If fighting on the battle field breaks out again before the cooldown finishes, it resets.

After the cooldown has passed, the game considers any new confrontation/fighting as a new battle. Therefore a new alarm sound will be played.
