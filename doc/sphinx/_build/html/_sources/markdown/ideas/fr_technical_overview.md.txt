Technical overview for requested features
--------

These are some thoughts for a feature rich mode. Don't worry, there will be a vanilla mode without all that fancy stuff just feeling as good as this famous 20-years-old game. So every fancy feature changing the gameplay drastically could be also disabled by users.


### Explanations:

1. ITEM: A description of the item
2. COMPLEXITY: subjective evaluation for the complexity of the implementation
3. AFFECTED ENGINE PARTS: core engine modules that needs to be worked on, e.g. renderer, API, converter
4. IMPLEMENTATION STATUS: links to pull requests or dependencies
5. MILESTONE: Which milestone this feature will appear in


|   Type           |  Task No.  |   Item        | Comp            | Aff. Engine   | Imp. Status   | Mile           |
| ---------------- | ---------- | ------------- | --------------- | ------------- | ------------- | -------------- |
GAIA               | G1         | Dynamic change of weather, seasons, day and night. Influence on terrain & line of sight (LOS). could be influenced by players with monk/myth unit.
GAIA               | G2         | Wooden buildings and other flammable material can be lit, fire spreads around your city. fire departments (new building) to extinguish fires or villagers need to put out a fire of with water from a well (new building)
GAIA               | G3         | Wild animals can steal food from your farms and food resources
GAIA               | G4         | Wolves form pack and act together, wander the map, circle around towns, get hungry and seek food (deer, villagers, wild boar, bear, etc.).
GAIA               | G5         | Renewable resources such as trees and animals that repopulate. Forest regeneration or seeding.
GAIA               | G6         | Buildings in later ages cost stone with possibility of deep mining stone. villagers can dig/mine natural resources.
GAIA               | G7         | Better fauna (Tapir, Tiger, ...). Improved animal colours
GAIA AI            | GI1        | Fish migrate and replenish.
GAIA AI            | GI2        | Mercenaries could be hired on behalf of the player
Gameplay           | GM1        | Roads on commonly traveled areas allow faster travel. Pathfinding would let the units stay on the road. Appearance and quality depends on traffic (matted grass, dirt path, etc.). Neutral to all players. Turns soft/regular terrains into roads, requires stone, does not work on irregular terrains; grants movement speed to all allied units + passive vision (like a foundation spots units walking over them).
Gameplay           | GM2        | Bridges for connections between islands. Blocks ships, allows land units to walk on. Terraforming. Wooden and stone bridges with different hp.
Gameplay           | GM3        | Canals: creates empty, shallow and navigatable water.
Gameplay           | GM4        | Single units can hide/bunk in the forest, only visible to a certain type of unit. Garrison function with no symbol, you can lose your units if you don't remember them.
Gameplay           | GM5        | New Unit: spies (can burn down your city), lame your production
Gameplay           | GM6        | New Unit: monk ship on water to convert ships
Gameplay           | GM7        | New Unit: transport waggon on land to slowly transport ships over land
Gameplay           | GM8        | Villagers need sleep at night, die at work (New Building: Caffeine industry)
Gameplay           | GM9        | GAIA and terrain interaction, e.g. flock of bird rises up from tree when tree is felled or many people approach, hearable sound indicator, players hear and see birds when someone comes to their woodline at home. Triggered rocks falling of cliffs.
Gameplay           | GM10       | Relics have special abilities (AOM), bonuses for attack and range. Generated randomly. Maybe also rolling relics with ranged attributes to bring into battle.
Gameplay           | GM11       | Bring Kings/Queens into battle for special abilities within some tiles radius.
Gameplay           | GM12       | Building produces in loop (AOM) as long as resources and pop space are available. Useful for unit prudction and farms in mill. Indicator over buildings or somewhere in the GUI for loop production. Toggle reseeding on/off.
Gameplay           | GM13       | Mills can produce sheep, cows, turkeys
Gameplay           | GM14       | "Conversion is enabled by researching Faith. Before, monks can only carry relics and heal units.
Gameplay           | GM15       | Buildings can be deconstructed to recover materials, slower than destroying. Leave a salvage pile behind. Salvage diminishes over time. Salvage can be collected by villagers at a higher rate than other resources.
Gameplay           | GM16       | Buildings can be captured when damaged to 20% or less. Player who repairs over 20% owns the building. New units can be produced out of this captured building from 50% on.
Gameplay           | GM17       | Attack/defense bonuses for some unit formations. Dependent on minimum amount of units in this formation. New marching formation with speed bonus but defense malus. Cavalry flanking.
Gameplay           | GM18       | Researches only affect newly produced units. Old ones need to be manually upgraded/retrained at production building.
Gameplay           | GM19       | Scale wonder cost to max pop, number of players and available resources.
Gameplay           | GM20       | A new 5th "free trade" age where allies share resources and give each other massive battles.
Gameplay           | GM21       | New unit: Lance Cavalry, run over units in defending formations
Gameplay           | GM22       | Researches can be globally and locally applied. e.g. upgrade just units garrisoned in the barracks for less research cost; e.g. you need 3 pikemen for the battle now, but you don't want to produce them in the future in masses (see GM18)
Gameplay           | GM23       | Units can level up by gaining experience doing what they normally do, fighting in battle against their counter unit will give more XP against that unit style. Villagers chopping a Forest Nothing map might be a real pro at chopping trees at the end of the game e.g. one experienced villager could be as productive as three unexperienced villagers in the end of the game.
Gameplay           | GM24       | Queue for technology researches.
Gameplay           | GM25       | Farms cannot be placed on desert ground.
Gameplay           | GM26       | Improved water battles.
Gameplay           | GM27       | Percentage based armor system similar to AoEO and AoM. Will need to rebalance whole gameplay.
Gameplay           | GM28       | Entirely rebalance milita line. Main gold unit similar to knights/archers. Instead of a counter unit currently.
Gameplay           | GM29       | Nerf the range on towers early and make them scale much better into the late game.
Gameplay           | GM30       | Unit queuing doesn't block resources. Resources are taken in account, when the unit is actually starting to be produced.
Gameplay           | GM31       | Make denying upgrades more easy, like in SC2. Often players will destroy buildings to cancel an upgrade in progress and gain the upgrade advantage. Maybe in a special mod with less hp for buildings.
Gameplay           | GM32       | Possibility to put archers/ranged units on a wall/castle to get elevation bonus.
Gameplay           | GM33       | Quickwalling: first XX% (variable percentage) of a foundation is walkable, setting for e.g. tournaments
Gameplay           | GM34       | Mesoamerican civilizations (already the most novel civs in the game) may have a unique tech tree. Possibility for a unique tech tree in API.
Gameplay           | GM35       | Controlled forest fires: Allows the player (technology researched at the university in the castle age) to destroy forests to create more space; this goes much faster/efficient than chopping, but obviously at the expense of the lumber and generates regular terrain.
Gameplay           | GM36       | Give siege rams an upgrade that prevents unit production while it's attacking a building. (5-second debuff that refreshes each attack).
Gameplay           | GM37       | Bigger maps, map sizes between giant and ludicrous
Gameplay           | GM38       | Foundation: turns regular terrain into solid terrain, requires stone, turns irregular terrain into regular terrain.
Gameplay           | GM39       | Different terrain types, take a look into gameplay.md for more explanations
Gameplay           | GM40       | Be able to temporarily toggle the map to remove terrain and show building footprints for walling without gaps.
Gameplay           | GM41       | Make rams push units away
Gameplay           | GM42       | Raise the amount of ressources that can be sold with hotkeys at the market (forest nothing)
Gameplay           | GM43       | A special flag that raises on a building that is researching a technology, much like the garrison flag. (see GR10)
Unit AI            | UAI1       | Improved grouping. Double click on a grouped unit in group formation selects same group not all units of same kind. Allow more than 10 groups. More intelligent assigning of units to existing groups. Subgroup selections. New unit to group takes same stance as the whole group. Multi-group functionality.
Unit AI            | UAI2       | Designated unit pings its position periodically.
Unit AI            | UAI3       | Deselect certain units quickly from a selection by right clicking their portrait.
Unit AI            | UAI4       | Better waypoints (AOM-style) - able to queue tasks for units, especially villagers.
Unit AI            | UAI5       | More intelligent units.
Unit AI            | UAI6       | Emergency evacuation points.
Unit AI            | UAI7       | Dynamic hardlocking on target units when attacking.
Unit AI            | UAI8       | Aggressive stance for monks (auto-convert). Auto-flee stance. Non-coward stance for villagers. Allow selecting the default stance in production buildings.
Unit AI            | UAI9       | Set rally point on moving units.
Unit AI            | UAI10      | Better attack move.
Unit AI            | UAI11      | Option to allow a unit to auto-explore the map. Can be deactivated in the lobby options.
Pathing            | PA1        | Groups can move as group or on command each with its own movement speed.
Pathing            | PA2        | Auto-resolution of blocking situations
Pathing            | PA3        | Individual path searching cost for tiles. Allow users to manually mark parts of the map as more expensive/No-go areas. e.g. firing range of castles.
Pathing            | PA4        | Allow setting any number of waypoints (basically an extension of the 'patrol' mechanic)
Pathing            | PA5        | Formations and movement when attacking.
Multiplayer        | MP1        | Finegraded options in Lobby-Settings (e.g. formations, allow/disallow functions). Vanilla mode to have them good ol' feelings. Disallow also units and buildings, e.g. bombardtower, walls, etc.
Multiplayer        | MP2        | Boosts/handicaps such as resource multipliers for players
Multiplayer        | MP3        | Forced no-rush (time limit or score limit until attacks are possible)
Multiplayer        | MP4        | Official accounts on servers for everyone. PGP-Support for network of trust. Built-in multiplayer mode that is so good that it unifies the community.
Multiplayer        | MP5        | Good matchmaking and skill-/experience points-algorithm. Ranked mode for multiplayer.
Multiplayer        | MP6        | Better team interaction. Work/resource sharing. Temporary unit control of groups for allies. Resource and unit sharing when defeated or drop. Alternatively if a player drops all of their units move to an ally and that ally's population cap changes. Cannot work when diplomacy is enabled.
Multiplayer        | MP7        | Tournament drafts, files, settings, replays are stored in a container. Host sets up the game, shares important (players) part of the container, game lobby allows no changes.
Multiplayer        | MP8        | Simple and fast qualifying for tournaments.
Multiplayer        | MP9        | Connection through in-game server browser/direct IP and connection through a browser-based protocol. Combination of dedicated servers, P2P and LAN multiplayer.
Multiplayer        | MP10       | In-game, lag-free, high-quality voice-communication between teams, same for casters.
Multiplayer        | MP11       | Figure out how to measure map control of someone and give a value for it. Show it in casting and spectating multiplayer games.
Multiplayer        | MP12       | More then eight player slots for multiplayer games (planned up to 40)
Multiplayer        | MP13       | More balance changes regularly. Mechanism to make changes globally easier. Update mechanisms. Balance crew. Meta adjustments.
Multiplayer        | MP14       | Opt-in spectating: You can join a multiplayer even if it already started. Doesn't take a player slot.
Multiplayer        | MP15       | Multiplayer mods looking for smurfs, trolls, racists to ban them.
Multiplayer        | MP16       | Game modes, See gameplay.md
UI                 | UI1        | Organizers can manage the whole tournament in the openage interface within their accounts. Drafting, (Time-)Planning, Invitations, Promo, Hosting.
UI                 | UI2        | Multiplayer submenu should give you relevant information about most viewed live games and caster channels, upcoming tournaments, upcoming qualifier rounds.
UI                 | UI3        | Spectator layer for public watching. Lock to casters viewpoints or freely move camera. Chat with spectators and mark points of interests (POI) on the map. No sync needed. Spectator dashboard.
UI                 | UI4        | Casters interface: Picture-in-picture mode and replay function (command for making a clip and queue it). Pinging on minimap. Marking POI. Casters views need to be synced. Show casters current camera positions on minimap. See which units are visible to players without FOG. Second window for casting preferences and overviews. Caster dashboard.
UI                 | UI5        | APM should be derivable for interfaces and also be show in the GUI.
UI                 | UI6        | Customizable hotkeys for everything. But good hotkeys from the beginning.
UI                 | UI7        | Help spotting units which are target  of a conversion. Highlight monk and unit.
UI                 | UI8        | Better after-game analysis. Normal mode with the basic information. Advanced mode with a massive amount of statistical infos and easy to read diagrams for smallest things. Population graphs AOE3-style at the end of a match. Show number of villager kills and lost resources (villagers carrying resources), maybe even in-game (caster- and spectator mode).
UI                 | UI9        | Copy and paste for building structures, e.g. select mill and the farms around it, copy and paste the foundations of them somewhere else. Same for production lines. (Factorio-style)
UI                 | UI10       | Show research and production status in the top right corner to easier follow your running researches.
UI                 | UI11       | Show how many villagers are working on each ressource (like in AOM, 100 Gold / 4 \[Villager-Symbol\] --> 100 Gold in the bank and 4 vills working to mine gold).
UI                 | UI12       | Add resource gain per minute data next to the original resources. It would be something like food: 537 (+68). make it switchable with amount of vills working at all the resource spots. (see UI11)
UI                 | UI13       | Hotkeys for technology researches and upgrades for units
UI                 | UI14       | All these things in here should be switchable-on/off in the options.
UI                 | UI15       | MP-GUI: game-browser in list-style but with HDs lobby filtering, amount of players? search bar for game names. better filters for games.
UI                 | UI16       | Save Camera positions: Ctrl+F1 to create a camera location hotkey similar to how you create control groups. Whenever you press the key it moves your screen to the exact location you created it. Also for spectating and casters.
UI                 | UI17       | Looking for a SC2-Interface in AOE-style? UI-Modding should be made easy.
UI                 | UI18       | Tooltips should be improved to properly showcase all the hidden information and bonuses for players who don't want to deeply research every facet of the game by themselves.
UI                 | UI19       | Host of lobby to ban civilizations, give handicaps
UI                 | UI20       | Idle villager button change colour when a villager goes idle
AI                 | AI1        | Handle units more intelligent.
AI                 | AI2        | Don't build in hazardous areas. Send military to clear out.
AI                 | AI3        | Better and more intelligent building placements, e.g. build more gates to prevent lock-ins. Better castle placement. Better market placements for trade in the late game.
AI                 | AI4        | Military units shouldn't fight in castle arrow fire.
AI                 | AI5        | Units marching react and don't just go away.
AI                 | AI6        | AI lames you (as the Barbarian AI). Understands how to do economic damage to your base. Adopts more to the current playing meta and also randomly choses to use older metas.
AI                 | AI7        | AI can deliver villagers with transportships to other islands.
Graphics           | GR1        | Zooming in and out with different settings: middle of screen, where mouse points, to selected units.
Graphics           | GR2        | Units throw torches to put buildings on fire instead of fighting against them with their swords. Villagers have different attack animations depending on where they were working before attacking units/buildings, e.g. farms -> hay fork, lumberjacks -> axe, stone or gold -> pickaxe.
Graphics           | GR3        | Animations for construction and destruction of buildings.
Graphics           | GR4        | Unique military and economic unit skins for each different region. Knights from Europe shouldn't look like the ones from Japan, but should be similar enough to tell they are both knights. At least 4 regions for unit graphics.
Graphics           | GR5        | Beautiful weather effects (see G1)
Graphics           | GR6        | Campaign cutscenes
Graphics           | GR7        | Some graphical optimisations for competitive play by design, e.g. small trees, alignment grid, fish outlines, short walls, etc., other option: to redesign everything that the problems why those mods exists are fixed. e.g. make the look behind walls better by design -> no short walls needed.
Graphics           | GR8        | Resource spots change size based on the amount of resource they have left.
Graphics           | GR9        | Sync ram attack animation with the actual attack.
Graphics           | GR10       | A special flag that raises on a building that is researching a technology, much like the garrison flag. (see GM44)
Sound              | SO1        | Options to change the volume of special game sounds (e.g. attack bell, farm depleted, units produced). In late game this sounds can get really annoying.
Campaign           | CP1        | Co-Op Campaigns
Localisation       | LC1        | Placeholder
Modding            | MD1        | Placeholder
