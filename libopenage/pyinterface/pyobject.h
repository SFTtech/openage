// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool as cppbool

// pxd: from libcpp.string cimport string
#include <string>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1, PyIfFunc2, PyIfFunc3
// pxd: from libopenage.pyinterface.functional cimport Func1
#include "functional.h"


// we want to avoid the Python.h import,
// we only need the prototype anyway.
#ifndef Py_OBJECT_H
// pxd: from cpython.ref cimport PyObject
extern "C" {
	typedef struct _object PyObject;
}
#endif

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
 *     PyObjectRef(PyObject *ref) except +
 *
 *     PyObject *get_ref() noexcept
 *     void set_ref(PyObject *ref) except +
 *     void set_ref_without_incrementing(PyObject *ref) except +
 *     void clear_ref_without_decrementing() noexcept
 *
 *
 * ctypedef PyObjectRef PyObj;
 *
 * ctypedef PyObjectRef *PyObjectRefPtr
 * ctypedef PyObject    *PyObjectPtr
 */
class PyObjectRef {
public:
	/**
	 * Initializes reference with nullptr.
	 */
	PyObjectRef() noexcept;

	/**
	 * Wraps a raw PyObject * pointer (calls Py_INCREF).
	 */
	PyObjectRef(PyObject *ref);

	/**
	 * Clones a PyObjectRef (calls Py_INCREF).
	 */
	PyObjectRef(const PyObjectRef &other);

	/**
	 * Move-constructs from an other PyObjectRef.
	 */
	PyObjectRef(PyObjectRef &&other) noexcept;

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
	PyObject *ref;


public:
	/**
	 * Provide the internal PyObject *.
	 */
	PyObject *get_ref() const noexcept {
		return this->ref;
	}

	/**
	 * Implicit conversion to PyObject *.
	 * Mainly for convenience to avoid all the get_ref() calls.
	 */
	PyObject *operator ()() const noexcept {
		return this->ref;
	}

	/**
	 * Like operator =(const PyObjectRef &other).
	 */
	void set_ref(PyObject *ref);

	/**
	 * Like set_ref, but does _NOT_ call PY_XINCREF.
	 * Only use in special cases, if you know exactly what you're doing.
	 */
	void set_ref_without_incrementing(PyObject *ref);

	/**
	 * Clears the internal reference, but does _NOT_ call PY_XDECREF.
	 * Only use in special cases, if you know exactly what you're doing.
	 */
	void clear_ref_without_decrementing() noexcept {
		this->ref = nullptr;
	}
};


/**
 * Stream operator for printing PyObjects
 */
std::ostream &operator <<(std::ostream &os, const PyObjectRef &ref);


// now follow the various Python callbacks that implement all of the above,
// and need to be installed by openage.cppinterface.pyobject.setup().

// for use by the reference-counting constructors

// pxd: PyIfFunc1[void, PyObjectPtr] py_xincref
extern PyIfFunc<void, PyObject *> py_xincref;
// pxd: PyIfFunc1[void, PyObjectPtr] py_xdecref
extern PyIfFunc<void, PyObject *> py_xdecref;

// for all of those member functions

// pxd: PyIfFunc1[string, PyObjectPtr] py_str
extern PyIfFunc<std::string, PyObject *> py_str;
// pxd: PyIfFunc1[string, PyObjectPtr] py_repr
extern PyIfFunc<std::string, PyObject *> py_repr;
// pxd: PyIfFunc1[int, PyObjectPtr] py_len
extern PyIfFunc<int, PyObject *> py_len;
// pxd: PyIfFunc1[cppbool, PyObjectPtr] py_callable
extern PyIfFunc<bool, PyObject *> py_callable;
// pxd: PyIfFunc2[void, PyObjectRefPtr, PyObjectPtr] py_call
extern PyIfFunc<void, PyObjectRef *, PyObject *> py_call;
// pxd: PyIfFunc2[cppbool, PyObjectPtr, string] py_hasattr
extern PyIfFunc<bool, PyObject *, std::string> py_hasattr;
// pxd: PyIfFunc3[void, PyObjectRefPtr, PyObjectPtr, string] py_getattr
extern PyIfFunc<void, PyObjectRef *, PyObject *, std::string> py_getattr;
// pxd: PyIfFunc3[void, PyObjectPtr, string, PyObjectPtr] py_setattr
extern PyIfFunc<void, PyObject *, std::string, PyObject *> py_setattr;
// pxd: PyIfFunc2[cppbool, PyObjectPtr, PyObjectPtr] py_isinstance
extern PyIfFunc<bool, PyObject *, PyObject *> py_isinstance;
// pxd: PyIfFunc1[cppbool, PyObjectPtr] py_to_bool
extern PyIfFunc<bool, PyObject *> py_to_bool;
// pxd: PyIfFunc2[void, PyObjectPtr, Func1[void, string]] py_dir
extern PyIfFunc<void, PyObject *, Func<void, std::string>> py_dir;
// pxd: PyIfFunc2[cppbool, PyObjectPtr, PyObjectPtr] py_equals
extern PyIfFunc<bool, PyObject *, PyObject *> py_equals;
// pxd: PyIfFunc2[void, PyObjectPtr, string] py_exec
extern PyIfFunc<void, PyObject *, std::string> py_exec;
// pxd: PyIfFunc3[void, PyObjectPtr, PyObjectRefPtr, string] py_eval
extern PyIfFunc<void, PyObject *, PyObjectRef *, std::string> py_eval;
// pxd: PyIfFunc3[void, PyObjectPtr, PyObjectRefPtr, PyObjectPtr] py_get
extern PyIfFunc<void, PyObject *, PyObjectRef *, PyObject *> py_get;
// pxd: PyIfFunc2[cppbool, PyObjectPtr, PyObjectPtr] py_in
extern PyIfFunc<bool, PyObject *, PyObject *> py_in;
// pxd: PyIfFunc2[void, PyObjectPtr, PyObjectRefPtr] py_type
extern PyIfFunc<void, PyObject *, PyObjectRef *> py_type;
// pxd: PyIfFunc1[string, PyObjectPtr] py_modulename
extern PyIfFunc<std::string, PyObject *> py_modulename;
// pxd: PyIfFunc1[string, PyObjectPtr] py_classname
extern PyIfFunc<std::string, PyObject *> py_classname;


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

} // pyinterface


/**
 * Contenience functions and types for the python interface.
 */
namespace py {

/**
 * Python object reference.
 */
using Obj = pyinterface::PyObjectRef;


/**
 * getattr(importlib.import_module("builtins"), name)
 */
Obj builtin(const std::string &name);


/**
 * importlib.import_module(name)
 */
Obj import(const std::string &name);


/**
 * str(value);
 */
Obj str(const std::string &value);


/**
 * bytes(value)
 */
Obj bytes(const char *value);


/**
 * int(value)
 */
Obj integer(int value);


/**
 * dict()
 */
Obj dict();


/**
 * None
 */
Obj none();

} // py
} // openage
