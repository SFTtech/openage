// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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


ssize_t read_whole_file(char **result, const std::string &filename) {
	return read_whole_file(result, filename.c_str());
}

ssize_t read_whole_file(char **result, const char *filename) {

	//get the file size
	ssize_t content_length = file_size(filename);

	if (content_length < 0) {
		throw Error(MSG(err) << "File nonexistant: " << filename);
	}

	//open the file
	FILE *filehandle = fopen(filename, "r");
	if (filehandle == NULL) {
		throw Error(MSG(err) << "Failed to open file: " << filename);
	}

	//allocate filesize + nullbyte
	*result = new char[content_length + 1];

	//read the whole content
	if (content_length != (ssize_t)fread(*result, 1, content_length, filehandle)) {
		fclose(filehandle);
		throw Error(MSG(err) << "Failed to read file: " << filename);
	} else {
		fclose(filehandle);
	}

	//make sure 0-byte is at the end
	(*result)[content_length] = '\0';

	//return the file size
	return content_length;
}

std::vector<std::string> file_get_lines(const std::string &file_name) {
	char *file_content;
	ssize_t fsize = util::read_whole_file(&file_content, file_name);

	char *file_seeker = file_content;
	char *current_line = file_content;

	auto result = std::vector<std::string>{};

	while ((size_t)file_seeker <= ((size_t)file_content + fsize)
	       && *file_seeker != '\0') {

		if (*file_seeker == '\n') {
			*file_seeker = '\0';

			result.push_back(std::string{current_line});

			current_line = file_seeker + 1;
		}
		file_seeker += 1;
	}

	delete[] file_content;
	return result;
}

std::unordered_map<std::string, std::vector<std::string>> csv_file_map;

void load_csv_files(std::string path) {

	log::log(MSG(info) << "Loading csv files");
	if (file_size(path) == -1) {
		log::log(MSG(info) << "Cannot find csv file at " << path);
		return;
	}

	std::vector<std::string> lines = file_get_lines(path);

	std::string current_file = "";
	for (auto& line : lines) {
		if (line[0] == '#' && line[1] == '#' && line[2] == ' ') {
			current_file = "./assets/converted" + line.erase(0, 3);
			csv_file_map.emplace(current_file, std::vector<std::string>());
		}
		else {
			if (line.empty() || line[0] == '#') {
				continue;
			}
			csv_file_map.at(current_file).push_back(line);
		}
	}
	log::log(MSG(info) << "Loaded csv files: " << csv_file_map.size());
}

}} // openage::util
