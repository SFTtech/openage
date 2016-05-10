nyan data format
================

**nyan** is the recursive acronym for **nyan: yet another notation**. *nyan* is
a human-readable text data format used in openage to represent game assets 
informations. *nyan* is the data format in which game developers specify game 
entities characteristics such as units health points, abilities, technologies, etc.
*nyan* is designed for human readability, simplicity and moddability. It enforce
a minimum set of grammatical rules to achieve its goal.

By default, the game engine does not know anything about entities properties.
The nyan reader provides all those characteristics to the game engine. nyan is used
in different contexts :

- definition of game entities (unit, building) base properties such as health points, 
ressource cost, abilities, civilization informations, etc...
- definition of game modes taking the form of patch applied over the game entities 
base properties. Those patch or "deltas" are applied by the game engine during the 
game initialization. Modes consists of a bunch of patches and can be exchanged
without tweaking the game base properties.
- definition of technologies properties such as base properties enhancement
and ressource cost. These technologies benefits are applied at runtime (after the game
initialization) and may be refered as "runtime deltas". Like the game entities base 
properties, technologies can be patched without modifying the base properties.

*nyan* consists of two separate, but very similar file formats. First, there
is the nyan specification (short: nyanspec) which describes a domain of object
types. Then, there is the data file format itself (simply called nyan), which defines
a set of objects, with their attribute values.

nyan specification (nyanspec)
-----------------------------

The **nyanspec** is used to describe a domain of object types that exists and
can be used in nyan data files. A **nyanspec** is defined in files with the
extension `*.nyanspec`.

### Basic nyanspec definition

The **nyanspec** consists of definitions for multiple nyan types. Each object
type has a name and a finite set of named attributes. For example the following
part of a nyanspec describes a simple unit-type in **openage**.

    UNITTYPE {
	    health_points : int,
	    speed : float,
	    can_build : set(BUILDING)
    }
    
For sake of completeness, here is a *very* basic (and not realistic) usage example
of this type in nyan data file :

    +UNITTYPE VILLAGER {
	    health_points = 10,
	    speed = 1,
	    can_build = [BARRACKS, TOWNCENTER]
    }
    
In the current section we will continue to provide some nyan data snippets as part of
the nyanspec examples. The actual syntax of nyan data files will be detailed in the
following sections. Let us focus on nyanspec for now.

As you could see, each attribute must have a name and a type. An attribute is
defined by its name, followed by a colon and then its type. Allowed types are all
nyan types and nyan built-in types. Possible built-in types are:

- `int`:  integers
- `float`:  floating point numbers
- `bool`:  boolean values, either true or false
- `string`:  character sequences

Furthermore, an attribute type can be a finite set of nyan objects of one type.
A set attribute is simply specified with the `set` keyword. The type of the set is
specified within the parenthesis following the `set` keyword. It is important to
remember that we are really talking of sets and not of bags. Therefore no duplicates
are allowed within a set and no order is guaranteed. 

Finally, for each attribute a default value can be specified after the attribute type
separated by an equals sign.

**Things to remember (tl;dr)**:

- nyan types are *exclusively* defined in the nyanspec file
- In the nyan data file, nyan objects are *instanciated from known types*. The nyan syntax
will be detailed in the following sections
- A set attribute can contain a *set* of objects of *one* given type.

### nyanspec - Dynamic attributes

An object can extend a nyan type if the nyanspec of this object type contains an ellipis.
For types with ellipis, the nyanspec simply specifies the minimum set of attributes for any
object of this type. Additionnal *dynamic attributes* can be specified in the nyan data file
when an object is instanciated.

Imagine a villager unit in **openage** has multiple abilities. This would lead to the
following (not realistic) example:

    # nyanspec
    ABILITY {
	    name : string	# Regular attribute specification
	    ...			# Dynamic attributes are allowed for this type
    }

    UNIT {
	    abilities : set(ABILITY)
    }
    
    # nyan
    +ABILITY ATTACK {
    	name = "attack",
    	damage = 10
    }
    
    +ABILITY BUILD {
    	name = "build",
    	construction_speed = 1
    }
    
    +UNIT VILLAGER {
    	abilities = [ATTACK, BUILD]
    }
    

