#include "strings.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

namespace engine {
namespace util {

char *format(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char *result = vformat(fmt, ap);

	va_end(ap);

	return result;
}

char *vformat(const char *fmt, va_list ap) {
	va_list aq;
	va_copy(aq, ap);

	size_t sz = vsnprintf(NULL, 0, fmt, aq) + 1;
	va_end(aq);

	char *result = new char[sz];

	vsnprintf(result, sz, fmt, ap);

	return result;
}

char *copy(const char *s) {
	size_t sz = strlen(s) + 1;

	char *result = new char[sz];
	memcpy(result, s, sz);

	return result;
}

size_t rstrip(char *s) {
	size_t strippedlen = strlen(s);

	while (strippedlen > 0) {
		if (s[strippedlen - 1] == '\n' ||
		    s[strippedlen - 1] == ' '  ||
		    s[strippedlen - 1] == '\t') {

			strippedlen -= 1;
		} else {
			break;
		}
	}

	s[strippedlen] = '\0';

	return strippedlen;
}

void string_tokenize_base(char *str, char delim, std::function<void(char *)> callback) {
	callback(str);

	char *r = str;
	//the output buf is the input buf. w <= r is guaranteed.
	//(r - w) will increment each time an escape sequence is read.
	char *w = str;

	while (true) {
		if (*r == '\0') {
			//we have read the end of the input string
			//append '\0' to output string
			*(w++) = '\0';

			break;
		}
		if (*r == delim) {
			//we have read a deliminiter
			//write a '\0', and store the pointer to
			//the next token (the char after the '\0')

			*(w++) = '\0';

			callback(w);

			r++;

			continue;
		}
		if (*r == '\\') {
			//an escaped char: increment the read pointer to point
			//at the escape code.
			r++;

			//analyze the escape code
			switch (*r) {
			case '\0':
				//string ended in the middle of an escape code
				//error
				return;
				break;
			case 'n':
				//a newline
				*r = '\n';
				break;
			default:
				//the escape code already represents the literal
				//character (e.g.: '\\', '\,').
				break;
			}

			//'fall through' to the default action
		}

		//copy current char to output buf
		*(w++) = *(r++);
	}
}

size_t string_tokenize_to_buf(char *str, char delim, char **buf, size_t bufsize) {
	size_t count = 0;

	//my first lambda!
	std::function<void(char *)> callback = [&buf, bufsize, &count] (char *arg) -> void {
		if (count < bufsize) {
			buf[count] = arg;
		}
		count++;
	};

	string_tokenize_base(str, delim, callback);

	return count;
}

size_t string_tokenize_dynamic(char *str, char delim, char ***result) {
	std::vector<char *> resultvector;

	//my second lambda! (actually not so exciting anymore now.)
	std::function<void(char *)> callback = [&resultvector] (char *arg) -> void {
		resultvector.push_back(arg);
	};

	string_tokenize_base(str, delim, callback);

	*result = new char *[resultvector.size() + 1];
	for (size_t i = 0; i < resultvector.size(); i++) {
		(*result)[i] = resultvector[i];
	}
	(*result)[resultvector.size()] = nullptr;

	return resultvector.size();
}

} //namespace util
} //namespace engine
