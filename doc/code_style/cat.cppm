// Copyright 2026+ the openage authors. See copying.md for legal info.

// [SFT]tech coding guidelines
// ---------------------------
//
// Please also have a look at `cat.cpp`!
//
// New code is written as C++ modules (example for old style headers in `cat.h`).
//
// Usually one .cpp file is the whole module, like `libopenage/util/color.cpp`:
// module declaration & the exported interface & implementation.
//
// Heavy implementation should be split off in a .cppm interface unit
// so changes of the implementation doesn't recompile all importers.
// (just like the "usual" split of .h and .cpp).
//
// Our cat is heavy, so she is split:
//  * `cat.cppm`: the interface unit, exported and documented (this file)
//  * `cat.cpp`: the implementation unit
//
// To make the code look nice and be readable:
//  * Sort things alphabetically
//  * Align continuation lines with spaces, but don't pad consecutive lines
//    into columns: realigning them on every change causes noisy diffs.
//  * If you write documentation correctly, then:
//    * It's parsable by Doxygen!
//    * -> Cool generated docs!
//  * Comment guideline: Use punctuation!


// A module unit starts with the global module fragment,
// the only place for #includes:
module;

// Includes are sorted alphabetically,
// std library includes first, then local ones:
#include <chrono>
#include <string>
#include <unordered_map>

#include "aperture/item.h"
#include "aperture/test_subject.h"

// The module name follows the file path,
// like `openage.util.color` for `libopenage/util/color.cpp`:
export module aperture.companion.cat;


// Try to keep the code < 120 chars in width (no hard limit...).
// All documentation is in the interface unit.
// No swearing in comments!


// Doxygen will generate nice shiny documentation files.
// Only exported names are visible to importers,
// so this is where the API and its documentation live:

/**
 * You may document a namespace if it's something special.
 * This documentation will also show up in generated docs then.
 */
export namespace aperture::companion {
// <- The namespace does not increase the indentation level!

// No preprocessor shit crap!
constexpr int DEFAULT_CAT_CURIOSITY = 42;

// A forward declaration attaches the name to the module it's written in,
// so types of headers and other modules can't be forward declared:
// include or import them instead.


/**
 * Type aliases are done with "using".
 * They can get proper documentation, too!
 */
using nap_schedule_t = std::unordered_map<std::string,
                                          std::chrono::minutes>;


/**
 * A cat in a test chamber.
 * Manages all kinds of useful information about your test subject.
 *
 * Classes are written in CamelCase!
 */
class Cat : public TestSubject {
public:
	/**
	 * Creates your cat.
	 * @param name: What to yell when the cat ignores you.
	 * @param curiosity: Initial curiosity (kills the cat, eventually).
	 */
	Cat(const std::string &name,
	    int curiosity = DEFAULT_CAT_CURIOSITY);

	~Cat();
	// Note that the destructor has no doc,
	// Because that would be entirely unnecessary (in most cases).

	/**
	 * Logs your cat's current mood.
	 */
	void print_mood() const;

	/**
	 * Ticks your cat.
	 * Simulates her for one physics frame.
	 *
	 * @see curiosity
	 * @returns true if the simulation was successful
	 *          and nothing was pushed off a table.
	 */
	bool tick();

protected:
	/** Let your cat push something off a table. */
	void push_off(const Item &item);

	/**
	 * Your cat's name.
	 * Yell it to watch the cat ignore you.
	 */
	std::string name;

	// To have doxygen parse one-liners, write the following:
	int lives; //!< Your cat's number of remaining lives.

	/**
	 * Your cat's curiosity.
	 * Measured in units of "what happens if I push this?".
	 * @see lives
	 */
	int curiosity;

	/** Where and how long your cat naps, regardless of your plans. */
	nap_schedule_t naps;
};


// Enum class names are lowercase_with_underscore
// if they somehow describe a type, use name_t.

enum class mood_t {
	SLEEPY,
	PLAYFUL,
	GRUMPY, // <- note the last , there!
};


/**
 * Sends a cat through a test chamber, once per life.
 * The cat does not care.
 */
void chamber_demo();

} // namespace aperture::companion
