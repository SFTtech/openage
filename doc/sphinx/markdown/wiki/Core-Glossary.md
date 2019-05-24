# Core Glossary

1. **[How to read this document](#how-to-read-this-document)**
2. [Buildsystem](#buildsystem)  
  2.1 [Continuous Integration System](#continuous-integration-system)
3. [Gamedata Handling](#gamedata-handling)  
  3.1 [Data Conversion](#data-conversion)  
  3.2 [Data Description Language](#data-description-language)  
  3.3 [Data Modding API](#data-modding-api)  
  3.4 [Runtime Database](#runtime-database)
4. [Game Simulation](#game-simulation)  
  4.1 [Pathfinding](#pathfinding)
5. [Networking](#networking)  
  5.1 [Matchmaking and Multiplayer Lobby](#matchmaking-and-multiplayer-lobby)
6. [Renderer](#renderer)  
  6.1 [Level 1 Implementation](#level-1-implementation)  
  6.2 [Level 2 Implementation](#level-2-implementation)  
  6.3 [Terrain Rendering](#terrain-rendering)
7. [Scripting](#scripting)  
  7.1 [Cpp-Python Interface](#cpp-python-interface)  
  7.2 [Scripting Modding API](#scripting-modding-api)

## How to read this document

Our *core glossary* should give a brief overview on the components of our engine and how we plan to implement them. The descriptions are short on purpose. For more info, consult the [docs](https://github.com/SFTtech/openage/tree/master/doc) or join our [developer chat](https://riot.im/app/#/room/#sfttech:matrix.org).

Every component is marked with a status which shows how far it has progressed so far.

* **Planned:** The feature is planned, but we have yet to decide what design pattern we are going to use.
* **Design:** In this phase, the design pattern for our component has been chosen and we are discussing how it will be integrated into the engine.
* **Under Construction:** We are implementing the feature right now.
* **Testing:** The main implementation is done and we are getting rid of leftover bugs.
* **Finished:** Our component is fully functional and ready to be used. However, further improvements are always welcome.

Sometimes there is a **Responsible** field, which lists the people from our team who are actively working on the component. If you are new and would like to participate, feel free to ping them in the developer chat.

## Buildsystem

### Continuous Integration System

**Status:** *Under Construction*

**Responsible:** *mic-e*, *jj*

Our CI system [kevin](https://github.com/SFTtech/kevin) checks builds and runs tests on the Pull Requests in the repository. Currently, it misses suport for macOS and Windows setups.

## Gamedata Handling

Everything concerning the data used for gameplay, including graphics, sounds, unit stats, scripts and interface elements.

### Data Conversion

**Status:** *Design*

**Responsible:** *mic-e*, *jj*

The data files of Genie Engine games have to be parsed and converted into a format that our modding API understands. Converted data is stored in several open formats, depending on the type of asset.

* **Ingame object stats, interface elements:** `.nyan` files
* **Graphical assets:** `.png` files for sprite sheets and a `.sprite` format for animation data (e.g. frame delay)
* **Sounds:** `.opus` files
* **Random Map Scripts:** openage python API scripts

**Further Reading**

* [Sprite definition format (discussion)](https://github.com/SFTtech/openage/issues/965)
* [Integrating nyan (discussion)](https://github.com/SFTtech/openage/issues/734)
* [Scripting API (discussion)](https://github.com/SFTtech/openage/issues/735)

### Data Description Language

**Status:** *Finished*

**Responsible:** *jj*

Definition data for units, buildings, techs, etc. is written in the [nyan](https://github.com/SFTtech/nyan) description language. The nyan language is *object-oriented*, *human-readable*, allows *inheritence* and easy modification of existing objects through a mechanism that is called *patching*. The main focus is *readability* and *moddability*.

**Further Reading**

* [nyan language - Quickstart Guide](Quickstart-Guide-for-AoE2-modders)
* [nyan language specification (could be outdated!)](https://github.com/SFTtech/nyan/blob/master/doc/nyan.md)

### Data Modding API

**Status:** *Design*

**Responsible:** *heinezen*

The interface that exposes engine features to modders and game developers. Our API consists of several [nyan](https://github.com/SFTtech/nyan) objects for which the engine defines specific behavior. The API is an [entity-component-system](https://en.wikipedia.org/wiki/Entity%E2%80%93component%E2%80%93system) with generic unit objects (`GameEntity`) that are defined through *abilities* and *boni* objects.

**Further Reading**

* [nyan API draft](https://github.com/SFTtech/openage/pull/1021)
* [Description of the API draft (blogpost)](https://blog.openage.sft.mx/d0-openage-modding-api-introduction.html)

### Runtime Database

**Status:** *Finished*

**Responsible:** *jj*

The runtime gamedata is handled by [nyan](https://github.com/SFTtech/nyan), a typesafe hierarchical key-value database with inheritance and dynamic patching. During a game, nyan takes care of the objects in the game world as well as unit upgrades and updates. nyan is maintained by us and is written specifically for RTS games like Age of Empires 2.

**Further Reading**

* [Overview of nyan in openage (blogpost)](https://blog.openage.sft.mx/t0-nyan-api-integration.html)

## Game Simulation

### Pathfinding

**Status:** *Planned*

We plan to adopt a pathfinder based on flow-fields to better deal with a high number of units.

**Further Reading**

* [Discussion about new pathfinding and grouping](https://github.com/SFTtech/openage/issues/366)
* [Crowd Pathfinding and Steering Using Flow Field Tiles](http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter23_Crowd_Pathfinding_and_Steering_Using_Flow_Field_Tiles.pdf)

## Networking

### Matchmaking and Multiplayer Lobby

**Status:** *Planned*

The [openage masterserver](https://github.com/SFTtech/openage-masterserver) provides matchmaking and serves as a registry point for (public) dedicated servers. It is not required for private and/or LAN matches.

## Renderer

The graphics subsystem is implemented in two levels. The first level is an abstraction over graphics APIs (OpenGL, Vulkan) and provides generic shader execution methods. The second level uses the first to draw openage-specific graphics, i.e. the actual world, units, etc.

### Level 1 Implementation

**Status:** *Finished*

**Responsible:** *Vtec234*

Abstraction over the Graphics APIs OpenGL and Vulkan. The level 1 renderer stage submits resources to be uploaded to the GPU and receives a handle that identifies the uploaded resource. Resources can be added, updated and removed. Currently supported resource types are shader and texture.

### Level 2 Implementation

**Status:** *??*

**Responsible:** *Vtec234*

API that is actually specific to openage, and is threadsafe. The level 2 renderer calls the level 1 renderer and updates it to match the game state. In some documentation this is also called the "presenter".

### Terrain Rendering

**Status:** *??*

**Responsible:** *Vtec234*, *jj*

The engine will support 3D terrain. Terrain textures are painted on the 3D ground and then rendered from the correct camera perspective (dimetric view for AoE2). Drawing the texture is independent from the tiling system.

## Scripting

openage is scriptable via Python.

### Cpp-Python Interface

**Status:** *Finished*

**Responsible:** *mic-e*

Exposes C++ functions to Python. Cython is used as glue-code.

**Further Reading**

* [Pyinterface documentation](https://github.com/SFTtech/openage/blob/master/doc/code/pyinterface.md)

### Scripting Modding API

**Status:** *Planned*

Allows modders to create gameplay scripts and change behavior of engine functions. It will probably also be used to setup the UI, for random maps and AI.

**Further Reading**

* [Scripting API (discussion)](https://github.com/SFTtech/openage/issues/735)