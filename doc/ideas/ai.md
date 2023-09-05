# Artificial Intelligence

## Interface

This is a very early draft with some ideas taken
from http://bwmirror.jurenka.sk/javadoc/bwapi/package-summary.html

This file shall provide information about the AI interface design.


### General ideas

 * An AI registers hooks for desired events
 * The C++ engine triggers python AI functions when desired events occur
 * The AI then can trigger actions on the controllable units
 * Periodically (1 Hz?), the AI gets ticked to allow event independent decisions
 * Get the AI its own dedicated thread


### Event types

Python interface basic events:

 * on_start(GameInfo info)
 * on_frame(Game game)
 * on_end(Player winner)

Some AIs may prefer an event driven approach (rule based AI):

 * on_unit_discover(Unit unit)
 * on_unit_complete(Unit unit)
 * on_unit_lost(Unit unit)
 * many more...


Some AIs will need to simulate ticks into the future to help
make decisions for the current tick. (min-max, MCTS, etc.)


### Relevant structures for the AI

Remember: These are just some ideas for possible interfaces,
they are not existent in the game yet.

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
	bool     can_build_at(Unit builder, TilePosition position, UnitType type);
	bool     can_create(Unit builder, UnitType type);
	bool     can_research(Unit unit, TechType type);
	bool     can_upgrade(Unit unit, UpgradeType type);
	Player[] enemies();     // all the enemy players that have not left or been defeated.
	Unit[]   get_all_units(); // returns all the visible units.

	// many more examples at: http://bwmirror.jurenka.sk/javadoc/bwapi/Game.html
}
```

```cpp
class Player {
	string  get_name();   // returns the name of the player.
	Faction get_faction();   // returns the faction of the player.
	Unit[]  get_units();  // returns the set of units the player own.
	bool    has_researched(TechType tech);
	int     get_resource()    // Returns the amount of resources the player owns.
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


## Gameplay

Improvements for the AI's gameplay

### Villagers

 - Don't collect resources in hazardous areas / send the military to clear them out
 - Send military task forces to guard strategic resources / tower them
 - Don't build in hazardous areas
 - Build more gates in walls to prevent lock-ins
 - Prefer to build castles and towers near the water/thin places
 - Create more trade cogs and trade ships

### Military

 - Avoid fighting in hazardous areas / prefer fighting in range of own castles
 - Coordinates reaction to attacks on units (during march, ...) (including attacks by towers/castles)
 - Disable scouting if the "all visible" option was selected.
 - Lame you (steal boar in the beginning on purpose)
 - Understand, how to do economic damage
   - e.g. if losing of all AI-units is inevitable, run and attack villagers to hurt economy
 - don't wait for hours to mass up army and then try to overrun you at minute 40
   - send smaller amount of troops earlier in the game to annoy you
   - while building up a big army in the back

### Ship

  - Transport ship can deliver villagers to another islands
