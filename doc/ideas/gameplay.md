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
 - Gaia flock of birds rises up from a tree when startled, when tree is felled or when many people approach and fly off-map, sound indicator hearable a little bit more far (players know whensomeone goes near a woodline at your starting base -> maybe towering?)

### Gaia Units

 - Bear
   - could have forage sights, farms, and shore fish as locations that it will try to “eat” from

 - Mercenaries could be hired in the wilderness at a camp to act on behalf of a player
   - not just buying a unit for gold, resources have to be determined
   - not really another unit to control for a player
   - could be kind of an extra scout, if you lose yours
   - or three units relatively weak but in replacement of the militia
     - you can send them to raid to your oppponent

  - Wolves
    - will form a pack and will act together
    - Wolves could wander the map, making efforts to avoid (circle around) “towns”
    - they should get hungry and seek food (deer, villagers, wild boar, bear, etc.)
    - while hungry, they should attack in a pack, kill something, and “eat”.
    - when not hungry, there should be a chance of attack but it should be more likely that the wolves will avoid contact and
move on
    - Wolves will howl from time to time. Players will be able to judge the severity of their “wolf problem”
based on the frequency of these howls

Resources
---------

### Infinite regeneration

 - Forest regeneration or seeding
 - Animals reproduce as long as there are two of the same kind left on the whole map

### Stone balancing

 - Stone cost for all buildings, rebalance stone amount per pile
 - Maybe new deep stone mines for later ages

### Salvage

  - salvage pile containing 30% of the resources originally used to construct a building appears any time
a building is destroyed or deconstructed
  - resources in this salvage pile will gradually diminish at the rate of 1 unit of resource per 10 seconds of game time
  - resource piles can be harvested by villagers or pillage capable units

### Deconstructing buildings

  - Buildings can be deconstructed to recover raw materials
  - and also deleted: doesn't recover material, but is faster


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


Capture buildings
--------
  - when a building is damaged to 20% or less, buildings will eject any units garrisoned,
cancel all research or training and cease any inherent behaviors (i.e. towers will no longer attack)
  - a building which has been damaged to 20% HP or less will become the property of the next player who repairs it to a level
above 20%


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

### Tournament mode

 - tournament settings (SQ/MQ, etc.) are stored in a container (importable file/or directly in the server environment depending on the architecture of the multiplayer)

   - the container stores direct link to download maps from an OpenAge community servers for this tournament (no p2p-sharing of maps with different versions anymore, still possible in private games)

   - container will contain also the drafting, which will become static during ongoing tournament
     - before the tournament the civ, team and map-drafting is handled directly by the community server and stores information inside the container
     - admin changes still possible for special use cases
   - when starting the tournament every player will download the important information from their pre-chosen and fixed parts and everything will be pre-configured and grey (if no spontaneous changes are allowed)

   - this brings the possibility to bring all the systems taking part in the tournament to one comparable and static setup (no admin RE because of forgotten MQ anymore)

   - everything regarding the tournament (replays, civdrafts, etc.) should be stored in this container after the event (so you could easily import it into your OpenAge and replay games -> also see 2. Spectating/Casting-Mode)

 - as an organiser of a tournament you should have the possibility to manage the whole tournament inside the OpenAge platform (not: making promo there, drafting here, hosting matches there, and so on)

   - also the possibility to directly invite a player from your OpenAge-account based on characteristics (like direct-invites for 1v1-Top15 Ladder-Tournament or Teamgames)
     - teams can connect their accounts to real clans reaching together a team elo depending on the combination of actual players (TatoH + DauT + TheViper > TatoH + DauT + Slam)

 - make it easier to qualify for your tournament (e.g. as a potential player -> Open "Tournaments" in multiplayer menu -> open "Qualifiers" -> Play games with fixed tournament settings -> maybe qualify for Tournament)

 - After clicking on the Tournament-Menu-Item you could have an overview of upcoming and open tournaments, with a calendar view of the next tournaments, a timeplan, the brackets and maybe even the possibility to one-click-spectate a Live-Tournament if you want (see 2. Spectating/Casting-Mode)

 - depending on the architecture of the multiplayer it could be even possible to spare out all that client modeling and just integrate an basic in-game browser which connects just to an OpenAge-community-server and from there you could manage everything. To play a game the user could just click on a link from the game browser which gives data to an internal protocol like openage://<game-id>/<foo>/<bar>/<link-to-settings-file>/<...>

     - this would result in a thinner client
     - but: taking too much multiplayer features out of the client leaves it helpless for private matches -> thin grade


