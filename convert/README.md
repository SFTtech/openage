how to convert gamedata
=======================

the original aoc game data format is, lets say, a challenge to use.
therefore it has to be converted in order to be usable with openage.

the python script for that is in the `convert/` folder, it will convert all the
media files to formats being usable by openage.

this also means that you have to own the original media files, as we are way
too lazy to create free media files for now, that's something the community can do..


to convert only media files we need at the moment, execute this:

	make media AGE2DIR="~/.wine-age/drive_c/programs/ms-games/aoe2"

the above command is an alias for the following python call:

	python3 convert media -o ./data/age/ $PATH_TO_YOUR_AOC_INSTALLATION $(make medialist)


to convert ALL original media files to openage format, execute the following:

	python3 convert media -o ./data/age $PATH_TO_YOUR_AOC_INSTALLATION

you will then find the converted files in `./data/age/`
you probably don't want to convert all the files (we don't use them yet..).
the conversion takes some time, so be patient.


try

	python3 convert --help

to see the convert script usage.
