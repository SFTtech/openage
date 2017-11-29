# Chat messages

Chat messages can deliver an extended ASCII string to other players (Korean version?). The maximum length of a message can differ from 65 characters (in-game chat) to 247 characters (lobby chat). The data fields used for a chat message contain a lot of information that is not interpreted by the game, including the fields for message length and player number. Like all other packets, chat messages are sent to every player taking part in the game.

It should be noted that unlike all other packets, chat messages are not validated for errors or malicious behavior. This makes AoC vulnerable to [message spoofing](05-chat_protocol.md).

## Definition

```ruby
def ChatMessage
  byte20 :header
  int8 :player_id
  int8 :send_button
  array :intended_receivers,
        type => :char,
        initial_length => 8
  int8 :separator
  int8 :zero
  int8 :message_length
  int32 :zero2
  array :message_string,
        type => :char,
        initial_length => :message_length + 1
  int32 :unaligned_memory
end
```

### Description

*:header*
The default header of AoC. The command byte is always `0x43`.

*:player_id*  
The ID of the player who sends the message.

*:send_button*  
Determines whether the messages was send by pressing `Enter` (`0x00`) or the "Send" button (`0xd9`) in the Chat Menu.

*:intended_receivers*  
The intended receivers of the message. If the ASCII character at position X is "Y" (`0x59`), the message will be displayed to Player X. Vice versa, if the character at position X reads "N" (`0x4e`), Player X will not be able to see the message.

*:separator*  
A separator value that is always `0x32`.

*:zero*  
This byte is always zero.

*:message_length*  
The length of the message in bytes.

*:zero2*  
These 4 bytes are always zero.

*:message_string*  
The raw C-String of the message. Always contains a `0x00` byte at the end to signal the end of the String.

*:misaligned_memory*  
4 bytes (one word) of misaligned memory. It is possible that the Genie Engine operates directly on cached memory for optimization, which results in this behavior. The 4 bytes contain a part of an altered version of message, presumably after it was checked for cheat codes. Which part is read depends on the message length, starting with the byte after position 12. For example, at message length 0, the altered message bytes 13-16 are read. A message of length 9 will append the altered message bytes 22-25.

# Example

```
0000   02 00 4e 59 59 4e 4e 4e 4e 4e 32 00 09 00 00 00
0010   00 61 62 63 64 65 66 67 68 69 00 00 18 dc 32
```

>`02` &mdash;  player_id  
>`00` &mdash;  send_button  
>`4e 59 59 4e 4e 4e 4e 4e` &mdash;  intended_receivers  
>`32` &mdash;  separator  
>`00` &mdash;  zero  
>`09` &mdash;  message_length  
>`00 00 00 00` &mdash;  zero  
>`61 62 63 64 65 66 67 68 69 00` &mdash;  message_string  
>`00 18 dc 32` &mdash;  misaligned_memory

# Misaligned memory of the above message

```
0000 00 00 00 00 00 01 00 00 00 18 dc 32 00 00 00 00
0010 00 00 00 00 00 01 00 00 00 XX 00 00 00 43 00 00
0020 00 XX 00 XX 00 1c dd 32 00 20 dd 32 00 b0 00 00
0030 00 6e 00 0a 00 XX 00 XX 00 00 58 c5 0a 20 dd 32
0040 00 YY YY YY
```
