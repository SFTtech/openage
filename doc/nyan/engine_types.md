Engine types
============

These types are defined by the openage engine. Every nyan object in the game
eventually inherits from one or more of these types.

Normally you'd store one or more *NyanObject*s in a *.nyan file, but as these
are defined in the engine and are here only for humans (like me, *beep blop*),
they are stored here for an overview.

Currently, this is a **proposal**.
This file does not perfectly (if at all) represent the already defined types in
libopenage, but hopefully will in the future. Many classes here, if not all,
are currently work in progress. Prepare for changes.

Shoutout:
* Everyone: Comment! Give us good ideas.
* libopenage-devs: change/erase this. You are the ones, who (are going to)
implement this and know what's necessary and what should be where.

```
Mod():
	# All patches in the 'patches' member are added on load time. So every mod
	# needs to inherit from this class and new patches to 'patches'.

	name : text
	patches : set(NyanPatch) # Add your mod's patches to this member.

Tech():
	# Every patch that needs to be evaluated after a specific event in the game
	# (e.g. research) needs to be added to 'updates'.

	name : text              # ingame name
	description : text       # ingame description
	updates : set(NyanPatch) # Add your new researches (or other patches) here.

#Proposal: call a `Tech' `Update' in nyan, and define Tech in openage like this:
Update():
	updates : set(NyanPatch)

HUDElement():
	#makes an element on the left of the HUD (building or unit menu)
	name : text
	description : text
	icon: text
Tech(Update, HUDElement):


Unit(): # WIP
	# Everything that has a location ingame.

	hp : float

Resource():
	name : text
	icon : file

ResourceSpot():
	# Anything with 'resource' in GatherAbility.allowedResource can gather this.
	# WIP: since this has a location ingame, make this inherit from Unit? or make e.g. Goldmine(Unit, ResourceSpot)?

	resource : Resource
	amount : int # initial amount
	decay : float # in units per second.


Building(Unit): # WIP: make another parent Object for Building?
	name : text


Animation(): # or Sprite() ?
	# Everything that needs to be animated inherits from this class.

	image : file # one picture that contains all frames or, if a directory,
                 # contains all frames. # WIP5: naming scheme?

#	metainfo : file # WIP6: how to read `animation'? describes, where in the big picture `animation' which frames are; whether to potentially mirror them.
	                # possibilities: 1. $renameMe is a regular file: description of `animation' (syntax?)
	                # 2. replace `renameMe' by more fields that describe `animation' (ugly imo, but maybe the best way)
	                # 3. remove `renameMe':
	                #    if $animation is a directory: look for '$animation/animate.nfo'
	                #    else: look for "$(basename $animation png)nfo"

Ability(): # WIP
	# Every unit (WIP: or even buildings, see 'CarryAbility',castles... ), that can do something, has an 'Ability'.
	# WIP: what members belong here?

HUDAbility(Ability, HUDElement): # WIP
	# Creates a button on the left of the HUD. For Trebuchet packing and
	# unpacking, unloading units or relics. Maybe even unit patrol andformations.

MoveAbility(Ability, Animation):
	# You want to move? Use this class.

	speed : float
	instant : bool = False # WIP: how about checking (speed < 0) ⇒ instant before
	                       # path searching?
	range : float

CooldownAbility(Ability):
	# Any executed 'Ability', that inherits from this, can only be executed again
	# after recharge_time in seconds.
	recharge_time : float

GatherAbility(Ability, Animation): # WIP
	# Gathering resources requires this Ability

	allowedResource : set(Resource) # Allowed ressources?
	maxAmount : int # maximum amount  that can be carried
	speed : float # gathered units per second

	# WIP2: proposal: allow different maximum amounts for different resources:

#	allowedResource : orderedset(Resource)
#	maxAmount : orderedSet(int) # does <10,5,0,10> work? it is a set after all.

	# WIP3: if WIP2 is not possible: implement an 'Ability' that allows to gather only one resource.
	# A worker would then need 4 gather abilities added to it's 'abilities' set.

CarryAbility(Ability): # WIP: find a better name?
	# Used by priests for relics and transport ship (both indirectly [animation needed]) and by castles etc. for many types of units.
	# WIP1: how should this look like? keep it general?
	# WIP3: HUDElement for releasing every contained unit and setting a meeting point needed. (But not for priests. -> Check for MovableUnit in allowedUnits?
	#       or use the alternative below for everything but relics.)

	allowedUnits : set(UnitTypes) # what can be collected?
	maxUnits : int
	maxUnitsize : int # maximum size of each unit. If we want this, every movable unit should have a size member declared by a base class.
	tooDamagedRate : float # percentage of health at which no units can be contained any longer.

# Alternative:
Shelter(Ability):
	# A 'Unit' which might hold other units. Used by Castle, Towncenter, TransporterShip, but not priest.

	allowedUnits : set(UnitTypes) # what can be collected?
	maxUnits : int
	maxUnitsize : int # maximum size of each unit. If we want this, every movable unit should have a size member declared by a base class.
	collapsingRate : float # percentage of health at which no units can be contained any longer.

AttackAbility(Ability):
	# Every attack shall inherit from this.
	# WIP: how will damage calculation work in oa?

	range : float
	rate : float
	damage : float # rename to baseDamage ?


HealingAbility(Ability): #WIP
	# Workers can repair buildings, ships and siege weapons. Buildings heal contained Units, Priests can heal moving units (including siege weapons?)
	# WIP: priests and workers need an animation for this, buildings not.
	# Should we write that as an attack with negative

	range : float
	rate : float
	damage/reversedamage

ResourceGenerator():
	# creates constantly resource. Used by relicts.

	resource : Resource
	rate : float

# Proposal
UnitTypes(): # Bad name, rename.
	# This class is used to group several ingame objects. This can be used for
	# research (e.g. boost all Infantry by +2 speed), AttackAbility (what
	# can (not) be attacked, CarryAbility and maybe more.

#and probably more. if you know what's missing, add it.

```

Open nyan implementation questions:

4. How do we add new animations?

  -> How would a mod add a new relict-like object that can be carried (the animation)? can we describe composition of frames? How is it done by the original game?

  -> We need a unified way/member-name to add a new animation to an 'Ability' -> let the ability inherit from 'Animation'

  -> animations are stored in one big picture (or allow one file per frame?). How do we tell where in the picture is which frame? How can you tell to mirror a frame?
5. Of what type are the ingame objects, that don't move and give ressources? goldmine, fish, dead deer? How are deer and dead deer connected?

Mod questions:
