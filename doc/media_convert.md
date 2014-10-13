How to use the original game assets?
------------------------------------

Openage currently depends on the original game assets, so you need a copy of the original *Microsoft Age of Empires II*.
The original AoE:TC game asset formats are, lets say, "special", so they need to be converted in order to be usable from openage.
That conversion is performed by the openage.convert python module; as of writing this text, you need to start the conversion manually.


Convert script invocation
-------------------------

To convert only media files we need at the moment, execute this:

	make media AGE2DIR="~/.wine-age/drive_c/programs/ms-games/aoe2"

The above command is an alias for the following python call:

	python3 -m openage.convert media -o ./userassets $PATH_TO_YOUR_AOC_INSTALLATION $(make medialist)


To convert ALL original media files to openage format, execute the following:

	python3 -m openage.convert media -o ./userassets $PATH_TO_YOUR_AOC_INSTALLATION

You will then find the converted files in `./userassets`
the conversion takes some time, so be patient.


Try

	python3 -m openage.convert --help

to see the convert script usage.
