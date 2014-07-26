#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../log.h"
#include "error.h"

namespace engine {
namespace util {

template <class lineformat>
bool subdata<lineformat>::fill(Dir basedir) {
	this->data = recurse_data_files<lineformat>(basedir, this->filename);
	return true;
}


ssize_t read_whole_file(char **result, std::string filename) {
	return read_whole_file(result, filename.c_str());
}

ssize_t read_whole_file(char **result, const char *filename) {

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

	//allocate filesize + nullbyte
	*result = new char[content_length + 1];

	//read the whole content
	if (content_length != fread(*result, 1, st.st_size, filehandle)) {
		fclose(filehandle);
		throw Error("failed reading the file %s", filename);
	} else {
		fclose(filehandle);
	}

	//make sure 0-byte is at the end
	(*result)[content_length] = '\0';

	//return the file size
	return content_length;
}

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


} //namespace util
} //namespace engine
