#ifndef OPENAGE_PYTHON_H_EACA293D12F6433CA8DEAF235D201B2D
#define OPENAGE_PYTHON_H_EACA293D12F6433CA8DEAF235D201B2D

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
