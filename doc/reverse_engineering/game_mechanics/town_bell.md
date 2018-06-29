# Town Bell

[Source](https://www.youtube.com/watch?v=pS4Xy3vVY6U)

## Range

The town bell has a range of 25 tiles with the measuring point being the middle of a town center. Therefore the effective range is 23 tiles from the edges of the building. Villagers will always move to the *closest* town center or tower/castle they can find.

If the town bell is rang a second time, all villagers within the 25 tile range will get back to work.

## Garrison limit

Villagers respect the garrison limit of nearby buildings. The town center for example has a garrison limit of 15 units. As a consequence, only the 15 villagers that are closest to the town center will be affected by the town bell. Everyone else will continue on their tasks. The same principle applies to garrisoning in towers and castles.

## Where will villagers garrison?

In the following figure the town bell range is represented by the dotted area. `C` is the town center, every `X` is a villager and `G` represents possible garrisons, e.g. towers or castles. The `O` shows a tile that is outside of the bell's range. The left and top row are coordinates for referencing, e.g. `coord(H,D)` is referencing the location of the town center.

```
__|ABCDEFGHIJKLMNO
------------------
A |OOOOOOOOOXOOOOO
B |OO...G.......OO
C |OO.......X...OO
D |XO....XC.....OO
E |OO...........OO
F |OO.X........XOO
G |OOOOOOOOOOOOGOO
```
Villagers within the 50x50 area will seek to garrison. The ones at `coord(A,D)` and `coord(J,A)` are outside of the range and will therefore not be affected.

Since villagers will choose the nearest garrison they can find, we would expect the unit at `coord(M,F)` to move to the building at `coord(M,G)`. However, this doesn't happen. Because `coord(M,G)` is not in range of the town bell, villagers will not choose it as a destination and instead move to the town center. This makes sense because the tower would not be notified to ungarrison the villagers when the bell rings for the second time.

Now let's pretend the the town center is already full, so that `coord(M,F)` can't garrison there either. Will the villager move to the tower at `coord(F,B)` if it still has not reached its garrison limit? No. The tower has to be within 25 tiles *from the villager's position*.

Villagers will therefore choose a garrison that is

* they are in the range of the town bell
* the garrison is within the 25 tile *range of the townbell*
* the garrison is within a 25 tile *range of the villager*
* the chosen garrison has not reached its garrison limit

Buildings will prefer villagers that are closest to them for garrisoning.

## Possible implementation

As soon as the town bell is triggered, the algorithm searches for villagers that are inside the 50x50 tile range of the specific town center. It would probably be more efficient to search through the list of villagers belonging to the player and matching their coordinates than scanning all 2500 tiles in range of the bell. All contemplable villagers are then put into a list. In the same way, the algorithm searches for towers, castles and town centers (and other possible garrisons) in the area and puts them into a second list. This can be done in `O(n+m)` time (n is the number of villagers, m the number of garrisons).

In the next step, the algorithm tries to find the nearest garrison for a villager by calculating the distances between the villager and all buildings on the second list. The closest distance calculated and the corresponding garrison are then saved. Afterwards the villager is put into a third list, which is sorted by the *closest distance to any garrison*. This step is repeated for every villager.

Last but not least, the villagers are assigned to a garrison. Since the third list is sorted by closest distance, villagers that are the nearest to a building are guaranteed to get a place, which satisfies the condition that towers prefer the villagers close to them. If the villager is assigned to a building which is already full, the algorithm recalculates the distances to other garrisons and sorts him back into the list.
