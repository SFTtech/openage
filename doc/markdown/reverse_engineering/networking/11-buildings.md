# Buildings

## Research

Used for researching technologies.

### Definition

```ruby
def Research
  int8 :action_identifier
  int24 :zero
  int32 :building_id
  int8 :player_number
  int8 :zero2
  int16 :technology_id
  int32 :const
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x65`.

*:zero*<br/>
The 3 bytes after *:action_identifier* are always zero.

*:building_id*<br/>
The ID of the building where the technology is researched.

*:player_number*<br/>
The number of the player for whom the technology is researched (`0x01` - `0x08`).

*:zero2*<br/>
The byte after *:player_number* is always zero.

*:technology_id*<br/>
The [ID of the technology](technology_ids.md) that is researched.

*:const*<br/>
This value is always `0xFFFFFFFF`.

### Examples

`65 00 00 00 63 1a 00 00 01 00 d9 00 ff ff ff ff`

>`65` &mdash; action_identifier<br/>
>`00 00 00` &mdash; zero<br/>
>`63 1a 00 00` &mdash; building_id<br/>
>`01` &mdash; player_number<br/>
>`00` &mdash; zero2<br/>
>`d9 00` &mdash; technology_id<br/>
>`ff ff ff ff` &mdash; const

## Build action

Used for building everything except walls.

### Definition

```ruby
def Build
  int8 :action_identifier
  int8 :selection_count
  int8 :player_number
  int8 :zero
  float :x_coord
  float :y_coord
  int16 :building_id
  int16 :zero2
  int32 :const
  int8 :sprite_id
  array :selected_ids,
        type => :int32,
        initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x66`.

*:selection_count*<br/>
The number of selected units.

*:player_number*<br/>
The number of the player who the building belongs to (`0x01` - `0x08`).

*:zero*<br/>
The byte after *:player_number* is always zero.

*:x_coord*<br/>
The x-coordinate of the construction site.

*:y_coord*<br/>
The y-coordinate of the construction site.

*:building_id*<br/>
The `UnitID` of the building.

*:zero2*<br/>
The two bytes after *:building_id* are always zero.

*:const*<br/>
This value is always `0xFFFFFFFF`.

*:sprite_id*<br/>
Represents which sprite is used for the building (only used for houses, which have several variations per civ).

*:selected_ids*<br/>
The IDs of the selected builders.

### Examples

`66 02 01 00 00 00 10 43 00 00 88 42 46 00 00 00 ff ff ff ff 02 00 00 00 6e 17 00 00 6c 17 00 00`

>`65` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`01` &mdash; player_number<br/>
>`00` &mdash; zero<br/>
>`00 00 10 43` &mdash; x_coord<br/>
>`00 00 88 42` &mdash; y_coord<br/>
>`46 00` &mdash; building_id<br/>
>`00 00` &mdash; zero2<br/>
>`ff ff ff ff` &mdash; const<br/>
>`02 00 00 00` &mdash; sprite_id<br/>
>`6e 17 00 00` &mdash; selected_id<br/>
>`6c 17 00 00` &mdash; selected_id

## Build wall action

Used for building walls.

### Definition

```ruby
def BuildWall
  int8 :action_identifier
  int8 :selection_count
  int8 :player_number
  int8 :start_x_coord
  int8 :start_y_coord
  int8 :end_x_coord
  int8 :end_y_coord
  int8 :zero
  int16 :building_id
  int16 :zero2
  int32 :const
  array :selected_ids,
        type => :int32,
        initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x69`.

*:selection_count*<br/>
The number of selected units.

*:player_number*<br/>
The number of the player who the wall belongs to (`0x01` - `0x08`).

*:zero*<br/>
The byte after *:player_number* is always zero.

*:start_x_coord*<br/>
The x-coordinate of the starting tile of the wall.

*:start_y_coord*<br/>
The y-coordinate of the starting tile of the wall.

*:end_x_coord*<br/>
The x-coordinate of the end tile of the wall.

