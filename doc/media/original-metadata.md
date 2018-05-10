Original Metadata
=================

All data relevant for the game (e.g. how much costs building the castle?
What cultures exist? Can my priest overclock his "Wololo?")
are stored in a binary format in the file `empires2_x1_p1.dat`.

The format is described in the [huge struct definiton](/openage/convert/gamedata/empiresdat.py).


These data files are basically a huge tree structure.
We don't handle cross-references to make this tree a graph in the convert
script (like id mappings, e.g. villager has creation sound id 42,
sound 42 plays the 1337.wav).

All data values are exported (in Python) to human-readable formats,
and get imported by the C++ Engine again.
The storage format is completely replaceable that way.

For simplicity the minimal format is `CSV`.

The data will get converted to [`nyan`](/doc/nyan), our custom data language.
It's designed to be redundancy-free, well readable and specially optimized for
moddability.
