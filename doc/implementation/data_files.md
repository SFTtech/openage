Openage data files
==================

The game engine uses data files to actually run a game.
The data files can be anything you imagine, but the common case will be the converted original media files.


Search locations
----------------

The engine has the possibility to set the data directories, which will be `/usr/share/openage/` and `$HOME/.openage/` in the future, but for now it will remain `./data/`.


Metadata files
--------------

Metadatae, as the name suggests, describe data files.

1. There are media metadata files. They come along with the media files. They describe:
  * which areas of images show what
  * which animation and unit is stored in the file
  * how long does the sound last
  * …
  * irgendwas mit medien halt
1. There are game metadata files. These are needed to tell the engine what game elements exist. This means:
  * what media files metadata shall be used
  * where is the storage location of the techtree
  * where are all the units stored
  * what describes the existing ages and technologies
1. There are interface metadata files. The whole game interface is constructed by these files.


Data files
----------

1. Media files.
   The media files feed the engine with images, sounds, animations, whatever.
   They are described by their metadata files.

1. Game content files.
   The whole game logic has to be defined somewhere.
   This is it.
   These files are discovered and described by the metadata files.
     * the technology tree
     * what civilisations exist
     * what are the possible ages
     * what is the meaning of our life on earth
     * what buildings can be built in what age
     * etc, you get the point
1. Scripting files.
   These allow to interactively manipulate and design the game content.
