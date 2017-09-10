# Unit interaction - Movement actions

## Stop action

This action stops the selected units from doing their currently active task (moving, gathering, attacking, etc.). It is also used to stop research in buildings.

### Definition

```ruby
class Stop
  int8 :action_identifier
  int8 :selection_count
  array :selected_ids,
        type :int32,
        initial_length :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x01`.

*:selection_count*  
The number of selected units. Is always `0x01` when stopping research in buildings, since AoC doesn't show the building queue when multiple buildings are selected.

*selected_ids*  
The IDs of the selected units. When stopping research, this is the ID of the building where the research is queued.

### Examples

Stop a bunch of (moving) units.

`01 03 99 06 00 00 98 06 00 00 97 06 00 00`

>`01` &mdash; action_identifier  
>`03` &mdash; selection_count  
>`99 06 00 00` &mdash; selected_id  
>`98 06 00 00` &mdash; selected_id  
>`97 06 00 00` &mdash; selected_id  

Stop a technology research from the building queue.

`01 01 5b 07 00 00`

>`01` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`5b 07 00 00` &mdash; building_id

## Move action

Used for moving units around the map.

### Definition

```ruby
def Move
	int8 :action_identifier
	int8 :player_id
	int16 :zero
	int32 :const
	int32 :selection_count
	float :x_coord
	float :y_coord
	array :selected_ids,
		:length => :selection_count,
 		:only_if => :selection_count < 0xFF
end
```

### Description

*:action_identifier*  
Always has the value `0x03`.

*:player_id*  
The ID of the player who moves his units (`0x01` - `0x08`).

*:zero*  
The two bytes following the *:player_id* are unused.

*:const*  
Always has the value `0xFFFFFFFF`.

*:selection_count*  
The number of selected units. When the value of this field is `0xFF`, the action is executed for the units referenced in the previous command.

*:x_coord*  
The x-coordinate of the targeted position.

*:y_coord*  
The y-coordinate of the targeted position.

*:selected_ids*  
The IDs of the selected units.

### Examples

Movement command with *:selected_ids* present.

`03 02 00 00 ff ff ff ff 02 00 00 00 00 40 22 43 ab aa 51 43 62 1a 00 00 5e 1a 00 00`

>`03` &mdash; action_identifier  
>`02` &mdash; player_id  
>`00 00` &mdash; zero  
>`ff ff ff ff` &mdash; const  
>`02 00 00 00` &mdash; selection_count  
>`00 40 22 43` &mdash; x_coord  
>`ab aa 51 43` &mdash; y_coord  
>`62 1a 00 00` &mdash; selected_id  
>`5e 1a 00 00` &mdash; selected_id

Consecutive movement command.

`03 02 00 00 ff ff ff ff ff 00 00 00 00 78 27 43 55 cd 39 43`

>`03` &mdash; action_identifier  
>`02` &mdash; player id  
>`00 00` &mdash; zero  
>`ff ff ff ff` &mdash; const  
>`ff 00 00 00` &mdash; selection_count  
>`00 78 27 43` &mdash; x_coord  
>`55 cd 39 43` &mdash; y_coord

## Setting Waypoints

Used for setting a waypoint.

### Definition

```ruby
def Waypoint
  int8 :action_identifier
  int8 :waypoint_count (?)
  int8 :selection_count
  int8 :x_coord
  int8 :y_coord
  array :selected_ids,
        type :int32,
        only_if => :selection_count < 0xFF
end
```

### Description

*:action_identifier*  
Always has the value `0x10`.

*:waypoint_count* (??)  
Could be the number of waypoints that are set. Is always `0x01`.

*:selection_count*  
The number of selected units. When the value of this field is `0xFF`, the action is executed for the units referenced in the previous command.

*:x_coord*  
The x-coordinate on the AoC grid.

*:y_coord*  
The y-coordinate on the AoC grid.

*:selected_ids*  
The IDs of the selected units. Can also be the ID of a building.

### Examples

Set a waypoint.

`10 01 01 34 16 a0 06 00 00`

>`10` &mdash; action_identifier  
>`01` &mdash; waypoint_count  
>`01 `&mdash; selection_count  
>`34` &mdash; x_coord  
>`16` &mdash; y_coord  
>`a0 06 00 00` &mdash; selected_id

Set consecutive waypoints.

`10 01 ff 31 17`

>`10` &mdash; action_identifier  
>`01` &mdash; waypoint_count  
>`ff `&mdash; selection_count  
>`31` &mdash; x_coord  
>`17` &mdash; y_coord

