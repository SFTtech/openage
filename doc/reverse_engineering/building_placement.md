Building placement constraints
==============================

Todo: everything

Building | Constraint
---------|-----------
Docks    | shallow water at the middle block and at least on ground block

Range to keep building
======================

When a builder is tasked with building a building and that is completed, it
will try to build another building. This not only applies to villagers present
when the building is completed but also to villagers who only arrive after the
building is completed.

It appears to choose the closest building, where the distance is not rounded up
to full tiles. When choosing whether or not a building is in range however,
only full tile distances are taken into account.

The range varies depending on the building size. They are however not just
simple circles generated with Bresenham or Midpoint. In the following ASCII art
pieces, each character represents one tile, where `o` is the tile the villager
is in, `.` tiles are in range and `X` tiles are out of range. Only one tile of
the building has to be in range, not all.

For {1x1,3x3,5x5}-tile-buildings (all with odd size, 5x5 is wonder and largest
building possible):

```
XXXXXXX.....XXXXXXX
XXXX...........XXXX
XXX.............XXX
XX...............XX
X.................X
X.................X
X.................X
...................
...................
.........o.........
...................
...................
X.................X
X.................X
X.................X
XX...............XX
XXX.............XXX
XXXX...........XXXX
XXXXXXX.....XXXXXXX
```

For {2x2,4x4(?)}-tile-buildings (all with even size):

```
XXXXXXXXXXXXXXXXXXX
XXXXXXX.....XXXXXXX
XXXXX.........XXXXX
XXX.............XXX
XXX.............XXX
XX...............XX
XX...............XX
X.................X
X.................X
X........o........X
X.................X
X.................X
XX...............XX
XX...............XX
XXX.............XXX
XXX.............XXX
XXXXX.........XXXXX
XXXXXXX.....XXXXXXX
XXXXXXXXXXXXXXXXXXX
```


