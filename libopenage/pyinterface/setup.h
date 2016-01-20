// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <functional>

#pragma once

/*
 * Code for verifying that all components of the Python interface have been
 * properly initialized.
 *
 * A "Python interface component" is a global object that is supposed to
 * be initialized externally by openage.cppinterface.setup.setup().
 * Examples include PyIfFunc and PyIfObjRef objects.
 */

namespace openage {
namespace pyinterface {


/**
 * Adds a Python interface component.
 * Usually, you'd call this from a component's constructor.
 *
 * thisptr is a pointer to the component object.
 *
 * checker is a function that must have defined behavior until
 * destroy_py_if_component has been called for thisptr.
 * It shall return true if the object has been properly initialized,
 * and shall not throw any exceptions.
 */
void add_py_if_component(void *thisptr, std::function<bool ()> checker);


/**
 * Signals that an interface component that has been previously registered
 * via add_py_if_component is no longer valid, e.g. because its destructor
 * has been invoked.
 * Usually, you'd call this from a component's destructor.
 */
void destroy_py_if_component(void *thisptr);


/**
 * Checks whether all objects that have been registered via add_py_if_component
 * have been properly initialized.
 *
 * If not, or if components have been destroyed already, raises an exception
 * that contains the component names.
 *
 * May be called only once.
 *
 * pxd: void check() except +
 */
void check();


}} // openage::pyinterface
