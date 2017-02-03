// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "csv.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "../error/error.h"
#include "../log/log.h"


namespace openage {
namespace util {


csv_file_map_t *load_multi_csv_file(Dir basedir, const std::string &fname) {
	std::string path = basedir.join(fname);

	log::log(MSG(info) << "Loading multi csv file: " << fname);
	std::vector<std::string> lines = file_get_lines(path);

	// TODO ASDF no new!
	csv_file_map_t *map = new csv_file_map_t();

	std::string current_file = "";
	for (auto& line : lines) {
		if (line[0] == '#' && line[1] == '#' && line[2] == ' ') {
			current_file = basedir.join(line.erase(0, 3)) + ".docx";
			map->emplace(current_file, std::vector<std::string>());
		}
		else {
			if (line.empty() || line[0] == '#') {
				continue;
			}
			map->at(current_file).push_back(line);
		}
	}
	log::log(MSG(info) << "Loaded csv files: " << map->size());
	return map;
}

}} // openage::util
