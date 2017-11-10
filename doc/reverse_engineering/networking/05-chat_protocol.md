# Messaging Protocol

AoE2's game structure has always been a little bit messy for today's standards, with units cramped into .slp files and cracks and flaws in the game engine that become more apparent with every expansion. But what about the old network protocol? Can something that works with a 28.8 modem be messy too? Well, the answer is yes but explaining every detail is kind of complicated. For that reason, we will focus on analyzing just the chat protocol because it actually is the most understandable aspect of the multiplayer network communication.

## Structure and length

The chat message data field in AoC has a minimum length of 43 bytes (for 1 character) and a maximum length of 107 bytes (65 characters). These 65 bytes difference is used for the message text. Characters are encoded in extended ASCII, so they support most of the American and European alphabets. The other (mandatory) 42 bytes form the header that is interpreted by the network engine of the game.

An example of the UDP data field (51 bytes) for the message "abcdefghi" is below. Keep in mind that a hexadecimal representation is used.

```
0000   13 8c 46 01 00 00 00 00 43 02 7b 00 9f 56 00 00
0010   9d 68 00 00 02 00 4e 59 59 4e 4e 4e 4e 4e 32 00
0020   09 00 00 00 00 61 62 63 64 65 66 67 68 69 00 00
0030   18 dc 32
```

We will now try to separate the field to learn more about the logical structure of the underlying protocol.

## Analyzing the data field

### Sender ID

```
13
```

The first byte is the ID of the player who sends the message. Every player gets their ID at the start of the game. The IDs change for every game. Therefore, players will usually not have the same IDs. The IDs of Player 2-4 are not random and seem to orientate themselves to the Player 1 ID. If Player 1 has the ID `11`, the other IDs will look like this:

Player|ID
------|--
1     |11
2     |13
3     |15
4     |17

The trend we see is that the IDs will be apart from each other by multiples of 2.

### Game ID

```
8c 46 01
```

The next three bytes are what will be called the "Game ID" because these three bytes are persistent throughout every packet (chat, but also sync, command, etc. packets) in a game session. The value seems to be generated from the Player IDs (more players and higher values for Player IDs equal a higher value for the Game ID), although there is no obvious exact pattern.

### Receiver ID

```
00
```

Our next byte is the Receiver ID which is not used for chat messages and therefore holds no value. Receiver IDs play a role in sync packets to synchronize the game state between the P2P clients. Since all chat messages are broadcasted to every player, a Receiver ID is unnecessary for this use case. Wait, did we just say all messages are broadcasted? Wouldn't that be problematic if you wanted to only communicate with one specific player? We will look at that in a minute.

### Game ID 2

```
00 00 00
```

This is the Game ID of the Receiver. Similar to the Receiver ID, the second Game ID is not necessary for chat messages.

### Packet type/Control bytes

```
43 02 7b 00
```

Next up, we have 4 bytes that clarify how the data in the following bytes has to be interpreted. The first byte, `43`, indicates that the packet contains a "chat message". The purpose of the other 3 bytes is unknown to me, but they vary between very few values. Possible values for them include:

Byte  |Values
------|------
1     |43
2     |02, 01
3     |00, 7b, 8b, 90
4     |00, 0c

The last 3 bytes are assumed to communicate more control parameters to the game engine.

### Counter 1 an Counter 2

```
9f 56 00 00
```
```
9d 68 00 00
```

4 bytes each are reserved for Counter 1 and Counter 2. Read from the left to the right, the first byte represents values 16^0, the second byte values 16^1, etc. These counters show which "sync cycle" the sender was on, when he sent the chat message. Counter 2 should always be 1 ahead of the last sync packet sent, if the game is properly synchronized.

### Player Number

```
02
```

Now we have 1 byte dedicated to the actual number (not to be confused with the Player ID) of the player who sent the message. In this case it is Player 2.

### Chat Window Close Byte

```
00
```

This byte is strange, as its name already indicates. The value of it is `00` when a player types a message and then presses ENTER, leaving the chat window open. However, if he presses the "Sent"-Button of the chat window, the value of the byte will be `d9`.

### String of intended receivers

```
4e 59 59 4e 4e 4e 4e 4e
```

As mentioned before, the chat messages are broadcasted to every player, even if the message is intended for just a subset of them. The reason the messages are displayed to the correct addressees can be seen here. The 8 byte ASCII string shown above dedicates each one of its bytes to one player. If the ASCII character at position X is "Y", the message will be displayed to Player X. Vice versa, if the character at position X reads "N", Player X will not be able to see the message. He will still receive the packet though which in consequence can be captured and read in Wireshark without problems.

