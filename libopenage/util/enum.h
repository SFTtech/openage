// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <typeinfo>

#include "compiler.h"

// pxd: from libcpp cimport bool


namespace openage {
namespace util {

/*
 * C++'s enum class has various deficits:
 *
 *  - Wrapping it via Cython requires hacks.
 *  - enum objects can't have any associated data.
 *  - they can't have member methods
 *
 * In cases where those features are required, we recommend to use references
 * to non-copyable objects instead of enum values. Which is what this class provides.
 *
 * This class provides is a simple base which
 *  - forbids copying
 *  - provides an equality operator which compares the memory addresses of the objects.
 *  - contains a const char *name, for use in operator <<
 *  - provides operator <<
 *  - contains a numeric value, as enums do
 *  - provides comparison operators which use the numeric value
 *
 * In order to contain the const references to these objects,
 * and as a namespace for static objects which contain the values,
 * use the Enum class below.
 *
 * For a full usage example, see enum_test.{h, cpp}.
 *
 * pxd:
 *
 * cppclass EnumValue[DerivedType, NumericType]:
 *     const char *name
 *     NumericType numeric
 */
template<typename DerivedType, typename NumericType=int>
class OAAPI EnumValue {
public:
	// implicit constructor: EnumValue(const char *value_name, int numeric_value)

	// enum values cannot be copied
	EnumValue(const EnumValue &other) = delete;
	EnumValue &operator =(const EnumValue &other) = delete;

	// an explicit deletion of the implicitly defined copy constructor and assignment operator
	// will implicitly delete the implicitly defined move constructor and assignment operator.
	// yay for C++

	// enum values are equal if the pointers are equal.
	constexpr bool operator ==(const DerivedType &other) const {
		return (this == &other);
	}

	constexpr bool operator !=(const DerivedType &other) const {
		return !(*this == other);
	}

	constexpr bool operator <=(const DerivedType &other) const {
		return this->numeric <= other.numeric;
	}

	constexpr bool operator <(const DerivedType &other) const {
		return this->numeric < other.numeric;
	}

	constexpr bool operator >=(const DerivedType &other) const {
		return this->numeric >= other.numeric;
	}

	constexpr bool operator >(const DerivedType &other) const {
		return this->numeric > other.numeric;
	}

	friend std::ostream &operator <<(std::ostream &os, const DerivedType &arg) {
		os << util::demangle(typeid(DerivedType).name()) << "::" << arg.name;
		return os;
	}

	const char *name;
	NumericType numeric;
};


/**
 * Container for possible enum values.
 * Inherit from this class with CRTP.
 *
 * This class should have the various values defined as `static constexpr`
 * members.
 * Due to the C++ standard, individual enum values must be declared
 * as `static constexpr` members of a class.
 * If they are declared as `static constexpr` constants in a namespace instead,
 * the linker will place the constant in the ELF file once per object file
 * instead of deduplicating it.
 *
 * Objects of this class contain a reference to a static constexpr value.
 *
 * The static constexpr values can be accessed through `operator ->`.
 *
 * See the documentation for the EnumValue class above.
 *
 * pxd:
 *
 * cppclass Enum[DerivedType]:
 *     const DerivedType &get() except +
 *
 *     bool operator ==(Enum[DerivedType] other) except +
 *     bool operator !=(Enum[DerivedType] other) except +
 *
 *     bool operator <(Enum[DerivedType] other) except +
 *     bool operator >(Enum[DerivedType] other) except +
 *     bool operator <=(Enum[DerivedType] other) except +
 *     bool operator >=(Enum[DerivedType] other) except +
 */
template<typename DerivedType>
class OAAPI Enum {
	using this_type = Enum<DerivedType>;

public:
	// disallow the empty constructor to ensure that value is always a valid pointer.
	constexpr Enum() = delete;
	constexpr Enum(const DerivedType &value) : value{&value} {}

	constexpr operator const DerivedType &() const {
		return *this->value;
	}

	constexpr Enum &operator =(const DerivedType &value) {
		this->value = &value;
		return *this;
	}

	constexpr const DerivedType *operator ->() const {
		return this->value;
	}

	constexpr bool operator ==(const this_type &other) const {
		return *this->value == *other.value;
	}

	constexpr bool operator !=(const this_type &other) const {
		return *this->value != *other.value;
	}

	constexpr bool operator <=(const this_type &other) const {
		return *this->value <= *other.value;
	}

	constexpr bool operator <(const this_type &other) const {
		return *this->value < *other.value;
	}

	constexpr bool operator >=(const this_type &other) const {
		return *this->value >= *other.value;
	}

	constexpr bool operator >(const this_type &other) const {
		return *this->value > *other.value;
	}

	constexpr const DerivedType &get() const {
		return *this->value;
	}

	friend std::ostream &operator <<(std::ostream &os, const this_type &arg) {
		os << *arg.value;
		return os;
	}

protected:
	const DerivedType *value;
};

}} // openage::util
