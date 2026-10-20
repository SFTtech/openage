# Prerequisite steps for FreeBSD users

This command should provide required packages for FreeBSD installation:

`sudo pkg install cmake cython eigen3 py-setuptools harfbuzz just ninja opus-tools opusfile png py-mako py-numpy py-lz4 py-pillow py-pygments py-toml python py-uv qt6 toml11`

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

`clang` is the base compiler however, it is possible to use `gcc>=16` or `clang>=21` from `pkg`:
 - `sudo pkg install gcc`

Select the one to be used by `./configure --help`.
