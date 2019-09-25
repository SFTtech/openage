# Others

## Resign/Disconnect

Used for resigning and disconnecting.

### Definition

```ruby
def Resign
  int8 :action_identifier
  int8 :player_number
  int8 :player_id
  int32 :disconnect
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x0b`.

*:player_number*<br/>
The player's number which can be changed in the lobby. Is only different from *:player_id* if playing in coop mode.

*:player_id*<br/>
The ID of the player.

*:disconnect*<br/>
Determines whether the player gave up himself (`0x00`) or was disconnected (`0x01`).

### Examples

`0b 02 03 00`

>`0b` &mdash; action_identifier<br/>
>`02` &mdash; player_number<br/>
>`03` &mdash; player_id<br/>
>`00` &mdash; disconnect

## Save Game

Used for saving the game.

### Definition

```ruby
def Save
  int8 :action_identifier
  int8 :exit
  int8 :player_id
  array :filename,
        type => :int8
  unknown :memory
  int32 :checksum
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x1b`.

*:exit*<br/>
Represents whether this action is a normal Save (`0x00`) or a Save & Exit (`0x01`).

*:player_id*<br/>
The ID of the player saving the game.

*:filename*<br/>
The filename of the savegame in ASCII characters.

*:memory*<br/>
The space between filename and checksum is filled with uninitialized memory.

*:checksum*<br/>
The last 4 bytes are a checksum.

### Examples

```
0000   1b 00 01 78 79 7a 2e 6d 73 78 00 7b 78 79 7a 00
0010   00 00 00 00 7e cd 83 7b 5b f3 83 7b 00 8c ff 3f
0020   00 00 00 80 03 00 00 00 b0 d7 32 00 03 00 00 00
0030   80 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00
0040   b0 d7 32 00 03 00 00 00 00 00 00 80 00 00 00 00
0050   06 f3 83 7b 88 d7 32 00 09 00 00 00 80 00 00 00
0060   ff ff ff ff cc d7 32 00 63 51 61 00 00 00 00 00
0070   63 51 61 00 3c d8 32 00 00 00 00 80 03 00 00 00
0080   b0 d7 32 00 03 00 00 00 87 51 61 00 02 00 00 00
0090   d2 ed 66 00 01 00 00 00 00 00 00 00 0c 00 00 00
00a0   00 00 00 00 01 00 00 00 03 00 00 00 00 00 00 80
00b0   03 00 00 00 16 00 00 00 f8 d7 32 00 9d 94 61 00
00c0   3c d8 32 00 00 80 00 00 40 00 00 00 a4 01 00 00
00d0   54 6d 14 0b 50 65 14 0b 58 6e 14 0b 00 00 00 00
00e0   00 00 00 00 b0 d7 79 0c 9d 39 61 00 3c d8 32 00
00f0   d0 ed 66 00 40 00 00 00 08 1b 68 00 b0 39 61 00
0100   3c d8 32 00 d0 ed 66 00 2c 00 00 00
```

>`1b` &mdash; action_identifier<br/>
>`00` &mdash; exit<br/>
>`01` &mdash; player_id<br/>
> `78 79 7a 2e 6d 73 78` &mdash; filename<br/>
> memory<br/>
> `2c 00 00 00` &mdash; checksum<br/>

## Diplomacy, Cheats and Game Speed

Used for diplomacy, cheating and changing the game's speed.

### Definition

```ruby
def Diplomacy
  int8 :action_identifier
  int8 :action_type
  int8 :source_player_number
  int8 :zero
  int8 :option
  byte24 :zero2
  float :option2
  int8 :diplomatic_stance
  byte24 :zero3
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x67`.

*:action_type*<br/>
Represents the specific action that was taken (diplomatic, changing speed or cheating).

Hex Value | Action
----------|-------
0x00      | Diplomacy
0x01      | Change Game Speed
0x04      | Cheat Response (if cheat is activated for all players; only "aegis")
0x05      | Allied Victory
0x06      | Cheat
0x0a      | Research Treason (only Regicide)
0x0b      | AI policy

*:source_player_number*<br/>
The player number of the player who has executed the action. In Cheat Responses, the value is `0x01` when "aegis" was turned on and `0x00` when it is turned off.

*:zero*<br/>
The byte after *:source_player_number* is always zero.

*:option*<br/>
Only used for diplomacy and cheats. Stays at `0x00` for game speed. The purpose of the option depends on the action. For diplomacy, this value represents the player number of the player who is targeted by the diplomatic action (which can be the same player number as *:source_player_number* when selecting "Allied Victory"). When changing the game speed, this value is always `0x00`. If the action is a cheat (`0x06`), then this field has the value of the `CheatID`, while for a cheat response the value is always `0x00`.

*:zero2*<br/>
The 3 bytes after *:option* are always zero.

*:option2*<br/>
Only used for diplomacy and changing game speed. Stays at `0x00000000` for cheats.

Hex Value  | Float  | Diplomatic Stance
-----------|--------|------------------
0x00000000 | 0.0f   | Allied
0x3f800000 | 1.0f   | Neutral
0x40400000 | 3.0f   | Enemy

Hex Value  | Float  | Game Speed
-----------|--------|------------------
0x3f800000 | 1.0f   | Slow
0x3fc00000 | 1.5f   | Normal
0x40000000 | 2.0f   | Fast

