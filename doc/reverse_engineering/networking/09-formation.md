# Unit interaction - Formation actions

## Stance action

Used for changing the stance of units.

### Definition

```ruby
class Stance
  int8 :action_identifier
  int8 :selection_count
  int8 :stance
  array :selected_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x12`.

*:selection_count*  
The number of selected units.

*:stance*  
Represents which stance the units are going to take. AoC supports 4 stances.

Hex Value | Stance
----------|-------
0x00      | Aggressive
0x01      | Defensive
0x02      | Stand Ground
0x03      | Passive

*:selected_ids*  
The IDs of the selected units.

### Examples

`12 01 02 4b 15 00 00`

>`12` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`02` &mdash; stance  
>`4b 15 00 00` &mdash; selected_id   

## Guard action

Used for guarding a unit with others.

### Definition

```ruby
def Guard
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  int32 :guarded_id
  array :selected_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x13`.

*:selection_count*  
The number of selected units that are put on guard.

*:zero*  
The two bytes after *:selection_count* are always zero.

*:guarded_id*  
The ID of the unit that will be guarded.  

*:selected_ids*  
The IDs of the guarding units.

### Examples

`13 03 00 00 41 15 00 00 44 18 00 00 4b 15 00 00 42 18 00 00`

>`13` &mdash; action_identifier   
>`03` &mdash; selection_count  
>`00 00` &mdash; zero  
>`41 15 00 00` &mdash; guarded_id  
>`44 18 00 00` &mdash; selected_id  
>`4b 15 00 00` &mdash; selected_id  
>`42 18 00 00` &mdash; selected_id

## Follow action

Used for following a unit.

### Definition

```ruby
def Follow
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  int32 :followed_id
  array :selection_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x14`.

*:selection_count*  
Number of selected units.

*:zero*
The two bytes after *:selection_count* are always zero.

*:followed_id*  
The ID of the unit that will be followed.

*:selection_ids*  
The IDs of the units that are following the unit with *:followed_id*.

### Examples

`14 02 00 00 40 18 00 00 44 18 00 00 42 18 00 00`

>`14` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`00 00` &mdash; zero  
>`40 18 00 00` &mdash; followed_id  
>`44 18 00 00` &mdash; selected_id  
>`42 18 00 00` &mdash; selected_id

## Patrol action

Used for patrolling units.

### Definition

```ruby
def Patrol
  int8 :action_identifier
  int8 :selection_count
  int8 :patrol_waypoint_count
  int8 :zero
  array :x_coords,
    type => :int32,
    length => 10
  array :y_coords,
    type => :int32,
    length => 10
  array :selection_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x15`.

*:selection_count*  
Number of selected units.

*:patrol_waypoint_count*  
Number of waypoints set (with `SHIFT + Mouse-Button`).

*:zero*  
The byte after *:patrol_waypoint_count* is always zero.

*:x_coords*  
The x-coordinates of the patrol waypoints. Always has a length of 10 which is also the maximum number of patrol waypoints. For every waypoint that is not set, a placeholder value of `0x00000000` is used.

*:y_coords*  
The x-coordinates of the patrol waypoints. Always has a length of 10 which is also the maximum number of patrol waypoints. For every waypoint that is not set, a placeholder value of `0x00000000` is used.

*:selection_ids*  
The IDs of the units that are patrolling.

### Examples

```
0000   15 02 03 00 00 f8 2c 43 55 65 2c 43 00 88 2b 43
0010   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0020   00 00 00 00 00 00 00 00 00 00 00 00 55 0d 46 43
0030   00 70 48 43 00 48 45 43 00 00 00 00 00 00 00 00
0040   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0050   00 00 00 00 43 18 00 00 40 18 00 00
```

>`15` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`03` &mdash; patrol_waypoint_count  
>`00 00` &mdash; zero  
>`00 f8 2c 43` &mdash; x_coord1  
>`55 65 2c 43` &mdash; x_coord2  
>`00 88 2b 43` &mdash; x_coord3  
>`00 00 00 00` &mdash; x_coord4  
>`00 00 00 00` &mdash; x_coord5  
>`00 00 00 00` &mdash; x_coord6  
>`00 00 00 00` &mdash; x_coord7  
>`00 00 00 00` &mdash; x_coord8  
>`00 00 00 00` &mdash; x_coord9  
>`00 00 00 00` &mdash; x_coord10  
>`55 0d 46 43` &mdash; y_coord1  
>`00 70 48 43` &mdash; y_coord2  
>`00 48 45 43` &mdash; y_coord3  
>`00 00 00 00` &mdash; y_coord4  
>`00 00 00 00` &mdash; y_coord5  
>`00 00 00 00` &mdash; y_coord6  
>`00 00 00 00` &mdash; y_coord7  
>`00 00 00 00` &mdash; y_coord8  
>`00 00 00 00` &mdash; y_coord9  
>`00 00 00 00` &mdash; y_coord10  
>`43 18 00 00` &mdash; selected_id  
>`40 18 00 00` &mdash; selected_id

## Formation action

Used for changing the formation type.

### Definition

```ruby
def Formation
  int8 :action_identifier
  int8 :selection_count
  int8 :player_id
  int8 :zero
  int8 :formation_type
  byte24 :zero2
  array :selected_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x17`.

*:selection_count*  
The number of selected units.

*:player_id*  
The ID of the player who changes the formation (`0x01` - `0x08`).

*:zero*  
The byte after *:player_id* is always zero.

*:formation_type*  
The formation type that is selected. AoC supports 4 formation types.

Hex Value | Formation
----------|-------
0x02      | Line
0x04      | Staggered
0x07      | Box
0x08      | Split

*:zero2*  
The 3 bytes after *:formation_type* are always zero.

*:selected_ids*  
The IDs of the units whos formation type are changed.

### Examples

`17 02 02 00 07 00 00 00 40 18 00 00 43 18 00 00`

>`17` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`02` &mdash; player_id  
>`00` &mdash; zero  
>`07` &mdash; formation_type  
>`00 00 00` &mdash; zero2  
>`40 18 00 00` &mdash; selected_id  
>`43 18 00 00` &mdash; selected_id
