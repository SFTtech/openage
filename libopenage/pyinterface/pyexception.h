// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libcpp.vector cimport vector
#include <vector>

// pxd: from libopenage.error.error cimport Error
#include "../error/error.h"
// pxd: from libopenage.error.backtrace cimport backtrace_symbol_constptr
#include "../error/backtrace.h"

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc2, Func1
#include "functional.h"
// pxd: from libopenage.pyinterface.pyobject cimport PyObjectRef, PyObjectPtr
#include "pyobject.h"


namespace openage {
namespace pyinterface {


/**
 * An implementation of the abstract Backtrace class that does not collect
 * Backtrace information itself, but instead determines the data on the fly
 * from a Python Exception object.
 */
class PyExceptionBacktrace : public error::Backtrace {
public:
	/**
	 * ref is a raw reference to the associated PyObject.
	 */
	PyExceptionBacktrace(PyObject *ref) : ref{ref} {}

	/**
	 * Accesses the associated Python exception object to translate the traceback as needed.
	 */
	void get_symbols(std::function<void (const error::backtrace_symbol *)> cb, bool reversed) const override;

private:
	PyObject *ref;
};


/**
 * Thrown by translate_exc_py_to_cpp() from exctranslate.h.
 *
 * These objects are constructed entirely empty, and designed to be filled
 * from describe_py_exception().
 *
 * pxd:
 *
 * cppclass PyException(Error):
 *     PyObjectRef py_obj
 *     void init_backtrace() except +
 */
class PyException : public error::Error {
public:
	/**
	 * Returns the type name of py_obj.
	 */
	std::string type_name() const override;


	/**
	 * Reference to the Python exception object.
	 */
	PyObjectRef py_obj;


	/**
	 * Initializes this->backtrace with a PyExceptionBacktrace object
	 * that points at this->py_obj->get_ref().
	 */
	void init_backtrace();
};


// pxd: PyIfFunc2[void, PyObjectPtr, Func1[void, backtrace_symbol_constptr]] pyexception_bt_get_symbols
extern PyIfFunc<void, PyObject *, Func<void, const error::backtrace_symbol *>> pyexception_bt_get_symbols;


}} // openage::pyinterface
