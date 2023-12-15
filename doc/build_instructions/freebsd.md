# Prerequisite steps for FreeBSD users

This command should provide required packages for FreeBSD installation:

`sudo pkg install cmake cython eigen3 harfbuzz opus-tools opusfile png py-mako py-numpy py-lz4 py-pillow py-pygments py-toml pylint python qt6 toml11`

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

`clang` is the base compiler however, it is possible to use either any version of `gcc>=10` or any other version of `clang` present in `pkg`:
 - `sudo pkg install gcc`

Select the one to be used by `./configure --help`.
