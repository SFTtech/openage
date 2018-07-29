# Prerequisite steps for FreeBSD users

This command should provide required packages for FreeBSD installation:

`sudo pkg install cmake cython eigen3 harfbuzz opus-tools opusfile png py-numpy py-pillow py-pygments pylint python qt5 sdl2 sdl2_image`

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

`clang` is the base compiler however, it is possible to use either any version of `gcc>=7` or any other version of `clang` present in `pkg`:
 - `sudo pkg install gcc`

Select the one to be used by `./configure --help`.
