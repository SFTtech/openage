nyan data format
================

**nyan** is the recursive acronym for **nyan: yet another notation**. It
consists of two separate, but very similar file formats. Firstly, there is the
nyan specification (short: nyanspec) which describes a domain of object types.
Secondly, there is the data file format itself, which defines a set of objects,
with their attribute values.

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

### Dynamic attributes

Sometimes there are cases where nyan types need dynamic attributes.
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
building needs a construction time attribute. Thus abilities should have dynamic
attributes. Those can be specified with ellipsis:

    ABILITY {
    	name : string,
    	...
    }

### Runtime deltas

Currently we are able to define nyan types that consist of named attributes and
dynamic attributes. Thus we are able to define types with static attribute
values and relations. But this is not always the case in **openage**.
Ingame technologies should have the ability to update other nyan objects
attributes during runtime. For example the _loom_ technology should update the
_villager_'s health points. Those so called _runtime-deltas_ can be specified in
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

### Basic object definition

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

### Anonymous object definition

### Runtime-delta definition

### Delta definition and application

nyanspec compiler
-----------------

Using the generated code
------------------------

Questions
---------

Why is there no inheritance between nyan types? This would avoid redefining same
attributes for multiple types.
