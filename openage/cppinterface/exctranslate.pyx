# Copyright 2015-2024 the openage authors. See copying.md for legal info.

"""
Provides the raise_py_exception and describe_py_exception callbacks for
exctranslate.cpp.
"""

from cpython.ref cimport PyObject, Py_XDECREF
from cpython.exc cimport (
    PyErr_Occurred,
    PyErr_Fetch,
    PyErr_NormalizeException,
    PyErr_SetObject
)

from libcpp cimport bool as cppbool

from libopenage.log.message cimport message
from libopenage.error.error cimport Error
from libopenage.error.backtrace cimport backtrace_symbol
from libopenage.pyinterface.functional cimport Func1
from libopenage.pyinterface.pyexception cimport (
    PyException,
    pyexception_bt_get_symbols
)

from libopenage.pyinterface.exctranslate cimport (
    init_exc_message,
    set_exc_translation_funcs
)

from ..testing.testing import TestError
from ..log import info

cdef extern from "Python.h":
    int PyException_SetTraceback(PyObject *ex, PyObject *tb)

# _PyTraceback_Add has been made private in Python 3.13
# see https://github.com/python/cpython/pull/108453
# TODO: Find another solution to add tracebacks
# cdef extern from "traceback.h":
#     void _PyTraceback_Add(const char *funcname, const char *filename, int lineno)


cdef void PyTraceback_Add(const char *functionname, const char *filename, int lineno) noexcept with gil:
    """
    Add a new traceback stack frame.

    Note: Currently does nothing, because _PyTraceback_Add is no longer
          accessible since Python 3.13.

    TODO: Find another solution to add tracebacks.
    """
    # _PyTraceback_Add(functionname, filename, lineno)


cdef class CPPMessageObject:
    """
    Holds a C++ message object.
    """
    cdef message val


class CPPException(Exception):
    def __init__(self, bytes typename, bytes text):
        super().__init__(text.decode(errors='replace'))
        self.typename = typename


cdef void py_add_backtrace_frame_from_symbol(const backtrace_symbol *symbol) noexcept with gil:
    """
    For use as a callback with Error->backtrace->get_symbols(), from within
    raise_exception.
    """
    PyTraceback_Add(
        <char *> symbol.functionname.c_str(),
        <char *> symbol.filename.c_str(),
        <int> symbol.lineno)


# the callback object that contains the above function, for passing to get_symbols().
cdef Func1[void, const backtrace_symbol *] py_backtrace_adder
py_backtrace_adder.bind_noexcept0(py_add_backtrace_frame_from_symbol)


cdef void raise_cpp_error_common(Error *cpp_error_obj, object obj_to_raise) noexcept:
    """
    Common code that is used by both raise_cpp_error and raise_cpp_pyexception.

    Takes a pointer to a C++ error::Error, and a "prepared" Python object.

     - sets the cpp_msg_obj member of the object,
     - recursively translates a potential cause exception,
     - sets the object as the active Python exception.
    """

    cdef CPPMessageObject msg

    if not hasattr(obj_to_raise, "cpp_msg_obj"):
        msg = CPPMessageObject()
        msg.val = cpp_error_obj.msg
        obj_to_raise.cpp_msg_obj = msg

    # get the cause from cpp_error_obj
    try:
        # if cpp_error_obj has no cause, this does nothing.
        # else, rethrow_cause throws a C++ exception. being marked 'except +',
        # the cpp -> py exception translator (this one) is invoked to translate
        # the exception to a Py exception.
        # This method will throw an Exception object, and the except clause will
        # be entered.
        # If the cause exception has a cause itself, further recursion occurs.
        #
        # If the cause was a std::exception, it is "converted" to a openage error
        # in order to prevent crashing everything.
        cpp_error_obj.rethrow_cause()

    except Exception as cause_exc:
        obj_to_raise.__cause__ = cause_exc

    PyErr_SetObject(type(obj_to_raise), obj_to_raise)


cdef void raise_cpp_error(Error *cpp_error_obj) except * with gil:
    # see the doc of the function pointer in exctranslate.h
    # raises a py exception that corresponds to the given Error obj.

    cdef bytes typename = cpp_error_obj.type_name()

    if typename == b"openage::testing::TestError":
        exc = TestError(cpp_error_obj.what().decode(errors='replace'))
    else:
        exc = CPPException(typename, <bytes> cpp_error_obj.what())

    raise_cpp_error_common(cpp_error_obj, exc)
    # A Python exception is active from this point on.
    # Don't use any Python functionality.


    # add traceback lines for cpp message obj metadata
    PyTraceback_Add(cpp_error_obj.msg.functionname,
                    cpp_error_obj.msg.filename,
                    <int> cpp_error_obj.msg.lineno)

    # add traceback lines for cpp_error_obj.backtrace
    if cpp_error_obj.backtrace != NULL:
        # Fix the C++ exception object's backtrace (remove unneeded entries)
        # Let's just hope that none of these throws...
        cpp_error_obj.trim_backtrace()
        cpp_error_obj.backtrace.get_symbols(py_backtrace_adder, False)


