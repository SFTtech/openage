#include "mom.h"

#include <cstdio>

#include <SDL/SDL.h>

#include "../whatever.h"
#include "lolinclude.h"


elts::YourMom *my_special_mom;

namespace elts {

YourMom::YourMom(int her_mass) : mass(her_mass), number_of_moons(0) {
	this->number_of_moons = (mass - (mass % 10))/10;
}

YourMom::~YourMom() {}

void YourMom::print_mass() {
	printf("current mass of your mom: %d\n", this->mass);
}

bool YourMom::tick() {
	this->mass += 1; //note that we're not using ++ here

	if (this->mass < 100) {
		this->mass *= 2;
	}

	for (int i = 0; i < 1337; i++) {
		this->mass += 1;
		if (this->mass % 10000 == 0) {
			break;
		}
	}

	switch (this->mass) {
	case 0:
		throw openage::util::Error("mom's mass is %d, impossible.", this->mass);

		//if you don't want to break, write a comment like this:
		//fall through

	case 42:
		this->mass *= 23;
		break;

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

	my_special_mom->print_mass();

	for (int i = 0; i < 235; i++) {
		my_special_mom->tick();
	}

	my_special_mom->print_mass();

	delete my_special_mom;
}
