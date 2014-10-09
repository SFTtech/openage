How to convert gamedata?
========================

The original AOE:TC game data format is, lets say, a challenge to use.
therefore it has to be converted in order to be usable with openage.

The python script for that is in the `py/openage/convert/` folder,
it will convert all the original files to formats being usable by openage.

This also means that you have to own the original media files, as we are way
too lazy to create free media files for now, that's something the community can do..


Conversion invokation
----------------------

To convert only media files we need at the moment, execute this:

	make media AGE2DIR="~/.wine-age/drive_c/programs/ms-games/aoe2"

The above command is an alias for the following python call:

	python3 -m openage.convert media -o ./data/age/ $PATH_TO_YOUR_AOC_INSTALLATION $(make medialist)


To convert ALL original media files to openage format, execute the following:

	python3 -m openage.convert media -o ./data/age $PATH_TO_YOUR_AOC_INSTALLATION

You will then find the converted files in `./data/age/`
the conversion takes some time, so be patient.


Try

	python3 -m openage.convert --help

to see the convert script usage.
