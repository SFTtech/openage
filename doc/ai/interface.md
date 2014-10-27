AI Interface
============

This is a very early draft with some ideas taken from http://bwmirror.jurenka.sk/javadoc/bwapi/package-summary.html

This file shall provide information about the AI interface design.


General ideas
-------------

 * An AI registers hooks for desired events
 * The C++ engine triggers python AI functions when desired events occur
 * The AI then can trigger actions on the controllable units


Event types
-----------

Python interface basic events:

 * on_start(GameInfo info)
 * on_frame(Game game)
 * on_end(Player winner)

Some AIs may prefer an event driven approach (rule based AI):
 * on_unit_discover(Unit unit)
 * on_unit_complete(Unit unit)
 * on_unit_lost(Unit unit)
 * many more...


Some AIs will need to simulate ticks into the future to help make decisions for the current tick. (min-max, MCTS, etc.)


Relevant structures for the AI
------------------------------

```cpp
    struct game_info {
        int num_players;
        int map_size;
        int pop_limit;
        resources_type;
        map_view;
        starting_age;
        victory_mode;
    }
```

```cpp
    class Game {
        Player[] allies();      // all the ally players that have not left or been defeated.
        bool     canBuildHere(Unit builder, TilePosition position, UnitType type);
        bool     canMake(Unit builder, UnitType type);
        bool     canResearch(Unit unit, TechType type);
        bool     canUpgrade(Unit unit, UpgradeType type);
        Player[] enemies();     // all the enemy players that have not left or been defeated.
        Unit[]   getAllUnits(); // returns all the visible units.

        // many more examples at: http://bwmirror.jurenka.sk/javadoc/bwapi/Game.html
    }
```

```cpp
    class Player {
        string getName();   // returns the name of the player.
        Race   getRace();   // returns the race of the player.
        Unit[] getUnits();  // returns the set of units the player own.
        bool   hasResearched(TechType tech);
        int    getWood()    // Returns the ammount of wood the player owns.
        // many more...
    }
```

```cpp
    class Unit {
        // used to get information about individual units as well as issue orders to units
    }
```

```cpp
    class Calculation {
        // used to get computation heavy calculations from the C++ engine
    }
```
