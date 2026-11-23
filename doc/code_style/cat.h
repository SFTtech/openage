// Copyright 2026+ the openage authors. See copying.md for legal info.

// [SFT]tech coding guidelines
// ---------------------------
//
// New code should be written as C++ modules:
// See `cat.cppm` for an example.
//
// This is an example of old-style header inclusion for comparison.

#pragma once

// Includes are sorted alphabetically,
// std library includes first, then local ones:
#include <chrono>
#include <string>

// includes from the project use relative paths.
#include "../gordon.h"
#include "aperture/item.h"
#include "aperture/test_subject.h"

namespace aperture::companion {

/** Doxygen compatible documentation */
class Cat : public TestSubject {
	// see cat.cppm for class definition
};


} // namespace aperture::companion
