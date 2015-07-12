// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_FILE_H_
#define OPENAGE_UTIL_FILE_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "../error/error.h"

#include "dir.h"

namespace openage {
namespace util {

ssize_t file_size(const std::string &filename);
ssize_t file_size(Dir basedir, const std::string &fname);

ssize_t read_whole_file(char **result, const char *filename);
ssize_t read_whole_file(char **result, const std::string &filename);

/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(const std::string &file_name);


/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template<class lineformat>
std::vector<lineformat> read_csv_file(const std::string &fname) {
	std::vector<std::string> lines = file_get_lines(fname);

	size_t line_count = 0;

	lineformat current_line_data;
	auto result = std::vector<lineformat>{};

	for (auto &line : lines) {
		int line_length = line.length();
		line_count += 1;

		//ignore empty and lines starting with #, that's a comment.
		if (line_length > 0 && line[0] != '#') {

			//create writable tokenisation copy of the string
			char *line_rw = new char[line_length + 1];
			strncpy(line_rw, line.c_str(), line_length);
			line_rw[line_length] = '\0';

			//use the line copy to fill the current line struct.
			int error_column = current_line_data.fill(line_rw);
			if (error_column != -1) {
				throw Error(MSG(err) <<
					"Failed to read CSV file: " <<
					fname << ":" << line_count << ":" << error_column << ": "
					"error parsing " << line);
			}

			delete[] line_rw;

			result.push_back(current_line_data);
		}
	}

	return result;
}

/**
 * reads data files recursively.
 * should be called from the .recurse() method of the struct.
 */
template<class lineformat>
std::vector<lineformat> recurse_data_files(Dir basedir, const std::string &fname) {
	std::vector<lineformat> result;
	std::string merged_filename = basedir.join(fname);

	if (0 < file_size(merged_filename)) {
		result = read_csv_file<lineformat>(merged_filename);

		//the new basedir is the old basedir
		// + the directory part of the current relative file name
		Dir new_basedir = basedir.append(dirname(fname));

		size_t line_count = 0;
		for (auto &entry : result) {
			line_count += 1;
			if (not entry.recurse(new_basedir)) {
				throw Error(MSG(err) <<
					"Failed to read follow-up files for " <<
					merged_filename << ":" << line_count);
			}
		}
	}
	else {
		//nonexistant file skipped, would return empty vector.
		throw Error(MSG(err) << "Failed to recurse to file " << merged_filename);
	}

	return result;
}


template<class lineformat>
std::vector<lineformat> read_csv_file(const char *fname) {
	std::string filename{fname};
	return read_csv_file<lineformat>(filename);
}

/**
 * referenced file tree structure.
 *
 * used to store the filename and resulting data of a file down
 * the gamedata tree.
 */
template<class cls>
struct subdata {
	std::string filename;
	std::vector<cls> data;

	bool read(Dir basedir) {
		this->data = recurse_data_files<cls>(basedir, this->filename);
		return true;
	}

	cls operator [](size_t idx) const {
		return this->data[idx];
	}
};


}} // openage::util

#endif
