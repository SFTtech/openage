## 0.4.0

### Date
Jun 27, 2019

### Description

Highlights

 * Event-driven gamestate preparations
 * Link to nyan library
 * New renderer implementation (partial)
 * Convert African Kingdoms data (up to HD version 4.3)
 * Cache files for asset loading
 * Modding API specification (v0.2)
 * Out-of-source tree builds
 * Windows MSVC builds and Windows 64-Bit installer

Visible changes

 * Scrolling only when windows is in focus
 *  Middle part of the lower UI
 *  Pop space (old gamestate)
 *  Market resource prices (old gamestate)
 *  Building costs and resource amounts (old gamestate)


## 0.3.0

### Date
Jul 28, 2015

### Description

Major refactoring that adds tons of internal features and a few new visible features.

 * New language versions: Python 3.4, C++14, Cython 0.22
 * Python code is now compliance-checked via pylint
 * Integrated various code sanitizers and the clang static analyzer
 * C++ <-> Python function calling interface, based on Cython
 * Proper and integrated C++ and Python logging subsystem
 * Dumped GLEW for libepoxy
 * Keybindings manager
 * New C++ and Python testing system

Visible features:

 * Unified program entry point
 * Asset conversion automatically runs when launching the game
 * Units can perform various commands
 * Units actively search for new commands
 * Units can fight
 * Mode indicator


## 0.2.3

### Date
Nov 18, 2014

### Description

New shiny stuff:
* Walking units
* Pathfinding
* Profiling
* Inotify asset refreshing
* First drafts for nyan
* Terrain debug grid (F4)
* Unit creation and death sounds
* Mac OSX support
* AOE:HD support

Improvements:
* Buildsystem heavily boosted
* Gamespec loading job in background
* Random things all over the code


## 0.2.2

### Date
Nov 8, 2014

### Description

import all raw orignal gamedata

the members to be available in C++ are defined in convert/gamedata

the code generation automatically creates all needed files

these reside in src/gamedata/ and are a huge tree structure
which equals the content of the empires2x1p1.dat file.


## 0.2.1

### Date
Nov 8, 2014

### Description

infinite terrain and sound

this was mainly background work, especially the original data
conversion was improved

basic sound support was implemented

some bugs in the terrain rendering algorithm were fixed

## 0.2.0

### Date
Nov 8, 2014

### Description

#### Milestone 2

rendering a single building (university) from the stock age2 media files

coloring works, it's done by a fragment shader that replaces
alpha-marked base colors (of the blue player)


## 0.1.0

### Date
Nov 8, 2014

### Description

#### Milestone 1

SDL window is working, with a gaben displayed: how awesome is that?!!

mouse clicks also trigger the drawing of a castle and village center,
these images were extracted from the age2 wiki.


## 0.0.0

### Date
Nov 8, 2014

### Description

Game environment specification completed