## Delete action

Used for deleting units and buildings.  

### Definition

```ruby
def Delete
  int8 :action_identifier
  byte24 :zero
  int32 :object_id
  int8 :player_id
  byte24 :zero
end
```

### Description

*:action_identifier*  
Always has the value `0x6a`.

*:zero*  
The 3 bytes following *:action_identifier* and *player_id* are unused.

*:object_id*  
ID of the unit or building that will be deleted.

*:player_id*  
The ID of the player who the unit belongs to (`0x01` - `0x08`).

### Examples

`6a 00 00 00 9f 06 00 00 02 00 00 00`

>`6a` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`9f 06 00 00` &mdash; object_id  
>`02` &mdash; player_id  
>`00 00 00` &mdash; zero  


## Attack ground

Used for the "attack ground" action of mangonels, onagers, trebuchets.

### Definition

```ruby
def AttackGround
	int8 :action_identifier
	int8 :selection_count
	int16 :zero
	float :x_coord
	float :y_coord
	array :selected_ids,
		:type => int32,
		:length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x6b`.

*:selection_count*  
The number of selected units.

*:zero*  
The 2 bytes following *:selection_count* are unused.

*:x_coord*  
The x-coordinate represented as a 32-bit float. Values have to be read backwards, so `ab 0a d3 41` translates to `0x41d30aab` or `26.3802f`.

*:y_coord*  
The y-coordinate represented as a 32-bit float. Values have to be read backwards, so `ab 3e 22 43` translates to `0x43223eab` or `162.245f`.

*selected_ids*  
The IDs of the selected units.

### Examples

`6b 02 00 00 ab 0a d3 41 ab 3e 22 43 3d 1a 00 00 42 1a 00 00`

>`6b` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`00 00` &mdash; zero  
>`ab 0a d3 41` &mdash; x_coord  
>`ab 3e 22 43` &mdash; y_coord  
>`3d 1a 00 00` &mdash; selected_id  
>`42 1a 00 00` &mdash; selected_id

## Ungarrison action

Used for unloading units from transport ships or rams and ungarrisoning units.

### Definition

```ruby
class Ungarrison
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  float :x_coord
  float :y_coord
  int8 :ungarrison_type
  byte24 :zero2
  int32 :release_id
  array :selected_ids,
		:type => int32,
		:length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x6f`.

*:selection_count*  
The number of selected units.

*:zero*  
The 2 bytes following *:selection_count* are unused.

*:x_coord*  
The x-coordinate represented as a 32-bit float. Values have to be read backwards, so `ab 0a d3 41` translates to `0x41d30aab` or `26.3802f`. When ungarrisoning from a building this value will always be `00 00 80 bf` = `-1.0f`.

*:y_coord*  
The y-coordinate represented as a 32-bit float. Values have to be read backwards, so `ab 3e 22 43` translates to `0x43223eab` or `162.245f`. When ungarrisoning from a building this value will always be `00 00 80 bf` = `-1.0f`.

*:ungarrison_type*

Is used to reflect the use of hotkeys to release only a certain type of units.

Hotkey                   | Hex Value | Action
-------------------------|-----------|--------
G                        | 0x00      | Release all
Mouse-BTN                | 0x03      | Release unit that was clicked on
SHIFT + Mouse-BTN        | 0x04      | Release units of the same type
CTRL + Mouse-BTN         | 0x05      | Release all except unit that was clicked on
CTRL + SHIFT + Mouse-BTN | 0x06      | Release all not of the same type

*:release_id*

The unit that was clicked on in the garrison queue. Value is `0xFFFFFFFF` if *:ungarrison_type* is `0x00`.

*:selected_ids*

The IDs of the rams, transport ships or buildings the units are released from.

### Examples

Unloading a transport ship.

`6f 01 00 00 00 c0 c7 41 55 4d 3f 43 00 00 00 00 ff ff ff ff 4c 1a 00 00`

>`6f` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`00 c0 c7 41` &mdash; x_coord  
>`55 4d 3f 43` &mdash; y_coord  
>`00` &mdash; ungarrison_type  
>`00 00 00` &mdash; zero2  
>`ff ff ff ff` &mdash; release_id  
>`4c 1a 00 00` &mdash; selected_id

Ungarrisoning all units from a building.

`6f 01 00 00 00 00 80 bf 00 00 80 bf 00 00 00 00 ff ff ff ff 4b 17 00 00`

