openage status
==============

This file contains the current feature set of openage.
Please update it accordingly when adding features.


Features
--------

* convert all original media files
* create a csv representation of all gamedata
* generate C code to read that representation
* flat, infinite terrain rendering
* tile blending (toggle with space)
* placement of a building (right click)
  * building selection (control + scrollwheel)
* terrain id modification
  * select current terrain (scrollwheel)
  * draw selected terrain (left click)
* map scrolling
  * arrow keys
  * middle mouse button
* screenshots (F2) -> `/tmp/openage_2019-12-31_23-59-59_xx.png`
* ingame console
  * open with backtick
  * readonly log view at the moment
  * very inefficient proof of concept rendering
* sound playback (when placing and removing buildings)
* automatic asset reloading by `inotify`
* integrated profiler: F12 to start/stop + show results
* unit selecting and moving mode entered by pushing 'm'
* villager functions
  * selected villagers place and construct buildings with 'y' which uses current editor building
  * villagers can gather resource objects
* selected buildings can train the current editor item by pressing 't'
