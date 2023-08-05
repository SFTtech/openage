# Game Simulation

*Game simulation* is the engine subsystem that implements game mechanics and data structures
used for gameplay.

The subsystem only contains components that are strictly necessary to simulate a game. Everything else,
e.g. the [input system](/doc/code/input/), [rendering](/doc/code/renderer/),
networking or scripting, are handled as separate subsystems.

1. [Architecture](#architecture)
2. [Workflow](#workflow)


## Architecture

![Game simulation overview UML](images/simulation_uml.svg)

The game simulation is instantiated from the main thread via a `GameSimulation` object.
This object controls the game loop, the current game session, and other simulation
parameters necessary to run the game.

The `Game` class represents a game session. Its instances store information about the
gamestate via a `GameState` object which includes references to objects inside the game world
(e.g. players or game entities). It also contains gameplay settings for the current session.

"Physical" objects in the game world are represented by `Terrain` and `GameEntity`. `Terrain`
is used to model the underlying map terrain, while [`GameEntity`](game_entity.md)
is used for every type of world object (e.g. units, buildings, trees, resources, ambience).


## Workflow

To initiate a game, a `GameSimulation` object must be created. This is usually done by
the `Engine` object in the main thread. On initialization, the game simulation automatically sets up
 several subcomponents required for running the game such as:

* [Event loop](/doc/code/event_system.md) for executing events
* Entity factory for creating game entities
* Mod manager for detecting/loading [openage modpacks](/doc/media/openage/modpacks.md)

After the `GameSimulation` object is initialized, modpacks that should be loaded in
the game session can be passed via the `set_modpacks(..)` method.

The game simulation loop is started using the `run()` method of the `GameSimulation` object.
`run()` should be run in its own thread with no other looping subsystems present. Before
the simulation loop is entered, the simulation sets up a new game session (as a `Game`
object). This loads the modpacks specified in the `set_modpacks(..)` method into the
session.

The logic of the simulation loop is quite simple as most of the execution happens in the
[event system](/doc/code/event_system.md). In every loop iteration, the current simulation time is fetched from the
[time subsystem](/doc/code/time.md). This time value is then passed to the simulation's
event loop which executes all events queued until this time.
