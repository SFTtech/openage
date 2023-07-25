# Game Entity

## Architecture

![game entity UML]()

- major components
  - game entity class
  - components
  - systems
  - manager

- separation of concerns
  - components = only data storage
  - systems = only game logic
  - manager/entity = handles to access data/initiate game logic

## Game Entity class

- content
  - entity ID
  - map of components
  - manager for event handling
  - render entity if should be animated (optional)

- game entity should capture every use case for world object
  - composition over inheritance
  - game entity attributes/capabilities defined by components

## Component Data Storage

- dual role
  - signify what game entity can be or do by component type (e.g. move)
  - store data for said attribute
  - provide common helper functions for interacting with data
  - atomic: components never directly depend on another component to be assigned to the entity

- differentiate between prsistent game data and runtime data
  - Persistent game data
    - unit stats as defined in the game/modpack files, e.g. movement speed
      - usually does not change often during gameplay and changes are predictable
    - managed as objects in nyan db
      - openage nyan API object engine.ability.Ability
    - component holds reference to nyan object ability
    - data must always be fetched from nyan
    - not all components manage persitant game data
      - e.g. Position
  - runtime data
    - data that changes due to gameplay interactions, e.g. current health
      - unpredictable
    - managed inside the component on curves
      - data structure that tracks values over time
    - manipulated by accessing the curve and adding/removing keyframes
      - or by helper method


## Control Flow

- doing stuff with the game entity components
- not implemented into game entity class / component
  - separation of concerns
  - data classes only store data
  - control flow is sperate and can be adjusted for different purposes

- major core parts
  - system
  - activity
  - manager

### System

- function that implements game logic
  - implemented as static functions of a class with only static methods
  - i.e. do not belong to a particular object
  - manipulate the components data of one or more entities

- atomic (like components): focus on one particular action
  - don't call other systems (unless its a subroutine)
  - no hardcoded transitions from one system to the next, e.g. hardcode attack after move
    - managed by activities instead

- ECS vs. event-based gamestate
  - in ECS systems are called every tick
  - event-based systems are called on events, so they only run when they need to
  - heavy use of prediction due to curves logic
    - only calculate the necessary keyframes
    - e.g. constructing buildings only needs 2 keyframes for start and finish

### Activities

- connect systems together in a node graph to form a control flow for game entity
  - models complex behaviour and action chains
  - usually, game entity's of the same type use the same node graph
    - e.g. all spearman have the same general behaviour
    - but can be changed via components

- workflow
  - nodes in graph denote what systems should be called for the game entity
  - or decide where to go next based on conditions
  - game entity's "action state" defined by the current node in the graph
  - activity system follows along the nodes in the graph to execute game logic on an entity
  - register callback events

- action state stored in ActivityComponent
  - stores reference to full activity graph used by game entity
  - current node in graph
  - events that are currently waited for
  - resume execution from current node when callback is received

- see more activities.md

### Manager

- listens for events for the game entity
- takes appropriate action when called back by events
  - re-enter activity system
