#include "mom.h"

#include <cstdio>

#include <SDL/SDL.h>

#include "../whatever.h"
#include "lolinclude.h"


//use tabs to indent code, if you display that as 4 or 8 or 13.37 doesn't matter
//use spaces for drawing ascii-art or adapting code positions after indentation
//
//=> this ensures custom tab-widths AND correct indentation levels.

class UselessDemonstration {
public:
	//example for using tabs for indentation and spaces for adjustment
	//the argument list is too long, therefore wrap it
	void lol_useless_function(char *rofl_a_text,
	                          int asdf,
	                          bool another_argument,
	                          char yeah_really_thats_many_arguments,
	                          int you_wont_believe_me,
	                          bool fak_u) {
		if (fak_u) {
			printf("%s: %d \n", rofl_a_text, asdf);
		}
		else if (another_argument) {
			//the argument list is too long again, so wrap the args
			//and indent them with spaces.
			printf("%05d pls... %c %s\n",
			       you_wont_believe_me,
			       yeah_really_thats_many_arguments,
			       rofl_a_text);
		}
		else {
			printf("%s trololo\n", rofl_a_text);
		}
	}
}

//variables in lowercase with _ as word separator,
//classes are written in camel case
elts::YourMom *my_special_mom;


//a namespace does not increase the indentation level.
namespace elts {

//classes are written in camel case (structs are lowercase like variables)
YourMom::YourMom(int her_mass) :
	mass(her_mass),
	number_of_moons(0) {
	//use this-> for referencing member variables/functions.
	this->number_of_moons = (mass - (mass % 10))/10;
}

YourMom::~YourMom() {}

void YourMom::print_mass() {
	printf("current mass of your mom: %d\n", this->mass);
}

bool YourMom::tick() {
	this->mass += 1; //we're not using var++

	//note the spaces
	//|                  |
	//v                  v
	if (this->mass < 100) {
		this->mass *= 2;
	}

	for (int i = 0; i < 1337; i++) {
		this->mass += 1;
		//always use {} for the conditional code block.
		if (this->mass % 10000 == 0) {
			break;
		}
	}

	switch (this->mass) {
	//the case label is at the same indentation level as its switch statement:
	case 0:
		throw openage::util::Error("mom's mass is %d, impossible.", this->mass);
		break;

	case 42:
		this->mass *= 23;
		//if you don't want to break, write a comment like this:
		//fall through

	default:
		//note that we're using printf even though we don't format anything
		printf("your mom's mass is pretty ok\n");
		break;
	}

	this->number_of_moons += 1;

	return true;
}

} //namespace elts


int main(int argc, char **argv) {
	my_special_mom = new elts::YourMom(1337);

	//no space after function name
	//                        |
	//                        v
	my_special_mom->print_mass();

	for (int i = 0; i < 235; i++) {
		my_special_mom->tick();
	}

	my_special_mom->print_mass();

	delete my_special_mom;
}
