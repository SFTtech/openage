#ifndef _COLOR_H_
#define _COLOR_H_

namespace openage {
namespace util {

class Color {
public:
	Color(unsigned r, unsigned g, unsigned b, unsigned a);
	Color();
	~Color();

	unsigned int r, g, b, a;
};

} //namespace util
} //namespace openage
#endif //_COLOR_H_
