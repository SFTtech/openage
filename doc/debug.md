# Debugging

## Buildsystem

### Printing Variables
In general you can use `message(STATUS "MY_VARIABLE=${MY_VARIABLE}")` or output status messages
now and then.

This can be more easily achieved with a built-in module:
```
include(CMakePrintHelpers)
cmake_print_variables(MY_VARIABLE)
```

For printing the properties of targets you use:
```
include(CMakePrintHelpers)
cmake_print_properties(
        TARGETS "${TARGETNAME}"
        PROPERTIES
            OUTPUT_NAME
            COMPILE_OPTIONS
            COMPILE_FLAGS
            LINK_OPTIONS
)
``` 

__NOTE:__ An overview over the target properties that can be printed you find [here](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#target-properties).


### Tracing
But sometimes you need to debug the workflow in a whole `*.cmake` or `CMakeLists.txt` file,
for example if you want to have a look what CMake is doing while setting up the build directory.

For this you can add `cmake --trace-source=<path to cmake file or cmakelists you want to trace>` to the 
initial CMake invocation. Adding `--trace-expand` to that expands all the variables to their values.

### Python scripts
Some parts of our buildsystem contain python scripts. You should always be able to debug them more easy
with your favourite Python debugging tool (e.g. [PyDev](https://www.pydev.org/)).

## Buildprocess

### Increase verbosity
You can pass flags that increase the verbosity of processes during build:


`make <your parameters> VERBOSE=1`, which is the same to `cmake <your parameters> -- <make parameters> VERBOSE=1` 
you can also set it by putting `CMAKE_VERBOSE_MAKEFILE on` in your CMakeLists.txt.

### 

### Linking

Depending on the linker in use you can increase the verbosity, the following:

It's good to know what the syntax is that is in use for options that will be passed to the linker tool.
You can read that up [here](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_LINKER_WRAPPER_FLAG.html).
Best is to use `LINKER:` prefix for portability.

For Clang you increase the verbosity of :

`add_link_options("LINKER:-v -v") => -Xlinker -v -v`

The first `-v` goes to the Linker due to `-Xlinker`, the second `-v` goes to Clang to increase the verbosity.


For GNU Gcc:
```
`add_link_options("LINKER:-v -v") => -Wl,-v -v`
```


## Engine
### Qt Creator IDE
See [ide.md](/doc/ide.md)
### GDB
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

##### Note:
The `run` executable is a compiled version of `run.py` that also embeds the interpreter.
The game is intended to be run by `run.py` but it is much easier to debug the `./run` file

#### GDBGUI

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

### lldb
TODO