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

#### Note:
The `openage` executable is a compiled version of `openage.py` that also embeds the interpreter.
The game is intended to be run by `openage.py` but it is much easier to debug the `./openage` file

### GDBGUI

[gdbgui](https://github.com/cs01/gdbgui) is a browser-based frontend for GDB.

To install gdbgui in Ubuntu:

```
sudo pip3 install gdbgui --upgrade
```
Then
```
gdbgui ./openage
```
The gdbgui web page will be at `http://127.0.0.1:5000`
Use the command `openage` in the GDB prompt to start debugging
