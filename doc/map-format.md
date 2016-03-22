# Map Format

1.  [What is this?](#what-is-this)
2.  [Version History](#version-history)
3.  [Random Map Generator](#random-map-generator)
4.  [Advantages](#advantages)
5.  [What is a Trigger?](#what-is-a-trigger)
6.  [Example savefile](#example-savefile)
7.  [Specification](#specification)
8.  [Actions implemeted](#actions-implemeted)
9.  [Action Ideas](#action-ideas)
10. [Conditions implemeted](#conditions-implemeted)
11. [Condition ideas](#condition-ideas)
12. [for devs](#for-devs)

## What is this?

This document descrcibes the file format for save files of openage.

## Version History

| Version | changes |
|---------|---------|
| 0.2     | new format -> json <br>trigger support |
| 0.1     | initial custom format |

The game-engine should get a very simple file for map generation. Everything more complex should be done bevorehand(rms in python).
It should only contain:
- the worldmap as tiles (is simple)
- players, their start resources and civs(technologies)
- list of object and where to place them
- list of triggers(in a nutshell: if this then that)
- low level api for game-engine, everything should be explicit

Since this should be read and written by machines, I choose a simple a simple exchange Format: json

## Random Map Generator

rms are "only" a step prior in the pipeline, rms gets a seed value and generates a scenario

rms-map -> rms-generator -> map-scenario(this format) -> game-engine

## Advantages

- new multiplayer-modes are as simple as defining the winning/loosing triggers
- mappers can do what they want: tower defense, regicide, ..
- rms can have triggers

## What is a Trigger?

A Trigger is a set of conditions and actions with a boolean function.
If the conditions are met, the actions will be executed.
The conditions are evaluated:

|gate | description |
|-----|-------------|
|or   | one of the conditions must be true  |
|and  | all conditions must be true         |
|xor  | exactly one conditions must be true |


## Example savefile

The doc/default_save.oas is an example savefile with a trigger:

conditions: wood >=10,wood <= 100, time-loop 1s

actions: add 5 gold

## Specification

```javascript
{"version" : "v0.2"
 "players"  : [
    {
        "player-id"    : 1,
        "food"         : 1000,
        "wood"         : 1000,
        "gold"         : 100,
        "stone"        : 100,
        "civilisation" : "celts",
        "max-units"    : 200,
         # technologies: dark-age,castle-age,wheel borrow.., but also special tech: spanish trade +33%
        "technologies" : [list-of-available-technologies]    # start and end age can be regulated here
        "researched"   : [list-of-researched-technolgies]    # forbid some technoligies
        "units"        : [list-of-available-units]
    }
 ],
 "triggers":[
      {  "id"        : 1,
         "active"    : true,
         "gate"      : "or",
         "conditions":[
             {  "type"     : "min-resources",
                "player"   : 1,
                "value"    : 10.0,
                "resource" :"wood"
             }
         ],
         "actions":[
            {  "type"    : "add-resource",
               "resource": "wood",
               "amount"  : 5,
               "player"  : 1
            }
         ]
      }
 ],
 "terrain":{
       "chunk-size" : 16,
       "chunks":[
          {
            "position-ne":1,
            "position-se":1,
            "tile-count":256,
            "tiles":[{"size":0,"terrain-id":10},{"size":0,"terrain-id":10}....],
           },
           {
            "position-ne":-1,
            "position-se":1,
            "tile-count":256,
            "tiles":[{"size":0,"terrain-id":10},{"size":0,"terrain-id":10}....],
           },
       ]
 },
 "units":[
      {  "isbuilding":false,
         "player":0,
         "position-ne":7,
         "position-se":20,
         "properties":{},
         "type":349
       },
       { "isbuilding":false,
         "player":0,
         "position-ne":3,
         "position-se":22,
         "properties":{},
         "type":349
       },
       { "isbuilding":true,
         "player":1,
         "position-ne":19,
         "position-se":-1,
         "properties": {
                 "completion":1
         },
         "type":109
       }
}
```

## Actions implemeted

| name         |fields                                                                                         | description                       |
| -------------|-----------------------------------------------------------------------------------------------| ----------------------------------|
| add-resouce  |type(string): add-gold <br>amount(float): 1.0<br>player(uint): 1 <br>resource(enum):wood       | adds amount of resource to player |

## Action Ideas

| name         |fields                                                                  | description                       |
| -------------|------------------------------------------------------------------------| ----------------------------------|
| player-defeat(player-id)             # special for game
| game-end()                           # game ends, remaining players are victorious
| set-property    (id,property,value)  # set property (example: change max health new hero)
| set-property-add(id,property,value)  # add value to property (example: healing unit)
|                                        # sub value from property (example: contaminated area)
| convert-unit    (id,newtype)         # example: convert unit to hero
| set-property-all    (player-id,type,property,value)  # set property for all units of a type
| set-property-all-add(player-id,type,property,value)
| convert-unit-all    (player-id,type,newtype)
| delete-trigger(id)                   # for performance reasons, nothing happens if id is invalid (double free)
| activate-trigger(id)
| deactivate-trigger(id)
| unlock-building(player-id,type)
| lock-building(player-id,type)       # if a modder wants a player to choose between buildings
| unlock-technology(player-id,type)
| lock-technology(player-id,type)     # if a modder wants a player to choose between technologies
| unlock-unit(player-id,type)
| lock-unit(player-id,type)           # if a modder wants a player to choose between units
| send-text(player-id,text)
| create-object(player-id,x,y,type)       # units and buildings
| create-world-object(x,y,type)           # gold, wood, neutral buildings,...
| destroy-object(id)                      # destroys object
| set-signal(player-id,x,y)
| set-circle(player-id,x,y)               # worldwonder circle, can be used to mark positions
| remove-fog-of-war(player-id,x,y)        # removes the fog of war for player in tile x,y
| change-diplomacy(player-id,player-id,status)
| play-sound(player-id,soundname)
| change-owner(id,player-id)              # change object owner to player-id
| create-trigger( {trigger} ) | | #dynamic trigger|

## Conditions implemeted

| name         |fields                                                                  | description                                                                           |
| -------------|------------------------------------------------------------------------| --------------------------------------------------------------------------------------|
| every-tick   | type(string): every-tick                                               | is always true                                                                        |
| max-resources| type(string): max-resources <br> player(uint):1 <br> value(float):100.0 <br> resource:food,wood,stone,gold| is true, if player has at maximum of that resource |
| min-resources| type(string): min-resources <br> player(uint):1 <br> value(float):100.0 <br> resource:food,wood,stone,gold| is true, if player has at least of that resource   |
| timer-loop   | type(string): timer-loop <br> value(uint):1000                                                            | is true, every x milliseconds since trigger creation|

## Condition ideas

| name         |fields                                                                  | description                                                                           |
| -------------|------------------------------------------------------------------------| --------------------------------------------------------------------------------------|
| world-wonder-complete(player-id)
| world-wonder-buildstart(player-id)
| world-wonder-destroyed(player-id)
| player-points(player-id,value)           || has at least x points
| team-points(player-id,value)
| owns-objects(player-id,type,value)       || player has x objects of type
| object-destroyd(id)                      || buildings and units
| objects-min(player-id,type,value)        || player-id has at least(=>) x objects of type type
| objects-max(player-id,type,value)        || player-id has at max(<=) x objects of type type
| unit-in-tile(player-id,x,y)              || circle,rectangle etc is higher order => can be translated
| technology-research-start(player-id,technology)
| technology-research-finished(player-id,technology)
| object-visible(object-id,player-id)
| object-invisble(object-id,player-id)
| object-captured(object-id,player-id)    || example: relics, converted-units
| defeated(player-id)                     || player-id is defeated

## for devs

triggers.cpp  is singleton and contains a list of all triggers

trigger.cpp   is a trigger which contains a list of conditions and actions

condition.cpp is virtual, check(..) must be implemented and returns if state is true|false

action.cpp    is virtual, execute() changes the gamestate

howto: new action

create hpp,cpp files in actions directory

gamestate_save.cpp implement json load function analog to the others which are already there, save function are not implemented yet

howto: new condition

analog action
