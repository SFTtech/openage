// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "python.h"

#include <Python.h>

#include "log/log.h"


namespace openage {

void python_init() {
	if (not Py_IsInitialized()) {
		//register signal handlers (kill, term, ...) to python?
		constexpr int init_signals = 1;
		Py_InitializeEx(init_signals);

		const char *py_version = Py_GetVersion();
		log::log(MSG(info) << "Python " << py_version << " has been initialized");
	}
}

void python_deinit() {
	if (Py_IsInitialized()) {
		Py_Finalize();
	}
}

} //namespace openage
