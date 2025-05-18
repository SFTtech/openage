# Activity Control Flow

The *activity control flow* is openage's method to make complex game entity behaviour
configurable.

1. [Motivation](#motivation)
2. [Architecture](#architecture)
3. [Workflow](#workflow)
   1. [Initialization](#initialization)
   2. [Advancing in the graph](#advancing-in-the-graph)
4. [Node Types](#node-types)


## Motivation

Unit behaviour in RTS games can get very complex. In many cases, units are not
just controlled by commands but other automated mechanisms, e.g. attacking enemy
units in the line of sight. Furthermore, commands do not always translate to a single
well-defined action. For example, an attack command usually results in a move action
and a subsequent attack action. Some commands may even execute different actions depending
on context.

All this means that we cannot view the control flow of a unit as a simple mapping of
command to action as is done in other games. Instead, we have to treat unit behaviour
as a complex chain of actions with branching paths, wait states for events, triggers
and feedback loops. Unless we want every command to be a hardcoded action chain, managing
this complexity is key to making unit behaviour configurable.


## Architecture

Game entity control flows in openage are modelled as directed node graph, so-called *activities*.
Nodes in the graph correspond to actions that execute for the game entity or conditional queries
and event triggers that indicate which path to take next. By traversing the node graph along
its paths, the game entities actions are determined. The currently visited node in the graph
corresponds to the current action of a unit.

Advancement to the next node can be initiated in several ways, depending on the
[node type](#node-types) of the current node.
It can happen automatically or be triggered by an event. In the latter case,
the event is handled by the `GameEntityManager` which calls an activity *system*
that processes the event to choose the next node.

Advancing in the graph, i.e. visiting nodes and performing actions costs no ingame time. Time
delays of actions, e.g. for using an game mechanic like movement, are instead handled by
scheduling and waiting for events at certain nodes in the graph (e.g. `XOR_EVENT_GATE` nodes).
This means that when running the activity system, the directed edges of the nodes are followed
until a node that waits for an event is reached. This allows the activity graph to support
complex action chains that can be executed in sequence.

Activities are reusable, i.e. they are intended to be shared by many game entities. Usually,
all game entities of the same type should share the same behaviour, so they get assigned
the same activity node graph.

An activity can also be represented visually like this:

![graph example](images/activity_graph.svg)

The design is heavily inpired by the [BPMN](https://en.wikipedia.org/wiki/Business_Process_Model_and_Notation)
representation. You don't need to know BPMN to understand the activity control flow because
we explain everything important about the graphs in our documentation. However,
you can use available [BPMN tools](https://bpmn.io/) to draw activity node graphs.

Like all game data, activities and node types for game entities are defined via the
[nyan API](doc/nyan/openage-lib.md).


## Workflow

![Activity Workflow](images/activity_workflow.png)

### Initialization

When a game entity is spawned, the engine first checks whether entity's `GameEntity` API object
has an ability `Activity` assigned. If that is the case, the activity graph is loaded from
the corresponding API objects defining the graph. Most of this step involves creates the
nodes and connections for the graph as well as mapping the API objects to node actions.

The loaded activity graph is stored in a `Activity` component that is assigned to the game
entity. At this point, the activity state of the entity is still uninitialized which allows
the entity or the component to be cached for faster assignment to entities using the same graph.
To let the entity become active, the `init(..)` method of the Activity component should be
called after the entity is completely initialized. This sets the activity state to the start
node of the actvity graph.

### Advancing in the graph

After the game entity is spawned, the `GameEntityManager` is called once to trigger the initial
behavior of the game entity. This advances the activity state until the first event branch where
an event is required for further advancement. The `GameEntityManager` now waits for events
for the entity to further advance in the graph.

A game entity's current activity state is stored in its `Activity` component in form of
a reference to the current node. Additionally, the components stores the list of events
the entity currently waits for to advance.

## Node Types

| Type              | Description               | Inputs | Outputs |
| ----------------- | ------------------------- | ------ | ------- |
| `START`           | Start of activity         | 0      | 1       |
| `END`             | End of activity           | 1      | 0       |
| `TASK_SYSTEM`     | Run built-in system       | 1      | 1       |
| `TASK_CUSTOM`     | Run custom function       | 1      | 1       |
| `XOR_EVENT_GATE`  | Wait for event and branch | 1      | 1+      |
| `XOR_GATE`        | Branch on condition       | 1      | 1+      |
| `XOR_SWITCH_GATE` | Branch on value           | 1      | 1+      |