cdef void raise_cpp_pyexception(PyException *cpp_pyexception_obj) except * with gil:
    # see the doc of the function pointer in exctranslate.h
    # raises a py exception that corresponds to the given PyException obj.

    exc = <object> <PyObject *> cpp_pyexception_obj.py_obj.get_ref()

    raise_cpp_error_common(cpp_pyexception_obj, exc)
    # A Python exception is active from this point on!

    # exc still has its proper backtrace from last time;
    # No new data was added during its C++ life.


cdef cppbool check_exception() noexcept with gil:
    # see the doc of the function pointer in exctranslate.h
    return (PyErr_Occurred() != NULL)


cdef void describe_exception(PyException *pyex) except * with gil:
    # see the doc of the function pointer in exctranslate.h
    # describes the current py exc and stores the desc in pyex.

    # implementation notice:
    # take care not to access any python functionality while a Python exception
    # is currently active!

    cdef PyObject *exc_type_ptr = NULL
    cdef PyObject *exc_value_ptr = NULL
    cdef PyObject *exc_traceback_ptr = NULL

    PyErr_Fetch(&exc_type_ptr, &exc_value_ptr, &exc_traceback_ptr)
    # no Python exception is active anymore from this point on.
    PyErr_NormalizeException(&exc_type_ptr, &exc_value_ptr, &exc_traceback_ptr)
    if exc_traceback_ptr != NULL:
        traceback = <object> exc_traceback_ptr
        PyException_SetTraceback(exc_value_ptr, exc_traceback_ptr)
    else:
        traceback = None

    # create a "real" object from the value ptr.
    cdef object exc = <object> exc_value_ptr

    # we no longer need those raw pointers.
    Py_XDECREF(exc_type_ptr)
    Py_XDECREF(exc_traceback_ptr)
    # hand the value raw pointer to pyex.py_obj.
    pyex.py_obj.set_ref_without_incrementing(exc_value_ptr)

    # set exc.msg
    if hasattr(exc, "cpp_msg_obj"):
        pyex.msg = (<CPPMessageObject> exc.cpp_msg_obj).val
    else:
        try:
            pyex.msg.text = str(exc).encode()
        except Exception as fetcherror:
            pyex.msg.text = (
                "Exception while fetching Py exception message: " +
                str(fetcherror)).encode(errors='replace')

        # for the metadata, we require the last traceback frame.
        if traceback is None:
            init_exc_message(&pyex.msg, b"", 0, b"")
        else:
            while traceback.tb_next is not None:
                traceback = traceback.tb_next

            # init meta with that frame's filename, lineno, functionname.
            init_exc_message(
                &pyex.msg,
                traceback.tb_frame.f_code.co_filename.encode(),
                traceback.tb_lineno,
                traceback.tb_frame.f_code.co_name.encode())

    # init exc.backtrace (possible after set_ref)
    pyex.init_backtrace()

    # set the cause exception as the active exception, to allow recursive
    # processing by translate_exc_py_to_cpp.
    cdef object cause = None
    if getattr(exc, "__cause__", None) is not None:
        cause = exc.__cause__
    elif getattr(exc, "__context__", None) is not None:
        cause = Exception("During handling, another exception occurred")
        cause.__cause__ = exc.__context__

    if cause is not None:
        PyErr_SetObject(type(cause), cause)
        # now a Python exception is active again.


cdef void pyexception_bt_get_symbols_impl(
        PyObject *py_obj,
        Func1[void, const backtrace_symbol *] callback) except * with gil:

    cdef object py_exc = <object> py_obj
    cdef object frame = getattr(py_exc, "__traceback__", None)

    cdef backtrace_symbol symbol
    symbol.pc = NULL

    while frame is not None:
        symbol.filename = frame.tb_frame.f_code.co_filename.encode()
        symbol.functionname = frame.tb_frame.f_code.co_name.encode()
        symbol.lineno = frame.tb_lineno

        callback.call(&symbol)

        frame = frame.tb_next


def setup(args):
    """
    Installs the functions defined here in their PyFunc pointers.
    """
    set_exc_translation_funcs(
        raise_cpp_error,
        raise_cpp_pyexception,
        check_exception,
        describe_exception)


    if args.trap_exceptions:
        info("Throwing errors will break into an attached debugger")
        Error.debug_break_on_create(True)
    else:
        Error.debug_break_on_create(False)

    pyexception_bt_get_symbols.bind0(pyexception_bt_get_symbols_impl)
