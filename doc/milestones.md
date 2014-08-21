Milestones
==========

Development plan:

1. do things
1. do more (magic) things
1. do absolutely awesome stuff
1. ask the community for more awesome stuff to do
1. ask the community to do the awesome stuff for us

Ahem, nope, this is the real plan:
List of milestones for the prototype releases:

1. text file that describes the technology (language, libraries, build system, coding standards, ...) we're using (may be incomplete, will most definitely be changed later on anyway)
1. applicatin runs, fullscreen, black, builds with build system (on GNU/Linux), _no_ menu, it directly launches into the game
1. render a single building (an university.), reading it from the stock age2 media files
1. render diamond-shaped map (monoterrain), with any number of buildings (of any kind, in grid), with mouse/keyboard scrolling
1. render units on map (with animations)
1. mouse selection and basic commands ("go here, delete")
1. path finding and collision
1. hud (lower control panel), could for example display unit name. hud for buildings supports unit creation
1. civ research state tracker, support for research in building hud
1. villager building menu for creation of buildings (instant)
1. realistic building creation (amount of work needed, building process)
1. resource system (and upper hud bar, creating units/building consumes resources)
1. resource gathering by villagers
1. health points, ballistics, attacking. units can be switched to different colors by short keys
1. terrain altitudes (plus effects on ballistics), working singleplayer without AI is done here. multiplayer stuff is the main focus now
1. dedicated server (!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)
   * lobby stuff not implemented yet
   * all clients connect to server (at the beginning only one client must be supported...)
   * initialization of session:
     * client: OHAI. i want session 23742, and be the blue player
     * server: <map blob> <plz wait for other clients to be ready.> <time synchronization> <go!>
   * game logic runs on both server and client, some serious syncing is done between them (important: physics frame IDs of user inputs)
1. support for multiple players
1. limitation of multiplayer players to one color
   * actually this is a small step, but there will be bugs, loads of them.
   * at this point, with a proper starting map (one villager per player on grasland), battles can be fought.


things to be done sometime:

* game recording, playback
* some rudimentary launch menu
* implement AI (use GlaDOS?)
* messaging and taunts (important!)
* client side prediction in multiplayer
* random map generation
* age2 map reader
  * scenario reader
  * maybe even the singleplayer campaigns

ideas 4 teh lulz:

* friendly fire enable button (lol)
* >64 player matches
  * more than 8 player base colors needed then

things to keep in mind:

* performance (responsiveness, fps, needed resources)
* exactly cloning age2 (also include the networking bugs, for original feeling!)
* good and funny code (coding standards..)
