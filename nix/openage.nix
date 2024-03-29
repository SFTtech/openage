{ pkgs
, lib
, stdenv
, fetchFromGitHub
, nyan
, makeWrapper
, python3
, ...
}:
let
  pname = "openage";
  version = "0.5.3";

  # Python libraries needed at build and run time
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

  src = fetchFromGitHub {
    owner = "SFTtech";
    repo = pname;
    rev = "v${version}";
    hash = "sha256-/ag4U7nnZbvkInnLOZ6cB2VWUb+n/HgM1wpo1y/mUHQ=";
  };

  nativeBuildInputs = [
    pkgs.qt6.wrapQtAppsHook
    pkgs.toml11

    makeWrapper
  ];

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
  postInstall = ''
    wrapProgram $out/bin/openage --set PYTHONPATH "${pyEnv}/${pyEnv.sitePackages}:$out/lib/python3.11/site-packages/"
  '';

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
