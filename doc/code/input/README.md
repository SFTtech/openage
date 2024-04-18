# Input Management

Input managment concerns the handling external input sources that are used to interact with the engine.
The most common example for such input sources are probably **keyboard and mouse**, but other things like the **GUI**,
**networking** and **scripting** also fall under this umbrella.

1. [Motivation](#motivation)
2. [Basic Architecture](#basic-architecture)
3. [Window System](#window-system)
   1. [Workflow](#workflow)
   2. [Class Relationships](#class-relationships)
   3. [Low-level Interface](#low-level-interface)
   4. [High-Level Interface (Controller)](#high-level-interface-controller)
4. [Scripting](#scripting)
5. [Network](#network)
6. [AI](#ai)


## Motivation

Input handling in real-time strategy games is inherently more complex than in other games for a
multitude of reasons. Players do not have control over just one but many entities, which can also have
a variety abilities and possible levels of control. Depending on the selected entity there may be
different actions available to the player. Keybindings may also switch depending on the current usage
context, so a key press can have different consequences depending on when and where it is pressed.
Besides that, not all inputs have a direct effect on the game world such as camera movement or
information displayed in the UI.

All this means that inputs cannot be translated one-to-one to gameplay events like in other games.
Instead, we require a system that provides context management, multiple input sources
and targeted control, while still being as flexible and configurable as possible when exposing
the built-in features of the engine.


## Basic Architecture

The basic architecture workflow is roughly the same for every type of external input source.
There is a low-level and a high-level interface.

Low-level interfaces do some basic pre-processing, sorting, context management and delegation
for raw input events received from an input source (e.g. window system, network socket, scripting,
etc.). Relevant information from the raw input is stripped and converted into a more generalized
input format for internal usage in the engine. The interface also may discard unnecessary inputs,
e.g. duplicate network packets or unbound key presses, at this point. Accepted inputs are then
sent to an associated high-level component for further processing.

High-level interfaces interact with specific engine components by executing the actions associated
with an input event. In other words: This is where the magic happens and the inputs have an actual
effect on the engine (or more specifically, the game simulation). Actions are looked up via
mappings of input event to action which are usually configurable, e.g. as hotkey bindings for inputs
from the window system.


## Window System

Input management for the window system handles all input events received via Qt, i.e. everything
from input devices such as

- keyboard
- mouse
- GUI
- ... (anything else that Qt can detect as an input device)


### Workflow

![Workflow](images/workflow_input_controller.svg)


### Class Relationships

![Class Relationships](images/class_relationships_input_controller.svg)


### Low-level Interface

The central component of the low-level interface is the `InputManager` which is managed by
the engine's presenter. It processes all raw input events (from `QEvent` objects) received from Qt.
Keyboard and mouse events are captured every frame and are forwarded directly to the input
manager via callbacks setup by the presenter.

GUI events are a special case as the GUI exists separately from the application window and
does not automatically receive the same keyboard/mouse inputs. Therefore, the input manager
may forward keyboard/mouse inputs to the GUI, check if they are accepted, and then process
the resulting GUI event.

In any case, the resulting `QEvent` representing the raw input is converted to a generalized
`input::Event` object containing the following information:
- **event class**: Basic event categorization, usually type of input device (e.g. keyboard, mouse, GUI)
- **code**: Unique identifier of the specific key/button that was pressed
- **modifiers**: Keyboard modifiers pressed alongside the key/button (e.g. CTRL, SHIFT, ALT)
- **state**: State of the button/key (e.g. pressed, released, double click)
- **raw event**: Reference to the original `QEvent`

The unique combination of *class*, *code*, *modifiers*, and *state* represents a specific key press
and provides the **signature** of an input event. (Key) bindings are created by mapping signatures
to actions, which are executed when the signature is encountered in a received input event. Other than
by signature, actions can also be mapped by event class to allow for catch-all scenarios where all
keys of a class are handled in the same way.

Bindings for the input manager are stored in `InputContext` objects. An input context represent a scope of
bindings which are accepted when the context is active. Contexts can become active through actions from
a binding or other events. An example for such a context is the villager build menu in AoE2 which activates
when a villager is selected. Different input contexts can bind the same event signature or class which
makes multiple key assignments possible. Additionally, the input manager always has a global input
context used as fallback if no other contexts are active.

The input manager allows multiple input contexts to be active at the same time. Contexts are managed
on a stack with the most recently added input context on top. When the input manager processes a new
input event, it checks all active contexts on the stack for bindings in the following order:

1. check top context
2. check other contexts on stack by traversing downwards
3. check global context

If no binding is found, nothing is done and the input manager waits for the next raw input input.

If a binding exists for the input event, the input manager retrieves the mapped `input_action` struct
signifying the action executed for the event. It should be stressed that, as a low-level interface,
the actions taken by the input manager mainly consists of forwarding event data to the correct
high-level interfaces. Therefore, these actions should not have any effect on the game simulation.

`input_action` contains the following information:
- **action type**: One of the pre-defined types (see below). Used for determining a default action.
- **custom action function**: Executed instead of the default action if set (optional).
- **execution flags**: Key-value pairs for configuration settings (optional).

Most types have a default action that is executed unless a custom function is defined in `input_action`.
These default actions are:
- **push context**: push a context on top of the stack
- **pop context**: remove the current top context
- **remove context**: remove a context from the stack
- **Controller**: forward event arguments in direction of gamestate (i.e. to high-level interface)
- **GUI**: forward event arguments to the GUI

In most cases, it should be sufficient to bind one of these options to an input event. Custom
functions should only be used for edge cases which cannot be handled otherwise.

For forwarding actions, additional arguments may be passed to the high-level interface. In the
case of the controller, the input manager passes the `event_arguments` struct which contains:
- **input event** (i.e. the generalized `input::Event`)
- **mouse position**
- **mouse motion** (e.g. for calculating mouse movement direction)
- **flags**: Key-value pairs for configuration settings (optional)


### High-Level Interface (Controller)

The high-level interface is implemented by the `Controller` class. Controllers provide a
connection between input management and gamestate. One controller represents the control scope
of a player outside (or faction inside) the game. Thus, they also have significantly more awareness
of what's going on inside the gamestate than the low-level input manager.

A controller receives input event data (as an `event_arguments` struct) as well as a `BindingContext`
from the input manager. It uses the provided information to create a game event that can then be attached
to the gamestate event loop. To do this, the controller looks up a `binding_action` struct from
in binding context using the input event signature or class (similiar to how its works in the low level
interface).

`binding_action` contains the following information:
- **transform function**: Transformation from event arguments to game event.
- **queue type**: Determines whether the created event is queued or passed to the gamestate immediately .
- **execution flags**: Key-value pairs for configuration settings (optional).

Game events can be queued before they are forwarded to the gamestate to allow chained commands,
e.g. setting a bunch of waypoints before giving the final move command.


## Scripting

TODO


## Network

TODO


## AI

TODO
