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



Resource():
	name : text
	icon : file

Building():
	name : text

Ability(): # WIP
	# Every unit (WIP: or even buildings, see 'CarryAbility',castles... ), that can do something, has an 'Ability'.
	# WIP: what members belong here?

HUDAbility(Ability): # WIP
	# Creates a button on the left of the HUD. For Trebuchet packing and unpacking, unit patrol, formations.
	name : text
	description : text
MoveAbility(Ability):
	# You want to move? Use this class.

	speed : float
	instant : bool = False # WIP: how about checking (speed < 0) ⇒ instant before
	                       # path searching?
	range : float

CooldownAbility(Ability):
	# Any executed 'Ability', that inherits from this, can only be executed again
	# after recharge_time in seconds.
	recharge_time : float

GatherAbility(Ability): # WIP
	# Gathering resources requires this Ability
	# WIP1: do we need a member for the current amount and type of resource here? or is that only inside the engine?

	allowedResource : set(Resource) # Allowed ressources?
	maxAmount : int # maximum amount  that can be carried
	speed : float # gathered units per second

	# WIP2: proposal: allow different maximum amounts for different resources:

#	allowedResource : orderedset(Resource)
#	maxAmount : orderedSet(int) # does <10,5,0,10> work? it is a set after all.

	# WIP3: if WIP2 is not possible: implement an 'Ability' that allows to gather only one resource.
	# A worker would then need 4 gather abilities added to it's 'abilities' set.
	# And this would allow to reference

	# WIP4: how do we reference Animations?
#	animation : file # one picture that contains all frames or, if a directory,
	                # contains all frames. # WIP5: naming scheme?
#	renameMe : file # WIP6: how to read `animation'? describes, where in the big picture `animation' which frames are; whether to potentially mirror them.
	                # possibilities: 1. $renameMe is a regular file: description of `animation' (syntax?)
	                # 2. replace `renameMe' by more fields that describe `animation' (ugly imo, but maybe the best way)
	                # 3. remove `renameMe':
	                #    if $animation is a directory: look for '$animation/animate.nfo'
	                #    else: look for "$(basename $animation png)nfo"

CarryAbility(Ability): # WIP: find a better name?
	# Used by priests for relicts.
	# WIP1: how should this look like? keep it general? can maybe be used by buildings to let units find shelter in them? or by mods for adding new relict-like objects
	# WIP2: can maybe merged with a modified GatherAbility?

#	goodMemberName : Portable # what can be collected? make a 'Portable' class and let relict inherit that? could then be also used by resources?
#	maxAmount

	animation : file # what does a priest look like when he's carrying it?
	                 # WIP: see GatherAbility.animation and nyan question 4 (further down).

AttackAbility(Ability):
	# Every attack shall inherit from this.
	# WIP: how will damage calculation work in oa?

	range : float
	rate : float
	damage : float # rename to baseDamage ?


Unit(): # WIP
	# Everything you can see in the game # (WIP: but not assets, which is basically everything you see. Maybe better:)
	# Everything that has some 'Ability' inherits from this.

	ability : set(Ability)



#and probably more. if you know what's missing, add it.

```

Open nyan implementation questions:

1. How do we say in Nyan, that a Towncenter attack depends on how many people are inside the building?
2. What about the building 'Ability' to carry units? will this lead to recursive references?
3. How do we reference hooks from Nyan/create new hooks in mods?

  -> If the {Towncenter,Castle} has certain damage, contained units need to leave.
  
  -> A relict in a Church leads to increasing gold.
4. How do we add new animations?

  -> How would a mod add a new relict-like object that can be carried (the animation)? can we describe composition of frames? How is it done by the original game?
  
  -> We need a unified way/member-name to add a new animation to an 'Ability'
  
  -> animations are stored in one big picture (or allow one file per frame?). How do we tell where in the picture is which frame? How can you tell to mirror a frame?
5. Of what type are the ingame objects, that don't move and give ressources? goldmine, fish, dead deer? How are deer and dead deer connected?

Mod questions:

1. Adding new units/buildungs: There are only blue units in the original assets. How does the engine now, what part of a frame needs new color?
