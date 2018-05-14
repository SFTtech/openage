# Unit interaction - Formation actions

## Stance action

Used for changing the stance of units.

### Definition

```ruby
def Stance
  int8 :action_identifier
  int8 :selection_count
  int8 :stance
  array :selected_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x12`.

*:selection_count*<br/>
The number of selected units.

*:stance*<br/>
Represents which stance the units are going to take. AoC supports 4 stances.

Hex Value | Stance
----------|-------
0x00      | Aggressive
0x01      | Defensive
0x02      | Stand Ground
0x03      | Passive

*:selected_ids*<br/>
The IDs of the selected units.

### Examples

`12 01 02 4b 15 00 00`

>`12` &mdash; action_identifier<br/>
>`01` &mdash; selection_count<br/>
>`02` &mdash; stance<br/>
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

*:action_identifier*<br/>
Always has the value `0x13`.

*:selection_count*<br/>
The number of selected units that are put on guard.

*:zero*<br/>
The two bytes after *:selection_count* are always zero.

*:guarded_id*<br/>
The ID of the unit that will be guarded.

*:selected_ids*<br/>
The IDs of the guarding units.

### Examples

`13 03 00 00 41 15 00 00 44 18 00 00 4b 15 00 00 42 18 00 00`

>`13` &mdash; action_identifier<br/>
>`03` &mdash; selection_count<br/>
>`00 00` &mdash; zero<br/>
>`41 15 00 00` &mdash; guarded_id<br/>
>`44 18 00 00` &mdash; selected_id<br/>
>`4b 15 00 00` &mdash; selected_id<br/>
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

*:action_identifier*<br/>
Always has the value `0x14`.

*:selection_count*<br/>
Number of selected units.

*:zero*<br/>
The two bytes after *:selection_count* are always zero.

*:followed_id*<br/>
The ID of the unit that will be followed.

*:selection_ids*<br/>
The IDs of the units that are following the unit with *:followed_id*.

### Examples

`14 02 00 00 40 18 00 00 44 18 00 00 42 18 00 00`

>`14` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`00 00` &mdash; zero<br/>
>`40 18 00 00` &mdash; followed_id<br/>
>`44 18 00 00` &mdash; selected_id<br/>
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
    type => :float,
    length => 10
  array :y_coords,
    type => :float,
    length => 10
  array :selection_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x15`.

*:selection_count*<br/>
Number of selected units.

*:patrol_waypoint_count*<br/>
Number of waypoints set (with `SHIFT + Mouse-Button`).

*:zero*<br/>
The byte after *:patrol_waypoint_count* is always zero.

*:x_coords*<br/>
The x-coordinates of the patrol waypoints. Always has a length of 10 which is also the maximum number of patrol waypoints. For every waypoint that is not set, a placeholder value of `0x00000000` is used.

*:y_coords*<br/>
The y-coordinates of the patrol waypoints. Always has a length of 10 which is also the maximum number of patrol waypoints. For every waypoint that is not set, a placeholder value of `0x00000000` is used.

*:selection_ids*<br/>
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

>`15` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`03` &mdash; patrol_waypoint_count<br/>
>`00 00` &mdash; zero<br/>
>`00 f8 2c 43` &mdash; x_coord1<br/>
>`55 65 2c 43` &mdash; x_coord2<br/>
>`00 88 2b 43` &mdash; x_coord3<br/>
>`00 00 00 00` &mdash; x_coord4<br/>
>`00 00 00 00` &mdash; x_coord5<br/>
>`00 00 00 00` &mdash; x_coord6<br/>
>`00 00 00 00` &mdash; x_coord7<br/>
>`00 00 00 00` &mdash; x_coord8<br/>
>`00 00 00 00` &mdash; x_coord9<br/>
>`00 00 00 00` &mdash; x_coord10<br/>
>`55 0d 46 43` &mdash; y_coord1<br/>
>`00 70 48 43` &mdash; y_coord2<br/>
>`00 48 45 43` &mdash; y_coord3<br/>
>`00 00 00 00` &mdash; y_coord4<br/>
>`00 00 00 00` &mdash; y_coord5<br/>
>`00 00 00 00` &mdash; y_coord6<br/>
>`00 00 00 00` &mdash; y_coord7<br/>
>`00 00 00 00` &mdash; y_coord8<br/>
>`00 00 00 00` &mdash; y_coord9<br/>
>`00 00 00 00` &mdash; y_coord10<br/>
>`43 18 00 00` &mdash; selected_id<br/>
>`40 18 00 00` &mdash; selected_id

## Formation action

Used for changing the formation type.

### Definition

```ruby
def Formation
  int8 :action_identifier
  int8 :selection_count
  int8 :player_number
  int8 :zero
  int8 :formation_type
  byte24 :zero2
  array :selected_ids,
    type => :int32,
    initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x17`.

*:selection_count*<br/>
The number of selected units.

*:player_number*<br/>
The number of the player for whom the formation is changed (`0x01` - `0x08`).

*:zero*<br/>
The byte after *:player_number* is always zero.

*:formation_type*<br/>
The formation type that is selected. AoC supports 4 formation types.

Hex Value | Formation
----------|-------
0x02      | Line
0x04      | Staggered
0x07      | Box
0x08      | Split

*:zero2*<br/>
The 3 bytes after *:formation_type* are always zero.

*:selected_ids*<br/>
The IDs of the units whose formation type are changed.

### Examples

`17 02 02 00 07 00 00 00 40 18 00 00 43 18 00 00`

>`17` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`02` &mdash; player_number<br/>
>`00` &mdash; zero<br/>
>`07` &mdash; formation_type<br/>
>`00 00 00` &mdash; zero2<br/>
>`40 18 00 00` &mdash; selected_id<br/>
>`43 18 00 00` &mdash; selected_id
