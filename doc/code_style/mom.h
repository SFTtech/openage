#ifndef OPENAGE_DOC_CODESTYLE_MOM_H_
#define OPENAGE_DOC_CODESTYLE_MOM_H_

// [SFT]tech coding guidelines
// ---------------------------
//
// please also have a look at `mom.cpp`
// to make the code look nice and be readable:
//  * sort things alphabetically.
//  * align things beautifully
//  * if you write documentation correctly, then:
//    * it's parsable by Doxygen
//    * cool generated docs!
//  * look at the following example code
//  * look at the "mom.cpp" file for a implementation example.


// .h files that don't need header guards shouldn't exist.
// if they are necessary, they must be explicitly marked with a comment
// of the following format:

// has no header guard: including your mom _twice_ would be impossible anyway.

#include <cstddef>
#include <cstdint>

// Try to keep the code < 120 chars in width (no hard limit...)
// All documentation is in the header
// No swearing in comments!

// No preprocessor shit crap!
constexpr int DEFAULT_MOM_MASS = 9001;


// Doxygen will then generate nice shiny documentation files.
// Exported (API) documentation of all source code is in the header file:

/**
 * You may document a namespace if it's something special.
 * This documentation will also show up in generated docs then.
 */
namespace elts {

// the namespace does not increase the indentation level!

/**
 * Write a short introductory sentence here first.
 * Then you can explain what the class/struct/variable/method is doing,
 * explain why it is the way it is.
 * If you got time afterwards, explain how to achieve world peace,
 * solve P=NP or just browse cat pictures on the internet.
 *
 * Global variables are declared extern in the header.
 * Keep in mind that we will probably murder you if you actually
 * use global variables.
 */
extern int64_t sum_mom_masses;

/**
 * Class about your mother.
 * Manages all kinds of useful information about your mother.
 *
 * Classes are written in CamelCase!
 */
class YourMom {
public:
	/**
	 * @param mass: initial mass for this mother.
	 */
	YourMom(int mass=DEFAULT_MOM_MASS);

	~YourMom();
	// note that the destructor has no doc,
	// because that would be entirely unnecessary.

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
	int mass; //!< your mom's mass (solar masses).

	/**
	 * Your mom's number of non-artificial satellites.
	 * Scales well with your mom's mass.
	 */
	int number_of_moons;
};

} // namespace elts


// Try to avoid forward declarations,
// use the newly defined types after their definition.
// avoid global variables whenever possible though!
extern elts::YourMom *my_special_mom;


#endif
