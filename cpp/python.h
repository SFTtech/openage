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
