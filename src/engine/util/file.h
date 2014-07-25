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

template <class cls>
struct subdata {
	std::string filename;
	std::vector<cls> data;

	bool fill(Dir basedir);
};


ssize_t read_whole_file(char **result, const char *filename);
ssize_t read_whole_file(char **result, std::string filename);


/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(std::string &file_name) {
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

/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template <class lineformat>
std::vector<lineformat> read_csv_file(std::string &fname) {
	std::vector<std::string> lines = file_get_lines(fname);

	size_t line_count = 0;
	lineformat current_line_data;
	auto result = std::vector<lineformat>{};

	for (auto &line : lines) {
		//ignore lines starting with #, that's a comment.
		if (line.length() > 0 && line[0] != '#') {
			if (not current_line_data.fill(line)) {
				throw Error("failed reading csv file %s in line %lu: error parsing '%s'",
				            fname.c_str(), line_count, line.c_str());
			}

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
std::vector<lineformat> recurse_data_files(Dir basedir, std::string &fname) {
	std::string merged_filename = basedir.join(fname);
	auto result = read_csv_file<lineformat>(merged_filename);

	//the new basedir is the old basedir
	// + the directory part of the current relative file name
	Dir new_basedir = basedir.append(dirname(fname));

	size_t line_count = 0;
	for (auto &line : result) {
		if (not line.recurse(new_basedir)) {
			throw Error("failed reading follow up files for %s in line %lu: failed when processing '%s'",
			            merged_filename.c_str(), line_count, line.c_str());
		}
		line_count += 1;
	}

	return result;
}


template <class lineformat>
std::vector<lineformat> read_csv_file(const char *fname) {
	std::string filename{fname};
	return read_csv_file<lineformat>(filename);
}

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