This definition works, as every ability should have a name, but it is not really obvious what
other attributes an ability has. The abilitiy to attack probably needs an attribute that describes
an attack damage, while the ability to build a building needs a construction speed attribute.
Thus abilities objects (here ATTACK and BUILD) can extend the ABILITY type.

As we've just seen a set attribute can store objects of one given type (here ABILITY type).
However, a nyan set can store heterogeneous objects of one type thanks to *dynamic attributes*
so that the villager can both attack and build!

**Things to remember (tl;dr)**:

- dynamic attributes are good, awesome and useful.
- use ... to define a dynamic-attribute-enabled type
- remember that objects with dynamic attributes may have the same type and thus bagged together
in one set.

### nyanspec - Runtime deltas declaration

Currently we are able to define nyan types that consist of named attributes and dynamic attributes.
Thus we are able to define types with static attribute values and relations. With **openage**, we
need  one more fundamental concept to represent things in nyanspec : *runtime deltas*. 
Ingame technologies should have the ability to update other nyan objects attributes during runtime.
For example the _loom_ technology should update the _villager_'s health points. This is where
runtime deltas come into play. Those so-called _runtime-deltas_ can be declared in the **nyanspec**
the following way:

    # nyanspec
    TECH {
    	description : string,
    	^UNITTYPE		# a runtime delta is HERE!
    	...
    }

    UNITTYPE {
    	name : string,
    	health_points : int,
    	...
    }
    
    # nyan
    +UNITTYPE SCOUT {
		name = "scout cavalry"
		health_points = 40
		damage = 3
		armor_pierce = 1
		armor_melee = 0
    }
    
	+TECH BLOODLINES {
	   description = "through centuries of in-breeding horses and knights, you manage to somehow increase their hp."
	   cost_food = 300
	   cost_gold = 200

	   ^UNITTYPE SCOUT {		# a runtime delta is HERE!
	       health_points *= 1.2                        
	   }
	}

A circumflex (^) followed by a nyan type name signals a runtime deltas attribute definitition.
In the nyanspec it specifies that the containing nyan type is allowed to change at runtime
the attributes of objects of the type specified at the right of the circumflex.

For example (see above) TECH type objects are allowed to change the attributes of UNITTYPE objects
at runtime.

Inside a nyan data file the circumflex recall the nyan type and specifies the object
(here SCOUT) the runtime delta applies. The following scope defines the runtime delta effect
(here health_points *= 1.2).

nyan data file (nyan)
---------------------

In contrast to the **nyanspec**, which describes all existing types, their
relations and attributes, the nyan data files, describe the concrete nyan
objects. Nyan data files have the file extension `*.nyan`.

### nyan - Basic object definition

All objects that are defined in nyan data files must be of a type defined in the
**nyanspec**. Hereafter we will use the following **nyanspec** as a base for all
nyan data files.

    ABILITY {
    	description : string,
    	...
    }
    
    UNITTYPE {
    	health_points : int,
    	abilities : set(ABILITY)
    }
    
    TECHNOLOGY {
    	^ABILITY,
    	^UNITTYPE
    }

Firstly we define a few abilites and a unit type that makes use of them.

    +ABILITY MOVE {
    	description = "Allows units to move.",
    	speed = 10.0
    }
    
    +ABILITY FIGHT {
    	description = "Allows units to fight.",
    	damage = 20,
    	ranged = false
    }
    
    +UNIT VILLAGER {
    	health_points = 50,
    	abilites = [ MOVE, FIGHT ]
    }
    
New objects can be introduced using the `+`-operator combined with the object's
nyan type. Each object must have a unique name within the whole nyan context
(which will be explained later). Attribute values are assigned using `=` with a
subsequent literal value or nyan object name. Literals for the different
built-in types look like the following:

- int: 1337
- float: 13.37
- bool: either true or false
- string: "test" or 'test', with escape sequences

Sets are defined using brackets. Values within the set are separated by commas.

### nyan - Anonymous object definition
TODO: Do we really need this? Why?

### nyan - Delta definition
nyan deltas enable the user to modify the game base properties and to patch existing objects.
The syntax used to defined deltas is very simple. A delta is an overlay over a pre-existring objects.
Suppose that a VILLAGER object is defined (with variaous attributes and values). To define a delta that updates the villager properties, we need to refer to the previously defined VILLAGER object with the following syntax using the @ character :

	@UNIT VILLAGER {
		health_points *= 1.1
	}

