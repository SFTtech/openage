#!/usr/bin/python3

#this script automatically generates vector header files from 'template.h'
#
#it must be run in order to actually build the openage project
#
#while it is true that its functionality could also be implementing using
#the C preprocessor, I don't want to do this to either
#  myself,
#  the compiler,
#  anyone who wants to read the code,
#  anyone who wants to read compiler error messages.
#
#the script configuration lies in 'gen.conf'
#the script documentation are the comments in this script.
#
#the script works by reading the template file and replacing placeholder
#variables such as [name] or [scalarmult] by their replacements ('vectorxy',
#'x * arg, y * arg').

#read the configuration file
exec(open('gen.conf').read(), globals())

#read the template file
template = open('template.h').read()

#remove the first two lines (the #error directive)
template = '\n'.join(template.split('\n')[2:])

#generate all output files
#the 'gen' dict comes from gen.conf
for name, fields in gen.items():
	filename = name + '.h'

	#this dict contains all the actual replacement rules for the template
	#the following are the variables that will be replaced,
	#with example values (for name == 'vectorxy', fields == ['x', 'y'])
	#
	# name        vectorxy
	# uppername   VECTORXY
	# vlist       x, y
	# tlist       T x, T y
	# ilist       x{x}, y{y}
	# negation    -x, -y
	# addition    x + arg.x, y + arg.y
	# difference  x - arg.x, y - arg.y
	# scalarmult  x * arg, y * arg
	# scalardiv   x / arg, y / arg
	# sqsum       x * x + y * y
	#
	# in the following, we fill replace with these values.

	replace = {}
	replace['name'] = name
	replace['uppername'] = name.upper()

	fieldsstring = lambda f: ', '.join((f(field) for field in fields))

	replace['vlist'] = fieldsstring(lambda f: f)
	replace['tlist'] = fieldsstring(lambda f: 'T ' + f)
	replace['ilist'] = fieldsstring(lambda f: f + '{' + f + '}')
	replace['negation'] = fieldsstring(lambda f: '-' + f)
	replace['addition'] = fieldsstring(lambda f: f + ' + arg.' + f)
	replace['difference'] = fieldsstring(lambda f: f + ' - arg.' + f)
	replace['scalarmult'] = fieldsstring(lambda f: f + ' * arg')
	replace['scalardiv'] = fieldsstring(lambda f: f + ' / arg')
	replace['sqsum'] = ' + '.join((f + ' * ' + f for f in fields))

	#do the actual replacing, and write the resulting header

	resultfile = template
	for k, v in replace.items():
		resultfile = resultfile.replace('[' + k + ']', v)

	open(filename, 'w').write(resultfile)
