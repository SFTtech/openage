// Copyright 2015-2015 the openage authors. See copying.md for legal info.
#ifndef OPENAGE_PYINTERFACE_EXCTRANSLATE_H_
#define OPENAGE_PYINTERFACE_EXCTRANSLATE_H_


/*
 * This hack replaces Cython's default 'except +' exception translator
 * with the one defined here.
 */
#define __Pyx_CppExn2PyErr ::openage::pyinterface::translate_exc_cpp_to_py


// pxd: from libcpp cimport bool as cppbool
// pxd: from libcpp.string cimport string
// pxd: from cpp.error.error cimport Error
// pxd: from cpp.log.message cimport message

// pxd: from cpp.pyinterface.pyexception cimport PyException

#include "../error/error.h"


namespace openage {
namespace pyinterface {

class PyException;


/**
 * Analyzes the current C++ exception,
 * and creates and raises an analogous Python CPPException object.
 *
 * If there's no current C++ exception, this terminates the
 * program non-gracefully (std::terminate).
 *
 * Designed to be called by auto-generated Cython code;
 * Do not use in any other way.
 */
void translate_exc_cpp_to_py();


/**
 * Analyzes the current Python exception,
 * and creates and throws an analogous C++ PyException object.
 *
 * Works, and is designed to be used in, situations where
 * there is no current Python exception (it's a no-op then).
 */
void translate_exc_py_to_cpp();


/**
 * For use by Python, to initialize a message's meta data.
 *
 * Initializes all values.
 *
 * pxd: void init_exc_message(message *msg, string filename, unsigned int lineno, string functionname) noexcept
 */
void init_exc_message(log::message *msg, const std::string &filename, unsigned int lineno, const std::string &functionname) noexcept;


/*
 * The PyIfFunc callback wrapper can _not_ be used to wrap these
 * functions, because those methods are used from within the Func code
 * itself, leading to
 *
 *  - infinite loops
 *  - immediate back-conversion of raised Python CPPException objects
 *
 * Thus, we're using old-school function pointers here.
 *
 * Note that this should be the only place in the entire C++ code base where
 * this is necessary.
 *
 * Everybody who calls one of those function pointers must manually ensure
 * that they are not nullptr (which would occur if libopenage has not been
 * properly initialized by the Python part of the code).
 */


/**
 * Installs the basic interface functions that are used for exception
 * translation.
 *
 * No PyIfFunc wrappers can be used here, because those internally
 * rely on exception translation themselves, which would lead to
 *
 *  - infinite loops
 *  - immediate back-conversion of raised Python CPPException objects
 *
 * Thus, we instead offer this setter function, to be called during
 * openage.cppinterface.exctranslate.setup().
 *
 * @param raise_cpp_error:
 *
 * Raises the given C++ Exception object as a Python exception.
 * called by translate_exc_cpp_to_py.
 *
 * @param raise_cpp_pyexception:
 *
 * Raises the given C++ PyException object as a Python exception.
 * called by translate_exc_cpp_to_py.
 *
 * The PyException still contains a reference to the original Python object
 * that was translated by describe_py_exception - we just use that.
 *
 * Invoked by openage::pyinterface::cpp_to_py.
 *
 * @param check_for_py_exception
 *
 * Used by translate_exc_cpp_to_py to check whether there is a currently-active
 * Python exception.
 *
 * @param describe_py_exception
 *
 * Used by translate_exc_cpp_to_py to gather information about
 * the current Python exception.
 *
 * Calls PyErr_Fetch and stores all sorts of information in the
 * PyException.
 * The current Python exception is cleared in the process.
 *
 * If the current Python exception had a cause, that cause is set
 * as the new active exception.
 * Call check_exception() afterwards to test whether that has happend.
 *
 * If there's no currently-active Python Exception, the behavior is
 * undefined; make sure to call check_exception() before.
 *
 * Afterwards, the PyException is ready for throwing.
 *
 * pxd:
 *
 * void (*set_exc_translation_funcs) (
 *     void (*)(Error *)       except * with gil,
 *     void (*)(PyException *) except * with gil,
 *     cppbool (*)()                    with gil,
 *     void (*)(PyException *) except * with gil
 * ) noexcept
 */
void set_exc_translation_funcs(
	void (*raise_cpp_error)(Error *),
	void (*raise_cpp_pyexception)(PyException *),
	bool (*check_for_py_exception)(),
	void (*describe_py_exception)(PyException *));


}} // openage::pyinterface

#endif
