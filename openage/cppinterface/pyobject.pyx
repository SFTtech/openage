# Copyright 2015-2015 the openage authors. See copying.md for legal info.

from libcpp.string cimport string
from libcpp cimport bool as cppbool

from cpython.ref cimport Py_XINCREF, Py_XDECREF, PyObject

from libopenage.pyinterface.functional cimport Func1

from libopenage.pyinterface.pyobject cimport (
    PyObjectRef,

    py_xincref,
    py_xdecref,

    py_str,
    py_repr,
    py_len,
    py_callable,
    py_call,
    py_hasattr,
    py_getattr,
    py_setattr,
    py_isinstance,
    py_to_bool,
    py_dir,
    py_equals,
    py_eval,
    py_exec,
    py_get,
    py_in,
    py_type,
    py_modulename,
    py_classname,

    py_builtin,
    py_import,
    py_createstr,
    py_createbytes,
    py_createint,
    py_createdict,
    py_getnone
)


import builtins
import importlib


cdef void xincref(void *ptr) with gil:
    Py_XINCREF(<PyObject *> ptr)


cdef void xdecref(void *ptr) with gil:
    Py_XDECREF(<PyObject *> ptr)


cdef string str_impl(void *ptr) except * with gil:
    return str(<object> <PyObject *> ptr).encode()


cdef string repr_impl(void *ptr) except * with gil:
    return repr(<object> <PyObject *> ptr).encode()


cdef int len_impl(void *ptr) except * with gil:
    return len(<object> <PyObject *> ptr)


cdef cppbool callable_impl(void *ptr) except * with gil:
    return callable(<object> <PyObject *> ptr)


cdef void call_impl(PyObjectRef *result_ref, void *ptr) except * with gil:
    cdef object result_obj = (<object> <PyObject *> ptr)()
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef cppbool hasattr_impl(void *ptr, string name) except * with gil:
    return hasattr(<object> <PyObject *> ptr, name.decode())


cdef void getattr_impl(PyObjectRef *result_ref, void *ptr, string name) except * with gil:
    cdef object result_obj = getattr(<object> <PyObject *> ptr, name.decode())
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void setattr_impl(void *ptr, string name, void *attr) except * with gil:
    setattr(
        <object> <PyObject *> ptr,
        name.decode(),
        <object> <PyObject *> attr)


cdef cppbool isinstance_impl(void *ptr, void *typeptr) except * with gil:
    return isinstance(<object> <PyObject *> ptr, <object> <PyObject *> typeptr)


cdef cppbool to_bool_impl(void *ptr) except * with gil:
    return bool(<object> <PyObject *> ptr)


cdef void dir_impl(void *ptr, Func1[void, string] callback) except * with gil:
    for name in dir(<object> <PyObject *> ptr):
        callback.call(name.encode())


cdef cppbool equals_impl(void *ptr, void *other) except * with gil:
    return (<object> <PyObject *> ptr) == (<object> <PyObject *> other)


cdef void exec_impl(void *context, string statement) except * with gil:
    exec(statement.decode(), <object> <PyObject *> context)


cdef void eval_impl(void *context, PyObjectRef *result_ref, string expr) except * with gil:
    cdef object result_obj = eval(expr.decode(), <object> <PyObject *> context)
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void get_impl(void *ptr, PyObjectRef *result_ref, void *key) except * with gil:
    cdef object result_obj = (<object> <PyObject *> ptr)[<object> <PyObject *> key]
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef cppbool in_impl(void *ptr, void *container) except * with gil:
    return (<object> <PyObject *> ptr) in (<object> <PyObject *> container)


cdef void type_impl(void *ptr, PyObjectRef *result_ref) except * with gil:
    cdef object result_obj = type(<object> <PyObject *> ptr)
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef string modulename_impl(void *ptr) except * with gil:
    return type(<object> <PyObject *> ptr).__module__.encode()


cdef string classname_impl(void *ptr) except * with gil:
    return type(<object> <PyObject *> ptr).__name__.encode()


cdef void builtin_impl(PyObjectRef *result_ref, string name) except * with gil:
    cdef object result_obj = getattr(builtins, name.decode())
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void import_impl(PyObjectRef *result_ref, string name) except * with gil:
    cdef object result_obj = importlib.import_module(name.decode())
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void createstr_impl(PyObjectRef *result_ref, string value) except * with gil:
    cdef object result_obj = value.decode()
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void createbytes_impl(PyObjectRef *result_ref, const char *value) except * with gil:
    cdef object result_obj = bytes(value)
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void createint_impl(PyObjectRef *result_ref, int value) except * with gil:
    cdef object result_obj = value
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void createdict_impl(PyObjectRef *result_ref) except * with gil:
    cdef object result_obj = dict()
    result_ref.set_ref(<void *> <PyObject *> result_obj)


cdef void getnone_impl(PyObjectRef *result_ref) except * with gil:
    cdef object result_obj = None
    result_ref.set_ref(<void *> <PyObject *> result_obj)


def setup():
    py_xincref.bind_noexcept0(xincref)
    py_xdecref.bind_noexcept0(xdecref)

    py_str.bind0(str_impl)
    py_repr.bind0(repr_impl)
    py_len.bind0(len_impl)
    py_callable.bind0(callable_impl)
    py_call.bind0(call_impl)
    py_hasattr.bind0(hasattr_impl)
    py_getattr.bind0(getattr_impl)
    py_setattr.bind0(setattr_impl)
    py_isinstance.bind0(isinstance_impl)
    py_to_bool.bind0(to_bool_impl)
    py_dir.bind0(dir_impl)
    py_equals.bind0(equals_impl)
    py_eval.bind0(eval_impl)
    py_exec.bind0(exec_impl)
    py_get.bind0(get_impl)
    py_in.bind0(in_impl)
    py_type.bind0(type_impl)
    py_modulename.bind0(modulename_impl)
    py_classname.bind0(classname_impl)

    py_builtin.bind0(builtin_impl)
    py_import.bind0(import_impl)
    py_createstr.bind0(createstr_impl)
    py_createbytes.bind0(createbytes_impl)
    py_createint.bind0(createint_impl)
    py_createdict.bind0(createdict_impl)
    py_getnone.bind0(getnone_impl)
