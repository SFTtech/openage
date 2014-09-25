#!/usr/bin/env python3
from . import cab

def dump_cabfolder(folder, outfile):
    while True:
        data = folder.pseudofile.read(4096)
        if data == b"":
            break
        outfile.write(data)

def print_cabfile(cabfile):
    print("\x1b[1mHeader\x1b[m")
    print(cabfile.header)

    for number, folder in enumerate(cabfile.folders):
        print("\x1b[1mFolder " + str(number) + "\x1b[m")
        print(folder)
    for number, file_ in enumerate(cabfile.files):
        print("\x1b[1mFile " + str(number) + "\x1b[m")
        print(file_)

if __name__ == "__main__":
    import sys
    cabfile = cab.CABFile(sys.argv[1])
    cabfile.readfiledata()
    #dump_cabfolder(cabfile.folders[0], open('folder0.lzx', 'wb'))
    #print('folder0.lzx: uncompressed size: ' + str(cabfile.folders[0].uncompressed_size))
    #print('folder0.lzx: window size: ' + str(cabfile.folders[0].comp_window_size))
    import code
    import rlcompleter
    import readline
    readline.parse_and_bind("tab: complete")
    c = code.InteractiveConsole(globals())
    c.interact()
