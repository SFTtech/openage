// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "csv.h"

#include "file.h"
#include "../error/error.h"
#include "../log/log.h"


namespace openage {
namespace util {


csv_file_map_t load_multi_csv_file(Dir basedir, const std::string &fname) {
	std::string path = basedir.join(fname);

	log::log(MSG(info) << "Loading multi csv file: " << fname);
	std::vector<std::string> lines = file_get_lines(path);

	csv_file_map_t map;

	std::string current_file;

	for (auto& line : lines) {
		if (line[0] == '#' && line[1] == '#' && line[2] == ' ') {
			// start a new file in the map
			current_file = basedir.join(line.erase(0, 3)) + ".docx";
			map.emplace(current_file, std::vector<std::string>{});
		}
		else {
			if (line.empty() || line[0] == '#') {
				continue;
			}
			// add line to the current file linelist
			map.at(current_file).push_back(line);
		}
	}

	log::log(MSG(info) << "Loaded multi-csv file: "
	         << map.size() << " sub-files");
	return map;
}

}} // openage::util
