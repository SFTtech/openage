// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "pyobject.h"

#include <unordered_set>

#include "../testing/testing.h"

#include "pyexception.h"

namespace openage {
namespace pyinterface {


PyObjectRef::PyObjectRef() noexcept
	:
	ref{nullptr} {}


PyObjectRef::PyObjectRef(PyObject *ref)
	:
	ref{ref} {

	py_xincref.call(this->ref);
}


PyObjectRef::PyObjectRef(const PyObjectRef &other)
	:
	ref{other.ref} {

	py_xincref.call(this->ref);
}


PyObjectRef::PyObjectRef(PyObjectRef &&other) noexcept
	:
	ref{other.ref} {

	// don't incref, because we simultaneously clear other.ref.
	other.ref = nullptr;
}


PyObjectRef &PyObjectRef::operator =(const PyObjectRef &other) {
	if (this->ref != nullptr) {
		py_xdecref.call(this->ref);
	}

	this->ref = other.ref;
	py_xincref.call(this->ref);

	return *this;
}


PyObjectRef &PyObjectRef::operator =(PyObjectRef &&other) {
	if (this->ref != nullptr) {
		py_xdecref.call(this->ref);
	}

	this->ref = other.ref;

	// don't incref, because we simultaneously clear other.ref.
	other.ref = nullptr;

	return *this;
}


PyObjectRef::~PyObjectRef() {
	if (this->ref != nullptr) {
		py_xdecref.call(this->ref);
	}
}


void PyObjectRef::set_ref(PyObject *ref) {
	if (unlikely(this->ref != nullptr)) {
		py_xdecref.call(this->ref);
	}

	this->ref = ref;
	py_xincref.call(this->ref);
}


void PyObjectRef::set_ref_without_incrementing(PyObject *ref) {
	if (unlikely(this->ref != nullptr)) {
		py_xdecref.call(this->ref);
	}

	this->ref = ref;
}


std::string PyObjectRef::str() const {
	return py_str.call(this->ref);
}


std::string PyObjectRef::repr() const {
	return py_repr.call(this->ref);
}


int PyObjectRef::len() const {
	return py_len.call(this->ref);
}


bool PyObjectRef::callable() const {
	return py_callable.call(this->ref);
}


PyObjectRef PyObjectRef::call() const {
	PyObjectRef result;
	py_call.call(&result, this->ref);
	return result;
}


bool PyObjectRef::hasattr(const std::string &name) const {
	return py_hasattr.call(this->ref, name);
}


PyObjectRef PyObjectRef::getattr(const std::string &name) const {
	PyObjectRef result;
	py_getattr.call(&result, this->ref, name);
	return result;
}


void PyObjectRef::setattr(const std::string &name, const PyObjectRef &attr) const {
	py_setattr.call(this->ref, name, attr.get_ref());
}


bool PyObjectRef::to_bool() const {
	return py_to_bool.call(this->ref);
}


bool PyObjectRef::isinstance(const PyObjectRef &type) const {
	return py_isinstance.call(this->ref, type.get_ref());
}


void PyObjectRef::dir(const Func<void, std::string> &callback) const {
	py_dir.call(this->ref, callback);
}


bool PyObjectRef::is(const PyObjectRef &other) const {
	return this->ref == other.get_ref();
}


bool PyObjectRef::equals(const PyObjectRef &other) const {
	return py_equals.call(this->ref, other.get_ref());
}


PyObjectRef PyObjectRef::eval(const std::string &expression) const {
	PyObjectRef result;
	py_eval.call(this->get_ref(), &result, expression);
	return result;
}


void PyObjectRef::exec(const std::string &statement) const {
	py_exec.call(this->get_ref(), statement);
}


PyObjectRef PyObjectRef::get(const PyObjectRef &key) const {
	PyObjectRef result;
	py_get.call(this->get_ref(), &result, key.get_ref());
	return result;
}


PyObjectRef PyObjectRef::get(const std::string &key) const {
	PyObjectRef keyobj = py::str(key);
	return this->get(keyobj);
}


PyObjectRef PyObjectRef::get(int key) const {
	PyObjectRef keyobj = py::integer(key);
	return this->get(keyobj);
}


bool PyObjectRef::in(const PyObjectRef &container) const {
	return py_in.call(this->get_ref(), container.get_ref());
}


PyObjectRef PyObjectRef::type() const {
	PyObjectRef result;
	py_type.call(this->get_ref(), &result);
	return result;
}


std::string PyObjectRef::modulename() const {
	return py_modulename.call(this->get_ref());
}


std::string PyObjectRef::classname() const {
	return py_classname.call(this->get_ref());
}


std::ostream &operator <<(std::ostream &os, const PyObjectRef &ref) {
	if (unlikely(ref.get_ref() == nullptr)) {
		os << "PyObjectRef[null]";
	} else {
		os << "PyObjectRef[" << ref.repr() << "]";
	}

	return os;
}


namespace py {

PyObjectRef builtin(const std::string &name) {
	PyObjectRef result;
	py_builtin.call(&result, name);
	return result;
}


PyObjectRef import(const std::string &name) {
	PyObjectRef result;
	py_import.call(&result, name);
	return result;
}


PyObjectRef str(const std::string &value) {
	PyObjectRef result;
	py_createstr.call(&result, value);
	return result;
}


PyObjectRef bytes(const char *value) {
	PyObjectRef result;
	py_createbytes.call(&result, value);
	return result;
}


PyObjectRef integer(int value) {
	PyObjectRef result;
	py_createint.call(&result, value);
	return result;
}


PyObjectRef none() {
	PyObjectRef result;
	py_getnone.call(&result);
	return result;
}


PyObjectRef dict() {
	PyObjectRef result;
	py_createdict.call(&result);
	return result;
}

} // py


namespace tests {

void pyobject() {
	TESTEQUALS(py::str("foo").repr(), "'foo'");
	TESTEQUALS(py::bytes("foo").repr(), "b'foo'");
	TESTEQUALS(py::integer(1337).repr(), "1337");

	PyObjectRef none;
	TESTNOEXCEPT(none = py::none());
	TESTEQUALS(none.repr(), "None");
	TESTEQUALS(py::builtin("None").equals(none), true);
	TESTEQUALS(py::builtin("None").is(none), true);

	PyObjectRef dict;
	TESTNOEXCEPT(dict = py::dict());
	TESTEQUALS(dict.repr(), "{}");
	TESTEQUALS(py::builtin("dict").call().equals(dict), true);
	TESTEQUALS(py::builtin("dict").call().is(dict), false);

	PyObjectRef deque;
	TESTNOEXCEPT(deque = py::import("collections").getattr("deque").call());
	TESTEQUALS(deque.repr(), "deque([])");

	TESTTHROWS(dict.exec("raise Exception()"));

	TESTNOEXCEPT(dict.exec("x = []"));

	TESTEQUALS(dict.get("x").repr(), "[]");
	TESTEQUALS(py::str("x").in(dict), true);
	TESTEQUALS(py::bytes("x").in(dict), false);

	TESTNOEXCEPT(dict.exec(
		"class A:\n"
		"    def __del__(self):\n"
		"        x.append(1)\n"
		"    def __str__(self):\n"
		"        return 'A'\n"
		"    def __repr__(self):\n"
		"        return 'A()'\n"
	));

	// test what happens when a goes out of scope
	{
		PyObjectRef a;
		TESTNOEXCEPT(a = dict.eval("A()"));
		TESTEQUALS(a.repr(), "A()");
		TESTEQUALS(a.str(), "A");
		TESTNOEXCEPT(a.setattr("foo", deque));
		TESTEQUALS(a.getattr("foo").is(deque), true);
		TESTEQUALS(a.hasattr("foo"), true);
		TESTEQUALS(a.hasattr("bar"), false);
	}

	PyObjectRef x;
	TESTNOEXCEPT(x = dict.get("x"));
	TESTEQUALS(x.repr(), "[1]");
	TESTEQUALS(x.len(), 1);
	TESTEQUALS(py::integer(1).in(x), true);
	TESTEQUALS(x.get(0).equals(py::integer(1)), true);
	TESTEQUALS(x.get(0).equals(py::integer(2)), false);

	TESTEQUALS(dict.isinstance(py::builtin("dict")), true);
	TESTEQUALS(dict.isinstance(py::builtin("list")), false);

	PyObjectRef pop;
	TESTNOEXCEPT(pop = x.getattr("pop"));
	TESTEQUALS(x.callable(), false);
	TESTEQUALS(pop.callable(), true);
	TESTEQUALS(pop.call().repr(), "1");

	TESTEQUALS(py::builtin("True").to_bool(), true);
	TESTEQUALS(py::builtin("False").to_bool(), false);
	TESTEQUALS(x.to_bool(), false);
	TESTEQUALS(dict.to_bool(), true);

	std::unordered_set<std::string> dir;
	TESTNOEXCEPT(pop.dir([&](std::string s) {dir.insert(s);}));
	dir.find("__self__") == dir.end() and TESTFAIL;
	dir.clear();

	TESTEQUALS(pop.getattr("__self__").is(x), true);
	TESTEQUALS(x.equals(deque), false);
	TESTEQUALS(x.equals(py::builtin("list").call()), true);

	TESTEQUALS(dict.type().is(py::builtin("dict")), true);
	TESTEQUALS(dict.modulename(), "builtins");
	TESTEQUALS(dict.classname(), "dict");
}


void pyobject_demo() {
	PyObjectRef globals = py::dict();

	PyObjectRef inputfunc = globals.eval("lambda: input('>>> ')");

	while (true) {
		PyObjectRef input;
		try {
			input = inputfunc.call();

		} catch (PyException &exc) {
			if (exc.type_name() == "builtins.EOFError") {
				std::cout << "goodbye." << std::endl;
				break;
			}

			std::cout << exc << std::endl;
			continue;
		}

		try {
			globals.exec(input.str());
		} catch (PyException &exc) {
			std::cout << exc << std::endl;
		}
	}
}


} // tests


PyIfFunc<void, PyObject *> py_xincref;
PyIfFunc<void, PyObject *> py_xdecref;


PyIfFunc<std::string, PyObject *> py_str;
PyIfFunc<std::string, PyObject *> py_repr;
PyIfFunc<int, PyObject *> py_len;
PyIfFunc<bool, PyObject *> py_callable;
PyIfFunc<void, PyObjectRef *, PyObject *> py_call;
PyIfFunc<bool, PyObject *, std::string> py_hasattr;
PyIfFunc<void, PyObjectRef *, PyObject *, std::string> py_getattr;
PyIfFunc<void, PyObject *, std::string, PyObject *> py_setattr;
PyIfFunc<bool, PyObject *, PyObject *> py_isinstance;
PyIfFunc<bool, PyObject *> py_to_bool;
PyIfFunc<void, PyObject *, Func<void, std::string>> py_dir;
PyIfFunc<bool, PyObject *, PyObject *> py_equals;
PyIfFunc<void, PyObject *, std::string> py_exec;
PyIfFunc<void, PyObject *, PyObjectRef *, std::string> py_eval;
PyIfFunc<void, PyObject *, PyObjectRef *, PyObject *> py_get;
PyIfFunc<bool, PyObject *, PyObject *> py_in;
PyIfFunc<void, PyObject *, PyObjectRef *> py_type;
PyIfFunc<std::string, PyObject *> py_modulename;
PyIfFunc<std::string, PyObject *> py_classname;


PyIfFunc<void, PyObjectRef *, std::string> py_builtin;
PyIfFunc<void, PyObjectRef *, std::string> py_import;
PyIfFunc<void, PyObjectRef *, std::string> py_createstr;
PyIfFunc<void, PyObjectRef *, const char *> py_createbytes;
PyIfFunc<void, PyObjectRef *, int> py_createint;
PyIfFunc<void, PyObjectRef *> py_createdict;
PyIfFunc<void, PyObjectRef *> py_getnone;


}} // openage::pyinterface
