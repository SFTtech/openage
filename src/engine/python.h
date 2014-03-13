#ifndef _ENGINE_PYTHON_H_
#define _ENGINE_PYTHON_H_

#include <Python.h>

namespace engine {

/**
 * initialize embedded python
 */
void python_init();

/**
 * finalize python environment
 */
void python_deinit();


} //namespace engine


#endif //_ENGINE_PYTHON_H_
