# Built-in Systems

Overview of the built-in systems in the game simulation.

1. [Idle](#idle)
2. [Move](#move)


## Idle

![Idle systems class UML](images/system_idle.svg)

Handles idle actions for game entities.

`idle(..)` updates the animation of the game entity. This requires the game
entity to have the `Idle` component. The function returns a time of 0 since
no actionsconsuming simulation time are taken.


## Move

![Move systems class UML](images/system_move.svg)

Handles movement actions for game entities.

`move_default(..)` moves a game entity to the new position specified in the function
call. This requires the game entity to have the `Move` and `Turn` components.
Waypoints for the exact path are fetched from the pathfinder.
For every straight path between waypoints, the game entity is turned first, then
moved (same as in *Age of Empires*). If an animation is available for the `Move`
component, this animation is forwarded as the game entity's active animation to the
renderer. The function returns the cumulative time of all turn and movement actions
initiated by this function.

`move_command(..)` processes the payload from a move *command* to call `move_default(..)`
with the payload parameters.
