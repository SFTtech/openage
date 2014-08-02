#include "python.h"

#include <Python.h>

#include "log.h"


namespace engine {

void python_init() {
	if (not Py_IsInitialized()) {
		//Py_SetProgramName(program_name);

		//register signal handlers (kill, term, ...) to python?
		constexpr int init_signals = 1;
		Py_InitializeEx(init_signals);

		const char *py_version = Py_GetVersion();
		log::msg("initialized python %s", py_version);
	}
}


void python_deinit() {
	if (Py_IsInitialized()) {
		Py_Finalize();
	}
}

} //namespace engine
