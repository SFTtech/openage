# openage save format

1.  [Questions](#1-questions)
2.  [Overview](#2-overview)
3.  [Header](#3-header)
4.  [String Table](#4-string-table)
5.  [Atrribute Table](#5-attribute-table)
6.  [Game Information](#6-game-information)
7.  [User Information](#7-user-information)
8.  [Chunk Table](#8-chunk-table)
9.  [Single Chunks](#9-single-chunks)
10. [Triggers](#10-triggers)
    1. [Event List](#101-event-list)
    2. [Condition List](#102-condition-list)
    3. [Action List](#103-action-list)
11. [Crypto](#11-crypto)

### 1 Questions

Do we something like ingame time vs realtime if the game is played at a different speed?

What do we want to track for stats?

| category   | stats  |
| ---------- | ------ |
| ressources | collected food/wood/gold/stone, maximum res in the bank? |
| units      | units/buildings killed/lost, units converted/lost to convertions, max units of each type |
| map        | discovered, maximum simultanously viewable (discovered - fog of war) |
| tech       | when was each tech/age researched? |
| battle     | what is a battle? units killed per timeslot per close area 30 units died in 15 seconds in radius of 20 Tiles TODO: definitions, maybe add cost for unit for calculation of the size of battle |

do we record timeseries? if yes fixed sizes(round robin database) or simple array snapshot per 10s/30s/60s/flexible?

every minute we save unit count per player?

crypto? multiplayer safe => server creates savefile, encrypts it, but one member of every team is needed to decrypt it and continue the game https://en.wikipedia.org/wiki/Secret_sharing

- savefile vs recorded game what happens if a savefile is continued, new recorded game or should recorded game stored with the savefile? if we dont store it, maybe we can reference it, so it can be continued if available
- how to store/reference stuff which is changed/modified/added to assets pack?


what are we trying to achieve?

- small size (compression per 'object')
- fast access to individual chunks
- everything should be checksummed(bitrot)
- statusinformation must be readable really fast (ingame listing)
- statsinformation should be easily be extensable, so no new format is needed

### 2 Overview
```
|--------------------------|
|    header                |
|--------------------------|
| string table             |
|--------------------------|
| key value table          |
|--------------------------|
| gameinformation          |
|--------------------------|
| userinformation          |
| | userheader           | |
| | user 1               | |
| | user 2               | |
|--------------------------|
|        chunktable        |
|--------------------------|
| single chunks            |
| | chunk 1 header        ||
| | | chunk 1 object 1 | | |
| | | chunk 1 object 2 | | |
| | chunk 2 header        ||
| | | chunk 2 object 1 | | |
| | | chunk 2 object 2 | | |
|--------------------------|
|        Triggers          |
|--------------------------|
```

types: default type is uint with width as seen in layout

| type   | info                                                                                      |
| ------ | ----------------------------------------------------------------------------------------- |
| uint+  | (variable length)[https://en.wikipedia.org/wiki/Variable-length_quantity], maximum uint64 |
| string | utf8 with uint+ length prefix                                                             |
| f32    | float 32Bit                                                                               |

endianess: Little-Endian

### 3 Header

| bytes__ | type      | name                              | info |
| ------- | --------- | --------------------------------- | ----- |
|   0- 10 | ascii[11] | openagesave                       | magic number( openage in ascii) |
|  11- 11 | uint8     | version                           | <div>save file version number,</div><div>1: this document describes version 1,</div><div>0: should be used while developing(so they won't be used in the wild</div> |
|  12- 15 | uint32    | string table offset               | |
|  16- 79 | byte[64]  | sha512 over stringtable           | checksum over all compressed bytes specified in length(inclusive) |
|  80- 83 | uint32    | key value table offset            | |
|  84-147 | byte[64]  | sha512 over key value table       | checksum over all compressed bytes specified in length(inclusive) |
| 148-151 | uint32    | gameinformation table offset      | |
| 152-215 | byte[64]  | sha512 over gameinformation table | checksum over all compressed bytes specified in length(inclusive) |
| 216-219 | uint32    | userinformation table offset      | |
| 220-283 | byte[64]  | sha512 over userinformation table | checksum over all compressed bytes specified in length(inclusive) |
| 284-287 | uint32    | chunktable table offset           | |
| 288-351 | byte[64]  | sha512 over chunktable table      | checksum over all compressed bytes specified in length(inclusive) |
| 352-355 | uint32    | single_chunks table offset        | |
| 356-419 | byte[64]  | sha512 over single_chunks table   | checksum over all compressed bytes specified in length(inclusive) |
| 420-423 | uint32    | triggers table offset             | |
| 424-487 | byte[64]  | sha512 over triggers table        | checksum over all compressed bytes specified in length(inclusive) |
| 488-488 | boolean   | crypto flag                       | |
| 489-489 | uint8     | key count                         | how many keys are necessary to encrypt file? must be >= 2 or 0 |
| 490-491 | byte[3]   | reserved                          | must be 0 |
| 492-516 | byte[25]  | timestamp                         | ISO 8601: 2017-01-11T22:07:12+00:00 |
| 517-772 | byte[216] | crypto key                        | must be 0, if crypto is not used |
| 773-788 | byte[16]  | crypto iv                         | must be 0, if crypto is not used |
| 789-852 | byte[64]  | sha512 over bytes 000-788         | header checksum, used to check if header is corrupted |

### 4 String Table

This table stores all the strings needed in the savegame, so they can compressed easily and simply referenced by a hashmap. The order of appearance in this list is there referrence id.

all bytes are offsets to "string table offset" (Header Section)

| bytes        | type                                 | name            | info                                                  |
| ------------ | ------------------------------------ | --------------- | ----------------------------------------------------- |
| 0-3          | uint32                               | length          | length of this compressed segment in bytes            |
| 4-7          | uint32                               | string count    | string count                                          |
| 8-(length-1) | string[string count] list of strings | these strings will be referenced by others:  id=count                   |

### 5 Key Value Table

all bytes are offsets to "key value table offset" (Header Section)

| bytes        | type   | name                 | info                                       |
| ------------ | ------ | -------------------- | ------------------------------------------ |
| 0-3          | uint32 | length               | length of this compressed segment in bytes |
| 4-7          | uint32 | key/value pair count | attribute count                            |
| 8-(length-1) | ..     | list of single value | Single Value @5.1, order = id              |

#### 5.1 Single Value


| bytes | type  | name             | info                                  |
| ----- | ----- | ---------------- | ------------------------------------- |
| 0-0   | uint8 | basic type       | one of the basic type below           |
| 1-..  | uint+ | string reference | reference to String Table (Section 4) |

type: 0=uint8, 1=uint16, 2=uint32, 3=uint64, 4=int8, 5=int16, 6=int32, 7=int64, 8=uint+, 9=int+, 10=f32, 11=f64, 12=string

### 6 Game Information:

gameinformation will be compressed: bytes 4 : length-1

all bytes are offsets to gameinformation offset

| bytes  | type        | name                     | info |
| ------ | ----------- | ------------------------ | --- |
| 0-3    | uint32      | length                   | length of this compressed segment in bytes |
| 4-4    | int8        | speed                    | game speed relative to normal in 1/10 steps 90 => 9 times faster, -4 => 0.4 times normal speed |
| 5-5    | uint8       | status                   | 0 = prestart, 1 = playing, 2 = finished, 3 = error/aborted |
| 6-9    | uint32      | gameframe                | current game frame, speed * gameframe / framespersecond = ingame time played (without pauses), TODO: maybe change to ms since start |
| 10-13  | uint32      | time played              | in seconds the time the game was running with pauses |
| 14-17  | uint32      | mod count                | |
| 18-21  | uint32      | mod list offset          | offset of the beginning of the mod list |
| 22-25  | uint32      | key value pair count     | |
| 26-29  | uint32      | list of key value offset | offset of the beginning of the key/value pair list |
| offset | string[]    | list of mods             | list of string(human readable mod/asset name) string(modid/assetid) |
| offset | key/value[] | list of key value pairs  | keys which have no influence on game continuing, so they may be there or not (so future keys can be added and will be ignored if unknown), references Key Value Table |

### 7 user information:

user information will be compressed: bytes 4 : length-1

| bytes | type          | name          | info                 |
| ----- | ------------- | ------------- | ------------------- |
| 0-3   | uint32        | length        | length of this segment in bytes |
| 4-4   | uint8         | player count  | Player count |
| 5-..  | single_user[] | list of users | list of users, layout per user described in 7.1 |

#### 7.1 single user

saves the state and stats of a single user

| bytes  | type        | name                                 | info |
| ------ | ----------- | ------------------------------------ | ------------------------- |
| 0-3    | uint32      | length                               | length of this compressed segment in bytes |
| 4-4    | uint8       | status                               | 0 = playing, 1 = won, 2 = lost |
| 5-5    | uint8       | type                                 | 0 = human, 1 = ai (TODO: ai type and paramenter in key/value list) |
| 6-9    | uint32      | key/value pairs count of user state  | name,civilisation,wood,food,color,...., |
| 10-13  | uint32      | key/value pairs offset of user state | relative to first byte of the user |
| 14-17  | uint32      | key/value pairs count of user stats  | first blood, kills, villager created |
| 18-21  | uint32      | key/value pairs offset of user stats | relative to first byte of the user |
| offset | key/value[] | list of key/value state              | |
| offset | key/value[] | list of key/value stats              | |

### 8 Chunk Table:

chunk table will be compressed: bytes 4 : length-1

| bytes              | type    | name               | info                                       |
| ------------------ | ------- | ------------------ | ------------------------------------------ |
| 0-3                | uint32  | length             | length of this compressed segment in bytes |
| 4-5                | uint16  | size of chunk side | as of now, we use 16                       |
| 6-9                | uint32  | chunk count        | chunk count                                |
| 7-[chunk_count*8]  | Chunk[] | list of chunks     | |

single chunk with offset in file:

| bytes | type   | name               | info                                       |
| ----- | ------ | ------------------ | ------------------------------------------ |
| 0-1   | uint16 | chunk_x            | x position of chunk                        |
| 2-3   | uint16 | chunk_y            | y position of chunk                        |
| 4-7   | uint32 | chunk offset       | absolute position of chunk in file         |

### 9 Single Chunks

where is this stored in the file?: single_chunk_segment + chunk_offset \

every single chunk will be compressed (bytes 4 : length-1) => every chunk will be compressed independendly

size of chunk side

| bytes  | type              | name            | info                                                     |
| ------ | ----------------- | --------------- | -------------------------------------------------------- |
| 0-3    | uint32            | length          | length of this compressed segment in bytes               |
| 4-7    | uint32            | tile count      | (x0,y0),(x0,y1),(x0,y2)... x * sizeChunkSide + y         |
| 8-..   | list of tiles[]   | tiles           | Tile(9.1)                                                |

#### 9.1 Tile

| bytes  | type              | name            | info                                                     |
| ------ | ----------------- | --------------- | -------------------------------------------------------- |
| 0-3    | uint32            | object offset   | start of list of object relative to Byte 0 of this Chunk |
| 4-7    | uint32            | object count    | number of objects on this Tile                           |
| 8-..   | string            | grass,grass2    | terrain                                                  |
| offset | list of objects[] | list of objects | layout described by section 9.1                          |

#### 9.2 Object

everything of an object will be saved as key/value pair. Example keys: health, name, playerid, ...

| bytes | type        | name               | info                                                                    |
| ----- | ----------- | ------------------ | ----------------------------------------------------------------------- |
| 0-3   | uint32      | length             | length of this object in bytes                                          |
| 4-7   | uint32      | attribute count    | attribute count                                                         |
| 8-..  | key/value[] | list of attributes | Attribute = [uint+ attribute id] look @Key Value Table, one must be objectid if it will referenced by trigger @section 10 |

### 10. Triggers

draft how triggers can be implemented:

event -> conditions -> actions

|           |                                                                                                                           |
| --------- | ------------------------------------------------------------------------------------------------------------------------- |
| event     | simple, must be fast calculable (only if true we will look at the conditions), event attributes can be used by conditions |
| condition | returns a boolen if a speficic gamestate is true, has access @event                                                       |
| action    | an action which changes the gamestate                                                                                     |

#### 10.1 Event list

| id  | event               | parameter                       | passed to conditions         | info                                                                                 |
| --- | ------------------- | ------------------------------- | ---------------------------- | ------------------------------------------------------------------------------------ |
|   0 | unit_dies           | -                               | unit,playerid,teamid         | fired when a unit of player PLAYERID is killed by an enemy(not converted or deleted) |
|   1 | building_finished   | -                               | building,playerid,teamid     | fired when player PLAYERID has finished building TYPE                                |
|   2 | building_destroyed  | -                               | building,playerid,teamid     | fired when a building of player PLAYERID is destroyed                                |
|   4 | unit_created        | -                               | unit,playerid,teamid         | fired when player PLAYERID creates unit UNITTYPE                                     |
|   5 | intervall           | TIME uint+                      | -                            | fired every INTERVALL milliseconds, but max only once per tick                       |
|   6 | timer               | TIME uint+                      | -                            | fired once after game is running >= TIME milliseconds                                |
|   7 | object_in_area      | POLYGON uint+,uint+,uint+,uint+ | unit,playerid,teamid         | fired when an object is in area POLYGON                                              |
|   8 | tech_researched     | -                               | tech,playerid,teamid         | fired when someone researched TECH                                                   |
|   9 | unit_conversion     |                                 | unit,playerFrom,playerTo     | fired when a unit is converted and not killed (tech: Heresy)                         |
|  10 | building_conversion |                                 | building,playerFrom,playerTo | fired when a building is converted and not destroyed (tech: Heresy)                  |

#### 10.2 Conditions list

| id  | condition                       | parameter                                       | needed     | info                                                               |
| --- | ------------------------------- | ----------------------------------------------- | ---------- | ------------------------------------------------------------------ |
|   0 | unit_is_type                    | TYPE string                                     | unit       | checks if unit is of type TYPE                                     |
|   1 | unit_has_attribute              | ATTRIBUTE string                                | unit       | checks if unt has attribute ATTRIBUTE                              |
|   2 | unit_check_bool_attribute       | ATTRIBUTE string                                | unit       | checks if unit has attribute ATTRIBUTE and if it is set to true    |
|   2 | unit_check_string_attribute     | ATTRIBUTE string, VALUE string                  | unit       | checks if unit has attribute ATTRIBUTE and if value == VALUE       |
|   3 | unit_check_uint_attribute       | ATTRIBUTE string, CONDITION uint8, AMOUNT uint+ | unit       | <div>checks if unit has attribute ATTRIBUTE and:</div><div>if AMOUNT condition ATTRIBUTE</div><div> 0 = "==", 1 = ">", 2 = "<", 3 = ">=", 4 = "<="</div> |
|   4 | unit_check_f32_attribute        | ATTRIBUTE string, CONDITION uint8, AMOUNT f32   | unit       | <div>checks if unit has attribute ATTRIBUTE and:</div><div>if AMOUNT condition ATTRIBUTE</div><div> 0 = "==", 1 = ">", 2 = "<"</div> |
|   6 | player_has_units_with_attribute | PLAYERID uint8, ATTRIBUTE string                | -          | player PLAYERID has units with attribute ATTRIBUTE                 |
|   7 | player_has_units_of_type        | PLAYERID uint8, UNITTYPE string                  | -          | player PLAYERID has units of type UNITTYPE                         |
|   8 | time_passed                     | TIME uint32                                     | -          | true if gametime >= TIME mark in milliseconds                      |
|   9 | random                          | CHANCE uint8                                    | -          | 1/CHANCE that it is true (random triggers :-) not allowed: 0       |
|  10 | player_has_accumulated          | PLAYERID uint8, TYPE string, AMOUNT uint+       | player     | player PLAYERID has accumulated over AMOUNT of type TYPE (@10.2.1) |
|  11 | team_has_accumulated            | TEAMID uint8,   TYPE string, AMOUNT uint+       | team       | team TEAMID has accumulated over AMOUNT of type TYPE (@10.2.1)     |
|  12 | unit_id                         | OBJECTID uint+                                  | unit       | OBJECTID references global object id from section 9.1              |
|  13 | building_id                     | OBJECTID uint+                                  | building   | OBJECTID references global object id from section 9.1              |
|  14 | is_player                       | PLAYERID uint8                                  | playerid   | true if PLAYERID = playerid from event                             |
|  15 | is_team                         | TEAMID uint8                                    | teamid     | true if TEAMID = teamid from event                                 |
|  16 | is_tech                         | TECH string                                     | tech       | true if TECH = tech from event                                     |

##### 10.2.1 accumulated Type list

units, buildings, gold, stone, food, wood, population, reliques, military, military_buildings, map_explored, units_killed, buildings_destroyed, units_lost, buildings_lost, units_converted, units_lost_converted

##### 10.2.2 attribute list
healthpoints, attack, defense, playerid, teamid

#### 10.3 Action list

| id  | action                | parameter                                    | info                                                            | usage                                               |
| --- | --------------------- | -------------------------------------------- | --------------------------------------------------------------- | --------------------------------------------------- |
|   0 | send_message          | PLAYERID uint16 , MESSAGE string             | sends MESSAGE to PLAYERID                                       | -                                                   |
|   1 | attribute_add_int     | attribute string, AMOUNT int+                | adds AMOUNT to ATTRIBUTE                                        | add health points to unit if it kills an other one  |
|   2 | attribute_add_f32     | attribute string, AMOUNT f32                 | adds AMOUNT to ATTRIBUTE                                        | adds 0.2 gold for every relique/per second          |
|   3 | attribute_set_int     | attribute string, AMOUNT int+                | sets AMOUNT to ATTRIBUTE                                        | sets health points to unit if it kills an other one |
|   4 | attribute_set_f32     | attribute string, AMOUNT f32                 | sets AMOUNT to ATTRIBUTE                                        |                                                     |
|   5 | attribute_set_string  | attribute string, STRING string              | sets STRING to ATTRIBUTE                                        | sets name of a unit                                 |
|   6 | kill_unit             | UNITID uint+                                 | kills unit UNITID                                               | kill unit if it go to forbidden map place           |
|   7 | player_looses         | PALYERID uint8                               | player PLAYERID looses the game                                 | -                                                   |
|   8 | player_wins           | PLAYERID uint8                               | player PLAYERID wins the game                                   | deletes trigger for performance reasons             |
|   9 | trigger_delete        | TRIGGERID uint+                              | deletes trigger TRIGGERID                                       | if the trigger should only run once, delete itself  |
|  10 | trigger_run           | TRIGGERID uint+                              | runs actions of trigger TRIGGERID without condition check       | may be use full for compression?                    |
|  11 | trigger_deactivate    | TRIGGERID uint+                              |                                                                 |                                                     |
|  12 | trigger_activate      | TRIGGERID uint+                              |                                                                 |                                                     |
|  13 | change_diplomacy      | PLAYER_1 unit8, PLAYER_2 uint8, STATUS uint8 | <div>changes diplomace from player1 to player2 to status STATUS</div><div>0=FRIEND,1=NEUTRAL,2=ENEMY</div> |          |
|  14 | reveal_map            | PLAYER uint8, POLYGON int+,int+,int+,int+    | reveals map POLYGON to user PLAYER                              | someone builds a wonder                             |
|  15 | reveal_map_with_fow   | PLAYER uint8, POLYGON int+,int+,int+,int+    | reveals map POLYGON with fog of war to user PLAYER              |                                                     |
|  16 | conceal_map           | PLAYER uint8, POLYGON int+,int+,int+,int+    | conceals map POLYGON to user PLAYER                             |                                                     |
|  17 | conceal_map_with_fow  | PLAYER uint8, POLYGON int+,int+,int+,int+    | conceals map POLYGON to user PLAYER(only fog of war is visible) |                                                     |
|  18 | play_sound            | PATH string                                  | play soundfile PATH                                             |                                                     |

Trigger: 1 event, 0 or more conditions, 1+ actions

| bytes | type      | info                    |
| ----- | --------- | ----------------------- |
| 0-1   | uint16    | length                  |
| 2-3   | uint16    | triggerid               |
| 4-5   | uint16    | priority: all triggers of the same priority will be executed in parallel, so no deletion (TODO: or writes to the same object) are allowed, execution order: asc, wholes in the sequence is allowed |
| 6     | uint8     | amount of conditions    |
| 7     | uint8     | amount of actions       |
| 8     | boolean   | trigger active          |
| 9-X   | event     | an event with parameter |
| X-Y   | condition | conditions tree         |
| Y-Z   | action    | list of actions         |

Event,Condition,Action: uint+(ID),parameter

Condition tree:

nodes(single Byte):


| id  | type      | children | info         |
| --- | --------- | -------- | ------------ |
| 0   | condition | 0        | leaf of tree |
| 1   | NOT       | 1        | node         |
| 2   | OR        | 2        | node         |
| 3   | AND       | 2        | node         |
| 4   | XOR       | 2        | node         |


```
                    root
                    and
                   /    \
                 or    player_has_units_of_type
               /    \
       time_passed  unit_is_type
```

serialisation/deserialisation:


```
                       +-----+
                       |start|
                       +--+--+
                          |
                          |
                          v
                      +---+----+     no    +---------------+
         +----------->+is node?+---------->+write condition|
         |            +---+----+           |   to stdout   |
         |                |                +---------------+
         |                |  yes
         |                v
         |           +----+------+
recursion|           | write node|
         |           | to stdout |
         |           +----+------+
         |                |
         |                |
         |                v
         |      +---------+------------+
         |      |recursive left        |
         |      |recursive right       |
         +------+or                    |
                |single recursive (not)|
                +----------------------+

                        +-----+
                        |start|
                        +--+--+
                           |
                           |
                           v               +--------------+
                       +---+----+   no     |read condition|
         +------------>+is node?+--------->+   return it  |
         |             +----+---+          +--------------+
         |                  |
         |                  |yes
recursion|                  v
         |      +-----------+----------------+
         |      |read node                   |
         |      |left  node = read recursive |
         |      |right node = read recursive |
         +------+or                          |
                |single node = read recursive|
                |return node                 |
                +----------------------------+

```


### 11. Crypto

everything from byte 826-end will be encrypted with aes.

Key distribution:

- encryption key will be randomly generated
- server writes savegame unencrypted to disk
- server encrypts everything after the last header byte with aes until file ending
- server writes crypto IV in header field
- generate team-count-1 times a random 256Bit(encryption key size) pseudo key
- key team N(the one without pseudo key): team1 xor team2 xor .. teamN-1 xor original encryption key and destroys the original encryption key

for every team:
- write pseudo key in header field and write the header checksum
- let every team member download the team savefile

Loading savegame:

- One team uploads savegame
- server knows how many other savegames are needed, server must ensure that all uploaded savegames have diffrent encryption key fields
- encryption key = encryption key1 xor encryption key2
- decrypt everything after header
- standard loading savegame

Every Member of a Team has the same savefile. For decryption one savefile of every team is needed. No Server has to save anything.
