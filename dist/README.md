# Distribution

Contains files useful for package distribution.

## Desktop file

The [openage.desktop](openage.desktop) file can be found in this directory and will be installed at
`/usr/share/applications/openage.desktop`.

## Desktop icon

In lieu of a logo/icon for openage a cropped version of [assets/gaben.png](/assets/gaben.png) is
created instead. See [assets/CMakeLists.txt](/assets/CMakeLists.txt) for details on how the icon
is generated. The final icon will be installed at `/usr/share/pixmaps/openage.png` via
`make install`.
