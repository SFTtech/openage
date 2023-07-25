# Built-in Systems

Overview of the built-in systems in the game simulation.

1. [Move](#move)
2. [Idle](#idle)

## Move

- handles movement actions for game entities
- move_command
  - move the entity using a move command
  - uses movement parameters from command to call move_default
- move default
  - move the game entity to a new position
  - requires Move and Turn components
  - workflow
    1. Turn
       1. calculate new angle in move direction
       2. calculate turn time from new angle and turn speed
       3. add keyframe for new angle at t + turn time
    2. Move
       1. calculate move time from path distance and move speed
       2. add keyframe for old position at t and for new position at t + turn time + move time
    3. Update animations if animated
  - returns duration of movement (t + turn time + move time)

## Idle

- handles idle action for game entities
- since idle means doing nothing, this only updates animations and sounds
- idle
  - requires Idle component
  - workflow
    1. Update animations if animated
  - returns duration of action (always 0)
