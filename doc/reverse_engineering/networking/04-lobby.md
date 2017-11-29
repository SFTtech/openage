# Lobby

The lobby uses a 196 byte data field to communicate game settings between the players. Lobbies always require a host to allow new players to connect. Only the hosting player can change the settings.

Packets are transmitted in intervals of 4s.

## Values and Length

```ruby
def LobbySettings
  byte20 :header
  4 bytes
  array :player_network_ids,
        type => :int32,
        initial_length => 8
  int8 :ready
  69 bytes
  int8 :checkboxes
  byte
        6bit :reveal_map
        2bit :game_speed
  1 byte
  byte
        4bit :starting_age
        4bit :starting_resources
  byte
        4bit :map_size
        4bit :difficulty
  int8 :map_id
  int8 :victory
  1 byte
  int16 :victory_limit
  int8 :max_population
  8 bytes
  array :player_civ_ids,
        type => :int8,
        initial_length => 8
  16 bytes
  array :teams,
        type => :int8,
        initial_length => 8
  int16 :zero
  int8 :map_description_length
  array :map_description,
        type => :int8,
        initial_length => :map_description_length + 6
end
```

*:player_network_ids*  
The network IDs of the players. If a slot is empty, the corresponding value in the array is `0x00`.

*:ready*  
This byte indicates readied slots. Every bit (read from right to left) is associated with a slot in the lobby. If the bit is set to one, the slot is closed. Slots are also considered ready when they are filled with an AI or are closed.

For example, the value `0x0c` translates to a binary notation of `00001100b`. Therefore, the third and forth slot are closed.

*:checkboxes*  
A byte which is used to determine which of the checkbox options in the lower right are checked. Every bit is associated with checkbox. A bit is set to 1, when the checkbox is **not** set to its standard setting.

Checkbox      | On|Off | Bit representation
--------------|--------|-------------------
Team together |  0|1   | `01000000b`
Lock teams    |  1|0   | `00001000b`
All tech      |  1|0   | `10000000b`
Lock speed    |  1|0   | `00000100b`
Allow cheats  |  1|0   | `00000001b`

*:reveal_map*  
Determines whether the fog of war is present. Uses the 5th and 6th bit of the byte.

Mode     | Value
---------|------
Normal   | 0x00
Explored | `00001000b`
Visible  | `00000100b`

*:game_speed*  
Sets the default game speed. Uses the 7th and 8th bit of the byte.

Speed  | Value
-------|------
Slow   | `00000001b`
Normal | `00000010b`
Fast   | `00000011b`

*:starting_age*  
Sets the starting age.

Age           | Value
--------------|------
Standard      | 0x0
Dark          | 0x2
Feudal        | 0x3
Castle        | 0x4
Imperial      | 0x5
Post-Imperial | 0x6

*:starting_resources*  
Sets the amount of starting resources.

Resources| Value
---------|------
Standard | 0x0
Low      | 0x1
Medium   | 0x2
High     | 0x3

*:map_size*  
Sets the map size.

Size       | Value
-----------|------
Very Small | 0x0
Small      | 0x1
Medium     | 0x2
Normal     | 0x3
Large      | 0x4
Giant      | 0x5

*:difficulty*  
Sets the difficulty of the AI.

Difficulty | Value
-----------|------
Very Easy  | 0x4
Standard   | 0x3
Medium     | 0x2
Hard       | 0x1
Hardest    | 0x0

*:map_id*  
The ID of map that will be generated.

*:victory*  
Sets the victory condition.

Victory           | Value
------------------|------
Standard          | 0x00
Conquest          | 0x01
Time Limit        | 0x07
Score             | 0x08
Last Man Standing | 0x0b

*:victory_limit*  
Amount of points to win in score mode or timer for the time limit game mode.

*:max_population*  
This field is used for setting the maximum population for the game.

*:player_civ_ids*  
The ID of the civs the players have chosen. Defaults to Random (`0x1e`).

*:teams*  
The teams of each individual player. For whatever reason, the value for no team is `0x01` which leaves the teams 1, 2, 3 and 4 with these values:

Team              | Value
------------------|------
None              | 0x01
1                 | 0x02
2                 | 0x03
3                 | 0x04
4                 | 0x05
Random            | 0x06

*:zero*  
These 2 bytes are always zero.

*:map_description_length*  
The length of the name a custom maps. Only present when custom maps are selected.

*:map_description*  
The name of the custom map encoded in extended ASCII. It starts with 6 bytes that are used for formatting and not displayed by the game.
