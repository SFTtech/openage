// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "pyobject_tests.h"

#include <unordered_set>

#include "pyobject.h"
#include "pyexception.h"
#include "../testing/testing.h"


namespace openage {
namespace pyinterface {
namespace tests {

void pyobject() {
	TESTEQUALS(py::str("foo").repr(), "'foo'");
	TESTEQUALS(py::bytes("foo").repr(), "b'foo'");
	TESTEQUALS(py::integer(1337).repr(), "1337");

	TESTEQUALS(py::None.repr(), "None");
	TESTEQUALS(py::builtin("None").equals(py::None), true);
	TESTEQUALS(py::builtin("None").is(py::None), true);

	TESTEQUALS(py::True.repr(), "True");
	TESTEQUALS(py::builtin("True").equals(py::True), true);
	TESTEQUALS(py::builtin("True").is(py::True), true);
	TESTEQUALS(py::True.to_bool(), true);

	TESTEQUALS(py::False.repr(), "False");
	TESTEQUALS(py::builtin("False").equals(py::False), true);
	TESTEQUALS(py::builtin("False").is(py::False), true);
	TESTEQUALS(py::False.to_bool(), false);

	py::Obj dict;
	TESTNOEXCEPT(dict = py::dict());
	TESTEQUALS(dict.repr(), "{}");
	TESTEQUALS(py::builtin("dict").call().equals(dict), true);
	TESTEQUALS(py::builtin("dict").call().is(dict), false);

	py::Obj deque;
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
		"    def __bytes__(self):\n"
		"        return b'bytes-A()'\n"
	));

	// test what happens when a goes out of scope
	{
		py::Obj a;
		TESTNOEXCEPT(a = dict.eval("A()"));
		TESTEQUALS(a.repr(), "A()");
		TESTEQUALS(a.str(), "A");
		TESTEQUALS(a.bytes(), "bytes-A()");
		TESTNOEXCEPT(a.setattr("foo", deque));
		TESTEQUALS(a.getattr("foo").is(deque), true);
		TESTEQUALS(a.hasattr("foo"), true);
		TESTEQUALS(a.hasattr("bar"), false);
	}

	py::Obj x;
	TESTNOEXCEPT(x = dict.get("x"));
	TESTEQUALS(x.repr(), "[1]");
	TESTEQUALS(x.len(), 1);
	TESTEQUALS(py::integer(1).in(x), true);
	TESTEQUALS(x.get(0).equals(py::integer(1)), true);
	TESTEQUALS(x.get(0).equals(py::integer(2)), false);

	TESTEQUALS(dict.isinstance(py::builtin("dict")), true);
	TESTEQUALS(dict.isinstance(py::builtin("list")), false);

	py::Obj pop;
	TESTNOEXCEPT(pop = x.getattr("pop"));
	TESTEQUALS(x.callable(), false);
	TESTEQUALS(pop.callable(), true);
	TESTEQUALS(pop.call().repr(), "1");
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
	py::Obj globals = py::dict();

	py::Obj inputfunc = globals.eval("lambda: input('>>> ')");

	while (true) {
		py::Obj input;
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


}}} // openage::pyinterface::tests
