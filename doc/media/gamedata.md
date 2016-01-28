Game Data
=========

All data relevant for the game (e.g. how much costs building the castle?
What cultures exist? Can my priest overclock his "Wololo?")
are stored in a binary format in the file `empires2_x1_p1.dat`.
The format is descibed in the [huge struct definiton](/doc/media/gamedata-struct.md).

The version the convert script currently is able to process: `"VER 5.7\x00"`

These data files are basically a huge tree structure.
We don't handle cross-references to make this tree a graph in the convert
script (like id mappings, e.g. villager has creation sound id 42,
sound 42 plays the 1337.wav).

All data values are exported (in Python) to human-readable formats,
and get imported by the C++ Engine again.
The storage format is completely replacable that way.

For simplicity the minimal format is `CSV`.
After that, `YAML` might be a candidate.

The long term goal is [`nyan`](/doc/nyan), which will be a custom language.
It's designed to be redundancy-free, well readable and
specially optimized for moddability.
