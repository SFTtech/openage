// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_FSLIKEOBJECT_H_
#define OPENAGE_UTIL_FSLIKEOBJECT_H_

#include <iostream>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc1
#include "../pyinterface/functional.h"

#include "../pyinterface/pyobject.h"

/*
 * C++ wrappers for openage.util.fslikeobject and openage.util.filelikeobject.
 */

namespace openage {
namespace util {


/**
 * Analogous to util.fslike.path.Path.
 * For use as constructor argument by RFile and WFile.
 */
class Path {
public:
    Path(pyinterface::PyObjectRef fs, std::vector<std::string> parts);

    std::shared_ptr<pyinterface::PyObjectRef> fs;
    std::vector<std::string> parts;
};


/**
 * Wraps a Python file-like object ('rb').
 */
class RFile : public std::istream {
public:
    RFile(Path &path);

private:
    pyinterface::PyObjectRef obj;
};


/**
 * Wraps a Python file-like object ('wb').
 */
class WFile : public std::ostream {
public:
    WFile(Path &path);

private:
    pyinterface::PyObjectRef obj;
};


}} // openage::util

#endif
