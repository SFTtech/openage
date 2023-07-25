# Game Simulation

- Components for simulating gameplay


1. [Architecture](#architecture)



## Architecture

- Hierarchy
    - game simulation
        - game
            - universe
                - terrain
                - world
                    - game entity
                        - activity manager
                        - components
                        - systems


- GameSimulation
    - initialized by engine
    - controls overarching control parameters
        - event loop
        - time loop
        - factory for creating game entity
        - mod manager
        - game
    - runs the main event loop
        - advances game by executing events for time t from time loop

- Game
    - manages current game session
    - store information about the running game
        - settings
        - game data (from nyan database)
        - current game state
            - players
            - game entities
            - other objects
        - universe

- universe
    - manages "physical" objects inside the game
        - terrain
        - game entities (via World reference)
    - may be integrated into gamestate class in later versions

- world
    - manages game entities inside the game
    - may be integrated into gamestate class in later versions

- terrain
    - manages the terrain in the game world
    - may be integrated into gamestate class in later versions

- game entity
    - object inside the game world (e.g. unit/building/tree/ambience)
    - capabilties defined as components
        - game data from nyan db
        - runtime data
    - controlled by events handled by manager class
    - altered/controlled by systems
