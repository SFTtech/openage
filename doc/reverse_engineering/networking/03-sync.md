# Sync

Everything about the packets that synchronize the game states. In this document, packets are distinguished by their length and command byte. For example a 16BC41 describes the packet with a length of 16 bytes and a command byte with value `0x41` in the header.

When the game is paused, only 16BC31 and 16BC32 packets are sent.

## Periodic

### 16BC41

A simple incremental counter that will increments every time a 16BC41 packet is sent. Packets are transmitted in intervals of 120ms.

```ruby
def 16BC41
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int32 :individual_counter
end
```

*:network_source_id*<br/>
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*<br/>
The destination of the packet. 16BC41 is one of the few packet types, where this value is not set to zero.

*:command*<br/>
Always has the value `0x41`.

*:option{1-3}*<br/>
These are options with unknown effects. They work the same way as outlined in [02-header.md](02-header.md).

*:individual_counter*<br/>
The individual counter of the player who sent the packet. It increments by 1 with every 16BC41 sync command. For individual players, this counter starts with values that are 1200 units (two players) or 2000 units (three players, `0x7d0`) apart from each other. In the latter example player 1 starts with the value `0x7d0`, player 2 with `0xfa0` and player 3 with `0x1770`. If the differences between these values is not a multiple of 2000, this indicates a de-sync.

### 16BC31

Makes sure that the game time of players stays synced. It always receives an answer in form of 16BC32. Packets are transmitted in intervals of 8s.

```ruby
def 16BC31
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int32 :time_passed
end
```

*:network_source_id*<br/>
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*<br/>
The destination of the packet. 16BC31 is one of the few packet types, where this value is not set to zero.

*:command*<br/>
Always has the value `0x31`.

*:option{1-3}*<br/>
All 3 options are always zero for this command.

*:time_passed*<br/>
Time passed in milliseconds since the game started for the player. The difference between player 1 and player 2 was roughly 3,220 ms during tests. Note that for 16BC31 packets present in the lobby, this value represents milliseconds **since the application** started instead.

### 16BC32

The answer for a 16BC31 packet. Packets are transmitted in intervals of 8s.

```ruby
def 16BC32
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int32 :time_passed
end
```

*:network_source_id*<br/>
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*<br/>
The destination of the packet. 16BC32 is one of the few packet types, where this value is not set to zero.

*:command*<br/>
Always has the value `0x32`.

*:option{1-3}*<br/>
They work the same way as outlined in [02-header.md](02-header.md).

*:time_passed*<br/>
Has the exact same value as the 16BC31 packet.

### 24BC35

Syncing the connection between players in lobby. Packets are transmitted in intervals of 2s.

```ruby
def 24BC35
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int32 :connecting1
  int32 :unknown
  int32 :connecting2
end
```

*:network_source_id*<br/>
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*<br/>
This value is always zero.

*:command*<br/>
Always has the value `0x35`.

*:option1*<br/>
`0x35` for both host and regular players when initiating the connection. Otherwise it has the value `0x30`.

*:option2*<br/>
`0x32` for the host and `0xf8` for regular players when initiating the connection. Otherwise it has the value `0x5d`.

*:option3*<br/>
`0x00` for the host and `0x0a` for regular players when initiating the connection. Otherwise it has the value `0x00`.

*:connecting1*<br/>
This field is only used to initiate a connection to a lobby. It always has the value `0x32d2a4` for the host and `0x503a87` for regular players when initiating a connection. Once the connecting player has joined the lobby, the value remains zero for all following packets.

*:unknown*<br/>
Field with unknown purpose. The value in here is unique for every player and sent with every packet.

*:connecting2*<br/>
This field is only used to initiate a connection to a lobby. It always has the value `0x5016b5` for the host and `0x32b5c4` for regular players when initiating a connection. Once the connecting player has joined the lobby, the value remains `0xFFFFFFFF` for all following packets.

### 26BC53

Only sent by the host from the lobby and not by other players. Sent in intervals of 3s.

```ruby
def 26BC53
  byte20 :header
  int16 :unknown1
  int16 :unknown2
  int16 :communication_turn
end
```

*:header*<br/>
The standard header. Works the same way as outlined in [02-header.md](02-header.md).

*:unknown1*<br/>
A field with unknown purpose. Changes between values of `0x04` and `0x08`.

*:unknown2*<br/>
Another field with unkown purpose. Possibly ping.

*:communication_turn*<br/>
The current communication turn as a 16 bit value.

### 32BC44

Sncs up the communication turns in the game. Packets are transmitted in intervals of 120ms.

