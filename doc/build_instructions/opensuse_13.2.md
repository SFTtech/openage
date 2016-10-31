# Prerequisite steps for openSUSE users (openSUSE 13.2)

If you already have python3 or one of packman/games repositories then no
need to run the first two lines. If unsure then just run last line and see
if all packages can be installed.

- `zypper addrepo http://download.opensuse.org/repositories/devel:languages:python3/openSUSE_13.2/devel:languages:python3.repo`
- `zypper refresh`
- `zypper install --no-recommends cmake doxygen fontconfig-devel harfbuzz-devel gcc49-c++ graphviz libSDL2-devel libSDL2_image-devel libfreetype6 libepoxy-devel libogg-devel libopus-devel opusfile-devel pkgconfig python3-Cython python3-Pillow python3-Pygments python3-devel libqt5-qtdeclarative-devel libqt5-qtquickcontrols`
