#ifndef _OPENAGE_PYTHON_H_
#define _OPENAGE_PYTHON_H_

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


#endif //_OPENAGE_PYTHON_H_