One could think that the messages were broadcasted so that they could be displayed in game recordings when you switch player perspectives. As it turns out, the recordings do not contain any chat messages that were not intended for you when you played the game. Furthermore, the chat window is locked to your message history permanently, even if you switch perspectives. If someone has any idea why chat messages are broadcasted to everyone in this game, let me know.

### Separator

```
32
```

This byte marks the end of the previous String. It doesn't change when we alter the number of players, the team composition or the message length, so it is assumed to be a separator.

### Empty bytes

```
00
```

These bytes were always empty in the tests and could be part of the separator mentioned above.

### Message length

```
09
```

Our next byte shows the message length. The maximum value it can reach is `41` which would be 65 in decimal notation.

### More empty bytes

```
00 00 00
```

Again we have empty bytes that didn't have values other than `00` during the tests. Maybe these bytes are an extension to the "Message length"-byte and the network protocol can handle more than 65 characters but they were limited in the final release.

### ASCII Message

```
61 62 63 64 65 66 67 68 69
```

What follows now are the message bytes. Each character is encoded in extended ASCII and represented by one byte. The example message was "abcdefghi" (very creative, I know).

### Separator 2

```
00
```

The end of the ASCII String is marked by 1 byte with the value `00`.

### Confusing part

```
00 18 dc 32
```

Last but not least the data ends with 4 bytes that have an unknown purpose but an interesting behavior. The bytes will look the same for messages of the same length, e.g. messages of length 9 will end with the above String, whether it is "abcdefghi", "?????????" or "123456789". By creating a message of length 1 and then extending following messages by one character each, we produce the following results:

Length  | Result
--------|-------
01      | 00 00 00 00
02      | 00 00 00 00
03      | 00 00 00 01
04      | 00 00 01 00
05      | 00 01 00 00
06      | 01 00 00 00
07      | 00 00 00 18
08      | 00 00 18 dc
09      | 00 18 dc 32

It seems that with every character added, the 4 bytes are "shifting" to the right and reveal a hidden message. But it is more complicated than that. For example, one time the result for a message of length 3 would result in `00 11 00 28`. Messages with 7 characters could have the values `92 19 0a 00` and `13 2c 0a 00`. These deviations from the pattern would occur when the "Chat Window Close Byte" had the value `d9`.

But even if we just look at the "normal" pattern, the byte values that we reveal are not fixed. Let's investigate more samples:

Length  | Result
--------|-------
0a      | 18 dc 32 00
0b      | dc 32 00 00
0c      | 32 00 00 00
0d      | 00 00 00 00
0e      | 00 00 00 00
0f      | 00 00 00 00
10      | 00 00 00 00
11      | 00 00 00 00
12      | 00 00 00 00
13      | 00 00 00 01
14      | 00 00 01 00
15      | 00 01 00 00
16      | 01 00 00 00
17      | 00 00 00 17
18      | 00 00 18 00
19      | 00 19 00 00
1a      | 1a 00 00 00

After the message reaches 13 character length it looks like the pattern repeats. Instead, a byte with the value of the "Length"-byte emerges. It even adjusts its value with every character that is added. The 4 bytes will show more strange values, if we add more characters. For now, we want to examine what happens to our 4 bytes, if the number of characters approaches its maximum of 65. For that purpose, we will send a messages that consists of the lower case alphabet that is repeated until 65 characters are reached.

Length  | Result
--------|-------
3f      | dd 32 00 41
40      | 32 00 41 42
41      | 00 41 42 43

Interestingly enough, at maximum message length the last 3 bytes will always contain the first 3 bytes of the actual message ASCII bytes in uppercase letters ("ABC"). The last 4 bytes of data start to look like someone purposefully created a buffer overflow and appended them to the message. But why waste 4 bytes of the message for this?

Anyway, here you can see the bits that you can reveal by adding one character at a time:

```
0000 00 00 00 00 00 01 00 00 00 18 dc 32 00 00 00 00
0010 00 00 00 00 00 01 00 00 00 XX 00 00 00 43 00 00
0020 00 XX 00 XX 00 1c dd 32 00 20 dd 32 00 b0 00 00
0030 00 6e 00 0a 00 XX 00 XX 00 00 58 c5 0a 20 dd 32
0040 00 YY YY YY
```

The value `XX` is the length of the message. These last 3 bytes `YY YY YY` are the first 3 bytes of the message in uppercase letters and are probably a result of the game checking for cheat codes.
