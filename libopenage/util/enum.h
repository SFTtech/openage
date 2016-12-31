// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool as cppbool
#include <iostream>
// pxd: from libcpp.string cimport string
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "compiler.h"
#include "constinit_vector.h"

namespace openage {
namespace util {


/**
 * C++'s enum class has various deficits:
 *
 *  - Wrapping it via Cython requires hacks.
 *  - enum objects can't have member variables or methods.
 *  - you can't even determine the name of an enum object.
 *  - no further members can be added from other files.
 *
 * This attempts to fix those issues.
 * It incurs a small runtime penalty for comparisions to constants,
 * since those are now global objects instead of compile-time constants.
 *
 * For an usage example, see enum_test.{h, cpp}.
 *
 * pxd:
 *
 * cppclass Enum[T]:
 *     const T *get() except +
 *     string name() except +
 *
 *     cppbool operator ==(Enum[T] arg) except +
 *     cppbool operator !=(Enum[T] arg) except +
 */
template<typename T>
class Enum {
public:
	/**
	 * Adds a new value to the enum.
	 *
	 * This should only be required at program init time,
	 * to initialize global objects.
	 */
	explicit Enum<T>(const T &obj) {
		if (unlikely(this->data.size() == 0)) {
			// add element #0, the default fallback value.
			this->data.push_back({"<default enum value>", T()});
		}

		this->id = this->data.size();
		this->data.push_back({util::symbol_name(reinterpret_cast<void *>(this)), obj});
	}


	/**
	 * Initializes this to the default value.
	 */
	Enum()
		:
		id{0} {}


	// regular low-cost copying.
	Enum(const Enum<T> &other)
		:
		id{other.id} {}


	Enum<T> &operator =(const Enum<T> other) {
		id = other.id;
		return *this;
	}


	bool operator ==(Enum<T> other) {
		return this->id == other.id;
	}


	bool operator !=(Enum<T> other) {
		return this->id != other.id;
	}


	// Allows you to access the associated member.
	const T &operator *() const {
		return this->data[this->id].second;
	}

	const T *operator ->() const {
		return &this->data[this->id].second;
	}


	// Allows you to explicitly access the associated member.
	const T *get() const {
		return &this->data[this->id].second;
	}


	/**
	 * Returns the object's symbol name.
	 */
	const std::string &name() const {
		return this->data[this->id].first;
	}


	using data_type = ConstInitVector<std::pair<std::string, T>>;


private:
	unsigned int id;


	/**
	 * Holds the names and objects for all enum values.
	 *
	 * Must be explicitly defined in some cpp file.
	 */
	static data_type data;
};


template<typename T>
std::ostream &operator <<(std::ostream &os, Enum<T> val) {
	os << val.name();
	return os;
}


}} // openage::util
