# This file contains the definition of a nix expression that builds openage.
# A similar one exists from nyan, the configuration language.
#
# The expression in this file is a function (to be called by callPackage)
# that returns a derivation. callPackage is used inside the flake.nix file.
# 
# To be compliant with nixpkgs, the dependencies can be passed via function
# arguments, similarly to how python3 or nyan is used here. The available
# packages that can be used when building/developing can be found among nixpkgs
# (https://search.nixos.org/) -as done with python3- or passed from the flake
# as done with nyan in the flake.nix file.
{ pkgs
, lib
, stdenv
, fetchFromGitHub
, nyan
, python3
, ...
}:
let
  pname = "openage";
  version = "0.5.3";

  # Python libraries needed at build and run time. This function creates a
  # python3 package configured with some packages (similar to a python venv).
  # The current python3 version is used, which is python3.11 at the moment of
  # writing.
  # If more packages are needed, they can be added here. The available packages
  # are listed at
  # https://search.nixos.org/packages?channel=unstable&query=python311Packages
  # For example, to add python311Packages.pybind11 to the dependencies, we could
  # add to this list ps.pybind11
  pyEnv = python3.withPackages (ps: [
    ps.mako
    ps.pillow
    ps.numpy
    ps.lz4
    ps.pygments
    ps.cython
    ps.pylint
    ps.toml
  ]);
in
stdenv.mkDerivation {
  inherit pname version;

  # This uses the current checked-out repository as source code
  src = ../.;

  # This fetches the code on github: one would use this when submitting the
  # package to nixpkgs.
  # src = fetchFromGitHub {
  #   owner = "SFTtech";
  #   repo = pname;
  #   rev = "v${version}";
  #   hash = "sha256-/ag4U7nnZbvkInnLOZ6cB2VWUb+n/HgM1wpo1y/mUHQ=";
  # };

  # Dependencies that are used only at build time
  nativeBuildInputs = [
    # This is needed for qt applications
    pkgs.qt6.wrapQtAppsHook
    pkgs.toml11
  ];

  # Dependencies that are used at build and run time
  buildInputs = with pkgs; [
    nyan
    pyEnv

    gcc
    clang
    cmake
    gnumake
    qt6.full

    eigen
    libepoxy
    libogg
    libpng
    dejavu_fonts
    ftgl
    fontconfig
    harfbuzz
    opusfile
    libopus
    qt6.qtdeclarative
    qt6.qtmultimedia
  ];

  # openage requires access to both python dependencies and openage bindings
  # Since nix places the binary somewhere in the nix store (/nix/store/blah),
  # it needs to know where to find the python environment build above. This
  # is done by setting the PYTHONPATH: the first part contains the path to 
  # the environment, the second part contains the path to the openage library
  # build with this code.
  postInstall = ''
    wrapProgram $out/bin/openage --set PYTHONPATH "${pyEnv}/${pyEnv.sitePackages}:$out/lib/python3.11/site-packages/"
  '';

  # Metadata
  meta = with lib; {
    description = "Free (as in freedom) open source clone of the Age of Empires II engine";
    longDescription = ''
      openage: a volunteer project to create a free engine clone of the Genie Engine used by Age of Empires, Age of Empires II (HD) and Star Wars: Galactic Battlegrounds, comparable to projects like OpenMW, OpenRA, OpenSAGE, OpenTTD and OpenRCT2.

      openage uses the original game assets (such as sounds and graphics), but (for obvious reasons) doesn't ship them
    '';
    homepage = "https://openage.sft.mx";
    license = licenses.lgpl3Plus;
    platforms = platforms.unix;
  };
}
