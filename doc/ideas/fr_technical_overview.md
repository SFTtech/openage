Technical overview for requested features
--------

### Explanations:

1. ITEM: A description of the item.
1. COMPLEXITY: subjective evaluation for the complexity of the implementation
1. AFFECTED ENGINE PARTS: core engine modules that needs to be worked on, e.g. renderer, API, converter
1. IMPLEMENTATION STATUS: links to pull requests or dependencies 
1. MILESTONE: What milestone this feature will appear as complete.



 Type	        |  Task No.          | Item  | Comp | Aff. Engine | Imp. Status | Mile
 ------------- | ------------- | ------------- | ------------- | --------- | --------- | ---------
GAIA | G1 | Dynamic change of weather, seasons, day and night. influence on terrain & LOS. could be influenced by players with monk/myth unit.
GAIA | G2 | Wooden buildings and other flammable material can be lit, fire spreads around your city. fire departments (new building) to extinguish fires or villagers need to put a fire of with water from a well (new building)
GAIA | G3 | Wild animals can steal food from your farms and deers
GAIA | G4 | Wolves form pack and act together, wander the map, circle around towns, get hungry and seek food (deer, villagers, wild boar, bear, etc.).
GAIA | G5 | Renewable resources such as trees and animals that repopulate. Forest regeneration or seeding.
GAIA | G6 | Buildings in later ages cost stone with possibility of deep mining stone. villagers can dig/mine natural resources.
GAIA AI | GI1 | Fish migrate and replenish.
GAIA AI | GI2 | Mercenaries could be hired on behalf of the player
Gameplay | GM1 | Roads on commonly traveled areas allow faster traffic. Pathfinding would let the units stay on the road. Appearance and quality depends on traffic (matted grass, dirt path, etc.). Neutral to all players.
Gameplay | GM2 | Bridges for connections between islands. blocks ships, allows land units to walk on. terraforming. wooden and stone bridges with different hp.
Gameplay | GM3 | Mercenaries could be hired at camp in the wilderness to act on behalf of a player
Gameplay | GM4 | single units can hide/bunk in the forest for sneak attacks, only visable to a certain type of unit. garrison function with no symbol, you can loose your units if you don't remember them.
Gameplay | GM5 | New Unit: spies (can burn down your city), lame your production
Gameplay | GM6 | New Unit: monk-ship on water to convert ships
Gameplay | GM7 | New Unit: transport waggon on land to slowly transport ships over land
Gameplay | GM8 | villagers need sleep at night, die at work (New Building: Caffeine industry)
Gameplay | GM9 | GAIA and terrain interaction, e.g. flock of bird rises up from tree when tree is felled or many people approach, hearable sound indicator, players hear and see birds when someone comes to their woodline at home. triggered rocks falling of cliffs.
Gameplay | GM10 | Relics have special abilities (AOM), bonuses for attack, range. generated randomly. maybe also rolling relics with ranged attributes to bring into battle.
Gameplay | GM11 | Bring Kings/Queens into battle for special abilities within some tiles radius.
Gameplay | GM12 | Building produces in loop (AOM) as long as resources and pop space are available. units and farms. indicator over buildings or in the GUI for loop production.
Gameplay | GM13 | Mills can produce sheep, cows, turkeys
Gameplay | GM14 | Researching faith in third age allows to convert units, before monk can heal and carry relics.
Gameplay | GM15 | Buildings can be deconstructed to recover materials, slower than destroying. leave a salvage pile behind. salvage dimishes per time. resources can be harvested by villagers on a higher rate.
Gameplay | GM16 | Buildings can be captured when damaged to 20% or less. Player who repairs over 20% owns the building. New units can be produced out of this captured building from 50% on.
Gameplay | GM17 | Attack/defense bonuses for some unit formations. Dependent on minimum amount of units in this formation. New marching formation with speed bonus but defense malus. Cavalry flanking.
Gameplay | GM18 | Researches only affect newly produced units. Old ones need to be manually upgraded/retrained at production building. Queue for researchings in buildings.
Gameplay | GM19 | Scale wonder cost to max pop, number of players and available resources.
Gameplay | GM20 | a new 5th "free trade" age where allies share resources and give each other massive battles.
Gameplay | GM21 | New unit: Lance Cavalry, run over units in defending formations
Unit AI | UAI1 | Improved grouping. manually coloring units for teammates. designated unit pings its position periodically. double click on a grouped unit in group formation selects same group not all units of same kind. allow more than 10 groups. more intelligent assigning of units to existing groups. sub-group selections.
Unit AI | UAI2 | More intelligent units. Emergency evacuation points. Formations and movement when attacking. Dynamic hardlocking on target units when attacking. Aggressive stance for monks (auto-convert). Auto-flee stance. Non-coward stance for villagers. Allow selecting the default stance in production building. Set rally point on moving units.
Pathing | PA1 | Groups can move as group or on command each with its own movement speed.
Pathing | PA2 | Auto-resolution of blocking situations
Pathing | PA3 | Individual path searching cost for tiles. Allow users to manually mark parts of the map as more expensive/No-go areas. e.g. firing range of castles.
Pathing | PA4 | Allow setting any number of waypoints (basically an extension of the 'patrol' mechanic)
Multiplayer | MP1 | Finegraded options in Lobby-Settings (e.g. formations, allow/disallow functions). Vanilla mode to have them good ol' feelings.
Multiplayer | MP2 | Boosts/handicaps such as resource multipliers for players
Multiplayer | MP3 | Forced no-rush (timelimit or score limit until attacks are possible)
Multiplayer | MP4 | Official accounts on servers for everyone. PGP-Support for network of trust.
Multiplayer | MP5 | Good matchmaking and skill-/experience points-algorithm.
Multiplayer | MP6 | Better team interaction. Work/resource sharing. Temporary unit control of groups for allies. Resource sharing when defeated.
Multiplayer | MP7 | Tournament drafts, files, settings, replays are stored in a container. Host sets up the game, shares important (players) part of the container, game lobby allows no changes.
Multiplayer | MP8 | Simple and fast qualifying for tournaments.
Multiplayer | MP9 | Connection through in-game server browser/direct IP and connection through a browser-based protocol.
Multiplayer | MP10 | in-game, lag-free, high-quality voice-communication between teams, same for casters.
UI  | UI1  | Organizers can manage the whole tournament in the openage interface within their accounts. Drafting, (Time-)Planning, Invitations, Promo, Hosting.
UI  | UI2  | Multiplayer submenu should give you relevant information about most viewed live games and caster channels, upcoming tournaments, upcoming qualifieer rounds.
UI  | UI3  | Spectator layer for public watching. Lock to casters viewpoints or freely move camera. Chat with spectators and mark points of interests (POI) on the map. No sync needed.
UI  | UI4 | Casters interface: Picture-in-picture mode and replay function (command for making a clip and queue it). Pinging on minimap. Marking POI. Casters views need to be synced. Show casters current camera positions on minimap. See which units are visible to players without FOG. Second window for casting preferences and overviews.
UI  | UI5  | APM should be derivable for interfaces and also be show in the GUI.
UI  | UI6  | Customizable Hotkeys for everything.
UI  | UI7  | Help spotting units which are taget  of a conversion. Highlight monk and unit.
UI  | UI8  | Better after-game analysis. Normal mode with the basic information. Advanced mode with a massive amount of statistical infos and easy to read diagrams for smallest things.
UI  | UI9  | Copy and paste for building structures. e.g. select mill and the farms around it, copy and paste the foundations of them somewhere else. Same for production lines. (Factorio-style)
UI  | UI10  | Show research status in the top right corner to easier follow your running researches.
UI  | UI11  | Show how many villagers are working on each ressource (like in AOM, 100 Gold / 4 \[Villager-Symbol\] --> 100 Gold in the bank and 4 vills working to mine gold)
AI | AI1 | Handles units more intelligent. Don't build in hazardous areas. Send military to clear out. Build more gates to prevent lock-ins. Better castle placement. Better Trade. Military units shouldn't fight in castle arrow fire. Units marching react and don't just go away. AI lames you (Barbarian). Understands how to do economic damage to your base. Adopts more to the current playing meta and also randomly choses to use older metas. AI can deliver villagers with transportships to other islands.
Graphics | GR1 | Zooming in and out with different settings: middle of screen, where mouse points, to selected units.
Graphics | GR2 | Units throw torches to put buildings on fire instead of fighting against them with their swords. Villagers have different attack animations depending on where they were working before attacking units/buildings, e.g. farms -> hay fork, lumberjacks -> axe, stone or gold -> pickaxe.
Modding | MD1 | Asset modification updates in-game models instantly. Live testing for modders. More challenging: Same for unit (stat) patches.
Sound | SO1 | Options to change the volume of special game sounds (e.g. attack bell, farm depleted, units produced). In late game this sounds can get really annoying.
Localisation | LC1 |
