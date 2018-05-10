# Pathfinding

[Source](http://www.gamasutra.com/view/feature/131844/postmortem_ensemble_studios_age_.php?page=2)

[Source](https://web.archive.org/web/20081221121604/http://www.gamasutra.com/features/gdcarchive/2000/pottinger.doc)

and *Ensemble Studios Official Strategies & Secrets to Microsoft's Age of Empires II: The Age of Kings*, Bruce Shelly, 1999, published by Sybex Inc.

## Algorithms

Age of Empires 2 uses three different algorithms for pathfinding plus two obstruction systems.

The first (high-level-)pathfinder is meant for long distances and uses mip-mapping to create a general route across the map. Criteria for the mip-map subdivision is *passibility*. It ignores moving objects, e.g. units or wildlife, though it takes "static" object like buildings into account. Whenever a building is constructed the pathfinder gets notified and updates the corresponding mip-map chains.

Further reading: [3D Pathfinding](http://www.in.tum.de/fileadmin/user_upload/Lehrstuehle/Lehrstuhl_XV/Teaching/SS06/Seminar/3dpf-doc-final.pdf) (p. 11)

The other two pathfinders are for short and medium distances. One uses tile-based pathfinding while the other one is a polygonal pathfinder that dynamically generates convex hulls. These are used in combat situations or when units cross paths.

## Examples

*(Please add more examples if you know any)*

```
...............      ...............      ...............      ...............
...............      ...............      ...............      ....XX.XXXXX...
......T........ ---> ......T........ ---> ....XXTXXXXX... ---> ....XXTXXXXX...
............... ---> ....XX.XXXXX... ---> ....XX.XXXXX... ---> ...............
....XXXXXXX....      ....XX.XXXXX...      ...............      ...............
....XXXXXXX....      ...............      ...............      ...............
```
Units will split up their formation if they encounter an obstruction such as a tree.

```
..............      ..............      ..............      ..............
..............      ..............      ..............      ....X....X....
....YYYYYY.... ---> ....YYYYYY.... ---> ....YYYYYY.... ---> ...XYYYYYYX...
.............. ---> ....XXXXXX.... ---> ...XXXXXXXX... ---> ...XXXXXXXX...
....XXXXXX....      ....XXXXXX....      ....XX..XX....      ..............
....XXXXXX....      ..............      ..............      ..............
```
When troops encounter an enemy formation they will try to go around to engage in combat. Though if units get killed and create a gap, the pathfinder has some difficulties to distribute units efficiently.
