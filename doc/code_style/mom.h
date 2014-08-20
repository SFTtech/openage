#ifndef _MOM_H_
#define _MOM_H_

#include <stdint.h>

// see `mom.cpp` for the code style entry point.

// try to keep the code < 120 chars in width (no hard limit...)
// all documentation is in the header
// no swearing in comments!

// no preprocessor shit crap!
constexpr int DEFAULT_MOM_MASS = 9001;

/**
 * you may document a namespace if it's something special.
 * this documentation will also show up in generated docs then.
 */
namespace elts {

// global variables are declared extern in the header.
extern int64_t sum_mom_masses;

/**
 * class about your mother.
 * manages all kinds of useful information about your mother.
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
	 * prints your mom's current mass to stdout.
	 */
	void print_mass();

	/**
	 * ticks your mom.
	 * simulates her for one physics frame.
	 *
	 * @see mass
	 * @returns true if the simulation was successful.
	 */
	bool tick();

protected:
	int mass; //!< your mom's mass (solar masses).

	/**
	 * your mom's number of non-artificial satellites.
	 * scales well with your mom's mass.
	 */
	int number_of_moons;
};

} // namespace elts


// try to avoid forward declarations,
// use the newly defined types after their definition.
extern elts::YourMom *my_special_mom;


#endif //_MOM_H_
