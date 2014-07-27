#ifndef _ENGINE_UTIL_FILE_H_
#define _ENGINE_UTIL_FILE_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "error.h"
#include "dir.h"
#include "../log.h"

namespace engine {
namespace util {

ssize_t file_size(std::string filename);
ssize_t file_size(Dir basedir, std::string fname);

ssize_t read_whole_file(char **result, const char *filename);
ssize_t read_whole_file(char **result, std::string filename);

/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(std::string &file_name);


/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template <class lineformat>
std::vector<lineformat> read_csv_file(std::string fname) {
	std::vector<std::string> lines = file_get_lines(fname);

	size_t line_count = 0;
	lineformat current_line_data;
	auto result = std::vector<lineformat>{};

	for (auto &line : lines) {
		int line_length = line.length();

		//ignore lines starting with #, that's a comment.
		if (line_length > 0 && line[0] != '#') {

			//create writable tokenisation copy of the string
			char *line_rw = new char[line_length + 1];
			strncpy(line_rw, line.c_str(), line_length);
			line_rw[line_length] = '\0';

			//use the line copy to fill the current line struct.
			if (not current_line_data.fill(line_rw)) {
				throw Error("failed reading csv file %s in line %lu: error parsing '%s'",
				            fname.c_str(), line_count, line.c_str());
			}

			delete[] line_rw;

			result.push_back(current_line_data);
		}
		line_count += 1;
	}

	return result;
}

/**
 * reads data files recursively.
 * should be called from the .recurse() method of the struct.
 */
template <class lineformat>
std::vector<lineformat> recurse_data_files(Dir basedir, std::string fname) {
	std::vector<lineformat> result;
	std::string merged_filename = basedir.join(fname);

	if (0 < file_size(merged_filename)) {
		result = read_csv_file<lineformat>(merged_filename);

		//the new basedir is the old basedir
		// + the directory part of the current relative file name
		Dir new_basedir = basedir.append(dirname(fname));

		size_t line_count = 0;
		for (auto &entry : result) {
			if (not entry.recurse(new_basedir)) {
				throw Error("failed reading follow up files for %s in line %lu",
				            merged_filename.c_str(), line_count);
			}
			line_count += 1;
		}
	}
	else {
		//nonexistant file skipped, return empty vector.
	}

	return result;
}


template <class lineformat>
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
template <class cls>
struct subdata {
	std::string filename;
	std::vector<cls> data;

	bool fill(Dir basedir) {
		this->data = recurse_data_files<cls>(basedir, this->filename);
		return true;
	}

	cls operator [](size_t idx) {
		return this->data[idx];
	}
};


} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
