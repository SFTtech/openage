# Activity Control Flow

- Configurable control flow for game entities
- using a node graph

1. [Motivation](#motivation)
2. [Architecture](#architecture)
3. [Node Types](#node-types)
4. [Activity](#activity)


## Motivation

- Commands in RTS often result in multiple actions that have to be taken
  - command does not translate to single atomic action
  - action chains for most commands, e.g. attack command results in actions move + attack

- action chains can get complex
  - branching paths depending on conditions
  - waiting for events or triggers
  - parallel actions
  - canceling an action requires clean up

- control flow of commands and actions should not be hardcoded!

## Architecture

- control flow modeled as a directed node graph
  - following along the paths in the graph tells the game entity what to do
  - current node == current action
  - paths == connections to the next action
  - activity == (sub)graph with start and end node
    - defines a scope or reusable
![graph example]()

- inpired by BPMN flow graph
  - you don't need to know BPMN because we explain everything relevant in here
  - but you can use BPMN draw tools to make graphs

## Node Types

- Start
  - 0 inputs / 1 output
  - no action
  - signifies start of activity
- End
  - 1 input / 0 outputs
  - no action
  - signifies end of activity
- Task System
  - 1 input / 1 output
  - references built-in system that should be executed when visiting
  - signifies an action
- Task
  - 1 input / 1 output
  - references a custom function that should be exectuted when visiting
  - signifies an action
  - for scripting or debugging
- Exclusive Event Gateway
  - 1 input / 1+ outputs
  - register event callback(s) when visiting
  - choose 1 output when event callback s triggered
  - signifies a branch
- Exclusive Gateway
  - 1 input / 1+ outputs
  - choose 1 output depending on condition function
  - i comparison to event gateway, conditions are checked immediately
  - signifies a branch
- activity
  - 1 input / 1 output
  - start a subactivity
  - signifies a nested action


## Activity

- encapsulates a node graph with start and end
- defines a scope of interaction
- allows reuse of common control flows
  - e.g. movement
