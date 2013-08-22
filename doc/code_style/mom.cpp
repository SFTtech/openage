#include "mom.h"

#include <cstdio>

#include <SDL.h>

#include "dad.h"

YourMom::YourMom(int mass) {
	this->mass = mass;
}

YourMom::~YourMom() {}

void YourMom::print_mass() {
	printf("current mass of your mom: %d\n", mass);
}

bool YourMom::tick() {
	mass += 1; //note that we're not using ++ here

	if (mass < 100) { 
		mass *= 2;
	}

	for (int i = 0; i < 1337; i++) {
		mass += 1;
		if (mass % 10000 == 0) {
			break;
		}
	}

	switch (mass) {
	case 0:
		throw Exc
		//if you don't want to break, write a comment like this:
		//fall through
	case 42:
		mass *= 23;
		break;
	default:
		//note that we're using printf even though we don't format anything
		printf("your mom's mass is pretty ok\n")
		break;
	}

	return true;
}
