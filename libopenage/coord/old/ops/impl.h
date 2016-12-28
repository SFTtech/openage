// Copyright 2013-2015 the openage authors. See copying.md for legal info.

// has no header guard: used in a preprocessor hack
// TODO obsolete this file using the python codegen

#include "../../util/macro/loop.h"
#include "../../util/macro/concat.h"

#define NEGATE(X) (SCALAR_TYPE) (-X)
#define ADDARG(X) (SCALAR_TYPE) (X + arg.X)
#define SUBARG(X) (SCALAR_TYPE) (X - arg.X)
#define MULARG(X) (SCALAR_TYPE) (X * arg)
#define DIVARG(X) (SCALAR_TYPE) (X / arg)
#define COMPARE(X) X == arg.X

//implementation of prototypes from abs.h

//conversion to relative vector
RELATIVE_TYPE ABSOLUTE_TYPE::as_relative() const {
	return RELATIVE_TYPE{MEMBERS};
}

//-abs -> abs
ABSOLUTE_TYPE ABSOLUTE_TYPE::operator-() const {
	return ABSOLUTE_TYPE{LOOP(NEGATE, MEMBERS)};
}

//abs + rel -> abs
ABSOLUTE_TYPE ABSOLUTE_TYPE::operator+(RELATIVE_TYPE arg) const {
	return ABSOLUTE_TYPE{LOOP(ADDARG, MEMBERS)};
}

//abs - abs -> rel
RELATIVE_TYPE ABSOLUTE_TYPE::operator-(ABSOLUTE_TYPE arg) const {
	return RELATIVE_TYPE{LOOP(SUBARG, MEMBERS)};
}

//abs - rel -> abs
ABSOLUTE_TYPE ABSOLUTE_TYPE::operator-(RELATIVE_TYPE arg) const {
	return ABSOLUTE_TYPE{LOOP(SUBARG, MEMBERS)};
}

//abs * scalar -> abs
ABSOLUTE_TYPE ABSOLUTE_TYPE::operator*(SCALAR_TYPE arg) const {
	return ABSOLUTE_TYPE{LOOP(MULARG, MEMBERS)};
}

//abs / scalar -> abs
ABSOLUTE_TYPE ABSOLUTE_TYPE::operator/(SCALAR_TYPE arg) const {
	return ABSOLUTE_TYPE{LOOP(DIVARG, MEMBERS)};
}

//abs += rel
ABSOLUTE_TYPE &ABSOLUTE_TYPE::operator+=(RELATIVE_TYPE arg) {
	*this = *this + arg;
	return *this;
}

//abs -= rel
ABSOLUTE_TYPE &ABSOLUTE_TYPE::operator-=(RELATIVE_TYPE arg) {
	*this = *this - arg;
	return *this;
}

//abs *= scalar
ABSOLUTE_TYPE &ABSOLUTE_TYPE::operator*=(SCALAR_TYPE arg) {
	*this = *this * arg;
	return *this;
}

//abs /= scalar
ABSOLUTE_TYPE &ABSOLUTE_TYPE::operator/=(SCALAR_TYPE arg) {
	*this = *this / arg;
	return *this;
}

//abs == abs
bool ABSOLUTE_TYPE::operator==(ABSOLUTE_TYPE arg) const {
	return (CONCAT(&&, LOOP(COMPARE, MEMBERS)));
}

//implementation of prototypes from rel.h

//conversion to absolute vector
ABSOLUTE_TYPE RELATIVE_TYPE::as_absolute() const {
	return ABSOLUTE_TYPE{MEMBERS};
}

//-rel -> rel
RELATIVE_TYPE RELATIVE_TYPE::operator-() const {
	return RELATIVE_TYPE{LOOP(NEGATE, MEMBERS)};
}

//rel + abs -> abs
ABSOLUTE_TYPE RELATIVE_TYPE::operator+(ABSOLUTE_TYPE arg) const {
	return ABSOLUTE_TYPE{LOOP(ADDARG, MEMBERS)};
}

//rel + rel -> rel
RELATIVE_TYPE RELATIVE_TYPE::operator+(RELATIVE_TYPE arg) const {
	return RELATIVE_TYPE{LOOP(ADDARG, MEMBERS)};
}

//rel - rel -> rel
RELATIVE_TYPE RELATIVE_TYPE::operator-(RELATIVE_TYPE arg) const {
	return RELATIVE_TYPE{LOOP(SUBARG, MEMBERS)};
}

//rel * scalar -> rel
RELATIVE_TYPE RELATIVE_TYPE::operator*(SCALAR_TYPE arg) const {
	return RELATIVE_TYPE{LOOP(MULARG, MEMBERS)};
}

//rel / scalar -> rel
RELATIVE_TYPE RELATIVE_TYPE::operator/(SCALAR_TYPE arg) const {
	return RELATIVE_TYPE{LOOP(DIVARG, MEMBERS)};
}

//rel += rel
RELATIVE_TYPE &RELATIVE_TYPE::operator+=(RELATIVE_TYPE arg) {
	*this = *this + arg;
	return *this;
}

//rel -= rel
RELATIVE_TYPE &RELATIVE_TYPE::operator-=(RELATIVE_TYPE arg) {
	*this = *this - arg;
	return *this;
}

//rel *= scalar
RELATIVE_TYPE &RELATIVE_TYPE::operator*=(SCALAR_TYPE arg) {
	*this = *this * arg;
	return *this;
}

//rel /= scalar
RELATIVE_TYPE &RELATIVE_TYPE::operator/=(SCALAR_TYPE arg) {
	*this = *this / arg;
	return *this;
}

//rel == rel
bool RELATIVE_TYPE::operator==(RELATIVE_TYPE arg) const {
	return (CONCAT(&&, LOOP(COMPARE, MEMBERS)));
}

//implementation of prototypes from free.h

//scalar * abs = abs
struct ABSOLUTE_TYPE operator*(SCALAR_TYPE scalar, ABSOLUTE_TYPE vectorval) {
	return vectorval * scalar;
}

//scalar * rel = rel
struct RELATIVE_TYPE operator*(SCALAR_TYPE scalar, RELATIVE_TYPE vectorval) {
	return vectorval * scalar;
}

#undef NEGATE
#undef ADDARG
#undef SUBARG
#undef MULARG
#undef DIVARG
