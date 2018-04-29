# Garrison

Mechanics related to garrisoning units in buildings or siege units.

## Number of projectiles

[Source](https://www.youtube.com/watch?v=tCBlWAkanPY)

How many projectiles a building fires depends on two variables, the *damage per second* value of the building and the (pierce) *damage per second* of each garrisoned unit. The amount of arrows added by a unit can be derived from this formula:

```
    additional_arrows = sum(unit_dps_pierce) / building_dps
```

`unit_dps_pierce` is calculated by dividing a unit's pierce attack value by its reload speed. `building_dps` is obtained in the same way, by dividing the building's attack value by its reload speed. Units doing melee damage do not increase the number of arrows or projectiles. However, for villagers `unit_dps_pierce` is a fixed value of `2.5`. Researching the Teuton unique technology *Crenellations* has a similar effect by setting `unit_dps_pierce` to `2.5` for every unit in the infantry armor class.

Bombard towers have even stricter rules in that only gunpowder units can increase the projectile count. The calculation of `building_dps` is also done slightly different. For the first additional projectile `sum(unit_dps_pierce)` is divided by `(building_dps * 2)` which effectively doubles the amount of required units for an additional shot. For the second and third cannon ball the standard formula is used again.

**Pseudocode**
```
    remaining_unit_dps = sum(unit_dps_pierce) - (building_dps * 2);

    if(remaining_unit_dps >= 0) {
        additional_arrows = 1 + (remaining_unit_dps / building_dps);
    }
    else {
        additional_arrows = 0;
    }
```

The result `additional_arrows` is floored and added to the default arrow count of the building. A table with the default projectile counts is shown below.

Building      | Default Arrow Count | Default Attack  | Maximum Arrow Count
--------------|---------------------|-----------------|--------------------
Town Center   | 0                   | 5               | 10
Castle        | 5                   | 11              | 20
Tower         | 1                   | 5/7/8\*         | 5
Bombard Tower | 1                   | 120\*\*         | 4

\* Damage of Watch Tower/Guard Tower/Keep

\*\* Calculation is different; see pseudocode above

Increasing the default attack damage of buildings by researching technologies also affects `building_dps`.

### Example

The Elite Janissary has a pierce attack value of `22` and a reload speed of `3.45` (The Conqueror's). With these values we can calculate `unit_dps_pierce`.

```
unit_dps_pierce = 22 / 3.45 = 6.37
```

Now that we know `unit_dps_pierce`, the amount of additional projectiles for each building can be obtained.

**Town Center**
```
    building_dps = 5 / 2.03 = 2.46
```
```
    additional_arrows = 6.37 / 2.46 = 2.58
```

**Castle**
```
    building_dps = 11 / 2.03 = 5.41
```
```
    additional_arrows = 6.37 / 5.41 = 1.17
```

**Keep**
```
    building_dps = 8 / 2.03 = 3.94
```
```
    additional_arrows = 6.37 / 3.94 = 1.61
```

**Bombard Tower**
```
(1st additional projectile)
    building_dps = (120 / 6) * 2 = 40
(Other additional projectile)
    building_dps = (120 / 6)     = 20
```
```
(1st additional projectile)
    additional_arrows = 6.37 / 40 = 0.155
(Other additional projectile)
    additional_arrows = 6.37 / 20 = 0.31
```

7 Elite Janissaries are needed to make the bombard tower fire one additional projectile. Two additional cannon balls are only achievable with 10 Elite Janissaries garrisoned in a bombard tower.

# Weird AoE2 Quirks

* Only units that do pierce damage can increase the number of projectiles fired by a building. Units that fire projectiles dealing melee damage - like Gbetos, Throwing Axemen and Mameluks - will have no effect on the arrow count.
* For the Cho Ko Nu `unit_dps_pierce` does not use the pierce attack value of the first arrow fired. Instead it only takes the damage the last arrow does into consideration. Because this arrow only does 3 pierce damage, the number of projectiles added to a building's attack is fairly low.
