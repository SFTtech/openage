Milestones
==========

1. stuff
2. magical stuff
3. truly awesome stuff
4. ask the community for more awesome stuff to do
5. ask the community to do the awesome stuff for us

Ahem, nope, this is the real roadmap:

- [x] 0: decide on the technology and architecture we're using.
    * language, libraries, build system, coding standards, ...
    * may be incomplete, will be changed later on anyway
- [x] 1: Create a database format for game data ([nyan](https://github.com/SFTtech/nyan))
- [x] 2: Renderer using OpenGL and Vulkan ([code doc](https://github.com/SFTtech/openage/tree/master/doc/code/renderer))
- [x] 3: Event system for the game simulation
- [x] 4: Design a modding API for the engine ([reference sheet](https://github.com/SFTtech/openage/tree/master/doc/nyan/api_reference))
- [X] 5: Convert AoE2 game data to our engine formats ([code doc](https://github.com/SFTtech/openage/tree/master/doc/code/converter))
- [ ] 6: Gamestate calculation with curves logic ([blogpost](https://blog.openage.sft.mx/t1-curves-logic.html))
- [ ] 7: Gameplay simulation! Where everything comes together
- [ ] 8: Multiplayer

things to be done sometime:

* game recording, playback
* lobby + lobby server
* implement AI Python API (use GlaDOS?)
* messaging and taunts (important!)
* random map generation (RMS scripts?)
* age2 map reader
  * scenario reader
  * maybe even the singleplayer campaigns

things to keep in mind:

* performance (responsiveness, fps, needed resources)
* high-quality code
* having fun!
