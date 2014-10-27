AI Interface 
----
This is a very early draft with some ideas taken from http://bwmirror.jurenka.sk/javadoc/bwapi/package-summary.html

---


**c++ engine should trigger events in the python AI interface**

Python interface basic events:
* onStart(GameInfo info)
* onFrame(Game game) 
* onEnd(Player winner)

Some AIs may prefer an event driven approach (rule based AI): 
* onUnitDiscover(Unit unit)
* onUnitComplete(Unit unit)
* onUnitLost(Unit unit)
* *(many more)*
    

Some AIs will need to simulate ticks into the future to help make decisions for the current tick. (min-max, MCTS, etc.)

---

####GameInfo structure:
* int num_players
* int map_size
* int pop_limit
* Resourses_Type
* Map_View
* Starting_Age
* Victory_Mode

####Game class:
* Player[] allies() -> all the ally players that have not left or been defeated.
* bool canBuildHere(Unit builder, TilePosition position, UnitType type)
* bool canMake(Unit builder, UnitType type)
* bool canResearch(Unit unit, TechType type)
* bool canUpgrade(Unit unit, UpgradeType type)
* Player[] enemies() -> all the enemy players that have not left or been defeated.
* Unit[] getAllUnits() -> Returns all the visible units.
    
    *(many more examples at: http://bwmirror.jurenka.sk/javadoc/bwapi/Game.html)*
 

####Player class:
* string getName() -> Returns the name of the player.
* Race 	getRace() -> Returns the race of the player.
* Unit[] getUnits() -> Returns the set of units the player own.
* bool hasResearched(TechType tech)
* int getWood() -> Returns the ammount of wood the player owns.
* *(many more)*

####Unit class: 
Used to get information about individual units as well as issue orders to units

####Calculations class:
Used to get computation heavy calculations from the c++ engine.
    
    
