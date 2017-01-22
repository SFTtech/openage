// Copyright 2013-2016 the openage authors. See copying.md for legal info.


// [SFT]tech coding guidelines
// ---------------------------
//
// See `mom.h` for a header example.
//
// This is our attempt to creating a good-looking C++ style.
//
// For enhanced beauty: Use your brain!
//   * Alphabetic sorting
//   * Sensible documentation
//   * Lovely indentation and alignment


// The associated header file comes first!
#include "mom.h"

// System includes follow, sorted alphabetically
#include <iostream>
#include <cmath>
#include <cstdio>
#include <SDL/SDL.h>

// Local includes next, sorted alphabetically
#include "../valve.h"
#include "half_life.h"
#include "log/log.h"

// Include errors are detected by having one associated cpp
// file for each header in the project.


// Indentation:
//
// Mix tabs and spaces in a sensible way:
//   - Tabs for logical indentation
//   - Spaces for alignments after that
//
//    ==> see doc/code_style/tabs_n_spaces.md
//
// => this ensures custom tab-widths AND consistent lineup levels.


// struct:
//   use it to pack variables
// class:
//   use it for the object orientation stuff
//
// Name of struct: lower_case_with_underscores
// Name of class: CamelCaseAsYouKnowIt
// Member methods: lower_case_names


/**
 * Example struct documentation.
 * Followed by an incredibly good example for writing docs.
 * Will be parsed by Doxygen.
 */
struct another_stupid_type {
	rofl_t blergh_gnampf;  //!< variables and functions in lowercase
	                       //   with _ as word separator.
	epictype42_t løl;      //!< no question, we use UTF-8 encoding.
	ub0rtype røfëlkøptør;  //!< proper documentation is Doxygen parsable.
};



/**
 * Yet another Valve game. Go on, nothing to see here.
 */
class HalfLife3 : public ValveGame {
// vvv The access modifier is at the same indent level as the class definition:
public:
	void release_game(const ValveTeam &team,
	                  uint128_t        passed_time,
	                  const Engine    *game_engine,
	                  Internet        *community,
	                  bool             keep_secret=true) {

		// ^ insert a empty line here to make it more readable.

		// http://www.abstrusegoose.com/440
		auto gaben = team.get_human("gaben");
		auto min_quality = gaben.get_quality_requirement(Engine::type);

		if (not keep_secret
		    and (this->story->awesomeness > 9000)
		    and (game_engine->quality_points > min_quality)) {

			// ^ This empty line is mandatory, too.
			team->start_ARG(community);

			// https://developer.valvesoftware.com/wiki/Valve_Time
			this->estimated_release_time = passed_time + ValveTime("66 days").seconds();

			team.get_product("steam")->release(this);
			community->go_crazy();
			World::infrastructure.collapse();
		}
		else {
			gaben->motivate(team);

			// When the argument list gets too long, align with spaces:
			team.drop_hint(community,
			               "we think you're kind of wasting your time, "
			               "but it's your time to waste.");

			// You may also start one line later
			// and then indent with a tab:
			team.relocate(
				game_engine->get_team()->get_current_hq_location(),
				gaben.get_table_location()
			);

			community->criticize(passed_time);
			team.continue_selling_hats();
		}
	}

private:
	/** links to the story of the game */
	Story *story;

	/** time in seconds estimated left until release */
	uint128_t estimated_release_time;
};


// ^ At the top level, use two newlines between statements.
// Note: Comments and preprocessor directives don't count as statements,
// In some cases variables and functions may be grouped, always judge
// yourself instead of blindly following the style guide, blah blah.


// Documentation for a namespace is in the header.
// Can't wait for C++17's "namespace parents::mom {" style!
namespace parents {
namespace mom {
// <- A namespace does not increase the indentation level!

YourMom::YourMom(int her_mass)
	:
	mass{her_mass},
	number_of_moons{0},
	stupidity{1} {
	// ^ use {}-style initializers wherever possible
	// < one tab indentation for the initializer list.

	// use this-> for referencing member variables/functions!
	this->number_of_moons = (this->mass - (this->mass % 10)) / 10;

	// access static variables like this:
	YourMom::sum_mom_masses += this->mass;
}


YourMom::~YourMom() {}


// <^ At the top level, use two newlines between statements.
void YourMom::print_mass() {
	// write todos like this:
	// TODO: mass could require uint128_t soon, adapt format string!
	printf("current mass of your mom: %d\n", this->mass);
}


// Note the space:  v
bool YourMom::tick() {
	// Note the spaces!
	//|                | |  |
	//v                v v  v
	if (this->stupidity > 5) {
		// spaces here:
		//        v  v
		this->mass += 1;
	}

	// Spaces again:
	// v      v v  v v v   v    v
	for (int i = 0; i < 42; i++) {
		// Always use braces for the conditional code block,
		// even if it's one statement only:
		if (this->mass > 20) {
			// Black hole? I guess it depends on the radius.
			break;
		}
		else if (this->stupidity > 100) {
			this->die();
		} else { // Both styles are allowed: '} else {' and '} \n else {'
			// your mom eats a bit.
			this->mass += 1;
			// ^ We're not using var++, except in for (..)

			if ((this->mass % 5) == 0) {
				this->number_of_moons += 1;
			}
		}
	}

	switch (this->stupidity) {
	// The case label is at the same indentation level
	// as its switch statement:
	// vvv
	case 0:
		throw ContentMafia::Error{"Your mom opposes DRM, investigation required."};
		break;

	case 8: {
		// ^ when defining new variables for a specific case,
		// | braces are required by C++ because it is "special".
		// | Use them like this.

		Apple::iPhone shiny_thing;
		this->buy(shiny_thing);
		this->stupidity += 2;
		return false;
	}

	case 42:
		ContentMafia::try_recruit_lobbyist(this);
		// fall through
	default:
		this->stupidity += 1;
		break;
	}

	return true;
}


// Unused parameters are commented out:
void YourMom::buy(const Thing & /*item*/) {
	// TODO: implement
}

}} // parents::mom
// ^ When closing namespaces,
//   close all in the same line and write a comment like this.
//   In C++17 this will only be a single }


int main(int argc, char **argv) {
	// Use {}-style constructors!
	parents::YourMom my_special_mom{1337};

	// No space after function name calling!
	//                       |
	//                       v
	my_special_mom.print_mass();

	for (int i = 0; i < 235; i++) {
		my_special_mom.tick();
	}

	my_special_mom.print_mass();
	return 0;
}
