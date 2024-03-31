{ lib
, stdenv
, fetchFromGitHub
, clang
, cmake
, flex
}:
let
  pname = "nyan";
  version = "0.3";
in
stdenv.mkDerivation
{
  inherit pname version;

  src = fetchFromGitHub {
    owner = "SFTtech";
    repo = pname;
    rev = "v${version}";
    hash = "sha256-bjz4aSS5RO+QuLd7kyblTPNcuQFhYK7sW1csNXHL4Qs=";
  };

  nativeBuildInputs = [
    clang
    cmake
  ];

  buildInputs = [
    flex
  ];

  meta = with lib; {
    description = "A data description language";
    longDescription = ''
      Nyan stores hierarchical objects with key-value pairs in a database with the key idea that changes in a parent affect all children. We created nyan because there existed no suitable language to properly represent the enormous complexity of storing the data for openage.
    '';
    homepage = "https://openage.sft.mx";
    license = licenses.lgpl3Plus;
    platforms = platforms.unix;
    mainProgram = "nyancat";
  };
}


