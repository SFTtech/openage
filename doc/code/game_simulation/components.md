# Built-in Components

Overview of the built-in game entity components in the game simulation.

1. [Internal](#internal)
   1. [Activity](#activity)
   2. [CommandQueue](#commandqueue)
   3. [Ownership](#ownership)
   4. [Position](#position)
2. [API](#api)
   1. [Idle](#idle)
   2. [Live](#live)
   3. [Move](#move)
   4. [Turn](#turn)


## Internal

Internal components do not have a corresponding nyan API object and thus only
store runtime data.

### Activity

![Activity Component UML](images/component_activity_uml.svg)

The `Activity` component stores a reference to the top-level activity for the
game entity. Essentially, this gives access to the entire activity node graph
used by the entity.

Additionally, the current activity state is stored on a discrete curve that
contains the last visited node.

`Activity` also stores the handles of events initiated by the activity system
for advancing to the next node. Once the next node is visited, these events
should be canceled via the `cancel_events(..)` method.


### CommandQueue

![CommandQueue Component UML](images/component_activity_uml.svg)

The `CommandQueue` component stores commands for the game entity in a [queue curve container](/doc/code/curves.md#queue).

Commands in the queue use `Command` class derivatives which specify a command type
and payload for the command.


### Ownership

![Ownership Component UML](images/component_ownership_uml.svg)

The `Ownership` component stores the ID of the player who owns the game entity.


### Position

![Position Component UML](images/component_position_uml.svg)

The `Position` component stores the location and direction of the game entity
inside the game world.

The 3D position of the game entity is stored on a continuous curve with value type
`phys3`.

Directions are stored as angles relative to the camera vector using clock-wise
rotation. Here are some example values for reference to see how that works in
practice:

| Angle (degrees) | Direction             |
| --------------- | --------------------- |
| 0               | look towards camera   |
| 90              | look left             |
| 180             | look away from camera |
| 270             | look right            |

Angles are stored on a segmented curve.

## API

API components have a corresponding nyan API object of type `engine.ability.Ability` defined
in the nyan API. This API object can be retrieved using the `get_ability(..)` method of the
component.

### Idle

![Idle Component UML](images/component_idle_uml.svg)

**nyan API object:** [`engine.ability.type.Idle`](/doc/nyan/api_reference/reference_ability.md#abilitytypeidle)

The `Idle` component represents the ingame "idle" state of the game entity, i.e. when
it is doing nothing.

The component stores no runtime data.


### Live

![Live Component UML](images/component_live_uml.svg)

**nyan API object:** [`engine.ability.type.Live`](/doc/nyan/api_reference/reference_ability.md#abilitytypelive)

The `Live` component represents the game entity's ability to have attributes (e.g. health).

An attribute's maximum limit is stored in the nyan API object, while
the game entity's current attribute values are stored in the component
on a discrete curve.


### Move

![Move Component UML](images/component_move_uml.svg)

**nyan API object:** [`engine.ability.type.Move`](/doc/nyan/api_reference/reference_ability.md#abilitytypemove)

The `Move` component represents the game entity's ability to move in the game world.
This also allows moving the game entity with move commands.

The component stores no runtime data.


### Turn

![Turn Component UML](images/component_turn_uml.svg)

**nyan API object:** [`engine.ability.type.Turn`](/doc/nyan/api_reference/reference_ability.md#abilitytypeturn)

The `Turn` component represents the game entity's ability to change directions in the game world.
Turning is implicitely required for moving but it also works on its own.

The component stores no runtime data.
