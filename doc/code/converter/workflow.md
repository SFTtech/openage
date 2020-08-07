# Workflow

- Converter has a general workflow
- Workflow is the same for every game edition
- Workflow should stay consistent if changes are made

- Converter is built to support multiple games
- Keep that in mind when adding features

![workflow structogram]()

## Game and Version detection

The conversion process starts with determining the game version. Users
are requested to provide a path to a game's root folder. The converter
then checks for filenames associated with a specific game version
and compares hashes of these files to pinpoint the version number of
the game.

A *game version* in the openage converter context is always a combination
of the 3 properties listed below.

- **Game Edition**: Standalone edition of a game. This refers to any release that
is runnable on its own without dependencies. A game may have been released in multiple
editions, e.g. AoE2 (1999 release, HD Edition, Definitive Edition).
- **Expansions**: A list of optional expansions or DLCs that were detected alongside
the game edition. The list can be empty if no expansions were found in the given path.
Note that for simplicity's sake, the *Rise of Rome*, *The Conquerors* and *Clone Campaigns*
expansions for the original releases of the games are currently handled as game editions
as we do not support the expansionless installations of these games.
- **Patch level**: The version number of the game edition.

To determine the game version, the converter iterates through all known game
editions and checks if all associated files can be found in the user-provided
path. If no matching game edition was found, the conversion process terminates
here. Once a matching game edition is detected, the converter iterates
through all possible expansions and again tries to find associated files
in the given path. In the last step, the patch level of the edition and the
expansions is identified. For that purpose, the converter calculates
the MD5 hash of expected files and retrieves the version number from a lookup
dict.

## Mounting

In the next step, the converter mounts asset subpaths and asset files into
a conversion folder. The paths for these assets, organized by media type,
are stored with the game edition and expansion definitions and can therefore
be determined from the game version.

Mount points are derived from the asset media types. Thus files of a certain media
type can always be found at the same location, regardless of the game version.
Container formats such as DRS and CAB are also mounted as folders, so that their
content can be accessed in the same way as "loose" files from the source folder.

## Reader

After all relevant source folders are mounted, the converter will begin the main
conversion process by reading the available data. Every source file format has
its own reader that is adpated to the constraints and features of the format.
However, reading always follows this general workflow:

* **Registration**: Enumerates all files with the requested format.
* **Parser**: Declares the structure of the file format and also tells the exporter
how to interpret them.
* **Output**: Use the parser to store a low-level representation of the data in
memory.

It should be noted that graphics/sound files are only registered during the Reader
stage and not parsed or output. The reason for this is that these files usually
do not require further in-memory processing and can be directly exported to
file. Therefore, parsing and output of these files does not happen until the
Export stage. Furthermore, the export of a graphic/sound file is only initiated
on-demand, i.e. if the export is requested during the Processor stage. This
allows us to skip unused media which results in a faster overall conversion time.

### Game Data

Game data for the Genie Engine is stored in the `.dat` file format. The `.dat` file
format contains only attribute values with no additional hints about their data type.
In addition to this, the `.dat` format can have a slightly different structure for
different game versions. As a result, all attribute data types have to be manually
declared and the parser is generated on-the-fly depending on the game version.

An attribute in the parser is defined by 4 parameters.

* **Human-readable name**: Will be used during the Processor stage to access
attribute values (e.g. `"attack"`).
* **Read type**: The data type plus additional info such as length (e.g. `char[30]`).
* **Value type**: Determines how an attribute value is interpreted (e.g. `ID_MEMBER`).
For example, a `uint32_t` value could be used for a normal integer value or define
an ID to a graphics/sound resource. Every unique value type is associated with
a Python object type used for storing the attribute.
* **Output mode**: Dtetermines whether the attribute is part of the reader output
or can be skipped (e.g. `SKIP`).

The Reader parses attributes one by one and stores them in a `ValueMember` subclass
that is associated with a value type. Its output is a list of `ValueMember` intances.

## Processor

The Processor stage is where all conversion magic happens. It can be further divided
into 3 substages.

- **Preprocessing stage**: Creates logical entities from the list of `ValueMember`s passed by
the Reader. These resemble structures from the *original* game (e.g. a unit in AoC).
- **Grouping stage**: Organizes logical entities from preprocessor stage into concept
groups. The groups represent an API concept from the *openage nyan API*.
- **Mapping stage**: The concept groups are transformed into actual nyan objects. Values
and properties from the concept groups are mapped to member values for objects in the
openage nyan API.

Processors may have subprocessors for better code structuring and readability. Furthermore,
there exists at least one processor for every game edition, although code sharing for
converting similar concepts is used and encouraged. For example, the processor for
converting AoE1 reuses methods from the AoE2 processor if the concept of a game mechanic
has not changed between the releases, e.g. movement. As a consequence, all functions
in a processor should be either static or class methods and be able to operate on
solely on the input parameters.

### Preprocessing

During preprocessing the processor first creates a container object for all objects that will
be created during conversion. This container is passed around as point of reference
for the converter's dataset. Additionally, logical entity objects are created from
the game data reader output. Examples for logical entities are units, techs, civs or
terrain definitions. Logical entities are inserted into the container object after
creation.

### Grouping/Linking

The grouping stage forms concept groups from single logical entities. A concept group
is a Python object that represents an openage API concept (and not a concept from the
original game). Concept group implementations bundle all data that are necessary
to create nyan objects from them during the mapping stage. An example for a concept
group is a *unit upgrade line*, e.g. the militia line from AoE2. The logical entities that
belong to this group are the units that are part of this line (militia, swordsman,
longswordsman, ...). They are stored in a list sorted by their order of upgrades.

Concept groups additionally provide functions to check if a group instance shows
certain properties. In the example of the *unit upgrade line*, such a property
could be that the units in the line are able to shoot projectiles or
that they are creatable in a building. These properties are then used during
the mapping stage to assign abilities to the unit line.

Concept group instances are also inserted into the container object from the
preprocessing stage after they are created.

### Mapping

During the mapping stage, nyan objects are created from data in the concept groups.
In general, it involves these 3 steps:

1. Check if a concept group has a certain property
2. If true, create and assign nyan API objects associated with that property
3. Map values from concept group data to the objects' member values

This is repeated for every property and for every concept group. Most values
can be mapped 1-to-1, although some require additional property checks.

If a mapped value is associated with a graphics/sound ID, the processor will
generate a *media export request* for that ID. The export request is a Python object
that contains the ID and the desired target filename. In the Export stage, the
source filename for the given ID is then resolved and the file is parsed, converted
and saved at the target location.

At the end of the mappping stage, the resulting nyan objects are put into nyan files
and -- together will the media export requests -- are organized into modpacks which
are passed to the exporter.

## Exporter

The exporter saves files contained in a modpack to the file system. nyan files
are stored as plaintext files, while media export requests are handled by parsing
the specified source file, converting it to a predefined target format and
writing the result into a file.
