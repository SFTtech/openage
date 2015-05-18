Milestones
==========

1. stuff
2. magical stuff
3. truly awesome stuff
4. ask the community for more awesome stuff to do
5. ask the community to do the awesome stuff for us

Ahem, nope, this is the real roadmap:

- [x] 0: decide on the technology we're using.
   -> language, libraries, build system, coding standards, ...
   -> may be incomplete, will be changed later on anyway
- [x] 1: application runs, fullscreen, black, builds with build system (on GNU/Linux),
      _no_ menu, it directly launches into the game
- [x] 2: render a single building (university),
      reading it from the stock age2 media files
- [ ] 3: render diamond-shaped map (monoterrain),
      with any number of buildings (of any kind, in grid),
      with mouse/keyboard scrolling
- [ ] 4: render units on the map (with animations)
- [ ] 5: mouse selection and basic commands ("go here, delete")
- [ ] 6: path finding and collision
- [ ] 7: hud (lower control panel), could for example display unit name.
      hud for buildings supports unit creation
- [ ] 8: civ research state tracker, support for research in building hud
- [ ] 9: villager building menu for creation of buildings
- [ ] 10: realistic building creation (amount of work needed, building process)
- [ ] 11: resource system (and upper hud bar, creating units/building consumes resources)
- [ ] 12: resource gathering by villagers
- [ ] 13: health points, ballistics, attacking.
       units can be switched to different colors by short keys
- [ ] 14: terrain altitudes (plus effects on ballistics),
       working singleplayer without AI is done here. multiplayer stuff is the main focus now
- [ ] 15: **dedicated server** (!), map user actions to physics frame ids and sync them
- [ ] 16: basic multiplayer support
       * there will (most probably) be bugs, loads of them.
       * at this point, with a proper starting map (one villager per player on grasland),
         battles can be fought.

things to be done sometime:

* game recording, playback
* some rudimentary launch menu
* lobby + lobby server
* implement AI Python API (use GlaDOS?)
* messaging and taunts (important!)
* client side prediction in multiplayer
* random map generation (RMS scripts?)
* age2 map reader
  * scenario reader
  * maybe even the singleplayer campaigns

things to keep in mind:

* performance (responsiveness, fps, needed resources)
* exactly cloning age2 (also include the networking bugs, for original feeling!)
* good and funny code (coding standards..)
