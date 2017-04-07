# Formations

## Principles of Formations in AoE2

Dave Pottinger's Articles [1](http://www.gamasutra.com/view/feature/131720/coordinated_unit_movement.php) [2](http://www.gamasutra.com/view/feature/131721/implementing_coordinated_movement.php)

1. Grouped units have the same speed
2. Grouped units take the same path
3. Grouped units arrive at the same time

## Structure

When talking about formations, one has to differentiate between

1. The selection of the player
2. The 'actual' formation
3. The subformations of a formation

Players can select any units they own and add them to their selection. Therefore, a selection can consist of varying unit types and allows mixing of buildings, military units, villagers and others. Not all of these units are capable of forming a formation. Buildings are excluded from formations because they can't move and so are unpacked trebuchets. Villagers, trade carts and fishing ships will not be part of any formation. If they are moved, they don't abide Pottinger's rules and will move to the target at their normal speed while each of them takes their own path. Hence, when villagers are part of a selection of military units, the military units will form a formation and then move to the target while the villagers will independently move there.

A formation consists of 4 subformations. Which unit type belongs to which subformation is hardcoded nto the game. A quick overview is shown below. The `^`-symbol shows the front of the formation.

```
...................
...^^^^^^^.........
1..Cavalry.........
2..Infantry (melee)
3..Ranged..........
4..Siege/Support...
...................
```

Every military unit in the game is sorted into one of these subformations depending on their `GroupID` (in some cases their `LineID`). Bear in mind, that the descriptions next to the subformations in the above figure are not fully accurate. For example, *cavalry archers* are part of the "Ranged"-subformation instead of the "Cavalry"-subformation. The figure below shows to which subformation a `GroupID` belongs.

```
...................
...^^^^^^^.........
1..Cavalry......... 912, 947
2..Infantry (melee) 906
3..Ranged.......... 900, 923, 936, 944, 955
4..Siege/Support... 902, 913, 918, 919, 920, 935, 943, 951, 959, Saboteur (UnitID: 706)
...................
5..Else............ 904, 921, 958, 961 (not part of any formation)
```

As you can see, the only exception to sorting by `GroupID` is the saboteur. Ships are also sorted into a formation by their `LineID` (see the section about **Ship Formations**).

Formations have a formation type which determines how the units are generally ordered (see **Formation Types**). The subformations inherit their formation type from the parent formation.

## Subformations

#### Individual Subformation

* Row Count

#### Influence of Other Subformations

* Row Count
* Width of other formations

### Mixing Unit Types

* units are somewhat evenly distributed
* every row alternates between unit Types
* order depends on which units where selected first

### Distance Between Units

* is determined by the widest Unit
* applied to all units inside subformations

## Formation Types

### Line Formation

* units form lines (see Row Count)
* standard formation
* weaker units behind

### Staggered Formation

* like line formation but doubles the space between units

### Boxed Formation

* units form a box
* weakest units in the center
* subformations:

```
...............
.1.1.1.1.1.1.1.
.1.2.2.2.2.2.1.
.1.2.3.3.3.2.1.
.1.2.3.4.3.2.1.
.1.2.3.3.3.2.1.
.1.2.2.2.2.2.1.
.1.1.1.1.1.1.1.
.........
```

* ordering of units inside subformation is random

### Flanked Formation

* leaves one big vertical gap between the line formation rows
* otherwise behaves like line formation

### Walking formation

* is chosen if the move order is placed more than 10 tiles away.
* is like line formation turned 90 degrees

## Ship Formations

### Structure

```
WARNING: These are LineIDs not GroupIDs
...................
...^^^^^^^.........
1..Cavalry......... -294
2..Infantry (melee) -293
3..Ranged.......... -283, -284, -292
4..Support......... -285
...................
5..Else............ Admiral Yun Shi (UnitID: 844)
```

### Mixing Sea and Land Units

* is possible (can only be seen in shallow water)

## Algorithmic solutions

### Sort units into subformations (Pseudocode)

### Ordering units inside subformation (Pseudocode)
