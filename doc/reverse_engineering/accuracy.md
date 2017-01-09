# Accuracy and Projectiles

Mechanics of accuracy.

## Distance

[Source](https://www.youtube.com/watch?v=HpzOTdItUGA)

Accuracy depends on the distance to the intended target. The following figure shows accuracy of shooting a standing target at various distances. `o` represents the archer (in this case: Elite Longbowman), `X` represents the target. Every `.` symbolizes a tile between the two units.

```
......................................
...1 Tile....oX.......................  100   % accuracy
......................................
...2 Tiles...o.X......................  100   % accuracy
......................................
...3 Tiles...o..X.....................  96.63 % accuracy
......................................
...4 Tiles...o...X....................  91.90 % accuracy
......................................
...6 Tiles...o.....X..................  87.82 % accuracy
......................................
...12 Tiles..o...........X............  83.17 % accuracy
......................................
...21 Tiles..o....................X...  81.77 % accuracy
......................................
```
If the target is standing 2 or less tiles away, arrows always seem to hit. For targets at range 3 or more, the archer's accuray detriorates. The deterioration slows down with every tile between archer and target and seems to approach a *minimum accuracy* (in this case: 80 %).

### Inaccuracy

[Source](https://www.youtube.com/watch?v=nh3GnmFSkeY)

The game probably determines first, if a shot is accurate (meaning it will 100 with a 100 % accuracy) or inaccurate. In the latter case the *accuracy error radius* comes into effect. This value is used by the game to calculate a margin between the target and the spot where the projectile lands.

Also, this margin is likely related to the distance between archer and target. In this case, the code for calculating the margin would look like this:

    (margin) = (distance) * rand(0,accuracy_error_radius)

Because the margin is very low for most of the units a projectile can still hit its intended target despite being inaccurate.

## Mechanics before and after "Ballistics"

[Source](https://www.youtube.com/watch?v=1AXTuKJ1dLg)

100 % accuracy in the game's sense must not be confused with hitting the target 100 % of the time.

Before researching Ballistics, units fire at the current positions of a target. If the target moves and is fast enough, it will not be hit by the projectile. This has the effect that even though the game determines a shot to be accurate, it doeesn't mean the target has to be hit.

After the player researched Ballistics, units tend to fire at the position where the target is supposed to be when the arrow lands. The targeted unit can still dodge the shot if it moves into another direction after the archer fires.
Ballistics does affect bombard towers but not other gunpowder or siege units.

## Stray Arrows and Scorpions

[Source2](https://www.youtube.com/watch?v=DZQn4-aAjGU)

If a ranged unit misses its target but the arrow happens to hit a different unit, this unit only takes **half** of the damage.

This behaviour also applies to scorpions and other units, which have arrows passing through units. Scorpions will only damage their targeted unit with full damage, even if the arrows move through other units first.
[Source](https://www.youtube.com/watch?v=o4Z0BTmzRgg)

## Target Size

[Source3](https://www.youtube.com/watch?v=33MEpQKCTes)

The chance to hit also depends on the unit size. Bigger units seem to have a larger area to hit which in consequence counters the effect of the *accuracy error radius*.

## Friendly Fire

Most of the ranged units do not injure friendly units with their projectiles. The Onager is an exception because its "area of effect" attack hits all units regardless of their owner. However, other units with "area of effect"-damage such as petards and demolition ships do not injure friendlies.

## Weird AoE2 Quirks

When a ranged unit dies but the projectile is still in the air, modifiers for damage and effects might change. Some of the effects include:

* When a trebuchet is packed before it has hit its target, the damage of the projectile will change to 1. If the trebuchet is destroyed before the projectile hits, it does no damage.
* If a scorpion is destroyed, the bolt can suddenly destroy trees on its path.
* The bonus for standing on a cliff or hill (+25 % damage) is removed after the units dies and all arrows that are still in the air will do normal damage.
* Portugese caravels fire in random directions, if attacking a melee unit at range 0. This seems to be a bug related to calculations concerning the *accuracy error radius*.

[Source](https://www.youtube.com/watch?v=O6fD4AC5sFQ)

Other things related to accuracy:

* Trebuchets also have a hard coded 80 % accuracy against buildings [Source](https://youtu.be/nh3GnmFSkeY?t=5m46s)
* British trebuchets after researching "Warwolf" have a 100 % accuracy against every unit except 1x1 tile buildings [Source](https://youtu.be/nh3GnmFSkeY?t=11m44s)
