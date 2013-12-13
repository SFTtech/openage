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

struct line_data {
	virtual ~line_data() {};

	//fancy inheritance stuff, =0 means this method is abstract
	virtual int fill(const char *by_line) =0;
	unsigned int idx;
};

template <class lineformat>
class File {
public:
	File() : size(0) {}
	~File() {
		this->free_content();
	}

	size_t size;
	lineformat **content;

	void free_content();
};

template <class lineformat>
void File<lineformat>::free_content() {
	for (size_t i = 0; i < this->size; i++) {
		//TODO: this segfaults...
		delete this->content[i];
	}
}



struct file_data *read_whole_file_s(const char *filename);
char *read_whole_file(const char *filename);

template <class lineformat>
ssize_t read_structured_file(util::File<lineformat> *result, const char *fname);



template <class lineformat>
ssize_t read_structured_file(util::File<lineformat> *result, const char *fname) {
	struct file_data *file = util::read_whole_file_s(fname);

	char *file_seeker = file->content;
	char *currentline = file->content;

	size_t line_count;
	size_t linepos = 0;

	bool wanting_count = true;

	result->free_content();

	while ((size_t)file_seeker <= ((size_t)file->content + file->size)
	       && *file_seeker != '\0'
	       && linepos < line_count) {

		if (*file_seeker == '\n') {
			*file_seeker = '\0';

			//ignore lines starting with #, that's a comment.
			if (*currentline != '#') {

				//scan for the entry count definition
				if (sscanf(currentline, "n=%lu", &line_count) == 1) {
					wanting_count = false;
					//allocate memory for entries
					result->content = new lineformat*[line_count];
				}
				else {
					if (wanting_count) {
						throw Error("meta info line found, but no entry count (n=...) found so far in %s", fname);
					}

					auto *line_data = new lineformat{};
					if (0 == line_data->fill(currentline)) {
						if (linepos != line_data->idx) {
							throw Error("line index %u mismatch! should be %lu.", line_data->idx, linepos);
						}
						result->content[line_data->idx] = line_data;
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

	log::dbg("%lu lines found in total", linepos);
	result->size = linepos;

	delete file;

	return line_count;
}

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
