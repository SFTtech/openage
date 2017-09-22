# Protocol Header

The header has a fixed length of 20 bytes and is present in almost all of the packets. Exceptions are the sync packets with 16 byte length, who have a smaller header.

## Definition

```ruby
def Header
  int32 :network_source_id
  int32 :network_dest_id
  int8 :command
  int8 :option1
  int8 :option2
  int8 :option3
  int32 :communication_turn
  int32 :individual_counter
end
```

## Description

*:network_source_id*  
The *:network_id* of the person who sent the packet. A *:network_id* is different for every game, but is not generated randomly for all players. When joining the lobby, every player gets assigned `last_network_id - 2` as his own *:network_id* where *last_network_id* is the ID of the person who joined before him.

*:network_dest_id*  
The *:network_id* of the person who should receive the packet. Is only used for sync packets and remains unused for most commands.

*:command*  
The command which tells the network engine how the data bytes should be interpreted.

*:option{1-3}*  
These are options with unknown effects.

*:communication_turn*  
The communication turn where the command is executed. Packets that contain a communication turn that has already passed are discarded.

*:individual_counter*  
The individual counter of the player who sent the packet. It increments by 1 with every 16BC41 sync command. For individual players this counter starts with values that are 2000 (`0x7d0`) apart from each other. Therefore, player 1 starts with the value `0x7d0`, player 2 with `0xfa0` and player 3 with `0x1770`. If the differences between these values is not a multiple of 2000, this indicates a de-sync.

## Examples

`82 b2 45 00 00 00 00 00 53 02 8f 0b 30 00 00 00 01 08 00 00`

>`82 b2 45 00` &mdash; network_source_id  
>`00 00 00 00` &mdash; network_dest_id  
>`53` &mdash; command  
>`02` &mdash; option1  
>`8f` &mdash; option2  
>`0b` &mdash; option3  
>`30 00 00 00` &mdash; communication_turn  
>`01 08 00 00` &mdash; individual_counter
