# README for map developers

## algorithm
- create startpoints of islands
- add random tiles to islands considering constraints (in parallel)
- add objects (trees,villagers) in order of appearance in the script
- create island connections

## reference
rms scripts are in ini style

[SECTION]

field1=value1

field2=value2

### METADATA
this is only used for displaying ingame, it has no influence over map generation

| field     | type            | info                |
| --------- | --------------- | ------------------- |
| author    | string          | openage-developers  |
| version   | string          | 0.1                 |
| name      | string          | arabia              |
| players   | string          | 2-8                 |
| teams     | string          | 2                   |
| gametypes | list of strings | deathmatch,conquest |

### MAP_SETUP

| field        | type         | default | info                                        |
| ------------ | ------------ | ------- | ------------------------------------------- |
| base_terrain | TERRAIN-TYPE | GRASS   |                                             |
| base_x       | int          | 7       | x base size + number of players = x mapsize |
| base_y       | int          | 7       | y base size + number of players = y mapsize |
| x_scaling    | enum         | sqrt    | one of: None,sqrt,linear, how does x scale with mapscale, sqrt: x_base * sqrt(mapscale), linear: x_base + mapscale |
| y_scaling    | enum         | sqrt    | look @ x_scaling                            |

### LAND_XXX
every LAND_XXX must be a unique string, if constraints like border can not be statisfied with the placement type the behavior is undefined

random: will create an island startpoint between the border constraints

circle: will create islands in a circle in the border constraints

| field                     | type            | default | info                                                        | placement: random | placement: (team_)circle |
| ------------------------- | --------------- | ------- | ----------------------------------------------------------- | ----------------- | -----------------        |
| player_lands              | boolean         | false   | makes island for all players                                | -                 | x                        |
| terrain                   | TERRAIN-TYPE    | GRASS   |                                                             | x                 | x                        |
| space_to_other_islands    | int             | 0       | minimum distance in tiles to other all other islands        | x                 | x                        |
| tiles                     | int             | maxint  | ~size in tiles                                              | x                 | x                        |
| tiles_scaling             | enum            | sqrt    | one of: None,sqrt,linear                                    | x                 | x                        |
| placement                 | enum            | random  | one of circle,team_cicle,random                             | x                 | x                        |
| placement_radius          | float           | 0.5     | 0<radius<1 relative radius of min(x,y)                      | -                 | x                        |
| placement_radius_variance | float           | 0.3     | radius +- variance, if 0, players will be placed in uniform | -                 | x                        |
| placement_angle_variance  | float           | 0.3     | 0<variance<1, if 0, players will be placed in uniform       | -                 | x                        |
| labels                    | list of strings | [ ]     | creates automatic labels for player_lands: _player_1        | x                 | x                        |
| border_ne                 | float           | 0       | 0<=border<=1 distance from ne border                        | x                 | x                        |
| border_nw                 | float           | 0       | 0<=border<=1 distance from nw border                        | x                 | x                        |
| border_se                 | float           | 0       | 0<=border<=1 distance from se border                        | x                 | x                        |
| border_sw                 | float           | 0       | 0<=border<=1 distance from sw border                        | x                 | x                        |

## OBJECT_XXX

| field                | type            | default     | info                                       | fill: true | fill: false |
| -------------------- | --------------- | ----------- | ------------------------------------------ | ---------- | ----------- |
| type                 | object-label    | VILLAGER    |                                            | x          | x           |
| islands              | list of strings | all_players |                                            | x          | x           |
| fill                 | boolean         | false       | the island will be filled with the object  | x          | x           |
| chance               | float           | 0.3         | chance of object placement per tile        | x          | -           |
| placement            | enum            | random      |                                            | -          | x           |
| number               | int             | 1           |                                            | -          | x           |
| number_scaling       | enum            | sqrt        | one of: None,linear,sqrt                   | -          | x           |
| groups               | int             | 1           |                                            | -          | x           |
| groups_scaling       | enum            | sqrt        | look @ number_scaling                      | -          | x           |
| obj_for_every_player | boolean         | false       | objects will be created for every player   | -          | x           |

## CONNECTION_XXX

single: creates a single connection between 2 islands

cross:  creates connections between all islands in islands

team: connects all islands of a team

cost: cost of a tile for a* algorithm, higher costs will be avoided

cost_OBJECT: OBJECT is one of the objects below

predefined islands labels: all_players


| field                | type            | default           | type: single | type: cross | type: team |
| -------------------- | --------------- | -----------       | ------------ | ----------- |------------|
| type                 | enum            | single,team,cross | x            | x           | x          |
| width                | int             | 1                 | x            | x           | x          |
| islands              | list of strings | all_players       | x            | x           | -          |
| pave_start_island    | boolean         | false             | x            | x           | x          |
| pave_end_island      | boolean         | false             | x            | x           | x          |
| subtitute_GRASS      | TERRAIN-TYPE    | ROAD              | x            | x           | x          |
| subtitute_WATER      | TERRAIN-TYPE    | SHALLOW           | x            | x           | x          |
| subtitute_SHALLOW    | TERRAIN-TYPE    | SHALLOW           | x            | x           | x          |
| subtitute_FORREST    | TERRAIN-TYPE    | ROAD              | x            | x           | x          |
| subtitute_DIRT       | TERRAIN-TYPE    | ROAD              | x            | x           | x          |
| subtitute_DEEP_WATER | TERRAIN-TYPE    | SHALLOW           | x            | x           | x          |
| subtitute_ROAD       | TERRAIN-TYPE    | ROAD              | x            | x           | x          |
| cost_GRASS           | int             | 1                 | x            | x           | x          |
| cost_WATER           | int             | 1                 | x            | x           | x          |
| cost_SHALLOW         | int             | 1                 | x            | x           | x          |
| cost_FOREST          | int             | 1                 | x            | x           | x          |
| cost_DIRT            | int             | 1                 | x            | x           | x          |
| cost_DEEP_WATER      | int             | 20                | x            | x           | x          |
| cost_ROAD            | int             | 1                 | x            | x           | x          |
| cost_OBJECT          | int             | 50                | x            | x           | x          |
                
## list of object labels
### supported terrain and default values:

| terrain     | substituted by | cost |
| ----------- | -------------- | ---- |
| DIRT        | ROAD           | 1    |
| DEEP_WATER  | SHALLOW        | 1    |
| FORREST     | ROAD           | 1    |
| GRASS       | ROAD           | 1    |
| ROAD        | ROAD           | 1    |
| SHALLOW     | SHALLOW        | 1    |
| WATER       | SHALLOW        | 1    |
    
### supported objects:
| object     | size |
| ---------- | ---- |
| TOWNCENTER | 4x4  |
| VILLAGER   | 1x1  |

### supported gaia objects:
- TREE1
- GOLD
- STONE
- SHEEP
- FORAGE
