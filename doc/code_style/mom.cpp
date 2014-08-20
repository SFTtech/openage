#include "mom.h"

#include <cstdio>
#include <SDL/SDL.h>

#include "lolinclude.h"
#include "../whatever.h"

// [SFT]tech coding guidelines
// ---------------------------
//
// to make the code look nice and be readable:
//  * sort things alphabetically.
//  * align things beautifully
//  * if you write documentation correctly, then:
//    * it's parsable by Doxygen
//    * cool generated docs!
//  * look at the following example code
//  * look at the "mom.h" header file for a header example.


// indentation:
//    ========> see doc/code_style/tabs_n_spaces.md <=========
// use tabs to indent code, if you display that as 4 or 8 or 13.37 doesn't matter.
// use spaces for drawing ascii-art or adapting code positions after indentation.
//
// => this ensures custom tab-widths AND correct indentation levels.



/**
 * Write a short introductory sentence here first.
 * Then you can explain what the class/struct is doing,
 * explain why it is the way it is.
 * If you got time afterwards, explain how to achieve world peace,
 * solve P=NP or just browse cat pictures on the internet.
 *
 * Write class name in CamelCase!
 */
class UselessDemonstration {
// the access modifier is at the same indent level
// as the class definition:
public:
	// i hope you read the tabs_n_spaces.md file,
	// now follows a practical application for it:

	// example for using tabs for indentation and spaces for adjustment
	// the argument list is too long, therefore wrap it
	// => tabs are used to reach level of `void`,
	//    spaces indent the alignment of each argument.
	void lol_useless_function(char *rofl_a_text,
	                          int   asdf,
	                          bool  another_argument,
	                          char  yeah_really_thats_many_arguments,
	                          int   you_wont_believe_me,
	                          bool  fak_u) {
		if (fak_u) {
			// try to align things nicely:
			asdf                *= 20;
			you_wont_believe_me  = 5;

			printf("%s: %d \n", rofl_a_text, asdf);
		}
		else if (another_argument) {
			// the argument list is too long again, so wrap the args
			// and indent them with spaces.
			printf("%05d pls... %c %s\n",
			       you_wont_believe_me,
			       yeah_really_thats_many_arguments,
			       rofl_a_text);
		}
		else {
			printf("%s trololo\n", rofl_a_text);
		}
	}
};


/**
 * Just a short struct demonstration.
 * Structs should be used to pack variables etc.
 * Use as few member functions as possible, mainly use them as a container.
 *
 * Should the need for a constructor arise, use a class!
 *
 * Write struct name in lower_case_with_underscores.
 */
struct another_stupid_type {
	epictype42_t løl;      //!< no question, we use UTF-8 encoding.
	ub0rtype røfëlkøptør;  //!< proper documentation is Doxygen parsable.
};

// variables in lowercase with _ as word separator,
// classes are written in camel case.
elts::YourMom *my_special_mom;


// a namespace does not increase the indentation level.
namespace elts {

// classes are written in camel case (structs are lowercase like variables)
YourMom::YourMom(int her_mass)
	:
	mass(her_mass),
	number_of_moons(0) {

	// use this-> for referencing member variables/functions!
	this->number_of_moons = (this->mass - (this->mass % 10))/10;
}

YourMom::~YourMom() {}

void YourMom::print_mass() {
	// write todos like this:
	// TODO: mass could require uint128_t soon, adapt format string!
	printf("current mass of your mom: %d\n", this->mass);
}

bool YourMom::tick() {
	this->mass += 1; // we're not using var++, except in for(..)

	// note the spaces!
	//|                  |
	//v                  v
	if (this->mass < 100) {
		// spaces here:
		//        v  v
		this->mass *= 2;
	}

	// spaces again:
	// v      v v  v v v     v    v
	for (int i = 0; i < 1337; i++) {
		this->mass += 1;
		// always use {...} for the conditional code block,
		// even if it's one statement only:
		if (this->mass % 10000 == 0) {
			break;
		}
	}

	switch (this->mass) {
	// the case label is at the same indentation level as its switch statement:
	case 0:
		throw openage::util::Error("mom's mass is %d, impossible.", this->mass);
		break;

	case 42:
		this->mass *= 23;
		// if you don't want to break, write a comment like this:
		// fall through

	default:
		// note that we're using printf even though we don't format anything.
		// you probably want to use the project's logger function though.
		printf("your mom's mass is pretty ok\n");
		break;
	}

	this->number_of_moons += 1;

	return true;
}

} //namespace elts


int main(int argc, char **argv) {
	my_special_mom = new elts::YourMom(1337);

	// no space after function name
	//                        |
	//                        v
	my_special_mom->print_mass();

	for (int i = 0; i < 235; i++) {
		my_special_mom->tick();
	}

	my_special_mom->print_mass();

	delete my_special_mom;
	return 0;
}
