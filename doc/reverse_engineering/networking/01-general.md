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
0x44    | Sync
0x4d    | Sync
0x51    | De-Sync
0x53    | Sync
0x5a    | Lobby

All packets with command `0x3e` have a second "command byte" after the header that represents the command a player has given ingame. To avoid confusion, we will call all player-issued commands "actions" and reserve the term "commands" for the actual network commands seen above. To align with this, the identifier for a player's action will be called "action byte". 31 of these can be found in network packets.

Action | Purpose
-------|------------
0x00   | Several Actions (Attacking, Resource gathering, Boarding Transport Ship)
0x0b   | Resign
0x01   | Stop
0x03   | Move
0x10   | Set waypoint
0x12   | Stance
0x13   | Guard
0x14   | Follow
0x15   | Patrol
0x17   | Formation
0x18   | Save
0x64   | Train unit (AI)
0x65   | Research
0x66   | Build
0x67   | Diplomacy/Cheats
0x69   | Build wall
0x6a   | Delete
0x6b   | Attack ground
0x6c   | Tribute
0x6f   | Unboard/Ungarrison
0x72   | Toggle gate
0x73   | Flare
0x75   | Garrison
0x77   | Train unit (Human)
0x78   | Rally point
0x7a   | Sell
0x7b   | Buy
0x7e   | Drop relic
0x7f   | Toggle townbell
0x80   | Back to work

When the game is recorded, the UDP data stream of a `0x3e` packet (without the header) is also written straight into the .mgx files with few changes. Viewing the recording will therefore simulate the exact actions that were done by the players. For more information on this check the **Further Reading** section below.

## Further Reading

To get a better understanding of the networking design and the underlying principles, it might be beneficial to read these sources.

[1] Dave Pottering, *1500 Archers on a 28.8: Network Programming in Age of Empires and Beyond*, 2001
https://www.gamasutra.com/view/feature/131503/1500_archers_on_a_288_network_.php?page=1
[2] Matthew Pritchard, *How to Hurt the Hackers: The Scoop on Internet Cheating and How You Can Combat It*, 2000
https://www.gamasutra.com/view/feature/3149/how_to_hurt_the_hackers_the_scoop_.php
[3] Stefan Kolb, *Age of Empires 2: The Conquerors — Savegame File Format Specification*
https://github.com/stefan-kolb/aoc-mgx-format