```ruby
def 32BC44
  byte20 :header
  int8 :command2
  int8 :unknown1
  int8 :unknown2
  int8 :unknown3
  int32 :communication_turn_offset
  int8 :ping1
  int8 :ping2
  int8 :unknown4
  int8 :unknown5
end
```

*:header*<br/>
The standard header. Works the same way as outlined in [02-header.md](02-header.md).

*:command2*<br/>
Always has the value of the command byte from the header `0x44`. It is unknown whether this is supposed to be the same as *:command* or if it has other use cases.

*:unknown1*<br/>
Possibly used to communicate the connection status. The value is different for every player, but consistent for most packets throughout the game. An exception are packets sent at the start of a game. The first 32BC44 packet sent by the lobby host always has the value `0x96` in this field. All other players will have the value `0xce` in their first packet.

Sometimes the value of the fields *:unknown{1-3}* and *:unknown5* will be `0x00`, but no reason for this behavior could be found.

*:unknown2*<br/>
Like *:unknown1* it is probably used to communicate the connection status. Values for the very first 32BC44 packet are `0x98` for the lobby host and `0x32` for all other players.

Sometimes the value of the fields *:unknown{1-3}* and *:unknown5* will be `0x00`.

*:unknown3*<br/>
Has the value `0xf7` most of the time.

Sometimes the value of the fields *:unknown{1-3}* and *:unknown5* will be `0x00`.

*:communication_turn_offset*<br/>
When the game desyncs, this field indicates the last communication turn where the game was in sync for the player sending this packet. While the game is synced, the value should match up with *:communication_turn* in the header.

*:ping1*<br/>
The difference between the ping from the current turn and the ping from the previous turn.

*:ping2*<br/>
Average time to receive an answer from the player the packet was sent to. Uses the ping times for the last 10-15 32BC44 packets for calculation.

*:unknown4*<br/>
Byte with unknown purpose. It will have the value `0x4c` for the player who was the lobby host and a different value for all other players (one player: `0x48`; two players: both `0x47`). When *:unknown{1-3}* and *:unknown5* have the value `0x00`, the value for *:unknown4* will always be `0x32`.

*:unknown5*<br/>
Has the value `0xf7` most of the time.

Sometimes the value of the fields *:unknown{1-3}* and *:unknown5* will be `0x00`.

### 56BC4D

Syncs up the communication turns in the game. Seems to check if some values are the same for every player. In every interval, the values of 56BC4D stay the same for all players (except for the last 4 bytes). Packets are transmitted in intervals of 8s - 16s.

```ruby
def 56BC4D
  byte20 :header
  int32 :unknown1
  int32 :communication_turn_check
  int32 :unknown2
  int32 :unknown3
  int32 :unknown3
  int32 :unknown5
  int32 :unknown6
  int32 :unknown7
  int32 :unknown8
end
```

*:header*<br/>
The standard header. Works the same way as outlined in [02-header.md](02-header.md).

*:communication_turn_check*<br/>
The communication turn which is validated. Always has the value (*:communication_turn* - 2).

*:unknown{1-7}*<br/>
These are the same for all players during a given interval. Some of them seem to be counters, while others change values with an unknown pattern.

*:unknown8*<br/>
Different for every interval and every player.

## Non-Periodic

### 24BC51

Utilized after dropping a player due to connection loss or de-sync.

```ruby
def 24BC51
  byte20 :header
  int32 :unknown3
end
```

*:header*<br/>
The standard header. Works the same way as outlined in [02-header.md](02-header.md).

*:last_synced_communication_turn*<br/>
The number of the communication turn that the remaining players will continue the game from.

### 24BC52

Used for readying in the lobby.

```ruby
def 24BC52
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int8 :unknown
  int8 :player_id
  int8 :unknown2
  int32 :zero
  int32 :unknown3
end
```

*:network_source_id*<br/>
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*<br/>
This value is always zero.

*:command*<br/>
Always has the value `0x52`.

*:option1*<br/>
Is `0x01` when readying and `0x00` when unreadying.

*:option2*<br/>
Is `0x1e` when readying and `0x00` when unreadying.

*:option3*<br/>
Is `0x00` when readying or unreadying.

*:unknown*<br/>
Field with unknown purpose. Always has value `0x01`.

*:player_id*<br/>
The ID of the player who is readying. Is `0x00` when player is unreadying.

*:unknown2*<br/>
Field with unknown purpose. Always has value `0x01` when player is readying. Is `0x00` when player is unreadying.

*:zero*<br/>
These 4 bytes are always zero.

*:unknown3*<br/>
Field with unknown purpose. Always has value `0x04` when player is readying. Is `0x00` when player is unreadying.
