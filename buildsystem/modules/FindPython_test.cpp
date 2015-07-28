// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <Python.h>

int main() {
	static_assert(PY_VERSION_HEX >= 0x03040000, PY_VERSION);

	Py_Initialize();
	Py_Finalize();
}