*:diplomatic_stance*<br/>
Only used for diplomacy. Changes depending on the diplomatic stance that was selected.

Hex Value | Diplomatic Stance
----------|------------------
0x00      | Allied
0x01      | Neutral
0x03      | Enemy

*:zero3*<br/>
The 3 bytes after *:diplomatic_stance* are always zero.

### Examples

Diplomacy.

`67 00 01 00 02 00 00 00 00 00 80 3f 01 00 00 00`

>`67` &mdash; action_identifier<br/>
>`00` &mdash; action_type<br/>
>`01` &mdash; source_player_number<br/>
>`00` &mdash; zero<br/>
>`02` &mdash; option<br/>
>`00 00 00` &mdash; zero2<br/>
>`00 00 80 3f` &mdash; option2<br/>
>`01` &mdash; diplomatic_stance<br/>
>`00 00 00` &mdash; zero3

Changing the game speed.

`67 01 01 00 00 00 00 00 00 00 c0 3f 00 00 00 00`

>`67` &mdash; action_identifier<br/>
>`01` &mdash; action_type<br/>
>`01` &mdash; source_player_number<br/>
>`00` &mdash; zero<br/>
>`00` &mdash; option<br/>
>`00 00 00` &mdash; zero2<br/>
>`00 00 c0 3f` &mdash; option2<br/>
>`00` &mdash; diplomatic_stance<br/>
>`00 00 00` &mdash; zero3

Cheating.

`67 06 01 00 68 00 00 00 00 00 00 00 00 00 00 00`

>`67` &mdash; action_identifier<br/>
>`06` &mdash; action_type<br/>
>`01` &mdash; source_player_number<br/>
>`00` &mdash; zero<br/>
>`68` &mdash; option<br/>
>`00 00 00` &mdash; zero2<br/>
>`00 00 00 00` &mdash; option2<br/>
>`00` &mdash; diplomatic_stance<br/>
>`00 00 00` &mdash; zero3

## Tribute

Used for sending resources to other players.

### Definition

```ruby
def Tribute
  int8 :action_identifier
  int8 :source_player_number
  int8 :target_player_number
  int8 :resource_type
  float :amount
  float :transaction_fee
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x6c`.

*:source_player_number*<br/>
The number of the player who sends the resources.

*:target_player_number*<br/>
The number of the player who receives the resources.

*:resource_type*<br/>
The resource type that is send.

Hex Value | Resource
----------|---------
0x00      | Food
0x01      | Wood
0x02      | Stone

*:amount*<br/>
The amount of resources being transferred.

*:transaction_fee*<br/>
The transaction fee.

### Examples

`6c 03 01 03 00 00 c8 42 cd cc 4c 3e`

>`6c` &mdash; action_identifier<br/>
>`03` &mdash; source_player_number<br/>
>`01` &mdash; target_player_number<br/>
>`03` &mdash; resource_type<br/>
>`00 00 c8 42` &mdash; amount<br/>
>`cd cc 4c 3e` &mdash; transaction_fee

## Flare

Used for launching a flare.

### Definition

```ruby
def Flare
  int8 :action_identifier
  byte24 :zero
  int32 :const
  int8 :zero2
  array :receiving,
        type => :int8,
        initial_length => 8
  byte24 :zero3
  float :x_coord
  float :y_coord
  int8 :player_number
  int8 :player_id
  int16 :zero4
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x0b`.

*:zero*<br/>
The 3 bytes after *:action_identifier* are always zero.

*:const*<br/>
These 4 bytes are always `0xFFFFFFFF`.

*:zero2*<br/>
The one byte after *:player_id* is always zero. Could also represent "Gaia" in *:receiving* or be a counter of some sorts that is never used.

*:receiving*<br/>
Every byte in this array represents a player number (first byte: player 1, second byte: player 2, ...). If the value of byte X is `0x01`, player X will see the flare. If the value of byte X is `0x00`, player X won't see the flare.

*:zero3*<br/>
The 3 bytes after *:receiving* are always zero.

*:x_coord*<br/>
The x-coordinate of the targeted position.

*:y_coord*<br/>
The y-coordinate of the targeted position.

*:player_number*<br/>
The player's number which can be changed in the lobby. Is only different from *:player_id* if playing in coop mode.

*:player_id*<br/>
The ID of the player.

*:zero4*<br/>
The 3 bytes after *:player_id* are always zero.

### Examples

`73 00 00 00 ff ff ff ff 00 01 01 01 00 00 00 00 00 00 00 00 00 00 c7 42 00 80 33 43 03 04 00 00`

>`73` &mdash; action_identifier<br/>
>`00 00 00` &mdash; zero<br/>
>`ff ff ff ff` &mdash; const<br/>
>`00` &mdash; zero2<br/>
>`01` &mdash; receiving_p1<br/>
>`01` &mdash; receiving_p2<br/>
>`01` &mdash; receiving_p3<br/>
>`00` &mdash; receiving_p4<br/>
>`00` &mdash; receiving_p5<br/>
>`00` &mdash; receiving_p6<br/>
>`00` &mdash; receiving_p7<br/>
>`00` &mdash; receiving_p8<br/>
>`00 00 00` &mdash; zero3<br/>
>`00 00 c7 42` &mdash; x_coord<br/>
>`00 80 33 43` &mdash; y_coord<br/>
>`03` &mdash; player_number<br/>
>`04` &mdash; player_id<br/>
>`00` &mdash; disconnect
