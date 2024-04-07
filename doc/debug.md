# How to debug openage?

## Your favorite IDE

Check the docs in [the `/doc/ide` subfolder](/doc/ide/).

## GDB
GDB can be used to debug C++ code in a terminal.

To being able to debug with GDB use `./configure` script such as:
```
./configure --mode=debug --compiler=gcc
```
build the game
```
make
```
Then
```
gdb -ex 'set breakpoint pending on' -ex 'b openage::run_game' -ex run --args run game
```
The game will be paused at the start of the function run_game() located in `libopenage/main.cpp`

**Note:** The `run` executable is a compiled version of `run.py` that also embeds the interpreter.
The game is intended to be run by `run.py` but it is much easier to debug the `./run` file

### Pretty Printers

Enabling pretty printing will make GDB's output much more readable, so we always recommend
to configure it in your setup. Your [favourite IDE](/doc/ide/) probably an option to enable pretty printers
for the standard library types. If not, you can get them from the [gcc repository](https://github.com/gcc-mirror/gcc/tree/master/libstdc%2B%2B-v3/python/libstdcxx) and register them in your local `.gdbinit` file.

Additionally, we have created several custom GDB pretty printers for types used in `libopenage`,
the C++ library that contains the openage engine core. To enable them, you have to load the project's
own init file [openage.gdbinit](/etc/openage.gdbinit) when running GDB:

```gdb
(gdb) source <path-of-openage-dir>/etc/openage.gdbinit
```

Your IDE may be able to do this automatically for a debug run. Alternatively, you can configure
an [auto-loader](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Python-Auto_002dloading.html#Python-Auto_002dloading)
that loads the scripts for you.


### GDBGUI

[gdbgui](https://github.com/cs01/gdbgui) is a browser-based frontend for GDB.

To install gdbgui in Ubuntu:

```
sudo pip3 install gdbgui --upgrade
```
Then
```
gdbgui ./run
```
The gdbgui web page will be at `http://127.0.0.1:5000`
Use the command `run` in the GDB prompt to start debugging
