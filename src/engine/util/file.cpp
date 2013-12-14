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
		throw Error("failed reading the file %s", filename);
	}

	//make sure 0-byte is at the end
	(*result)[content_length] = '\0';

	//return the file size
	return content_length;
}


} //namespace util
} //namespace engine
