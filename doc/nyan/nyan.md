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

Each attribute must have a name and a type. An attribute is defined by its name,
followed by a colon and its type. Allowed types are all nyan types and nyan
built-in types. Possible built-in types are:

- `int`:  integers
- `float`:  floating point numbers
- `bool`:  boolean values, either true or false
- `string`:  character sequences

Further an attribute can be a finite set of nyan objects, which is specified
with the `set` keyword. It is important to remember that we are really talking
of sets and not of bags. Therefore no duplicates are allowed within a set and no
order is guaranteed. For each attribute a default value can be specified after
the attribute type separated by an equals sign.

### nyanspec - nyan interfaces definition

A nyan set can store objects of heterogeneous types thanks to implicit interface.
A nyan interface is simply a nyan type that define a minimum set of attributes
that other types can implement.
Imagine a unit in **openage** has multiple abilities. This would lead to the
following definition:

    ABILITY {
	    name : string
    }

    UNIT {
	    abilities : set(ABILITY)
    }

This definition works, as every ability should have a name, but it is not really
obvious what other attributes an ability has. The abilitiy to attack probably
needs an attribute that describes an attack damage, while the ability to build a
building needs a construction speed attribute. Thus abilities can implicitly
extend the ABILITY interface. For example, the following types are valid
implementation of the ABILITY interface :

    ATTACK_ABILITY {
    	name : string,
    	int : damage
    }
    
    BUILD_ABILITY {
    	name : string,
    	int : construction_speed
    }

### nyanspec - Runtime deltas declaration

Currently we are able to define nyan types that consist of named attributes and
dynamic attributes. Thus we are able to define types with static attribute
values and relations. But this is not always the case in **openage**.
Ingame technologies should have the ability to update other nyan objects
attributes during runtime. For example the _loom_ technology should update the
_villager_'s health points. Those so called _runtime-deltas_ can be declared in
the **nyanspec** the following way:

    TECHNOLOGY {
    	name : string,
    	^UNITTYPE
    }

    UNITTYPE {
    	health_points : int
    }

A circumflex followed by a nyan types' name signals, that this nyan type is
allowed to change the attributes of the specified type during runtime.

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
    }
    
    MOVE_ABILITY {
    	description : string,
    	speed : float
    }
    
    FIGHT_ABILITY {
    	description : string,
    	damage : int,
    	ranged : bool
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

    +MOVE_ABILITY MOVE {
    	description = "Allows units to move.",
    	speed = 10.0
    }
    
    +FIGHT_ABILITY FIGHT {
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

### Anonymous object definition
TODO: Do we really need this? Why?

### nyan - Delta definition
The syntax used to defined deltas is very simple. A delta is an overlay over an pre-existring objects.
Suppose that a VILLAGER object is defined (with variaous attributes and values). To define a delta that updates the villager properties, we need to refer to the previously defined VILLAGER object with the following syntax using the @ character :
@UNIT VILLAGER {
	health_points *= 1.1
}

The previous definition defines a delta over the VILLAGER object that increase the villager health_points by 10%. This delta would be applied at the game initialization.

### nyan - Runtime-delta definition
A runtime-delta is simply a delta that is defined inside the scope of another object (typically a technology object). The game runtime can then decides when to apply the delta associated with a particular object.
The following example defines the LOOM technology that upgrades the VILLAGER objects healh_points attribute :
+TECHNOLOGY LOOM {
	name = "Loom"
	@UNIT VILLAGER {
		health_points = 50
	}
}

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
