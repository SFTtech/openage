// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PYTHON_H_
#define OPENAGE_PYTHON_H_

#include <Python.h>

namespace openage {

/**
 * initialize embedded python
 */
void python_init();

/**
 * finalize python environment
 */
void python_deinit();


} //namespace openage


#endif
