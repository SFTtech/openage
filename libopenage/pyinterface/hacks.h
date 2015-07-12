// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PYINTERFACE_HACKS_H_
#define OPENAGE_PYINTERFACE_HACKS_H_

/*
 * This file contains a collection of hacks to work around particular Cython
 * bugs and/or features.
 *
 * This header file is automatically included from all .pxd files via a include
 * directive that is added by pxdgen.h.
 *
 * Note that all issues with Cython should be reported upstream.
 * They usually get fixed pretty fast, and openage always uses the newest
 * version of Cython.
 */


// this value exists mainly so that something can be cdef'd when pxdgen
// includes this header file.
// "holds the number of hacks".
constexpr int hacks = 1;


/**
 * Include exctranslate.h, which defines its own replacement for
 * __Pyx_CppExn2PyErr.
 */
#include "exctranslate.h"


#endif
