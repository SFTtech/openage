# Sync



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

All values before *:individual_counter* work the same way as outlined in [02-header.md](02-header.md). The only thing lacking in 16BC41 is the turn counter.

*:individual_counter*  
The individual counter of the player who sent the packet. It increments by 1 with every 16BC41 sync command. For individual players, this counter starts with values that are 1200 units (two players) or 2000 units (three players, `0x7d0`) apart from each other. In the latter example player 1 starts with the value `0x7d0`, player 2 with `0xfa0` and player 3 with `0x1770`. If the differences between these values is not a multiple of 2000, this indicates a de-sync.

### 16BC31

### 16BC32

### 24BC35

### 32BC44

### 56 Byte

## Non-Periodic

### 26 Bytes
