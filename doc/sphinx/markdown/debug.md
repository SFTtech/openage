# How to debug openage?
## Qt Creator IDE
See [ide.md](/doc/ide.md)
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
The `run` executable is a compiled version of `run.py` that also embeds the interpreter.
The game is intended to be run by `run.py` but it is much easier to debug the `./run` file

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
