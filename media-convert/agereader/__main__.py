#!/usr/bin/python3
from util import args, mkdirs
from drs import DRS

def main():
	src = args.srcdir
	dest = args.destdir

	drsfiles = {
		"graphics": DRS(src + "/Data/graphics.drs"),
		"interfac": DRS(src + "/Data/interfac.drs"),
		"gamedata": DRS(src + "/Data/gamedata.drs"),
		"sounds": DRS(src + "/Data/sounds.drs"),
		"terrain": DRS(src + "/Data/terrain.drs")
	}

	for drsfilename in drsfiles:
		drsfile = drsfiles[drsfilename]
		path = dest + "/" + drsfilename + ".drs"
		mkdirs(path)

		for file_extension, file_id in drsfile.files:
			file_data = drsfile.get_file_data(file_extension, file_id)
			open(path + "/" + str(file_id) + "." + file_extension, 'wb').write(file_data)

if __name__ == "__main__":
	main()
