# Prerequisite steps for FreeBSD users

This command should provide required packages for FreeBSD installation:

`sudo pkg install python py-pillow py-numpy py-pygments cython harfbuzz cmake sdl2 sdl2_image opusfile opus-tools png pylint qt5`

clang is the base compiler however, it is possible to use either any version of gcc>=4.9 or any other version of clang present in pkg:
 - `sudo pkg install gcc`

Select the one to be used by `./configure`.
