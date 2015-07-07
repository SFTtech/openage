// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PYINTERFACE_PYOBJECT_H_
#define OPENAGE_PYINTERFACE_PYOBJECT_H_

// pxd: from libcpp cimport bool as cppbool

// pxd: from libcpp.string cimport string
#include <string>

// pxd: from cpp.pyinterface.functional cimport PyIfFunc1, PyIfFunc2, PyIfFunc3
// pxd: from cpp.pyinterface.functional cimport Func1
#include "functional.h"

namespace openage {
namespace pyinterface {


/**
 * Holds a Python object, as a PyObject * pointer.
 *
 * Reference counting (Py_INCREF and Py_DECREF) is correctly performed
 * in the various copy/move constructors/operators.
 *
 * pxd:
 *
 * cppclass PyObjectRef:
 *     PyObjectRef() noexcept
 *
 *     void *get_ref() noexcept
 *     void set_ref(void *ref) except +
 *     void set_ref_without_incrementing(void *ref) except +
 *     void clear_ref_without_decrementing() noexcept
 *
 *
 * ctypedef PyObjectRef *PyObjectRefPtr
 */
class PyObjectRef {
public:
	/**
	 * Initializes reference with nullptr.
	 */
	PyObjectRef();

	/**
	 * Wraps a raw PyObject * pointer (calls Py_INCREF).
	 */
	explicit PyObjectRef(void *ref);

	/**
	 * Clones a PyObjectRef (calls Py_INCREF).
	 */
	PyObjectRef(const PyObjectRef &other);

	/**
	 * Move-constructs from an other PyObjectRef.
	 */
	PyObjectRef(PyObjectRef &&other);

	/**
	 * Assigns from an other PyObjectRef
	 * (calls Py_XDECREF on the old value, and Py_XINCREF on the new one).
	 */
	PyObjectRef &operator =(const PyObjectRef &other);

	/**
	 * Move-assigns from an other PyObject
	 * (calls Py_XDECREF on the old value).
	 */
	PyObjectRef &operator =(PyObjectRef &&other);

	/**
	 * Destroys the object, calls Py_XDECREF.
	 */
	~PyObjectRef();

	// operator == is not implemented because users might expect it to
	// implement either of .is() or .equals().

	/**
	 * str(obj)
	 */
	std::string str() const;

	/**
	 * repr(obj)
	 */
	std::string repr() const;

	/**
	 * len(obj)
	 */
	int len() const;

	/**
	 * callable(obj)
	 */
	bool callable() const;

	/**
	 * obj()
	 *
	 * Implement other combinations of arguments as needed.
	 */
	PyObjectRef call() const;

	/**
	 * getattr(obj, name)
	 */
	PyObjectRef getattr(const std::string &name) const;

	/**
	 * hasattr(obj, name)
	 */
	bool hasattr(const std::string &name) const;

	/**
	 * setattr(obj, name)
	 */
	void setattr(const std::string &name, const PyObjectRef &attr) const;

	/**
	 * isinstance(obj, type)
	 */
	bool isinstance(const PyObjectRef &type) const;

	/**
	 * bool(obj)
	 */
	bool to_bool() const;

	/**
	 * for elem in dir(obj):
	 *     callback(elem)
	 */
	void dir(const Func<void, std::string> &callback) const;

	/**
	 * obj is other
	 */
	bool is(const PyObjectRef &other) const;

	/**
	 * obj == other
	 */
	bool equals(const PyObjectRef &other) const;

	/**
	* eval(expression, obj)
	*/
	PyObjectRef eval(const std::string &expression) const;

	/**
	* exec(statement, obj)
	*/
	void exec(const std::string &statement) const;

	/**
	 * obj[key]
	 */
	PyObjectRef get(const PyObjectRef &key) const;

	/**
	 * obj[key]
	 */
	PyObjectRef get(const std::string &key) const;

	/**
	 * obj[key]
	 */
	PyObjectRef get(int key) const;

	/**
	 * obj in container
	 */
	bool in(const PyObjectRef &container) const;

	/**
	 * type(obj)
	 */
	PyObjectRef type() const;

	/**
	 * type(obj).__module__
	 */
	std::string modulename() const;

	/**
	 * type(obj).__name__
	 */
	std::string classname() const;


private:
	/**
	 * Internal PyObject * for obj.
	 */
	void *ref;


public:
	void *get_ref() const noexcept {
		return this->ref;
	}

	/**
	 * Like operator =(const PyObjectRef &other).
	 */
	void set_ref(void *ref);

	/**
	 * Like set_ref, but does _NOT_ call PY_XINCREF.
	 * Only use in special cases, if you know exactly what you're doing.
	 */
	void set_ref_without_incrementing(void *ref);