### Spectating/Casting

 - the casting mode could be an advanced spectating mode, with more features than just replaying/live-spectating the game

 - ZOOM! (also in-game not just for casters)
 - maybe even turning the game in 90° angles
   - the old models could stay 2D and just be fit into the turned environment(?)

 - the feature set of CaptureAge could be understood as a spectator mode
   - if they add Picture-in-Picture and replay-functions goes more into direction what's understood in this context as casting mode

 - casting mode should help you to either work on your own or collaboratively work together with one or more Co-Caster(s) on the presentation of the actions in a game
   - casters should have the possibility to join a live-game together as a caster-team
   - e.g. two separate OpenAge-accounts could be temporarily bound together (master-slave-principle)
     - they both work together at the master-device (streaming device from main caster), each caster stil has it's own independant view of the game

   - there should be an option to cut scenes together in a live game, so that if one caster sees an action somewhere on the map, he/she gives a command to the game that itself clips (saves camera position and game status) it and puts it into a queue on the master-device to replay it

   - outside of the main spectating window could be a second window (used for two monitor solutions) with the waiting list for the replays and all the other options and features special to the casting mode

 - in-game lag-free voice-communication between casters


### Team interaction

 - Better team interaction
 - Work/resource sharing
 - Unit sharing (transport boats)
  - give temporary unit control of a selected group to an ally (e.g. in a fight) with an easy command
 - Give resources of resigning players to allies
  - maybe even the research of team ressources in late-imp
  - or could be even a 5th "basic globalisation/Hanse/free trade" age where allys share ressources
  - when you resign, give your buildings to your allies (original AoE devs idea)
 - Color markings
 - Create signs
 - Paint on map
 - Create arrows
 - in-game lag-free voice-communication between teamplayers?

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
 - "terraforming"-style
   - build a construction into the sea and build docks & harbours at it
 - Long build time per tile
 - wooden bridge with less HP, stone bridge with more HP and faster walking speed?

### Streets

 - Ability to build streets on land
   - textures already exists
 - increases the speed of trade carts, vills, siege or in general land units
 - destructible just by mangonel-line or trebuchet (ground attack units) and maybe decay over time or usage, repairable (or not?)
 - uses stone, maybe 1/3 the cost of a stonewall

 - Ideas of the original developers:
   - Roads improve trade by allowing trade units to move more quickly over the terrain
   - Neutral to all players
   - Build automatically
   - Progressive
   - Decay when not used
   - Appearance and quality depends on traffic (matted grass, dirt path, etc.)

New Units
-------------

### Water

 - monk-ship to convert ships on water

### Land

 - transport waggon to slowly transport ships over land from one pond to another


Relics & Kings
---------

 - Relics could have special abilities like in AOM
   - e.g. they can have attack bonuses (patches) for special units or economic/military bonuses
   - special abilities for every relic could either be generated when the map is generated or when the relic is discovered based on the actual needs of the player
   - so the later you go out to get the relic the more it could get useful for you, because it could be better shaped on your personal military/economy but the risk is higher, that another player was going out before you
   - if a player scouts the relic first and the ability gets generated in this moment, it will be for the player who scouted it first, so he knows, that this relic could help their own economy/military alot so the player will try to fight about this relic against the enemy heavily -> new gameplay aspect

  - Relics could have ranged attributes
    - idea of the devs of AoE II
    - bring the relic into the battle
      - attributes with up to 12 range could have an big impact of the fight

  - Kings could be special units that abstractly represent the player in the game
    - could be better than average infantry units, could provide a +1 bonus to attack and defense for all of their units (not allied units)
within a 5 tile radius
  - Queen units with an somehow identical functionality, may also be added to the game



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
https://www.reddit.com/r/aoe2/comments/bg4p3m/spirit_of_the_law_stealing_villagers_and_other/



 - They considered having gaia mercenaries who could be hired to fight on the player's behalf (apparently using their own AI rather than direct control)
 - Destroyed buildings were going to leave behind "salvage" that villagers or pillagers could harvest
 - Faith allowed you to START converting enemy units, rather than providing resistance (gameplay change, first a monk is used to collect relics, after researching it can convert units)
