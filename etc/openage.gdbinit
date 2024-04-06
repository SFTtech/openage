python
import sys, os

print(f".gdbinit Python: current working directory is {os.getcwd()}")
print(f".gdbinit Python: adding custom pretty-printers directory to the GDB path: {os.getcwd() + '../../etc'}")

sys.path.insert(0, "../../etc")

import gdb_pretty.printers
# from gdb_pretty.printers import register_openage_printers
# register_openage_printers(None)
end
