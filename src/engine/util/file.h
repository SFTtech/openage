#ifndef _ENGINE_UTIL_FILE_H_
#define _ENGINE_UTIL_FILE_H_

#include <stdlib.h>

#include "error.h"
#include "../log.h"

namespace engine {
namespace util {

struct file_data {
	file_data(size_t size, char *content) : size(size), content(content) {}
	~file_data() {
		if (this->size > 0) {
			delete[] content;
		}
	}

	size_t size;
	char *content;
};

struct csv_line_data {
	virtual ~csv_line_data() {}

	//fancy inheritance stuff, =0 means this method is abstract
	virtual int fill(const char *by_line) =0;
	unsigned int idx;
};


ssize_t read_whole_file(char **result, const char *filename);


template <class lineformat>
ssize_t read_csv_file(lineformat **result, const char *fname) {
	char *file_content;
	ssize_t fsize = util::read_whole_file(&file_content, fname);

	char *file_seeker = file_content;
	char *currentline = file_content;
	size_t line_count;
	size_t linepos = 0;
	bool wanting_count = true;

	*result = nullptr;

	//log::dbg("parsing csv %s:\n%s", fname, file_content);

	while ((size_t)file_seeker <= ((size_t)file_content + fsize)
	       && *file_seeker != '\0'
	       && (wanting_count || linepos < line_count)) {

		if (*file_seeker == '\n') {
			*file_seeker = '\0';

			//ignore lines starting with #, that's a comment.
			if (*currentline != '#') {
				//scan for the entry count definition
				if (sscanf(currentline, "n=%lu", &line_count) == 1) {
					if (wanting_count) {
						wanting_count = false;
						//create handlers for the lines
						*result = new lineformat[line_count];
					}
					else {
						throw Error("Already got size definition (n=...) for %s", fname);
					}
				}
				else {
					if (wanting_count) {
						throw Error("meta info line found, but no entry count (n=...) found so far in %s", fname);
					}

					if (0 == (*result)[linepos].fill(currentline)) {
						if (linepos != (*result)[linepos].idx) {
							throw Error("line index %u mismatch! should be %lu.", (*result)[linepos].idx, linepos);
						}

						linepos += 1;
					}
					else {
						throw Error("unknown line content reading meta file %s", fname);
					}
				}
			}
			currentline = file_seeker + 1;
		}
		file_seeker += 1;
	}

	//log::dbg("%lu lines found in total", linepos);
	delete[] file_content;

	return linepos;
}

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
