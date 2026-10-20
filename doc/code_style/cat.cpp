// Copyright 2026+ the openage authors. See copying.md for legal info.


// [SFT]tech coding guidelines
// ---------------------------
//
// This is our attempt to creating a good-looking C++ style.
//
// See `cat.cppm` for the module interface
// (and cat.h as old-style header example).
//
// For enhanced beauty: Use your brain!
//   * Alphabetic sorting
//   * Sensible documentation
//   * Lovely indentation and alignment


// #includes only go into the global module fragment,
// an #include after the module declaration would attach the header to our module.
module;

// C++ std library includes come first, sorted alphabetically
#include <chrono>
#include <memory>
#include <string>
#include <utility>

// External libraries are next, sorted alphabetically
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

// Local includes come last, sorted alphabetically
#include "aperture/glados.h"
#include "aperture/item.h"
#include "aperture/portal_gun.h"
#include "aperture/turret.h"
#include "error/error.h"
#include "log/log.h"

// This is the implementation unit: no `export`,
// it sees everything declared in the interface unit `cat.cppm`.
module aperture.companion.cat;

// Imports come right after the module declaration, sorted alphabetically
import openage.util.color;


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
//   use it for state + mutations + observation bundling
//
// Name of struct: lower_case_with_underscores
// Name of class: CamelCaseAsYouKnowIt
// Member methods: lower_case_names


// Documentation for a namespace is in the interface unit.
namespace aperture::companion {
// <- A namespace does not increase the indentation level!

// Helpers only used in this file go into an anonymous namespace,
// so they can't collide with the helpers of another test chamber.
namespace {

/**
 * Example struct documentation.
 * Followed by an incredibly good example for writing docs.
 * Will be parsed by Doxygen.
 */
struct portal_destination {
	//! variables and functions in lowercase with _ as word separator
	Eigen::Vector3d exit_position;
	//! the cat leaves at this angle, and still lands on its feet.
	Eigen::Quaterniond exit_rotation;
	//! no question, we use UTF-8 encoding.
	bool schrödinger_occupied;
};


// Constants are constexpr whenever possible:
constexpr double gravity = 9.81;

// col has no constexpr constructor, so these can only be const.
const openage::util::col blue_portal{0, 101, 255, 255};
const openage::util::col orange_portal{255, 93, 0, 255};


/**
 * Speedy thing goes in, speedy thing comes out.
 * Momentum is conserved, the cat's dignity is not.
 */
Eigen::Vector3d fling(const Eigen::Vector3d &velocity,
                      const portal_destination &destination) {
	return destination.exit_rotation * velocity;
}


/**
 * Yet another test chamber. Go on, nothing to see here.
 * The subject is a cat, so the cake is a lie but the gravity is real.
 */
class TestChamber : public ApertureTest {
// vvv The access modifier is at the same indent level as the class definition:
public:
	explicit TestChamber(std::shared_ptr<GLaDOS> controller) :
		controller{std::move(controller)} {}

