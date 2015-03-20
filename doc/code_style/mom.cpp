#include "mom.h"

#include <cmath>
#include <cstdio>
#include <SDL/SDL.h>

#include "../valve.h"
#include "crossplattform/opengl.h"
#include "half_life.h"

// [SFT]tech coding guidelines
// ---------------------------
//
// please also have a look at `mom.h` for a header example.


// Indentation:
//    ========> see doc/code_style/tabs_n_spaces.md <=========
// Tabs for indentation. Display that as 4 or 8 or 13.37: fully flexible.
// Spaces for ascii-arts or code alignments after tabs.
//
// => this ensures custom tab-widths AND correct indentation levels.



// Structs: Used to pack variables, nothing else!
// Need member functions/constructor? Use a class!
//
// Struct names: lower_case_with_underscores.


/**
 * Struct documentation.
 * Incredibly good example for writing docs.
 */
struct another_stupid_type {
	rofl_t blergh;         //!< variables and functions in lowercase
	                       //   with _ as word separator.
	epictype42_t løl;      //!< no question, we use UTF-8 encoding.
	ub0rtype røfëlkøptør;  //!< proper documentation is Doxygen parsable.
};


// Classes: complex containers.
//
// Class names: CamelCase


/**
 * Represents yet another Valve game.
 * Go on, nothing to see here.
 */
class HalfLife3 : public ValveGame {
// vvv The access modifier is at the same indent level as the class definition:
public:
	// I hope you already read the tabs_n_spaces.md file,
	// as now follows a practical application for it:

	// Example for using tabs for indentation and spaces for alignment.
	// The argument list is too long, therefore wrap it.
	// => Tabs are used to reach level of `void release_...`,
	//    spaces indent the alignment of each argument!
	//
	// Variable name alignment is optional.
	void release_game(const ValveTeam &team,
	                  uint128_t        passed_time,
	                  Engine          *game_engine,
	                  Internet        *community,
	                  bool             keep_secret=true) {

		// ^ this empty line is mandatory due to the broken line and
		// indentation change.
		// In general, if in doubt, insert a newline.
		// They make things more readable (TM).

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
			world::infrastructure.collapse();
		} else {
			gaben->motivate(team);

			// When the argument list gets too long, align with spaces:
			team.drop_hint(community,
			               "we think you're kind of wasting your time, "
			               "but it's your time to waste.");

			// You may also start one line later and then indent with tabs.
			team.relocate(
				game_engine->get_team()->get_current_hq_location(),
				gaben.get_table_location()
			)

			community->criticize(passed_time);
			team.continue_selling_hats();
		}
	}

private:
	Story *story;
	uint128_t estimated_release_time;
};


// ^ At the top level, use two newlines between statements.
// Note: Comments and preprocessor directives don't count as statements,
// In some cases variables and functions may be grouped, always judge
// yourself instead of blindly following the style guide, blah blah.


// Documentation for a namespace is in the header.
namespace elts {
namespace mom {
// <- A namespace does not increase the indentation level!


// ^ At the top level, use two newlines between statements.
YourMom::YourMom(int her_mass)
	:
	mass{her_mass},
	number_of_moons{0},
	stupidity{1} {
	// ^ use {}-style initializers wherever possible
	// < one tab indentation for the initializer list.

	// use this-> for referencing member variables/functions!
	this->number_of_moons = (this->mass - (this->mass % 10)) / 10;

	YourMom::sum_mom_masses += this->mass;
}


YourMom::~YourMom() {}


void YourMom::print_mass() {
	// write todos like this:
	// TODO: mass could require uint128_t soon, adapt format string!
	printf("current mass of your mom: %d\n", this->mass);
}


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
		// Always use {...} for the conditional code block,
		// even if it's one statement only:
		if (this->mass > 20) {
			// Black hole? I guess it depends on the density.
			break;
		}
		else if (this->stupidity > 100) {
			this->die();
		} else { // Both styles are allowed: } else { and } \n else {
			// your mom eats a bit.
			this->mass += 1;
			// ^ We're not using var++, except in for(..)

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
		// | braces are required. Use them like this.

		Apple::iPhone shiny_thing;
		shiny_thing.get_bought_by(this);
		this->stupidity += 2;
		break;
	}

	case 42:
		ContentMafia::try_recruit_lobbyist(this);
		// If you don't want to break, write a comment like this:
		// Fall through

	default:
		this->stupidity += 1;
		break;
	}

	return true;
}

}} // namespace elts::mom
// ^ When closing namespaces, close all in the same line and write a comment like this.


int main(int argc, char **argv) {
	// Use {}-style constructors!
	elts::YourMom my_special_mom{1337};

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
