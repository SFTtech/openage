# Unit creation

## Training unit (human player)

Used for training units with human player.

### Definition

```ruby
def TrainHM
  int8 :action_identifier
  byte24 :zero
  int32 :building_id
  int16 :unit_id
  int16 :train_count
end
```

### Description

*:action_identifier*  
Always has the value `0x77`.

*:zero*  
The 3 bytes after *:action_identifier* are always zero.

*:building_id*  
The ID of the building where the units are trained.

*:unit_id*  
The `UnitID` (`LineID` for villagers) of the unit which is trained.

*:train_count*  
The number of trained units.

### Examples

`77 00 00 00 4d 1a 00 00 5d 00 05 00`

>`77` &mdash; action_identifier  
>`00 00 00` &mdash; zero  
>`4d 1a 00 00` &mdash; building_id  
>`5d 00` &mdash; unit_id  
>`05 00` &mdash; train_count

## Rally Point action

Used for setting rally points.  

### Definition

```ruby
def RallyPoint
  int8 :action_identifier
  int8 :selection_count
  int16 :zero
  int32 :target_id
  int32 :target_unit_id
  float :x_coord
  float :y_coord
  array :selected_ids,
    type => :int32,
    length => :selection_count
end
```

### Description

*:action_identifier*  
Always has the value `0x78`.

*:selection_count*  
The number of selected buildings.

*:zero*  
The two bytes following *:selection_count* are always zero.

*:target_id*  
If the rally point is set on another object, this has the ID of this object. Otherwise has the value `0xFFFFFFFF`.

*:target_unit_id*  
The `UnitID` of the target. Will be `ff ff 00 00` if no target is selected.

*:x_coord*  
The x-coordinate of the rally point.

*:y_coord*  
The y-coordinate of the rally point.

*:selected_ids*  
The IDs of the buildings for which the rally point will be set.

### Examples

Set rally point in the open.

`78 01 00 00 ff ff ff ff ff ff 00 00 ab b2 17 43 00 20 49 42 43 17 00 00`

>`78` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`ff ff ff ff` &mdash; target_id  
>`ff ff 00 00` &mdash; target_unit_id  
>`ab b2 17 43` &mdash; x_coord  
>`00 20 49 42` &mdash; y_coord  
>`43 17 00 00` &mdash; selected_id  

Set rally point on object.

`78 01 00 00 83 17 00 00 66 00 00 00 00 80 29 43 00 00 1a 42 43 17 00 00`

>`78` &mdash; action_identifier  
>`01` &mdash; selection_count  
>`00 00` &mdash; zero  
>`83 17 00 00` &mdash; target_id  
>`66 00 00 00` &mdash; target_unit_id  
>`00 80 29 43` &mdash; x_coord  
>`00 00 1a 42` &mdash; y_coord  
>`43 17 00 00` &mdash; selected_id  
