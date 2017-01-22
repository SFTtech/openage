Damage per attack
=================

How is damage calculated?

## Measurement

(dark age, no tech)

unit            | enemy    | stated attack | stated defense | damage (uphill) | damage (level) | damage (downhill)
----------------|----------|---------------|----------------|-----------------|----------------|------------------
teutonic knight | house    | 17            | 0              | 16              | 21             | 26
teutonic knight | villager | 17            | 0              | 13              | 17             | 22
villager        | villager | 3             | 0              | 2               | 3              | 4
trebuchet       | castle   | 200           | 0              | 323             | 431            | 539
ram             | castle   | 2             | 0              | 88              | 117            | 146


## Attack damage calculation theory

Each unit has a list of attack and armor classes.

When some unit attacks another, each attacker's attack classes is used only if the target has the matching armor class.

For example: **Halberdier vs Marmeluke**

Halberdier has attack classes and values:
* class 4 (Melee): 6
* class 5 (War Elephants): 28
* class 8 (Cavalry): 32
* class 16 (Ships, Camels, Saboteurs): 16
* class 21 (Buildings): 1
* class 29 (Eagle Warrior): 1

Mameluke has armor classes and values:
* class 3 (Ranged Attack): 0
* class 4 (Melee): 1
* class 8 (Cavalry): 11
* class 15 (Archers): 0
* class 16 (Ships, Camels, Saboteurs): 0
* class 19 (Special Units): 0


In this example, the classes 4, 8 and 16 are used.

Damage is calculated as follows:

``` python

overall_damage = 0
for attack_damage, armor_absorbtion in matching_attack_classes(attacker, attacked):
    damage = attack_damage - armor_absorbtion
    if damage > 0:
        overall_damage += damage

if overall_damage < 1:
    overall_damage = 1

return overall_damage
```

So in the example we have:
`(6 - 1) + (32 - 11) + (16 - 0) = 5 + 21 + 16 = 42`


All units (except trees, gregarious animals, hunting wolves and some units from beta versions)
have a default value of **1000** for each armor class that they don't have.

## Damage by Projectiles

The damage of a shot in AoE2 is most likely calculated after it has hit the target. This leads to weird behaviour when units die including:

* Losing environmental bonuses such as uphill or cliff bonus damage (+25 %)
* Dealing no damage, e.g. trebuchets
* Dealing only 1 damage, e.g. if japanese trebuchets are packed before they hit their target

It also has other consequences: Technologies will affect projectiles that were in the air when research finished.
