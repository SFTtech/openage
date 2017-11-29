# General Structure

This document defines the general structure of the protocol.

## Test Setup

The test environment consists of 4 hosts to simulate a 4-player multiplayer game. Each host was equipped with:

* Ubuntu 17.04
* Age of Empires 2: The Conquerors 1.0c
* Wine 1.8.7
* Wireshark 2.2.6

One game was established by using the DirectPlay feature, while all other play sessions were established over LAN. There is no detectable difference for the communication protocol and DirectPlay is only used for the purpose of connecting players.

## Transport Protocols

The game utilizes TCP to let players connect to a play session and UDP for all game configuration and ingame communications. This is not surprising, since UDP has the advantage of lightweight packets and therefore fast communication. This document will focus on the interesting part of the packets, i.e. the UDP data field.

## Packet types

There are 3 basic types of packets; sync packets, chat message packets and (player) action packets. Sync packets are used to synchronize communication turns, determine turn timers, calculate latency and validate game state. Chat message packets transport everything the players type in the ingame or lobby chat over the network. The last type we will discuss are the action packets which are utilized for commands from a player or an AI (e.g. movement, unit training).

Sync packets can be further categorized into periodic (sent in regular intervals) and non-periodic (sent to validate the game state after a player's action) packets. Most of the network communication during a multiplayer game consists of synchronization data.

Packets are recognized by a one byte long "command byte" in the header. So far, we have identified 11 different network commands.

Command | Purpose
--------|--------------
0x31    | Sync
0x32    | Sync
0x35    | Sync (Lobby)
0x3e    | Player-issued
0x41    | Sync
0x43    | Chat Message
0x44    | Sync --TBD
0x4d    | Sync --TBD
0x51    | De-Sync --TBD
0x52    | Readying (Lobby)
0x53    | Sync --TBD
0x5a    | Lobby

All packets with command `0x3e` have a second "command byte" after the header that represents the command a player has given ingame. To avoid confusion, we will call all player-issued commands "actions" and reserve the term "commands" for the actual network commands seen above. To align with this, the identifier for a player's action will be called "action byte". 34 of these can be found in network packets.

Action | Purpose
-------|------------
0x00   | Primary Action (Attacking, Resource gathering, Boarding Transport Ship)
0x01   | Stop
0x02   | Primary Action (AI)
0x03   | Move
0x0a   | Move (AI)
0x0b   | Resign
0x10   | Set waypoint
0x12   | Stance
0x13   | Guard
0x14   | Follow
0x15   | Patrol
0x17   | Formation
0x1b   | Save & Exit
0x1f   | Coordinated Move (AI)
0x64   | Train unit (AI)
0x65   | Research
0x66   | Build
0x67   | Diplomacy/Cheats/Change Speed
0x69   | Build wall
0x6a   | Delete
0x6b   | Attack ground
0x6c   | Tribute
0x6e   | Repair
0x6f   | Unboard/Ungarrison
0x72   | Toggle gate
0x73   | Flare
0x75   | Garrison/Stop building unit
0x77   | Train unit (Human)
0x78   | Rally point
0x7a   | Sell
0x7b   | Buy
0x7e   | Drop relic
0x7f   | Toggle townbell
0x80   | Back to work

When the game is recorded, the UDP data stream of a `0x3e` packet (without the header) is also written straight into the .mgx files with few changes. Viewing the recording will therefore simulate the exact actions that were done by the players. For more information on this check the **Further Reading** section below.

Much of the actions where already figured out by Stefan Kolb as part of his [.mgx Specification](https://github.com/stefan-kolb/aoc-mgx-format). This analysis will use his document style ([Example](https://github.com/stefan-kolb/aoc-mgx-format/blob/master/spec/body/actions/03-move.md)) as a template.

## Values and data types

Values in the network protocol can have a length of one, two or four byte. A little endian notation is used. Therefore, values with a length of two and four bytes have to be read starting with the rightmost byte.

The data is described with few data types, which are shown in the table below.

Length   | Data Types
---------|------------
1 byte   | int8, (ASCII) char
2 byte   | int16
4 byte   | int32, float
other    | (1-dimensional) array

Most of the fields present in the network protocol have a fixed length. The use cases for variable length fields are usually lists of `unit_id`s or waypoints and will be handled as arrays in this document.

## ID system

Age of Empires II uses an ID system to reference every object in the game by a unique numerical identifier with a length of 4 bytes. IDs are assigned by using a simple counter, that assigns every new object a the next unassigned number. New objects are not necessarily created by players. For example, cutting down a tree replaces a standing with a fallen tree, whereby the latter is handled as a new object. As a rule of thumb, one can assume that objects have been replaced with new ones when their sprite has changed.

The players are referenced with not less than three IDs; their *network_source_id*/*network_dest_id*, the *player_id* and the *player_number*. *network_ids* are present in the header to determine from which person a packet comes from or is sent to. The reason why *player_id* and *player_number* are handled differently is due to an undocumented cooperative multiplayer mode. In this mode, which can be activated by assigning to players the same *player_number*, two or more players share control over the same civilization. In consequence, *player_id* is unique for every player, *player_number* not necessarily.  

## Coordinate system

In this document, we will assume that AoC uses a carthesian coordinate system with the left corner as an origin point. Keep in mind, that AoC uses a **Quadrant 4** representation for its coordinates, which means that the y-axis is represented by the edge in the bottom left and the x-axis by the top left edge.

## Further Reading

To get a better understanding of the networking design and the underlying principles, it might be beneficial to read these sources.

[1] Dave Pottering, *1500 Archers on a 28.8: Network Programming in Age of Empires and Beyond*, 2001
https://www.gamasutra.com/view/feature/131503/1500_archers_on_a_288_network_.php?page=1

[2] Matthew Pritchard, *How to Hurt the Hackers: The Scoop on Internet Cheating and How You Can Combat It*, 2000
https://www.gamasutra.com/view/feature/3149/how_to_hurt_the_hackers_the_scoop_.php

[3] Stefan Kolb, *Age of Empires 2: The Conquerors — Savegame File Format Specification*,
https://github.com/stefan-kolb/aoc-mgx-format

[4] Renée Kooi, *RecAnalyst*, https://github.com/goto-bus-stop/recanalyst
