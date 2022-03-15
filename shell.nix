{ pkgs? import <nixpkgs> {} }:
pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.gcc
    pkgs.clang
    #pkgs.glibc
    #pkgs.libcxx
    #pkgs.lld
    #pkgs.gdb
    pkgs.cmake
    pkgs.gnumake
    pkgs.qt5.full
    #pkgs.qtcreator

    pkgs.eigen
    pkgs.python39
    pkgs.python39Packages.jinja2
    pkgs.python39Packages.pillow
    pkgs.python39Packages.numpy
    pkgs.python39Packages.lz4
    pkgs.python39Packages.pygments
    pkgs.python39Packages.cython
    pkgs.libepoxy
    pkgs.libogg
    pkgs.libpng
    pkgs.dejavu_fonts
    pkgs.ftgl
    pkgs.fontconfig
    pkgs.harfbuzz
    pkgs.SDL2
    pkgs.SDL2_image
    pkgs.opusfile
    pkgs.libopus
    pkgs.python39Packages.pylint
    pkgs.python39Packages.toml
    pkgs.libsForQt5.qt5.qtdeclarative
    pkgs.libsForQt5.qt5.qtquickcontrols
  ];
}
