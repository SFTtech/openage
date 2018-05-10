// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

/*
 * This file contains a collection of hacks to work around particular Cython
 * bugs and/or features.
 *
 * This header file is automatically included when building Cython modules.
 *
 * Note that all issues with Cython should be reported upstream.
 * They usually get fixed pretty fast, and openage always uses the newest
 * version of Cython.
 */


/**
 * Include exctranslate.h, which defines its own replacement for
 * __Pyx_CppExn2PyErr.
 */
#include "exctranslate.h"
