# Prerequisite steps for Arch Linux users

> NOTE: [aur](https://aur.archlinux.org/packages/openage-git/) exists, to install via **yaourt** run `yaourt -S openage-git`.

This command should provide required packages for Arch Linux installation:

`sudo pacman -S --needed python python-jinja2 python-pillow python-numpy python-pygments cython libepoxy libogg libpng ttf-dejavu freetype2 fontconfig harfbuzz cmake sdl2 sdl2_image opusfile opus python-pylint qt5-declarative qt5-quickcontrols`

If you don't have a compiler installed, you can select between these commands to install it:
 - `sudo pacman -S --needed gcc`
 - `sudo pacman -S --needed clang`

You can install both compilers and select the one to be used by `./configure`.
