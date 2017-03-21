// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// we want to avoid the Python.h import,
// we only need the prototype anyway.
#ifndef Py_OBJECT_H
// pxd: from cpython.ref cimport PyObject
extern "C" {
	typedef struct _object PyObject;
}
#endif
