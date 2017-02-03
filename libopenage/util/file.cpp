// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "file.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../error/error.h"
#include "../log/log.h"


namespace openage {
namespace util {

ssize_t file_size(const std::string &filename) {
	struct stat st;

	if (stat(filename.c_str(), &st) < 0) {
		return -1;
	}

	return st.st_size;
}

ssize_t file_size(Dir basedir, const std::string &fname) {
	return file_size(basedir.join(fname));
}


std::string read_whole_file(const std::string &filename) {
	std::ifstream file{filename};

	std::string str{
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	};

	return str;
}

std::vector<std::string> file_get_lines(const std::string &file_name) {
	std::string line;
	std::vector<std::string> result{};
	std::ifstream file{file_name};

	while (std::getline(file, line)) {
		result.push_back(line);
	}

	return result;
}

csv_file_map_t *csv_file_map = nullptr;

csv_file_map_t *load_multi_csv_file(Dir basedir, const std::string &fname) {
	std::string path = basedir.join(fname);

	log::log(MSG(info) << "Loading multi csv file: " << fname);
	std::vector<std::string> lines = file_get_lines(path);

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
