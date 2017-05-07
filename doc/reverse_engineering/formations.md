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

A formation always consists of 4 subformations (which can be empty). What unit type belongs to what subformation is hardcoded into the game. A quick overview is shown below. The `^`-symbol shows the front of the formation.

```
...................
...^^^^^^^.........
1..Cavalry.........
2..Infantry (melee)
3..Ranged..........
4..Siege/Support...
...................
```

Every military unit in the game is sorted into one of these subformations depending on their `GroupID` (in some cases their `LineID`). Bear in mind, that the descriptions next to the subformations in the above figure are not fully accurate. For example, *cavalry archers* are part of the "Ranged"-subformation instead of the "Cavalry"-subformation. To avoid confusion specific subformations will from now on be referenced by their index (e.g. `subformation[1]` equals "Cavalry"-subformation) The figure below shows to which subformation a `GroupID` belongs.

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

### Ship Formations

Ships use the same formation system and are sorted into a subformation by similar rules. Instead of using `GroupID`, `LineID` determines which subformation is chosen (probably because all battle ships have the same `GroupID`). The figure below shows the subformations to which each class of ships is passed.

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

Because the same formation system is used for land and sea units it is possible to mix them inside a formation. This behavior can be seen when one selects ships and land units and then sends them in shallow water.

## Subformations

Subformations are the parts of the formation where the ordering of units into lines takes place. How units are ordered depends on a variety of factors and is also determined by other subformations in the parent formation.

### Individual Subformation

For a start one has to look at the behavior of an individual subformation first. Let's assume we have a single subformation consisting of ranged units (`subformation[3]`).

```
............
...^^^^^^...
...RRRRRR...
...RRRRRR...
............
```

The number of lines (or `row_count`) in this subformation is always determined by the number of its units. Unit width and different unit types have no influence here. The `row_count` can be calculated by an algorithm ([see here](https://gist.github.com/Piruzzolo/e744c9da5ed18898fae85f29a560e90f)). After the value has been calculated the units will be sorted into the lines starting from the top left.

```
............
...^^^^^^...
..>RRRRRR...
...RRRRRR...
............
```

#### Mixing Unit Types

If a subformation is formed with units of more than one unit type, AoE2 tries to evenly distribute them. This is achieved by alternating between unit types when sorting them into a line. To determine the unit type, the `UnitID` value is used.

```
............
...^^^^^^...
..>ASASSS...
...SSSSSS...
............
```

In the above example, we have a subformation consisting of 2 archers `A` and 10 skirmishers `S`. Beginning from the top left the subformation alternates between archers and skirmishers until there are no more archers left. The skirmishers fill up the remaining space.

The same rules apply, if more than two unit types are used. We will now have a look at a subformation containing 5 archers `A`, 3 skirmishers `S`, 6 longbowman `L` and 1 throwing axeman `T`.

```
..............
...^^^^^^^^...
..>LASTLASL...
...LASLALA....
..............
```

Which unit type is sorted in first depends on the order in which the player selected the unit types (or how they are ordered inside the selection queue). In the first example of this section, the player selected the archers first and added the skirmishers to his selction. The second and above example would be the result of selecting longbowman first, archers second, skirmishers third and throwing axeman last.

#### Distance Between Units

How far units in a subformation stay apart is determined by the width of the widest units and the length of the longest unit. All units inside the subformation will follow this rule, regardless of their own height or length. As an example, one can select a few archers and a scorpion. Scorpions are one of the widest units in `subformation[3]` and will therefore determine the distance between all of the units. Furthermore, one will notice that all rows of the subformation are affected and not solely the line the scorpion is in.

### Influence of other Subformations

So far we have only examined the grouping of individual subformation but left out the behavior of the parent formation. The parent formation knows about the variables of each subformation, e.g. `row_count` and `distance_between_units`. We will now see, how the parent formation can influence the `row_count` of other subformations.

Let's view two subformations, one with swordsman and one with rams, independently.

```
.............
...SSSSSSS...
...SSSSSSS...
.............
```
```
............
...RRRRRR...
............
```

We would expect that the grouping of units inside the parent formation looks exactly the same, with the two subformation just joined together. However, this is not the case. Instead, the parent formation will look like this:

```
....................
...SSSSSSSSSSSSSS...
.......RRRRRR.......
....................

```

The reason for the change of the `row_count` for `subformation[2]` is the width of `subformation[4]`. Rams are about two times as wide as swordsman, hence the subformation they are in will also be much wider than `subformation[2]` if it is selected individually. To deal with the different in width, the number of units per row of `subformation[2]` is extended. In general one can say that the widest subformation in the parent formation influences the `row_count` of all other subformations.

## Formation Types

### Line Formation

This is the standard formation whenever the player makes a selection. Units form lines (see all examples in **Subformations**) and are arranged with the weaker subformations in the back and the stronger ones in the front.

### Staggered Formation

Behaves like the Line Formation except that the space between units is doubled for every subformation.

### Boxed Formation

Units form the outline of a square. The weaker units are in the center while stronger units are placed on the outside. When representing a subformation with its corresponding number, it will look like this:

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

In comparison to the other formation types, the ordering of units with the boxed formation type in the subformations seems to be random and does not follow any specific rule. It is assumed that the units are grouped anew every time a movement order is placed.

This formation type is also rarely used in competitive games and presumably disfavored by players.

### Flanked Formation

This type of formation leaves a big vertical gap between the rows of a line formation and effectively splits it in half. Otherwise it behaves exactly like a Line Formation regarding ordering of units.

Flanked formations play a crucial part in competitive play, when countering the attack of onagers against dense infantry.

### Marching formation

The marching formation cannot be select by the player but is used by the game when a movement order is placed more than 10 tiles away from the current location of the player's selection. It behaves like a Line Formation that is turned around by 90 degrees. Because this formation type is much narrower than all other types, units can fit through small gaps in tree lines and walls.

## Algorithmic solutions

### Sort units into subformations (Pseudocode)

### Ordering units inside subformation (Pseudocode)
