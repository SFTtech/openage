Convert Script
==============

The convert script exists for transforming media files to openage-compatible
formats, and generating code for parsing the generated files. It is written
in **Python** and contains **C++ extensions**.

As we are using the media assets of the original game, the input format is
preexisting and we have to deal with it.

Unfortunately the original asset binary formats are a bit "special",
so we need to convert the files to a format that is less special, and more
usable. This is what the convert script does.


The convert script is divided into two parts:
original media dependend and independend exports.

Converting Media
----------------

* depends on existing original game installation
* converts images, sounds, units and all other data values
  to compatible formats
* creates files for the proprietary data package,
  its files will be used if free media files are missing.

Exporting Structures
--------------------

* totally independent of original game installation
* exports original content structures
* prevents redundancy:
  * data structures are needed for media export (to convert them)
  * data structures are needed to import them in the game
  * => structure definition at a single point.
* generation is integrated into make system
* generated files placed in `cpp/gamedata/`


Conversion Procedure
====================

Media Files
-----------

A media file is a sound or image file. In the original media files, they are
stored in [drs archives](/doc/media/drs-files.md).

Each media file has an id and a file extension.
Files can be "redefined" by overlay archives.

All available archives have to be ordered, so that 'high priority' files
override 'low priority' files. Mods have a higher priority than the base game
to be able to apply its modifications.

The overlay procedure works by assigning unique media files to the desired
file source or archive.

A dict has to be filled with values to assign the data source:

    (fileid, extension, destination) => (file archive)

The resulting dict is a mapping between all available media files and the
archive each should be obtained from.

The files can then be converted to the appropriate format, the conversion
procedure is selected by their file extension. Graphics are stored in the
[SLP format](/doc/media/slp-files.md) and are converted to *png*. Sounds are
*wav* files which are compressed to *opus*.


Gamedata Files
--------------

Gamedata means data describing the configuration and behavior of the game
entities. Namely this defines the available buildings, units, their creation
cost, etc.

The [gamedata files](/doc/media/gamedata.md) are parsed and converted to a
huge tree of *csv* files. This is not optimal and will be improved soon(tm).


### Data Updates ###

To modify any data extracted from the original files in a hardcoded manner,
simply update or delete values in the parsed python data structure.  These
modifications will be applied like this:

```python
def fix_data(data):
	# make lists for all units building locations
	for each unit:
		unit.building_loc = [unit.building_loc]

	huskarl_castle.building_loc.append(huskarl_barracks.building_loc)
	delete huskarl_barracks

	# value updating is done like this:
	get_unit(data, "hussar")["hp"] = 9001
	data.terrain.terrains[6].slp_id = 1337

	return data
```


Dynamic Input Assets
====================

Multiple original game versions are compatible with the conversion script.
To ensure correct conversion, game versions and patch levels have to be
detected and conversion has to be customized for them.

Upon launch, the engine tries to load asset packs.
An asset pack can be created from the existing age of empires installation,
or from the installation CD image (or by a modder).

The supported input data versions and their handling must be hardcoded:
* Define version profiles in a config file
* Specify archive names and where to find them
* This enables using mods like "forgotten empires"
* Type-specific storage and folder structure
  * Define rules for saving specific ids/filesuffixes to specific folders
    * e.g. map "slp" => "graphics/",
      meaning all slps will be stored into that folders
    * Map specific filenames,
      e.g. "015015.slp" => "graphics/terrain/deepwater",
   -> This leads to "graphics/terrain/deepwater_015015.png"
* Select profiles by existences of "empires2*.dat"
  * If "Games/Forgotten\ Empires/Data/empires2_x1_p1.dat" exists,
    use the forgotten profile
  * If no "Data/empires2_x1_p1.dat" exists, the 1.0c patch is missing
  * => We can determine which version is available,
       by checking which dat files are here
  * ==> We are compatible to age2:aok, age2:aoc, age2:aoc1.0c, age2:fe

Extraction depends on gamedata content:
* Parse referenced/needed files from game data (empires2*.dat)
* Extract these files and convert them
