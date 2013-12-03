#include "color.h"

namespace openage {
namespace util {

Color::Color(unsigned r, unsigned g, unsigned b, unsigned a) : r(r), g(g), b(b), a(a) {}

Color::Color() : r(0), g(0), b(0), a(0) {}

Color::~Color() {}

} //namespace util
} //namespace openage
