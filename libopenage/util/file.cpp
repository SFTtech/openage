// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "file.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../error/error.h"

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


}} // openage::util