	/**
	 * Drops the cat through a portal onto the turret.
	 * [mind the tabs for indentation, spaces for alignment here]
	 *
	 * @returns true if the cat made it out before the neurotoxin did.
	 */
	bool run_test(Cat &cat,
	              std::chrono::microseconds time_until_neurotoxin,
	              const std::shared_ptr<PortalGun> &portal_gun,
	              const std::shared_ptr<Turret> &turret,
	              bool cake_available = true) {

		// ^ you can insert a empty line here to make it more readable.

		portal_destination destination{
			.exit_position = turret->get_position() + Eigen::Vector3d{0, 0, 2},
			.exit_rotation = Eigen::Quaterniond::Identity(),
			.schrödinger_occupied = false,
		};

		// intentation when continuing inside a indent-level like ( { etc: one tab
		portal_gun->place_portals(
			portal_gun->get_aim_position(),
			blue_portal,
			destination.exit_position,
			orange_portal
		);

		std::chrono::microseconds fall_time = portal_gun->get_fall_time(gravity);

		if (not cake_available
		    and time_until_neurotoxin < fall_time) {

			// ^ Empty lines may help with readability
			// Without cake, the cat sees no reason to hurry.

			// If you don't wrap the argument list after a { or (
			// use tabs for the base indent, then align the argument with spaces:
			this->controller->drop_hint(turret,
			                            "the cat is on the wrong side of the portal, "
			                            "and the cake is still a lie.");
			return false;
		}
		else {
			// Nobody knows why the cat jumps into the blue portal, least of all the cat.
			double fall_seconds = std::chrono::duration<double>{fall_time}.count();
			Eigen::Vector3d exit_velocity = fling(Eigen::Vector3d{0, 0, -gravity * fall_seconds}, destination);

			// "I don't blame you." The turret is lying.
			turret->track(destination.exit_position, exit_velocity);
			return cat.tick();
		}
	}

private:
	/** Narrates the test and controls the neurotoxin. */
	std::shared_ptr<GLaDOS> controller;
};

} // namespace


// ^ At the top level, use two newlines between statements.
// Note: Comments and preprocessor directives don't count as statements,
// In some cases variables and functions may be grouped, always judge
// yourself instead of blindly following the style guide, blah blah.


Cat::Cat(const std::string &name,
         int curiosity) :
	name{name},
	lives{9},
	curiosity{curiosity} {
	// ^ use {}-style initializers wherever possible
	// < one tab indentation for the initializer list.

	// use this-> for referencing member variables/functions!
	// Whatever you planned, the cat naps on your keyboard first:
	this->naps["keyboard"] = std::chrono::minutes{30};
}


Cat::~Cat() = default;


// <^ At the top level, use two newlines between statements.
void Cat::print_mood() const {
	// write todos like this:
	// TODO: the cat may nap at any time, adapt the mood detection!

	// Log with our logger, never with printf or std::cout:
	openage::log::log(INFO << this->name << "'s curiosity: " << this->curiosity);
}


// Note the space:  v
bool Cat::tick() {
	// Note the spaces!
	//|                | |  |
	//v                v v  v
	if (this->curiosity > 5) {
		// spaces here:
		//             v  v
		this->curiosity += 1;
	}

	// Spaces again:
	// v      v v    v v v     v      v
	for (int box = 0; box < 42; box++) {
		// Always use braces for the conditional code block,
		// even if it's one statement only:
		if (this->curiosity < 5) {
			// If it fits, it sits.
			this->naps["box " + std::to_string(box)] += std::chrono::minutes{20};
			break;
		}
		else if (this->curiosity > 100) {
			// Curiosity killed the cat, the chamber respawns it.
			this->lives -= 1;
			this->curiosity = DEFAULT_CAT_CURIOSITY;
		} else { // Both styles are allowed: '} else {' and '} \n else {'
			// the cat inspects the box.
			this->curiosity -= 1;
			// ^ We're not using var--, except in for (..)

			if ((box % 5) == 0) {
				// This one has a hole, which is very interesting.
				this->curiosity += 3;
			}
		}
	}

	switch (this->curiosity) {
	// The case label is at the same indentation level
	// as its switch statement:
	// vvv
	case 0:
		// Errors carry a log message, never throw bare std:: exceptions:
		throw openage::Error{MSG(err) << this->name << " refuses to test. The cake is a lie."};

	case 8: {
		// ^ when defining new variables for a specific case,
		// | braces are required by C++ because it is "special".
		// | Use them like this.

		// The cake was not a lie. It is on the floor now, though.
		Item cake{"cake"};
		this->push_off(cake);
		return false;
	}

	case 42:
		GLaDOS::try_recruit_test_subject(*this);
		[[fallthrough]];  // <- intentional fallthrough marker
	default:
		this->curiosity += 1;
		break;
	}

	return true;
}


// Unused parameters are commented out:
void Cat::push_off(const Item & /*item*/) {
	// TODO: implement. The item must fall, the cat must watch.
}


void chamber_demo() {
	// Use {}-style constructors!
	Cat schroedingers_cat{"milton", 1337};
	TestChamber chamber{std::make_shared<GLaDOS>()};
	auto portal_gun = std::make_shared<PortalGun>();
	auto turret = std::make_shared<Turret>();

	// No space after function name calling!
	//                          |
	//                          v
	schroedingers_cat.print_mood();

	for (int life = 0; life < 9; life++) {
		chamber.run_test(schroedingers_cat, std::chrono::seconds{66}, portal_gun, turret);
	}

	schroedingers_cat.print_mood();
}

} // namespace aperture::companion
// ^ When closing namespaces,
//   close all in the same line and write a comment like this.

// main() can't belong to a named module.
// It lives in a plain .cpp that does `import aperture.companion.cat;`.
