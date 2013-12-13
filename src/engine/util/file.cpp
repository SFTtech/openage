#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../log.h"
#include "error.h"

namespace engine {
namespace util {


char *read_whole_file(const char *filename) {
	return read_whole_file_s(filename).content;
}

struct file_data read_whole_file_s(const char *filename) {

	//get the file size
	struct stat st;
	if (stat(filename, &st) < 0) {
		throw Error("failed getting filesize of %s", filename);
	}

	//open the file
	FILE *filehandle = fopen(filename, "r");
	if (filehandle == NULL) {
		throw Error("failed opening file %s", filename);
	}

	size_t content_length = (size_t) st.st_size;

	//read the whole content
	char *str = new char[content_length + 1];
	if (content_length != fread(str, 1, st.st_size, filehandle)) {
		throw Error("failed reading the file %s", filename);
	}

	//make sure 0-byte is at the end
	str[content_length] = '\0';

	//return the file contents
	struct file_data ret = {content_length, str};
	return ret;
}

template <class filestructure>
ssize_t read_structured_file(util::File<filestructure> *result, const char *fname) {
	struct file_data file = util::read_whole_file_s(fname);

	char *file_seeker = file.content;
	char *currentline = file.content;

	size_t line_count;
	size_t linepos = 0;

	bool wanting_count = true;

	result->free_content();

	while ((size_t)file_seeker <= ((size_t)file.content + file.size)
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
					result->content = new filestructure*[line_count];
				}
				else {
					unsigned int idx;
					if (wanting_count) {
						throw Error("meta info line found, but no entry count (n=...) found so far in %s", fname);
					}

					filestructure *line_data = new filestructure();
					if (0 == line_data->fill(currentline)) {
						if (linepos != line_data->idx) {
							throw Error("line index %u mismatch! should be %u.", idx, linepos);
						}
						result->content[idx] = line_data;
						linepos += 1;
					}
					else {
						throw Error("unknown line content reading texture meta file %s", fname);
					}
				}
			}
			currentline = file_seeker + 1;
		}

		file_seeker++;
	}

	delete file.content;

	return line_count;
}


template <class filestructure>
File<filestructure>::File() : size(0) {

}

template <class filestructure>
File<filestructure>::~File() {
	this->free_content();
}

template <class filestructure>
void File<filestructure>::free_content() {
	for (size_t i = 0; i < this->size; i++) {
		delete this->content[i];
	}
}

} //namespace util
} //namespace engine
