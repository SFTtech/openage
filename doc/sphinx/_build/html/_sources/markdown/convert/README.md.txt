Convert Script
==============

The convert script transforms media files to openage-compatible
formats, and generates code for parsing the generated files. It is written
in **Python**, **Cython** and contains **C++ extensions**.

As we are using the media assets of the original game, the input format is
pre-existing and we have to deal with it.

Unfortunately the original asset binary formats are a bit "special",
so we need to convert the files to a format that is less special, and more
usable. This is what the convert script does.


The convert script is divided into two parts:
original media dependent and independent exports.

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
* generated files placed in `libopenage/gamedata/`


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
to be able to apply their modifications.

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


Original metadata files
-----------------------

This data describes the configuration ane behavior of the original game and its
entities. Namely this defines the available buildings, units, their creation
cost, etc.

The [gamedata files](/doc/media/original_metadata.md) are parsed and converted to a
huge tree of *csv* files. This is not optimal and will be improved soon(tm).


### Data Updates ###

To modify any data extracted from the original files in a hardcoded manner,
simply update or delete values in the parsed python data structure.  These
modifications can be applied like this:

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
