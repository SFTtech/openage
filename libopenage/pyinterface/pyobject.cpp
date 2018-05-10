// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "pyobject.h"


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


std::string PyObjectRef::bytes() const {
	return py_bytes.call(this->ref);
}


int PyObjectRef::len() const {
	return py_len.call(this->ref);
}


bool PyObjectRef::callable() const {
	return py_callable.call(this->ref);
}


PyObjectRef PyObjectRef::call() const {
	PyObjectRef result;
	py_call0.call(&result, this->ref);
	return result;
}


PyObjectRef PyObjectRef::call(std::vector<PyObject *> &args) const {
	PyObjectRef result;
	py_calln.call(&result, this->ref, args);
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


int64_t PyObjectRef::to_int() const {
	return py_to_int.call(this->ref);
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
	py_eval.call(this->ref, &result, expression);
	return result;
}


void PyObjectRef::exec(const std::string &statement) const {
	py_exec.call(this->ref, statement);
}


PyObjectRef PyObjectRef::get(const PyObjectRef &key) const {
	PyObjectRef result;
	py_get.call(this->ref, &result, key.get_ref());
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
	return py_in.call(this->ref, container.get_ref());
}


PyObjectRef PyObjectRef::type() const {
	PyObjectRef result;
	py_type.call(this->ref, &result);
	return result;
}


std::string PyObjectRef::modulename() const {
	return py_modulename.call(this->ref);
}


std::string PyObjectRef::classname() const {
	return py_classname.call(this->ref);
}


std::ostream &operator <<(std::ostream &os, const PyObjectRef &ref) {
	if (unlikely(ref.get_ref() == nullptr)) {
		os << "PyObjectRef[null]";
	} else {
		os << "PyObjectRef[" << ref.repr() << "]";
	}

	return os;
}


PyIfFunc<void, PyObject *> py_xincref;
PyIfFunc<void, PyObject *> py_xdecref;

PyIfFunc<std::string, PyObject *> py_str;
PyIfFunc<std::string, PyObject *> py_repr;
PyIfFunc<std::string, PyObject *> py_bytes;
PyIfFunc<int, PyObject *> py_len;
PyIfFunc<bool, PyObject *> py_callable;
PyIfFunc<void, PyObjectRef *, PyObject *> py_call0;
PyIfFunc<void, PyObjectRef *, PyObject *, std::vector<PyObject *>&> py_calln;
PyIfFunc<bool, PyObject *, std::string> py_hasattr;
PyIfFunc<void, PyObjectRef *, PyObject *, std::string> py_getattr;
PyIfFunc<void, PyObject *, std::string, PyObject *> py_setattr;
PyIfFunc<bool, PyObject *, PyObject *> py_isinstance;
PyIfFunc<bool, PyObject *> py_to_bool;
PyIfFunc<int64_t, PyObject *> py_to_int;
PyIfFunc<void, PyObject *, Func<void, std::string>> py_dir;
PyIfFunc<bool, PyObject *, PyObject *> py_equals;
PyIfFunc<void, PyObject *, std::string> py_exec;
PyIfFunc<void, PyObject *, PyObjectRef *, std::string> py_eval;
PyIfFunc<void, PyObject *, PyObjectRef *, PyObject *> py_get;
PyIfFunc<bool, PyObject *, PyObject *> py_in;
PyIfFunc<void, PyObject *, PyObjectRef *> py_type;
PyIfFunc<std::string, PyObject *> py_modulename;
PyIfFunc<std::string, PyObject *> py_classname;

PyIfFunc<void, PyObjectRef *, const std::string&> py_builtin;
PyIfFunc<void, PyObjectRef *, const std::string&> py_import;
PyIfFunc<void, PyObjectRef *, const std::string&> py_createstr;
PyIfFunc<void, PyObjectRef *, const std::string&> py_createbytes;
PyIfFunc<void, PyObjectRef *, int> py_createint;
PyIfFunc<void, PyObjectRef *> py_createdict;
PyIfFunc<void, PyObjectRef *> py_createlist;

} // pyinterface


namespace py {
using namespace pyinterface;


Obj builtin(const std::string &name) {
	Obj result;
	py_builtin.call(&result, name);
	return result;
}


Obj import(const std::string &name) {
	Obj result;
	py_import.call(&result, name);
	return result;
}


Obj str(const std::string &value) {
	Obj result;
	py_createstr.call(&result, value);
	return result;
}


Obj bytes(const std::string &value) {
	Obj result;
	py_createbytes.call(&result, value);
	return result;
}


Obj integer(int value) {
	Obj result;
	py_createint.call(&result, value);
	return result;
}


Obj dict() {
	Obj result;
	py_createdict.call(&result);
	return result;
}


Obj list() {
	Obj result;
	py_createlist.call(&result);
	return result;
}


Obj None;
Obj True;
Obj False;


} // py
} // openage
