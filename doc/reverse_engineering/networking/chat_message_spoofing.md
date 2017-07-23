# Spoofing and forging AoC chat messages

## Setup

The test environment consisted of 2 hosts and one router to simulate a 2-player multiplayer game. The hosts were equipped with:

* Ubuntu 17.04
* Age of Empires 2: The Conquerors 1.0c
* Wine 1.8.7

The router was placed as a central point between the two hosts and used for capturing the network traffic. In this example, we also use the router to send the forged UDP packet to one of the host, though in practice, the packet could be spoofed by one of the hosts as well. The setup looked like this:

* Ubuntu 17.04
* Wireshark 2.2.6
* Packet Sender 5.3.1

## Preparation

To construct a message you need to know the following parameters:

* Valid Sender ID of the Player who is going to be spoofed
* Game ID
* Current value of Counter 2 of the spoofed sender
* Player number of the spoofed sender
* Player number of the receiver(s)

An attacker that is in game with other players will have no problems getting to know the player numbers. As the game is constantly synced up, he can easily get the value for Counter 2 and Game ID.

Deriving the valid Sender ID of the receiving player is more difficult, depending on the time the attacker starts capturing the network traffic. The easiest way to discover all Player IDs is by capturing the 196 byte packets that are sent in the lobby. There the IDs of Players 1-8 have a fixed byte position in the data stream. The IDs can also be determined by looking at other packet types, but I will not discuss this here.

## Forging the packet

With the above knowledge and the protocol structure in mind, we are now able to create a packet to our liking. I personally used Packet Sender for this purpose, but you can use similar tools too, for example if you also want to spoof the IP address. We will limit our efforts to the UDP part of the packet.

First of all, we grab a sample packet (e.g. the one from my previous post), since we just need to change a few values. The Sender ID and Game ID will be replaced with our spoofed values. The command byte has to be left at value `43`, otherwise the packet will not be recognized as a message. The 3 control bytes after should contain values that fit the message command, but it's not necessary to set them to a specific value. `02 7b 00` and `02 00 00` worked fine for me.

The value of Counter 1 is irrelevant but Counter 2 must have a greater value than the one of the latest 32 byte sync packet (command byte: `44`). If the value of Counter 2 is smaller, the message will not display. The value of Counter 2 determines when the message is displayed or more precisely: The message will be displayed as soon as the values of Counter 2 in the sync packet and of Counter 2 in the message packet are equal. With today's internet connection speeds, a sync packet is sent every 120 ms, which you can use to anticipate the time the forged message will display.

The Player Number has to be set to spoofed player number. The 8 bytes of the *String of intended receivers* also must change accordingly: The value at position X should be `59` when player X is supposed to see the message.

The last thing to change are the message bytes. They will have to be converted to extended ASCII and then placed at the correct location within the data stream. I also want to stress that you don't have to change the value of the message length byte to the exact length of the message, since AoC doesn't seem to check that and it has no effect when the message is displayed. I would recommend setting it to a value that AoC accepts (`00` to `41`) though because I haven't tested all possible values for that byte thoroughly. Forged messages can be longer than the maximum of 65 characters.

## Execution

Once the data bytes of the packet are ready, we can simply copy them into the HEX field of Packet Sender. Then we set the target IP address and port number to the receiver's address and port. The AoC standard port is `2350`, but can vary depending on which services run on the receivers computer. Before sending, the port should be checked in Wireshark first.

After sending the packet, the receiver will see the message on screen when Counter 2 of the sync packet reaches the value chosen in the message packet. The player which IDs and player number was spoofed will not see the message, if it is not also sent to him.

## Example

As an example we want to send the german original first formulation of the categorical imperative by Immanuel Kant to Player 1. This is not achievable when in game, since our message is 110 characters long (AoC limit: 65 characters). Hence, if Player 1 sees this message, we know that spoofing is possible.

1. Firstly, we create our message with the spoofed parameters:

```
0000 c0 13 0d 00 00 00 00 00 43 02 7b 00 dc 15 00 00
0010 01 37 00 00 02 00 59 59 4e 4e 4e 4e 4e 4e 32 00
0020 03 00 00 00 00 48 61 6e 64 6c 65 20 6e 75 72 20
0030 6e 61 63 68 20 64 65 72 6a 65 6e 69 67 65 6e 20
0040 4d 61 78 69 6d 65 2c 20 64 75 72 63 68 20 64 69
0050 65 20 64 75 20 7a 75 67 6c 65 69 63 68 20 77 6f
0060 6c 6c 65 6e 20 6b 61 6e 6e 73 74 2c 20 64 61 73
0070 73 20 73 69 65 20 65 69 6e 20 61 6c 6c 67 65 6d
0080 65 69 6e 65 73 20 47 65 73 65 74 7a 20 77 65 72
0090 64 65 2e 00 00 00 00 01
```

2. Next up, the data bytes have to be pasted into Packet Sender. Target IP will be `192.168.160.2`, the IP address of Player 1. AoC runs on port `2350`.

[Screenshot](images/aoe2-message-spoofing1.png)

3. We will see the packet in Wireshark when it is sent. The value of Counter 2 is about 128 ticks behind the one in the latest sync packet. It will therefore take another 16 seconds to be displayed.

[Screenshot](images/aoe2-message-spoofing2.png)

4. A few seconds later and the message indeed has arrived for Player 1 and will be displayed.

[Screenshot](images/aoe2-message-spoofing3.png)
