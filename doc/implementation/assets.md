Openage assets
==============

The game engine requires assets to actually run a game.
Any assets may be used, but the common case will be to convert the original media files.


Asset directories
-----------------

The engine uses two asset directories, global_assets and user_assets.

 - global_assets contains assets that are immutable by the user (e.g.: shaders). They are located in `assets/`, and installed to /usr/share/openage.
 - user_assets contains assets created by the user (e.g.: converted original game assets, settings, ...). Files in user_assets have priority over files in global_assets. An installed version of openage uses ~/.openage as user_asset directory; in the development tree, `userassets/` is used.


Metadata assets
---------------

Metadata assets are used to describe the assets (duh.).

 - Media metadata describe:
  * which areas of an texture images show what (texture atlases...)
  * which animation and unit is stored in the file
  * how long does the sound last
  * etc
 - Game metadata files: Describe existing game elements
  * what media asset metadata shall be used
  * where is the storage location of the techtree
  * where are all the units stored
  * what describes the existing ages and technologies
  * etc...
 - Interface metadata: Used to construct the interface


Data assets
-----------

- Media assets
   The media asset files feed the engine with images, sounds, animations, whatever.
   They are accompanied by metadata assets.

- Game content files
   Describe the game logic
     - techtree
     - civilsations
     - ages
     - units
     - etc, you get the point
- Scripting files
   These allow to interactively manipulate and design the game content.
