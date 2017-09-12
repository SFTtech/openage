# Buildings

## Research

Used for researching technologies.

### Definition

```ruby
def Research
  int8 :action_identifier
  int24 :zero
  int32 :building_id
  int8 :player_id
  int8 :zero2
  int16 :technology_id
  int32 :const
end
```

### Description

*:action_identifier*  
Always has the value `0x65`.

*:zero*  
The 3 bytes after *:action_identifier* are always zero.

*:building_id*  
The ID of the building where the technology is researched.

*:player_id*  
The ID of the player who researches the technology (`0x01` - `0x08`).

*:zero2*  
The byte after *:player_id* is always zero.

*:technology_id*  
The [ID of the technology](technology_ids.md) that is researched.

*:const*  
This value is always `0xFFFFFFFF`.

## Examples

`65 00 00 00 63 1a 00 00 01 00 d9 00 ff ff ff ff`

>`65` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`63 1a 00 00` &mdash; building_id  
>`01` &mdash; player_id  
>`00` &mdash; zero2  
>`d9 00` &mdash; technology_id  
>`ff ff ff ff` &mdash; const

## Build action

Used for building everything except walls.

### Definition

```ruby
def Build
  int8 :action_identifier
  int8 :selection_count
  int8 :player_id
  int8 :zero
  float :x_coord
  float :y_coord
  int16 :building_id
  int16 :zero2
  int32 :const
  int32 :sprite_id
  array :selected_ids,
        type => :int32,
        initial_length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x66`.

*:selection_count*  
The number of selected units.

*:player_id*  
The ID of the player who the building belongs to (`0x01` - `0x08`).

*:zero*  
The byte after *:player_id* is always zero.

*:x_coord*  
The x-coordinate of the construction site.

*:y_coord*  
The y-coordinate of the construction site.

*:building_id*  
The `UnitID` of the building.

*:zero2*  
The two bytes after *:building_id* are always zero.

*:const*  
This value is always `0xFFFFFFFF`.

*:sprite_id*  
Represents which sprite is used for the building (only used for houses, which have several variations).

*:selected_ids*  
The IDs of the selected builders.

### Examples

`66 02 01 00 00 00 10 43 00 00 88 42 46 00 00 00 ff ff ff ff 02 00 00 00 6e 17 00 00 6c 17 00 00`

>`65` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`01` &mdash; player_id  
>`00` &mdash; zero  
>`00 00 10 43` &mdash; x_coord  
>`00 00 88 42` &mdash; y_coord  
>`46 00` &mdash; building_id  
>`00 00` &mdash; zero2  
>`ff ff ff ff` &mdash; const  
>`02 00 00 00` &mdash; sprite_id  
>`6e 17 00 00` &mdash; selected_id  
>`6c 17 00 00` &mdash; selected_id

## Build wall action

Used for building walls.

### Definition

```ruby
def BuildWall
  int8 :action_identifier
  int8 :selection_count
  int8 :player_id
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

*:action_identifier*  
Always has the value `0x69`.

*:selection_count*  
The number of selected units.

*:player_id*  
The ID of the player who the building belongs to (`0x01` - `0x08`).

*:zero*  
The byte after *:player_id* is always zero.

*:start_x_coord*  
The x-coordinate of the starting tile of the wall.

*:start_y_coord*  
The y-coordinate of the starting tile of the wall.

*:end_x_coord*  
The x-coordinate of the end tile of the wall.

*:end_y_coord*  
The y-coordinate of the end tile of the wall.

*:building_id*  
The `UnitID` of the wall.

*:zero2*  
The two bytes after *:building_id* are always zero.

*:const*  
This value is always `0xFFFFFFFF`.

*:selected_ids*  
The IDs of the selected builders.

### Examples

`69 02 01 a0 4a ac 47 00 48 00 00 00 ff ff ff ff 6c 17 00 00 6e 17 00 00`

>`69` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`01` &mdash; player_id  
>`a0` &mdash; start_x_coord  
>`4a` &mdash; start_y_coord  
>`ac` &mdash; end_x_coord  
>`47` &mdash; end_y_coord  
>`00` &mdash; zero  
>`48 00` &mdash; building_id  
>`00 00` &mdash; zero2  
>`ff ff ff ff` &mdash; const  
>`6c 17 00 00` &mdash; selected_id  
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

*:action_identifier*  
Always has the value `0x6e`.

*:selection_count*  
The number of selected units.

*:zero*  
The 2 bytes after *:selection_count* are always zero.

*:repaired_id*  
The ID of the building or siege that is repaired.

*:selected_ids*  
The IDs of the selected repairmen.

### Examples

`6e 02 00 00 70 1a 00 00 a0 1a 00 00 9f 1a 00 00`

>`6e` &mdash; action_identifier  
>`02` &mdash; selection_count  
>`00 00` &mdash; zero  
>`70 1a 00 00` &mdash; repaired_id  
>`a0 1a 00 00` &mdash; selected_id  
>`9f 1a 00 00` &mdash; selected_id

## Toggle gate action

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

*:action_identifier*  
Always has the value `0x72`.

*:zero*  
The 3 bytes after *:action_identifier* are always zero.

*:gate_id*  
The ID of the gate.

### Examples

`72 00 00 00 bf 1a 00 00`

>`72` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
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

*:action_identifier*  
Always has the value `0x7f`.

*:zero*  
The 3 bytes after *:action_identifier* are always zero.

*:building_id*  
The ID of the town center where the bell is rung.

*:active*  
Represents if the townbell is active (`0x00`) or not active (`0x01`).

*:zero2*  
The 3 bytes after *:active* are always zero.

### Examples

`7f 00 00 00 68 17 00 00 01 00 00 00`

>`7f` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`68 17 00 00` &mdash; building_id  
>`01` &mdash; active  
>`00 00 00 00` &mdash; zero2

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

*:action_identifier*  
Always has the value `0x80`.

*:zero*  
The 3 bytes after *:action_identifier* are always zero.

*:building_id*  
The ID of the building from which the villagers are released.

### Examples

`80 00 00 00 68 17 00 00`

>`80` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`68 17 00 00` &mdash; building_id
