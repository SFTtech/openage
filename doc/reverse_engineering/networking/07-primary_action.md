# Unit interaction - Primary action

Used for every action that is executed by selecting a unit and using `Right-Mouse-Botton` on an object in the game environment. It depends on the context (*:target_id*, *:selected_id*) which specific action is executed. Units with several right-click abilities (monks, villagers) will use their primary action for all of them.

Because primary actions are contextual, a primary action can have the exact same effect of another action (that was executed by using a button from the HUD or a hotkey). Notable examples are repairing and garrisoning units into rams/transport ships.

Action that classify as a primary action include attacking, gathering, repairing, converting, healing, picking up relics and boarding transport ships.

## Definition

```ruby
def Attack
    int8 :action_identifier
    int8 :player_id
    int16 :zero
    int32 :target_id
    int8 :selection_count
    byte24 :zero2
    float :x_coord
    float :y_coord
    array :selected_ids,
      type => :int32,
      length => :selection_count,
      onlyif => :selection_count < 0xFF
end
```

## Description

*:action_identifier*  
Always has the value `0x01`.

*:player_id*  
The ID of the player who executes the action (`0x01` - `0x08`).

*:zero*  
The two bytes following *:player_id* are unused.

*:target_id*  
The ID of the targeted object.

*:selection_count*  
The number of selected units. When the value of this field is `0xFF`, the action is executed for the units referenced in the previous command.

*:zero2*  
The three bytes following *:selection_count* are unused.

*:x_coord*  
The x-coordinate of the targeted position.

*:y_coord*  
The y-coordinate of the targeted position.

*:selected_ids*  
The IDs of the selected units.

## Examples

Gathering stone with villager.

`00 02 00 00 92 17 00 00 01 00 00 00 00 80 33 43 00 80 16 43 4d 17 00 00`

>`00` &mdash; action_identifier  
>`02` &mdash; player_id  
>`00 00` &mdash; zero  
>`92 17 00 00` &mdash; target_id  
>`01` &mdash; selection_count  
>`00 00 00` &mdash; zero2  
>`00 80 33 43` &mdash; x-coord  
>`00 80 16 43` &mdash; y-coord  
>`4d 17 00 00` &mdash; selected_id  

Attacking a building.

`00 02 00 00 38 1a 00 00 02 00 00 00 00 00 40 42 00 00 0b 43 5e 1a 00 00 62 1a 00 00`

>`00` &mdash; action_identifier  
>`02` &mdash; player_id  
>`00 00` &mdash; zero  
>`38 1a 00 00` &mdash; target_id  
>`02` &mdash; selection_count  
>`00 00 00` &mdash; zero2  
>`00 00 40 42` &mdash; x-coord  
>`00 00 0b 43` &mdash; y-coord  
>`5e 1a 00 00` &mdash; selected_id  
>`62 1a 00 00` &mdash; selected_id

Boarding a transport ship with the same units

`00 02 00 00 57 1a 00 00 ff 00 00 00 7a 7c 47 43 52 da 38 43`

>`00` &mdash; action_identifier  
>`02` &mdash; player_id  
>`00 00` &mdash; zero  
>`57 1a 00 00` &mdash; target_id  
>`ff` &mdash; selection_count  
>`00 00 00` &mdash; zero2  
>`7a 7c 47 43` &mdash; x-coord  
>`52 da 38 43` &mdash; y-coord
