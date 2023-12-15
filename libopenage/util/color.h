// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once


namespace openage {
namespace util {

struct col {
	col(unsigned r, unsigned g, unsigned b, unsigned a) :
		r{r}, g{g}, b{b}, a{a} {}

	unsigned r, g, b, a;

	void use();
	void use(float alpha);
};

} // namespace util
} // namespace openage
