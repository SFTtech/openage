nyan data conversion
====================

We depend on the original game.
We have a converter which spits out a files in our formats
by parsing the original data.

Gamedata conversion
-------------------

* `openage/convert/gamedata/` specifies the format of `empires_x1_p1.dat`
  * this format allows reading the binary file and storing it into tables
  * these tables and the original format suck,
    information is redundant and we can heavily improve it
  * buuut: the file format is given (every original game has it.)
  * :arrow_right: data transformation.
* the original file will be read according to the original format (we can't help it)
* a transformation is specified
  * mapping:   original format -> our awesome format
  * redundancy is purged (e.g. no more double huskarl unit (1x castle, 1x barracs))
