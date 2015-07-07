#!/usr/bin/env python3

"""
Openage can, and should, be launched via python3 -m openage.

However, for dynamic analysis using LLVM's sanitizer, a self-compiled
executable is needed.

This file is Cythonized with an embedded interpreter, producing ./run,
which satisifies that requirement.
"""

if __name__ == '__main__':
	from openage.__main__ import main
	main()
