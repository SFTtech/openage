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
 - Chemistry industry: Caffeine, Penicillin, ...?


### Forests

 - A single villager can hide (per some tile area) in the forest for sneak attacks. Maybe can only visible to a certain type of unit. But invisible to other villagers (until they pop out and start building stuff)


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

Similar to "Regicide", but with a building.
Players have some time to fortify their castle.
A player loses if the castle falls.

### Barbarian Invasion

An asymmetric game mode where 1 or 2 technologically advanced players compete against 3 - 6 "barbarians". The barbarians start in Dark Age with a few extra villagers, but have to build up their economy from the ground up. The "civilized" players start in Castle Age with a large amount of villagers, a decent economy and some defensive buildings. For barbarian players, the goal is to destroy a wonder in the civilized players' cities before a time limit is reached. The civilized players have to build defenses and try to protect against the hordes that are pressing at their gates.

### Phantom Mode

A mode similar to *Trouble in Terrorist Town* and *Secret Hitler*. The game starts with all 8 players being neutral or allied. Their goal is to find 2 "phantoms" and eliminate them. The 2 phantoms are allied from the start and know about each other, while every other player is clueless whether other players are friendly or phantoms. Phantom players have to manipulate the others into distrusting their friends. Gameplay could be spiced up with feature like purchasing units which don't have player colours from a town in the middle of the map, sabotage units and limiting the allied line of sight gained from researching cartography.

### Pure Battle Mode

No buildings, just units. The game generates a map and players can choose a starting position. Then they have a few minutes and a set amount of resources to select an army composition and some techs. After the first phase is over they place their units on the battlefield and have to use what they assembled to destroy their opponent. Utilizing height advantages, microing and tactical positioning contrast the strategic decisions of creating the army. The player who destroys his opponent, inflicts the most resource damage to others or holds strategic positions
wins the battle.

Unit handling
-------------

### Improved grouping

 - Manually coloring units (e.g. for teammates)
 - "Mobile ping": designated unit pings its position periodically
 - Implicit group definitions/reselections (double-click?)

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
 - Scale wonder cost to max pop and number of players (make wonders great again)
 - We can show the world that competitive games can be free open source!

### Team interaction

 - Better team interaction
 - Work/resource sharing
 - Unit sharing (transport boats)
 - Give resources of resigning players to allies
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
 - construction per tile
  - build your own "tongue of land"
  - could build docks & havens at it 
 - Long build time
 - wooden bridge with less HP, stone bridge with more HP and faster walking speed?

### Streets

 - Ability to build streets on land
 - increases the speed of trade carts, vills, siege or in general land units
 - destructible just by mangonel-line (ground attack units) and maybe decay over time or usage, repairable
 - uses stone, maybe 1/3 the cost of a stonewall

New Units
-------------

### Water

 - monk-ship to convert ships on water

### Land

 - transport waggon to slowly transport ships over land from one pond to another


Resources
---------

### Infinite regeneration

 - Forest regeneration or seeding
 - Animals reproduce as long as there are two of the same kind left on the whole map

### Stone balancing

 - Stone cost for all buildings, rebalance stone amount per pile.
 - Maybe new deep stone mines for later ages.

Production
---------

 - let the last produced unit in a buidling be produced for an infinite time (as long as ressources and pop space are available) --> AOM-style
 - same for farms (either reseed a certain number with a waiting line or an option "reseed forever" as long as ressources are available)
 - Mills can produce sheep, cows, turkeys, etc.

For the lulz
------------

 - Big fat damage numbers!!!1
 - Friendly-fire toggle!!111


Implement features the AoE-devs didn't do
------------
See here:
https://de.scribd.com/document/318886164/AGE-2-Design-Document#browse-all-btn
https://www.reddit.com/r/aoe2/comments/bg4p3m/spirit_of_the_law_stealing_villagers_and_other/

 - They considered having gaia mercenaries who could be hired to fight on the player's behalf (apparently using their own AI rather than direct control)
 - Destroyed buildings were going to leave behind "salvage" that villagers or pillagers could harvest
 - Faith allowed you to START converting enemy units, rather than providing resistance (gameplay change, first a monk is used to collect relics, after researching it can convert units)



