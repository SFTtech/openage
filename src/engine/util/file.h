#ifndef _ENGINE_UTIL_FILE_H_
#define _ENGINE_UTIL_FILE_H_

#include <stdlib.h>

namespace engine {
namespace util {

struct file_data {
	size_t size;
	char *content;
};

struct line_data {
	int fill(const char *by_line);
	unsigned int idx;
};

template <class filestructure>
class File {
public:
	File();
	~File();

	size_t size;
	filestructure **line;

	void free_content();
};

struct file_data read_whole_file_s(const char *filename);
char *read_whole_file(const char *filename);

template <class filestructure> ssize_t read_structured_file(util::File<filestructure> *result, const char *fname);


} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_FILE_H_
