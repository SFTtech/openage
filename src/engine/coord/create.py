#!/usr/bin/python3

#this script is a helper to create new coordinate system structs
#
#while there is no need to use it in any stage of development or building,
#it greatly simplifies the creation of new coordinate system/vector types.
#it has been developed in lieu of the erroneous manifold copy-pasting of the
#template files
#
#argv[1]  struct name
#argv[2]  scalar type alias
#argv[3]  scalar type name
#argv[4:] member variables

def readtemplate(fname):
	with open(fname) as f:
		data = f.read()
	#remove the first two lines (the '#error' directive)
	return '\n'.join(data.split('\n')[2:])

#read the template files
templateh = readtemplate('template.h')
templatecpp = readtemplate('template.cpp')

#read command-line arguments
import sys
name = sys.argv[1]
scalaralias = sys.argv[2]
scalartype = sys.argv[3]
memberlist = ', '.join(sys.argv[4:])

#generate template replacement dict
replace = {
	'name': name,
	'NAME': name.upper(),
	'scalaralias': scalaralias,
	'scalartype': scalartype,
	'memberlist': memberlist
}

def substandwrite(data, fname):
	for k, v in replace.items():
		data = data.replace('[' + k + ']', v)
	with open(fname, 'w') as f:
		return f.write(data)

#replace/write template data
substandwrite(templateh, name + '.h')
substandwrite(templatecpp, name + '.cpp')