	/**
	 * Clears the internal reference, but does _NOT_ call PY_XDECREF.
	 * Only use in special cases, if you know exactly what you're doing.
	 */
	void clear_ref_without_decrementing() noexcept {
		this->ref = nullptr;
	}
};


std::ostream &operator <<(std::ostream &os, const PyObjectRef &ref);


namespace py {

/**
 * getattr(importlib.import_module("builtins"), name)
 */
PyObjectRef builtin(const std::string &name);


/**
 * importlib.import_module(name)
 */
PyObjectRef import(const std::string &name);


/**
 * str(value);
 */
PyObjectRef str(const std::string &value);


/**
 * bytes(value)
 */
PyObjectRef bytes(const char *value);


/**
 * int(value)
 */
PyObjectRef integer(int value);


/**
 * {}
 */
PyObjectRef dict();


/**
 * None
 */
PyObjectRef none();


} // py


// installed by openage.pyinterface.pyobject.setup().
// now follow the various Python callbacks that implement all of the above,
// and need to be installed by openage.pyinterface.pyobject.setup().

// for use by the reference-counting constructors

// pxd: PyIfFunc1[void, void *] py_xincref
extern PyIfFunc<void, void *> py_xincref;
// pxd: PyIfFunc1[void, void *] py_xdecref
extern PyIfFunc<void, void *> py_xdecref;

// for all of those member functions

// pxd: PyIfFunc1[string, void *] py_str
extern PyIfFunc<std::string, void *> py_str;
// pxd: PyIfFunc1[string, void *] py_repr
extern PyIfFunc<std::string, void *> py_repr;
// pxd: PyIfFunc1[int, void *] py_len
extern PyIfFunc<int, void *> py_len;
// pxd: PyIfFunc1[cppbool, void *] py_callable
extern PyIfFunc<bool, void *> py_callable;
// pxd: PyIfFunc2[void, PyObjectRefPtr, void *] py_call
extern PyIfFunc<void, PyObjectRef *, void *> py_call;
// pxd: PyIfFunc2[cppbool, void *, string] py_hasattr
extern PyIfFunc<bool, void *, std::string> py_hasattr;
// pxd: PyIfFunc3[void, PyObjectRefPtr, void *, string] py_getattr
extern PyIfFunc<void, PyObjectRef *, void *, std::string> py_getattr;
// pxd: PyIfFunc3[void, void *, string, void *] py_setattr
extern PyIfFunc<void, void *, std::string, void *> py_setattr;
// pxd: PyIfFunc2[cppbool, void *, void *] py_isinstance
extern PyIfFunc<bool, void *, void *> py_isinstance;
// pxd: PyIfFunc1[cppbool, void *] py_to_bool
extern PyIfFunc<bool, void *> py_to_bool;
// pxd: PyIfFunc2[void, void *, Func1[void, string]] py_dir
extern PyIfFunc<void, void *, Func<void, std::string>> py_dir;
// pxd: PyIfFunc2[cppbool, void *, void *] py_equals
extern PyIfFunc<bool, void *, void *> py_equals;
// pxd: PyIfFunc2[void, void *, string] py_exec
extern PyIfFunc<void, void *, std::string> py_exec;
// pxd: PyIfFunc3[void, void *, PyObjectRefPtr, string] py_eval
extern PyIfFunc<void, void *, PyObjectRef *, std::string> py_eval;
// pxd: PyIfFunc3[void, void *, PyObjectRefPtr, void *] py_get
extern PyIfFunc<void, void *, PyObjectRef *, void *> py_get;
// pxd: PyIfFunc2[cppbool, void *, void *] py_in
extern PyIfFunc<bool, void *, void *> py_in;
// pxd: PyIfFunc2[void, void *, PyObjectRefPtr] py_type
extern PyIfFunc<void, void *, PyObjectRef *> py_type;
// pxd: PyIfFunc1[string, void *] py_modulename
extern PyIfFunc<std::string, void *> py_modulename;
// pxd: PyIfFunc1[string, void *] py_classname
extern PyIfFunc<std::string, void *> py_classname;


// pxd: PyIfFunc2[void, PyObjectRefPtr, string] py_builtin
extern PyIfFunc<void, PyObjectRef *, std::string> py_builtin;
// pxd: PyIfFunc2[void, PyObjectRefPtr, string] py_import
extern PyIfFunc<void, PyObjectRef *, std::string> py_import;
// pxd: PyIfFunc2[void, PyObjectRefPtr, string] py_createstr
extern PyIfFunc<void, PyObjectRef *, std::string> py_createstr;
// pxd: PyIfFunc2[void, PyObjectRefPtr, const char *] py_createbytes
extern PyIfFunc<void, PyObjectRef *, const char *> py_createbytes;
// pxd: PyIfFunc2[void, PyObjectRefPtr, int] py_createint
extern PyIfFunc<void, PyObjectRef *, int> py_createint;
// pxd: PyIfFunc1[void, PyObjectRefPtr] py_createdict
extern PyIfFunc<void, PyObjectRef *> py_createdict;
// pxd: PyIfFunc1[void, PyObjectRefPtr] py_getnone
extern PyIfFunc<void, PyObjectRef *> py_getnone;


}} // openage::pyinterface

#endif
