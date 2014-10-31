#!/usr/bin/env python3

# Copyright 2013-2014 the openage authors. See copying.md for legal info.

# this script is a helper to create new coordinate system structs
#
# while there is no need to use it in any stage of development or building,
# it greatly simplifies the creation of new coordinate system/vector types.
# it has been developed in lieu of the erroneous manifold copy-pasting and
# adjusting of the template files
#
# argv[1]  struct name
# argv[2]  scalar type name
# argv[3:] member variables

def readtemplate(fname):
    with open(fname) as f:
        data = f.read()
    # remove the first two lines (the '#error' directive)
    return '\n'.join(data.split('\n')[4:])

# read the template files
templateh = readtemplate('template.h.in')
templatecpp = readtemplate('template.cpp.in')

# read command-line arguments
import sys
name = sys.argv[1]
scalartype = sys.argv[2]
memberlist = ', '.join(sys.argv[3:])

# generate template replacement dict
replace = {
    'name': name,
    'absname': name,
    'relname': name + '_delta',
    'NAME': name.upper(),
    'scalartype': scalartype,
    'memberlist': memberlist
}

def substandwrite(data, fname):
    for k, v in replace.items():
        data = data.replace('[' + k + ']', v)
    with open(fname, 'w') as f:
        return f.write(data)

# replace/write template data
substandwrite(templateh, name + '.h')
substandwrite(templatecpp, name + '.cpp')
