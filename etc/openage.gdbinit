python
import sys, os

print("Loading openage.gdbinit")
print(f"Adding custom pretty-printers directory to the GDB path: {os.getcwd() + '../../etc'}")

sys.path.insert(0, "../../etc")

import gdb_pretty.printers
end
