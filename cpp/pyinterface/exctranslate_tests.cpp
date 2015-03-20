// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "exctranslate_tests.h"

#include <vector>

#include "../testing/testing.h"

#include "pyexception.h"

namespace openage {
namespace pyinterface {
namespace tests {


template<int i = 3>
void throw_foo() {
	throw_foo<i - 1>();
}


template<>
void throw_foo<0>() {
	throw Error(MSG(err) << "foo", true, true);
}


void err_cpp_to_py_helper() {
	try {
		try {
			throw Error(MSG(err) << "rofl");
		} catch (...) {
			TESTFAIL;
		}
	} catch (...) {
		throw_foo();
	}
}


void err_py_to_cpp() {
	// due to the nature of this test, we need to be careful about
	// throwing TestError to indicate test failure...

	// we'll instead call this helper lambda, which will return
	// a std::string error message.

	std::string testresult = []() -> std::string {

		try {
			err_py_to_cpp_helper.call();
			return "err_py_to_cpp_helper didn't throw an exception";

		} catch (PyException &exc) {
			// this is what we expected.
			// now let's see whether the object contains the expected data.
			if (exc.type_name() != "openage.cppinterface.exctranslate_tests.Bar") {
				return "unexpected exc typename: " + exc.type_name();
			}

			std::string msg = exc.what();
			if (msg != "bar") {
				return "unexpected exc message: " + msg;
			}

			try {
				exc.rethrow_cause();
				return "exc had no cause";

			} catch (PyException &cause) {
				if (cause.type_name() != "openage.testing.testing.TestError") {
					return "unexpected cause typename: " + cause.type_name();
				}

				std::string causemsg = cause.what();
				if (causemsg != "foo") {
					return "unexpected cause message: " + causemsg;
				}

				try {
					cause.rethrow_cause();
					return "OK";
				} catch (...) {
					return "cause unexpectedly had a cause itself.";
				}

			} catch (std::exception &cause) {
				return "exc had cause of unexpected type " + util::demangle(typeid(cause).name());
			} catch (...) {
				return "exc had cause of nonstandard type";
			}

		} catch (std::exception &cause) {
			return "exc had unexpected type " + util::demangle(typeid(cause).name());
		} catch (...) {
			return "exc had nonstandard type";
		}

	}();

	if (testresult != "OK") {
		TESTFAILMSG(testresult);
	}
}


void err_py_to_cpp_demo() {
	try {
		err_py_to_cpp_helper.call();
		log::log(MSG(err) <<
			"Unexpectedly, the helper did not raise an Exception");

	} catch (Error &err) {
		log::log(MSG(info) <<
			"The helper raised the following Exception:" << std::endl <<
			err);
	}
}


void bounce_call(Func<void> func, int times) {
	bounce_call_py.call(func, times);
}


PyIfFunc<void> err_py_to_cpp_helper;
PyIfFunc<void, Func<void>, int> bounce_call_py;


}}} // openage::pyinterface::tests
