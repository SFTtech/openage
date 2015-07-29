Gameplay improvement ideas
==========================

This file contains ideas to "enhance" the original Age of Kings gameplay.
Don't worry, vanilla mode will always be available.


Environment
-----------

### Weather

 - Change view distance (Line of sight / Fog of War)
 - Fog, rain, winter, summer
 - Unit slowdowns (depending on terrain)
 - Can be predicted by tech
 - Can be modified globally or locally (by voodoo priests)

### Fire

 - Buildings made out of wood and other flammable material can be lit.
 - Fire departments in the city can extinguish those fires (Stronghold anybody?).
 - Sneaky spies could burn your city down

### Day/night cycle

 - At night: View distance reduced, everything darker.
 - Villagers need sleep, otherwise they die at work.
 - Chemistry industry: Coffeine, Peniciline, ...?


Map extensions
--------------

### 3D terrain

 - Villagers can dig down, can discover natural resources.
 - Dirt, stone, etc are new resources, can be placed elsewhere
 - (add any minecraft/terraria-like stuff here)

### Infinite maps

 - Maps with infinite size! (Your PC is the limit!)
 - Spherical/Toroidal/Cylindrical surfaces would be possible, too.


Research
--------

 - Only newly-built units receive the bonus when units are upgraded
 - Already-built units need to be "overhauled" for a cost back at the barracks.
   (reminder: just an idea, there will be vanilla mode)
 - Research should be placed in creating order, like in WarCraft. It's too annoying
   to click on building again after each research


Game modes
----------

### Zombie Survival Mode

The map has a bunch of zombie spawners; after an initial build-up phase,
they start producing waves zombie hordes, fast zombies, tanks, petard zombies;
the zombie AI tries to get to your villagers (magic pathfinding);
killed units turn into zombies as well. spawners can be destroyed,
but the closer or the more powerful you get, the more zombies will spawn.
To win, survive for a specified period of time, build a wonder,
or destroy the spawners (or something...).

### Conquer the castle

Similar to "Murder the King", but with a building.
Players have some time to fortify their castle.
A player loses if the castle falls.


Unit handling
-------------

### Improved grouping

 - Manually coloring units (e.g. for teammates)
 - "Mobile ping": designated unit pings its position periodically
 - Implicite group definitions/reselections (double-click?)

### Better movement

 - Move groups as group, or each unit with its own movement speed
 - Auto-resolution of blocking situations
 - Individual path searching cost for tiles
  - Allow user to mark parts of the map as more expensive
  - Auto-set a higher path cost for tiles in the firing range of an enemy castle
 - Allow setting any number of waypoints (basically an extension of the 'patrol' mechanic)
 - Formations:
  - Attack/defense bonuses for some unit formations.
  - Dependent on minimum amount of units in this formation.
  - New marching formation with speed bonus but defense malus.
  - Cavalry flanking

### More intelligent units

 - Restrictions for action areas (don't do anything here)
   - Wood chopping
   - Castle rampages
   - Avoid hazardous areas during pathfinding (via a cost modifier)
     - Automatically for stationary hazards (castles, towers, ...)
     - Manually "painted" areas
 - Emergency evacuation points
 - Formations and movement when attacking
 - Dynamic hardlocking on target units when attacking
   - Don't lock on unreachable current target
   - Attack the blocking units first

### Better attack stances

 - Aggressive stance for monks (auto-convert)
 - Auto-flee stance (especially for monks): Auto-task "flee" action as soon as an enemy unit comes near
 - Non-coward stance for villagers
 - Allow selecting the default stance


Multiplayer
-----------

### Lobby settings

 - Most of the above should be available as lobby settings
 - Boosts/handicaps such as resource multipliers for players
 - Forced no-rush (timelimit or score limit until attacks are possible)

### Matchmaking and competitive mode

 - Some kind of "official" account on servers for everybody
 - Automatic skill groups to create balanced matches
 - We can show the world that competitive games can be free open source!

### Team interaction

 - Better team interaction
 - Work/resource sharing
 - Unit sharing (transport boats)
 - Color markings
 - Create signs
 - Paint on map
 - Create arrows

### MMO games

Play on one single map with 9001 players. Dynamically extend the map.

Massively slow down the game speed, and you get a "browser game"-like experience.


New Buildings
-------------

### Bridges

 - Ability to build bridges in water
 - Blocks ships, allows land units to walk
 - Forgotten Empires might already have wooden bridge textures
 - Destructible, repairable, ...
 - Long build time


Resources
---------

### Infinite regeneration

 - Forest regeneration or seeding
 - Mills can produce sheep, cows, turkeys, etc.

### Stone balancing

Stone cost for all buildings, rebalce stone amount per pile.
Maybe new deep stone mines for later ages.


For the lulz
------------

 - Big fat damage numbers!!!1
 - Friendly-fire toggle!!111
