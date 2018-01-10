# Multiplayer

## Idea

People like collaborating and sharing scenarios, but strangely enough the number of games who allow simultaneous editing over the network is close to zero. The openage editor should different and designed to be used by multiple people. A lot of inspiration can be taken from Minecraft where "editing" a map is the whole point of the multiplayer mode.

## Features

* Editor runs on the client, server distributes changes to everyone connected
* Map status is saved on the server for download
* Map is saved on the client for offline editing (changes could be pushed to the server, similar to `git`)
* Simple rights management (view, place, delete)
* Advanced rights management
    * Users can be allowed or denied to place/delete certain objects
    * Users can be allowed or denied to use certain editor functions (scripting, terrain editing)
    * Put users in groups
    * Designate zones on the map which can have their own rights management
* Spectator mode
* Cloning the map
* Save multiple release versions on the server
* Password protected access
* Let users attach notes with explanations to objects
* share editor config/quickbar/whatever with other users

## Optional

* Web Interface to edit maps when you're bored at work
* Version control with git
* Automatic upload to aok.heavengames.com, openage map repository
