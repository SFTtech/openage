# Prerequisite steps for Arch Linux users

> NOTE: `openage` is packaged in the [AUR](https://aur.archlinux.org/packages/openage-git/)!

This command should provide required packages for Arch Linux installation:

`sudo pacman -S --needed eigen python python-jinja python-pillow python-numpy python-pygments cython libepoxy libogg libpng ttf-dejavu freetype2 fontconfig harfbuzz cmake sdl2 sdl2_image opusfile opus python-pylint python-toml qt5-declarative qt5-quickcontrols`

If you don't have a compiler installed, you can select between these commands to install it:
 - `sudo pacman -S --needed gcc`
 - `sudo pacman -S --needed clang`

You can install both compilers and select the one to be used by `./configure`.

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.
