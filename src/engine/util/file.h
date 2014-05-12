#ifndef _ENGINE_UTIL_FILE_H_
#define _ENGINE_UTIL_FILE_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "error.h"
#include "../log.h"

namespace engine {
namespace util {

/**
 * stores the link between fileename and it's content
 */
template <class lineformat>
struct subdata {
	std::string filename;
	std::vector<lineformat> data;

	void fill() {
		this->data = read_csv_file<lineformat>(this->filename.c_str());
	}
};

ssize_t read_whole_file(char **result, const char *filename);


template <class lineformat>
std::vector<lineformat> read_csv_file(const char *fname) {
	char *file_content;
	ssize_t fsize = util::read_whole_file(&file_content, fname);

	char *file_seeker = file_content;
	char *current_line = file_content;
	bool fill_result;
	size_t line_length = 0;
	size_t line_count  = 0;
	lineformat current_line_data;

	auto result = std::vector<lineformat>{};

	while ((size_t)file_seeker <= ((size_t)file_content + fsize)
	       && *file_seeker != '\0') {

		if (*file_seeker == '\n') {
			*file_seeker = '\0';

			//measure length to ignore empty lines
			line_length = strlen(current_line);

			//ignore lines starting with #, that's a comment.
			if (*current_line != '#' && line_length > 0) {

				//parse the line data to the temporary result
				fill_result = current_line_data.fill(current_line);

				//filling the line failed
				if (!fill_result) {
					throw Error("failed reading meta file %s in line %lu: error parsing '%s'", fname, line_count, current_line);
				}

				//store the line in the returned vector
				result.push_back(current_line_data);
			}
			current_line = file_seeker + 1;
			line_count += 1;
		}
		file_seeker += 1;
	}

	//log::dbg("%lu lines found in total", linepos);
	delete[] file_content;

	return result;
}

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
