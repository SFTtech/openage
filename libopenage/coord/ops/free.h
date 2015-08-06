// Copyright 2013-2015 the openage authors. See copying.md for legal info.

// has no header guard: used in a preprocessor hack
// TODO obsolete this file using the python codegen

//free operators for RELATIVE_TYPE and ABSOLUTE_TYPE

struct RELATIVE_TYPE operator*(SCALAR_TYPE scalar, RELATIVE_TYPE vectorval);
struct ABSOLUTE_TYPE operator*(SCALAR_TYPE scalar, ABSOLUTE_TYPE vectorval);
