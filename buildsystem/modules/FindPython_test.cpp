// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#include <Python.h>

int main() {
	// set by preprocessor:
	constexpr uint64_t min_ver = TARGET_VERSION;

	static_assert(PY_VERSION_HEX >= min_ver, "unsuitable: " PY_VERSION);

	Py_Initialize();
	Py_Finalize();
}
