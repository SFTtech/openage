# Built-in Components

Overview of the built-in game entity components in the game simulation.

1. [Internal](#internal)
2. [API](#api)


## Internal

- components used by every game entity
- no corresponding nyan API object

- Activity
  - reference to the top-level activity for the game entity
  - current node in activity
  - list of scheduled events the entity waits for to continue the control flow
- CommandQueue
  - stores commands from the input system for the game entity
  - commands are basically signals from the input system to handle conditional branches in the activity control flow
    - commands have type
    - payload depends on type
- Owner
  - stores ID of player who owns the unit
  - allow basic access control
- Position
  - 3D position of game entity in the world
  - angle of game entity relative to camera, clockwise rotation

## API

- components which are assigned depending on whether the game entity defnition has a matching nyan API object (engine.ability.Ability)

- Idle
  - nyan API: engine.ability.type.Idle
  - represents the ingame "idle" state of a unit, i.e. where it is not active
  - only used to access animations, sounds from nyan db
- Live
  - nyan API: engine.ability.type.Live
  - stores attributes of entity and their current values, e.g. health
- Move
  - nyan API: engine.ability.type.Move
  - allows the entity to move
  - required for issuing move commands
  - stores nothing
  - speed, animations, sounds from nyan db
- Turn
  - nyan API: engine.ability.type.Turn
  - allows the entity to turn, i.e. change its angle
  - stores nothing
  - speed, animations, sounds from nyan db
