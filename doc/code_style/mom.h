// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_DOC_CODESTYLE_MOM_H_
#define OPENAGE_DOC_CODESTYLE_MOM_H_

// [SFT]tech coding guidelines
// ---------------------------
//
// Please also have a look at `mom.cpp`!
// To make the code look nice and be readable:
//  * Sort things alphabetically.
//  * Align things beautifully!
//  * If you write documentation correctly, then:
//    * It's parsable by Doxygen!
//    * -> Cool generated docs!
//  * Look at the following example code.
//  * Look at the "mom.cpp" file for a implementation example.
//  * Comment guideline: Use punctuation!

#include <cstddef>
#include <cstdint>


// Try to keep the code < 120 chars in width (no hard limit...).
// All documentation is in the header.
// No swearing in comments!

// No preprocessor shit crap!
constexpr int DEFAULT_MOM_MASS = 9001;


// Doxygen will then generate nice shiny documentation files.
// Exported (API) documentation of all source code is in the header files:

/**
 * You may document a namespace if it's something special.
 * This documentation will also show up in generated docs then.
 */
namespace parents {
namespace mom {
// <- The namespace does not increase the indentation level!


/**
 * Class about your mother.
 * Manages all kinds of useful information about your mother.
 *
 * Classes are written in CamelCase!
 */
class YourMom : public MassiveEntity {
public:
	/**
	 * Creates your mom.
	 * @param mass: Initial mass for this mother.
	 */
	YourMom(int mass=DEFAULT_MOM_MASS);

	~YourMom();
	// Note that the destructor has no doc,
	// Because that would be entirely unnecessary (in most cases).

	/**
	 * Prints your mom's current mass to stdout.
	 */
	void print_mass();

	/**
	 * Ticks your mom.
	 * Simulates her for one physics frame.
	 *
	 * @see mass
	 * @returns true if the simulation was successful.
	 */
	bool tick();

protected:
	// To have doxygen parse one-liners, write the following:
	int64_t mass; //!< Your mom's mass (solar masses).

	/**
	 * Your mom's number of non-artificial satellites.
	 * Scales well with your mom's mass.
	 * @see mass.
	 */
	int number_of_moons;

	/**
	 * Your mom's stupidity.
	 * Measured in unit of tolerance towards DRM.
	 */
	int stupidity;

	static int64_t sum_mom_masses; //!< Sum of all mom's masses in the world.
};


}} // namespace parents::mom


#endif
