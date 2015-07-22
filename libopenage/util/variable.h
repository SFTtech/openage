// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_VARIABLE_H_
#define OPENAGE_UTIL_VARIABLE_H_

namespace openage {
namespace util {

/**
 * A typed value holder
 */
class VariableBase {
public:
	virtual ~VariableBase() {}

	/**
	 * sets the type and value
	 */
	template<class T, class V> void set(const V &value);

	/**
	 * returns the stored value
	 * throws an exception if the template
	 * does not match the set type
	 */
	template<class T> const T &get() const;

};


template<class T>
class Variable : public VariableBase {
public:
	Variable(const T &initial_value)
		:
		value(initial_value) {}


	const T &get() const {
		return this->value;
	}

	void set(const T &v) {
		this->value = v;
	}

	/**
	 * accessable typed value
	 */
	T value;
};


template<class T>
const T &VariableBase::get() const {
	return dynamic_cast<const Variable<T> &>(*this).get();
}

template<class T, class V>
void VariableBase::set(const V& value) {
	return dynamic_cast<Variable<T> &>(*this).set(value);
}


}	// namespace util
}	// namespace openage

#endif
