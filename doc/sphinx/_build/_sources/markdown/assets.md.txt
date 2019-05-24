openage assets
==============

The game engine requires assets to actually run a game.
Any assets may be used, but the common case will be to convert the original media files.


Asset directories
-----------------

When launching the game, [assets are converted automatically](/doc/media_convert.md).

They will be placed in `assets/`.

Multiple search paths like `/usr/share/openage`
and `~/.local/share/openage` are currently not implemented.
This will hopefully be done soon.


Metadata assets
---------------

Metadata assets are used to describe the assets (duh.).

* Media metadata describe:
  * Which areas of an texture images show what (texture atlases...)
  * Which animation and unit is stored in the file
  * How long does the sound last
  * etc
* Game metadata files: Describe existing game elements
  * What media asset metadata shall be used
  * Where is the storage location of the techtree
  * Where are all the units stored
  * What describes the existing ages and technologies
  * etc...
* Interface metadata: Used to construct the interface


Data assets
-----------

- Media assets
   The media asset files feed the engine with images, sounds, animations, whatever.
   They are accompanied by metadata assets.

- Game content files: **Describe the game logic**
     - techtree
     - civilizations
     - ages
     - units
     - etc, you get the point
