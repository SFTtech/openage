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
    pkgs.qt6.full
    #pkgs.qtcreator

    pkgs.eigen
    pkgs.python39
    pkgs.python39Packages.mako
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
    pkgs.opusfile
    pkgs.libopus
    pkgs.python39Packages.pylint
    pkgs.python39Packages.toml
    pkgs.libsForQt6.qt6.qtdeclarative
    pkgs.libsForQt6.qt6.qtquickcontrols
    pkgs.libsForQt6.qt6.qtmultimedia
  ];
}