*:end_y_coord*<br/>
The y-coordinate of the end tile of the wall.

*:building_id*<br/>
The `UnitID` of the wall.

*:zero2*<br/>
The two bytes after *:building_id* are always zero.

*:const*<br/>
This value is always `0xFFFFFFFF`.

*:selected_ids*<br/>
The IDs of the selected builders.

### Examples

`69 02 01 a0 4a ac 47 00 48 00 00 00 ff ff ff ff 6c 17 00 00 6e 17 00 00`

>`69` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`01` &mdash; player_number<br/>
>`a0` &mdash; start_x_coord<br/>
>`4a` &mdash; start_y_coord<br/>
>`ac` &mdash; end_x_coord<br/>
>`47` &mdash; end_y_coord<br/>
>`00` &mdash; zero<br/>
>`48 00` &mdash; building_id<br/>
>`00 00` &mdash; zero2<br/>
>`ff ff ff ff` &mdash; const<br/>
>`6c 17 00 00` &mdash; selected_id<br/>
>`6e 17 00 00` &mdash; selected_id

## Repair

Used for repairing buildings and siege.

### Definition

```ruby
def Repair
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  int32 :repaired_id
  array :selected_ids,
        type => :int32,
        initial_length => :selection_count
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x6e`.

*:selection_count*<br/>
The number of selected units.

*:zero*<br/>
The 2 bytes after *:selection_count* are always zero.

*:repaired_id*<br/>
The ID of the building or siege that is repaired.

*:selected_ids*<br/>
The IDs of the selected repairmen.

### Examples

`6e 02 00 00 70 1a 00 00 a0 1a 00 00 9f 1a 00 00`

>`6e` &mdash; action_identifier<br/>
>`02` &mdash; selection_count<br/>
>`00 00` &mdash; zero<br/>
>`70 1a 00 00` &mdash; repaired_id<br/>
>`a0 1a 00 00` &mdash; selected_id<br/>
>`9f 1a 00 00` &mdash; selected_id

## Toggle gate

Used for setting a gate to closed/open.

### Definition

```ruby
def ToggleGate
  int8 :action_identifier
  byte24 :zero
  int32 :gate_id
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x72`.

*:zero*<br/>
The 3 bytes after *:action_identifier* are always zero.

*:gate_id*<br/>
The ID of the gate.

### Examples

`72 00 00 00 bf 1a 00 00`

>`72` &mdash; action_identifier<br/>
>`00 00 00` &mdash; zero<br/>
>`bf 1a 00 00` &mdash; gate_id

## Townbell

Used for ringing the town bell.

### Definition

```ruby
def Townbell
  int8 :action_identifier
  byte24 :zero
  int32 :building_id
  int8 :active
  byte24 :zero2
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x7f`.

*:zero*<br/>
The 3 bytes after *:action_identifier* are always zero.

*:building_id*<br/>
The ID of the town center where the bell is rung.

*:active*<br/>
Represents whether the townbell is active (`0x01`) or not active (`0x00`).

*:zero2*<br/>
The 3 bytes after *:active* are always zero.

### Examples

`7f 00 00 00 68 17 00 00 01 00 00 00`

>`7f` &mdash; action_identifier<br/>
>`00 00 00` &mdash; zero<br/>
>`68 17 00 00` &mdash; building_id<br/>
>`01` &mdash; active<br/>
>`00 00 00` &mdash; zero2

## Back to work

Used for sending villagers back to work.

### Definition

```ruby
def BackToWork
  int8 :action_identifier
  byte24 :zero
  int32 :building_id
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x80`.

*:zero*<br/>
The 3 bytes after *:action_identifier* are always zero.

*:building_id*<br/>
The ID of the building from which the villagers are released.

### Examples

`80 00 00 00 68 17 00 00`

>`80` &mdash; action_identifier<br/>
>`00 00 00` &mdash; zero<br/>
>`68 17 00 00` &mdash; building_id
