# Damage

[Source](http://ageofempires.wikia.com/wiki/Attack)

Close combat and projectile damage calculations.

## Measurement

(dark age, no tech)

unit            | enemy    | stated attack | stated defense | damage (uphill) | damage (level) | damage (downhill)
----------------|----------|---------------|----------------|-----------------|----------------|------------------
teutonic knight | house    | 17            | 0              | 16              | 21             | 26
teutonic knight | villager | 17            | 0              | 13              | 17             | 22
villager        | villager | 3             | 0              | 2               | 3              | 4
trebuchet       | castle   | 200           | 0              | 323             | 431            | 539
ram             | castle   | 2             | 0              | 88              | 117            | 146

## Attack damage formula

The damage in AoE1 and AoE2 is calculated with the following formula

```
damage = max(1, ((max(0, melee_damage - melee_armor)
                + max(0, pierce_damage - pierce_armor)
                + sum(max(0, attack_bonus - attack_resist)))
                * elevation_factor) * stray_factor)
```

Explanation:

* `melee_damage`: Melee damage of the attacking unit
* `melee_armor`: Melee armor of the defending unit
* `pierce_damage`: Pierce damage of the attacking unit
* `pierce_armor`: Pierce armor of the defending unit
* `attack_bonus`: Attack bonus (flat additive) against a target's armor class.
* `attack_resist`: Attack resist (flat additive) against an attack bonus.
* `elevation_factor`: Multiplies damage by `1.25` (`1.33` in AoE1) if the attacking units is on higher elevated terrain or on top of a cliff. Similarly, units on lower ground receive a malus that multiplies their damage by `0.75` (`0.66` in AoE1). The malus doesn't apply when standing at the bottom of a cliff.
* `stray_factor`: When a projectile misses its intended target but hits a bystanding unit, damage is halved. Check [Accuracy](accuracy.md) for more details.

Standing on top of cliffs and additionally being higher elevated does not increase the damage bonus. Also, standing on higher elevated terrain while at the bottom of a cliff does not negate the bonus damage of the unit on top of a cliff. However, it does gives the unit at the bottom of the cliff the elevation damage bonus of `1.25`. The only requirement for the application of the cliff damage bonus is that there must be a cliff in between the units. The distance to the cliff does not matter.

### Armor

[Source](http://ageofempires.wikia.com/wiki/Armor)

Armor is organized through *armor classes* of which there are 30 in AoC and 32 in AoE2:HD. Units can have several armor classes, but will always have the classes *melee* and *pierce* assigned. Units have a value for each of their armor classes which indicate the `attack_resist` against attacks directed at the armor class. All units (except trees, gregarious animals, hunting wolves and some units from beta versions) have a default value of **1000** for each armor class that they don't have.

An attack is always applied against one or more armor classes. We will look at an example to show how the damage is calculated.

An archer attack a spearman. The attack of the archer does `0` damage against the *melee* armor class, `4` damage against the *pierce* armor class and `3` damage against the *spearman* armor class. The spearman has the armor classes *melee* with value `0`, *pierce* with value `0`, *infantry* with value `0` and *spearman* with value `0`. Now we can calculate the outcome of the attack:

```
damage =   max(0, archer_melee_damage - spearman_melee_armor)
         + max(0, archer_pierce_damage - spearman_pierce_armor)
         + max(0, archer_spearman_damage - spearman_spearman_armor)

       = max(0, 0 - 0) + max(0, 4 - 0) + max(0, 3 - 0) = 7
```

The attack does a total of 7 damage. The *infantry* armor class of the spearman is ignored because the archer's attack does not do damage against it.

As a second example, we will let a spearman attack a cataphract. This time, we will list attack and defense in a table.

Attack class  | Attack value |
--------------|--------------|
melee         | 3            |
pierce        | 0            |
cavalry       | 15           |
war elephant  | 15           |
camel         | 12           |
ship          | 9            |
eagle warrior | 1            |
building      | 1            |

Defense class | Attack resist |
--------------|---------------|
melee         | 2             |
pierce        | 1             |
cavalry       | 12            |
unique unit   | 0             |

The matching armor classes are *melee*, *pierce* and *cavalry*. We can derive a formula for the damage from that.

```
damage =   max(0, spearman_melee_damage - cataphract_melee_armor)
         + max(0, spearman_pierce_damage - cataphract_pierce_armor)
         + max(0, spearman_cavalry_damage - cataphract_cavalry_armor)

       = max(0, 3 - 2) + max(0, 0 - 1) + max(0, 15 - 12) = 4
```

The attack does a total of 4 damage.

Only the values for the melee class and the pierce class are visible in the interface.

## Damage by Projectiles

The damage of a shot in AoE2 is most likely calculated after it has hit the target. This leads to weird behaviour when units die, including:

* Losing environmental bonuses such as uphill or cliff bonus damage (+25 %)
* Dealing no damage, e.g. trebuchets
* Dealing only 1 damage, e.g. if japanese trebuchets are packed before they hit their target

It also has other consequences. Technologies will affect projectiles that were in the air when research finished.
