# Sync

Everything about the packets that synchronize the game states. In this document, packets are distinguished by their length and command byte. For example a `16BC41` describes the packet with a length of 16 bytes and a command byte with value `0x41` in the header.

- pausing?

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

*:network_source_id*  
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*  
The destination of the packet. 16BC41 is one of the few packet types, where this value is not set to zero.

*:command*  
Always has the value `0x41`.

*:option{1-3}*  
These are options with unknown effects. They work the same way as outlined in [02-header.md](02-header.md).

*:individual_counter*  
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

*:network_source_id*  
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*  
The destination of the packet. 16BC31 is one of the few packet types, where this value is not set to zero.

*:command*  
Always has the value `0x31`.

*:option{1-3}*  
All 3 options are always zero for this command.

*:time_passed*  
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

*:network_source_id*  
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*  
The destination of the packet. 16BC32 is one of the few packet types, where this value is not set to zero.

*:command*  
Always has the value `0x32`.

*:option{1-3}*  
They work the same way as outlined in [02-header.md](02-header.md).

*:time_passed*  
Has the exact same value as the 16BC31 packet.

### 24BC35

Syncing the connection between players in lobby. Packets are transmitted in intervals of 2s.

```ruby
def 16BC35
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

*:network_source_id*  
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*  
This value is always zero.

*:command*  
Always has the value `0x35`.

*:option1*  
`0x35` for both host and regular players when initiating the connection. Otherwise it has the value `0x30`.

*:option2*  
`0x32` for the host and `0xf8` for regular players when initiating the connection. Otherwise it has the value `0x5d`.

*:option3*  
`0x00` for the host and `0x0a` for regular players when initiating the connection. Otherwise it has the value `0x00`.

*:connecting1*  
This field is only used to initiate a connection to a lobby. It always has the value `0x32d2a4` for the host and `0x503a87` for regular players when initiating a connection. Once the connecting player has joined the lobby, the value remains zero for all following packets.

*:unknown*  
Field with unknown purpose. The value in here is unique for every player and sent with every packet.

*:connecting2*  
This field is only used to initiate a connection to a lobby. It always has the value `0x5016b5` for the host and `0x32b5c4` for regular players when initiating a connection. Once the connecting player has joined the lobby, the value remains `0xFFFFFFFF` for all following packets.

### 32BC44

### 56 Byte

## Non-Periodic

# 24BC52

Used for readying in the lobby. Only

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

*:network_source_id*  
Works the same way as outlined in [02-header.md](02-header.md).

*:network_dest_id*  
This value is always zero.

*:command*  
Always has the value `0x52`.

*:option1*  
Is `0x01` when readying and `0x00` when unreadying.

*:option2*  
Is `0x1e` when readying and `0x00` when unreadying.

*:option3*  
Is `0x00` when readying or unreadying.

*:unknown*  
Field with unknown purpose. Always has value `0x01`.

*:player_id*  
The ID of the player who is readying. Is `0x00` when player is unreadying.

*:unknown2*  
Field with unknown purpose. Always has value `0x01` when player is readying. Is `0x00` when player is unreadying.

*:zero*  
These 4 bytes are always zero.

*:unknown3*  
Field with unknown purpose. Always has value `0x04` when player is readying. Is `0x00` when player is unreadying.
