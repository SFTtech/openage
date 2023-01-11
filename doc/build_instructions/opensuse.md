# Prerequisite steps for openSUSE users (openSUSE Tumbleweed)

- `zypper install --no-recommends cmake doxygen eigen3-devel fontconfig-devel gcc-c graphviz++ harfbuzz-devel libSDL2-devel libSDL2_image-devel libepoxy-devel libfreetype6 libogg-devel libopus-devel libpng-devel libqt6-qtdeclarative-devel libqt6-qtquickcontrols opusfile-devel python3-Cython python3-Jinja2 python3-lz4 python3-Pillow python3-Pygments python3-toml python3-devel`

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

# Troubleshooting

## CMake Warning: We need a Python interpreter >= 3.9 that is shipped with libpython and header files

Tumbleweed's default Python packages may not contain Python version 3.9. To upgrade to Python 3.9, install the `python39` package and corresponding modules with the following command:

- `zypper install python39 python39-Cython python39-Jinja2 python39-lz4 python39-Pillow python39-Pygments python39-toml python39-devel`
