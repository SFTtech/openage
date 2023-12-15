# Prerequisite steps for Arch Linux users

> NOTE: `openage` is packaged in the [AUR](https://aur.archlinux.org/packages/openage-git/)!

This command should provide required packages from the Arch Linux repositories:

`sudo pacman -S --needed eigen python python-mako python-pillow python-numpy python-lz4 python-pygments cython libepoxy libogg libpng ttf-dejavu freetype2 fontconfig harfbuzz cmake opusfile opus python-pylint python-toml qt6-declarative qt6-multimedia`

Additionally, you have to install [`toml11`](https://aur.archlinux.org/packages/toml11) from the AUR.
If you have `yay`, you can run this command:

`yay -S toml11`

If you don't have a compiler installed, you can select between these commands to install it:
 - `sudo pacman -S --needed gcc`
 - `sudo pacman -S --needed clang`

You can install both compilers and select the one to be used by `./configure`.

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.