The previous definition defines a delta over the VILLAGER object that increase the villager health_points by 10%. This delta would be applied at the game initialization.

### nyan - Runtime-delta definition
A runtime-delta is simply a delta that is defined inside the scope of another object (typically a technology object). The game runtime can then decides when to apply the delta associated with a particular object.
The following example defines the LOOM technology that upgrades the VILLAGER objects health_points attribute :
	
	+TECH LOOM {
		description = "Loom"
		^UNITTYPE VILLAGER {
			health_points += 50   # The loom technology gives 50 more health points to the villager
		}
	}

A runtime delta can also be part of a nyan delta. For example, The previously defined LOOM technology can be
patched the following way :
	
	@TECH LOOM {
		^UNITTYPE VILLAGER {
			health_points += 50    # The loom technologie now gives 100 more health points to the villager
		}
	}

### nyan - inheritance

nyan objects can inherit attribute values *from objects of the same type*. When an object inherits from another object, it can patch the inherited properties. For example :

	+UNITTYPE SCOUT {
	    name = "scout cavalry"
	    health_points = 40
	    damage = 3
	    armor_pierce = 1
	    armor_melee = 0
	}

	+UNITTYPE LIGHTCAVALRY : SCOUT {
	    name = "light calvalry" 	# override the name
	    health_points += 20 		# patch the health point property
	    damage += 2					# patch the damage property
	    							# inherits all other properties from SCOUT
	}

When a subobject inherits from an object, the subobject is still concerned by deltas and runtime deltas primary applied to the original object. Continuing the previous example :
	
	# nyan base properties
	+TECH BLOODLINES {
		description = "through centuries of in-breeding horses and knights, you manage to somehow increase their hp."
		cost_food = 300
		cost_gold = 200

	   ^UNITTYPE SCOUT {
	       health_points *= 1.2    # also affects the LIGHTCAVALRY subobject!
	   }
	}
	
	+TECH LIGHTCAVALRY {
	     description = "upgrades your scouts to light cavalry."
	     cost_gold = 400
	     upgrade_from = SCOUT
	     upgrade_to = LIGHTCAVALRY
	}
	
	# inside a nyan mod
	@UNITTYPE LIGHTCAVALRY {
	    health_points += 5
	}
	
	@TECH BLOODLINES {
	    cost_gold *= 1.5
	    ^UNIT SCOUT {
	        health_points *= 1.3	# same thing here this affects the LIGHTCAVALRY subobject!
	    }
	}

Well, a lot of things are happening in this last example.
So, regarding the execution order of the changes applied to the SCOUT and LIGHTCAVALRY units:

1. All enabled datapacks are read first; after reading base, SCOUT has 40hp and LIGHTCAVALRY has +=20hp.; after reading the mod, SCOUT has 45 hp and LIGHTCAVALRY has +=20hp.

2. Once all the datapacks have been read, inheritance is resolved: SCOUT has 45hp, LIGHTCAVALRY has 65hp, and BLOODLINES applies to SCOUT and LIGHTCAVALRY. This is the information (unit types) that will be available at game start.

3. Now when the player researches bloodlines in-game, that player's SCOUT and LIGHTCAVALRY unit types get modified to have 59hp and 85hp.

4. If the player researches the light cavalry tech, all the player's units that have unit type SCOUT get changed to unit type LIGHTCAVALRY.

nyanspec compiler
-----------------
The nyanspec compiler is used to generate C++ code from a nyanspec file. The generated C++ code consists of structs and parsers associated with the nyan types defined in the nyanspec file. The nyanspec compiler itself is written in Python and resides in py/openage/nyan.

Using the generated code with _libnyan_
------------------------
The generated C++ code is used by openage to parse the nyan files and to create an in-memory game data model. libnyan is written in C++, it manages and combines all the informations contained in the game data model. During the game, libnyan is queried by the game engine and can be considered as a _view_ over the game data model.

Questions
---------

Why is there no inheritance between nyan types? This would avoid redefining same
attributes for multiple types.

Why not generate the nyanspec and nyan files directly by the convert script ? Then generate the associated C++ code (structs and parsers) used by libnyan ?
