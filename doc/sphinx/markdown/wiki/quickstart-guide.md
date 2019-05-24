# Quickstart Guide

The data definition we use for openage is vastly different from the system used in Age of Empires and the Genie Engine. For describing our game data we utilize **nyan**. nyan allows extensive moddability while still being understandable for humans. If you are familiar with YAML or JSON, you will probably spot many similarities. But even if you don't, there's no need to worry just yet. This guide will teach you the basics of nyan and the underlying principles.

## Overview

Nyan manages the configuration of our game entities. For that purpose it provides the following features:

**Human-readable plain-text modding**: Everything in the game data files is editable without the need for binary extraction. You just require a text editor. For those of you who are scared of text editors, additional helper tools will be made available.

**Object-oriented**: Game entities are defined as objects with multiple key-value pairs.

**Inheritance**: Objects can inherit from other objects instead of redefining everything for a minor change.

**Easy modification through patches**: Objects are not static in nyan. They can be modified indefinitely by patching their attributes.

**Optimized mod-combination**: Because every object has a unique identifier, mods can easily reference each others data. Of course, data from Age of Empires 2 is no exception as it will simply be treated as one of many modpack.

nyan is only used for the *definition* and *composition* of game data. Everything concerning *behaviour*, e.g. what, when, how and if objects are handled, is done by the openage engine or can be implemented with our Scripting API (coming soon).

## Data types

nyan has two basic data types, **objects** and **patches**.

* Objects define the attributes, abilities and hierarchy of any game entity. Abilities themselves are also defined as objects. Objects can inherit from multiple other objects. Examples for objects would be first tier buildings, first tier units, trees, animals and the relic.

* Patches only change attributes, abilities and hierarchy of game entities. A patch always requires a target which can be an object or a patch. Patches can inherit from multiple other patches if they have the same target. Objects can also gain new inheritance parents through applying a patch. In the AoE2 modpack patches would include research, all unit upgrades and stats adjustment.

## Unit upgrades in nyan

In AoE2 all units are defined as separate objects. From the viewpoint of the Genie Engine upgrading a unit to a higher tier *replaces* every old unit with a new one. Beecause of this replacement stragety, stat upgrades from the Blacksmith, civ boni and unique techs have to be applied globally. This means they either have to affect every unit in a certain unit class or none of the units.

With nyan only the first tier of a unit class is defined as an object. Every upgrade will be done through patches. For example, the first tier in the spearman line (Spearman) will be defined as an object, while the "Pikeman" upgrade will only be a patch for the Spearman. In simpler words: The Pikeman will not be a separate unit in nyan!

Although this might sound unintuitive at first, it provides several advantages:

* In AoE2 it's all or nothing, upgrades always affect every unit of a tier. nyan allows individual units to be upgraded. This makes game mechanics like stat upgrades through leveling possible.
* The replacement strategy leads to another problem: Objects that are already deployed on the battlefield have to be replaced correctly. Converted units [are known for having problems with that](https://youtu.be/_gjpDWfzaM0?t=2m6s). With nyan, no replacement for objects is needed. Animations, stats and abilities can be upgraded on-the-fly.
* Patches can be applied multiple times and are not necessarily tied to a Tech. Their activation just has to be scripted.
* Patches can be applied to other patches. With this method mods or custom game modes can increase or decrease the effects of Techs without having to manipulate the base game files.

## Objects

An object's definition looks like this:

```
TentacleMonster(Unit):
    name = "Splortsch"
    hp = 2000
```

In openage objects do not have IDs and are referenced by the human-readable identifier (also called *fqon*). In this example the identifier is `TentacleMonster`. `TentacleMonster` inherits values from another object `Unit` which is written in parentheses. 

The two indented key-value pairs (called *members* in nyan) are the attributes of our object. A member called `name` with a text value `"Splortsch"` and the member `hp` with an integer value of `2000`. An object can have an unlimited number of members. Members support several primitive types such as text, integers, floats, boolean values, file references, sets, ordered sets and dicts. They can also reference other nyan objects.

## Patches

The patch syntax looks like this:

```
AdvancedTentacleMonster<TentacleMonster>():
    hp += 500
```

The syntax of patches is very similar to the objects' notation. In addition to the identifier (`AdvancedTentacleMonster`) there is a target `TentacleMonster` which is written in angled brackets. Therefore, we know that `AdvancedTentacleMonster` changes members of `TentacleMonster`.

As mentioned before, a patch cannot add new members to the target object. It can only change ones that are already defined. You can choose to change only a subset of the members like in our example where we only change the member `hp`. The operator `+=` indicates that the value of `hp` is increased by 500. When the patch is applied, `TentacleMonster` will have 2500 hp. If it's applied a second time, `TentacleMonster` will have 3000 hp.

Several more operators exist for operations such as substraction `-=`, multiplication `*=`, division `/=` and others.

## Creating a tech

The previously defined patch describes the changes to the `TentacleMonster`, but that is not enough to create a tech. Usually techs require a name and have a cost, so we want to define that information as well. The solution is simple: We will create a Tech Object that references our patch.

```
BiggerTentacleTech(Tech):
    name = "Big Tentacle"
    cost = {Slime: 50, Love: 200}
    updates = {AdvancedTentacleMonster}
```

We have now created a new object called `BiggerTentacleTech` that inherits from `Tech` and has the members `name`, `cost` and `updates`. `name` has a text value with the name of our technology. `cost` is a dictionary which denotes the resource cost of the research. In this case, researching costs 50 slime and 200 love. We also referenced the patch in the member `updates`. `updates` is a set-member and can therefore theoretically hold more than one patch. This way you can bundle several patches together.

## Nyan and the openage engine

As we have established before, nyan only defines our data and handling has to be done by the engine. We will now give some examples of the tasks the engine will do and how they relate to the definition written in nyan. Before we begin, it is important to stress that processing nyan objects in the engine (e.g. through patching) never (!) changes the files on disk. During a game the engine keeps its own database of runtime objects. nyan merely acts as a provider of information that is accessed by the engine when necessary.

Things that are tracked by the engine include:

* **Current HP**: The engine keeps track of the current HP of a unit. In nyan we would only define the maximum HP.
* **Ownership**: Who owns a specific unit or object is saved in the engine's own database. Change of ownership, e.g. through conversion, is also decided by the engine.
* **Patch History**: The engine knows when and how often a patch was applied to a specific unit or which of the units are patched. It can also revert a patch if necessary. nyan only knows the contents of a patch and the potential targets.

Even though nyan only defines data, that doesn't mean you have to program the behaviour for every new unit by yourself. The engine provides standard functionality for units, buildings, techs and abilities. For example, a new object that inherits from `Unit()` in nyan will be treated like every other unit by the engine. Specifics for standard behaviour is discussed in the more detailed guides (coming soon). If you want your units to do something slightly different than what is defined in the engine, you can usually utiize the Scripting API. Of course, you can also suggest new gamelogic features to us or implement them yourself so they will become available for everyone that uses the engine.