>`6F` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`00 00 80 bf` &mdash; x_coord  
>`00 00 80 bf` &mdash; y_coord  
>`00` &mdash; ungarrison_type  
>`00 00 00` &mdash; zero2  
>`ff ff ff ff` &mdash; release_id  
>`4b 17 00 00` &mdash; selected_id  

Ungarrisoning with `CTRL + SHIFT`.

`6f 01 00 00 00 00 80 bf 00 00 80 bf 06 00 00 00 5a 1a 00 00 4b 17 00 00`

>`6F` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`00 00 80 bf` &mdash; x_coord  
>`00 00 80 bf` &mdash; y_coord  
>`06` &mdash; ungarrison_type  
>`00 00 00` &mdash; zero2  
>`5a 1a 00 00` &mdash; release_id  
>`4b 17 00 00` &mdash; selected_id  

## Garrison action

Used for garrisoning units into buildings as well as packing and unpacking of trebuchets.

### Definition

```ruby
def Garrison
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  int32 :building_id
  int8 :garrison_type
  byte24 :zero
  float :x_coord
  float :y_coord
  int32 :const
  array :selected_ids,
		:type => int32,
		:length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x75`.

*:selection_count*  
The number of selected units.

*:zero*  
The 2 bytes following *:selection_count* are unused.

*:building_id*  
The ID of a building, ram or transport ship which the units will be garrisoned in. When packing/unpacking a trebuchet, this value is always `0xFFFFFFFF`.

*:garrison_type*

Hex Value | Action
----------|--------
0x01      | Pack trebuchet
0x02      | Unpack trebuchet
0x05      | Garrison units into building, ram or transport ship

*:zero2*  
The 3 bytes following *:garrison_type* are unused.

*:x_coord*  
The x-coordinate represented as a 32-bit float. Value is always `00 00 00 00` when *:garrison_type* is `0x01` and  `00 00 80 bf` = `-1.0f` when it is `0x02` and `0x05`.

*:y_coord*  
The x-coordinate represented as a 32-bit float. Value is always `00 00 00 00` when *:garrison_type* is `0x01` and  `00 00 80 bf` = `-1.0f` when it is `0x02` and `0x05`.

*:const*  
This value was always `0xFFFFFFFF` in testing.

*:selected_ids*  
The IDs of the rams, transport ships or buildings the units are released from.

### Examples

Packing a trebuchet.

`75 01 00 00 ff ff ff ff 01 00 00 00 00 00 00 00 00 00 00 00 ff ff ff ff 65 1a 00 00`

>`75` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`ff ff ff ff` &mdash; building_id  
>`01` &mdash; garrison_type  
>`00 00 00` &mdash; zero2  
>`00 00 00 00` &mdash; x_coord  
>`00 00 00 00` &mdash; y_coord  
>`ff ff ff ff` &mdash; const  
>`65 1a 00 00` &mdash; selected_id  

Unpacking a trebuchet.

`75 01 00 00 ff ff ff ff 02 00 00 00 00 00 80 bf 00 00 80 bf ff ff ff ff 65 1a 00 00`

>`75` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`ff ff ff ff` &mdash; building_id  
>`02` &mdash; garrison_type  
>`00 00 00` &mdash; zero2  
>`00 00 80 bf` &mdash; x_coord  
>`00 00 80 bf` &mdash; y_coord  
>`ff ff ff ff` &mdash; const  
>`65 1a 00 00` &mdash; selected_id  

Garrisoning units into a building.

`75 02 00 00 46 17 00 00 05 00 00 00 00 00 80 bf 00 00 80 bf ff ff ff ff 6f 1a 00 00 6d 1a 00 00`

>`75` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`00 00` &mdash; zero  
>`46 17 00 00` &mdash; building_id  
>`05` &mdash; garrison_type  
>`00 00 00` &mdash; zero2  
>`00 00 80 bf` &mdash; x_coord  
>`00 00 80 bf` &mdash; y_coord  
>`ff ff ff ff` &mdash; const  
>`6f 1a 00 00` &mdash; selected_id  
>`6d 1a 00 00` &mdash; selected_id

## Drop relic action

Dropping a relic on the ground.

### Definition

```ruby
def DropRelic
  int8 :action_identifier
  byte24 :zero
  int32 :monk_id
end
```

### Description

*:action_identifier*  
Always has the value `0x7e`.

*:zero*  
The 3 bytes following *:action_identifier* are unused.

*:monk_id*  
The ID of the monk who will drop the relic.

### Examples

`7e 00 00 00 17 1b 00 00`

>`7e` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`17 1b 00 00` &mdash; monk_id
