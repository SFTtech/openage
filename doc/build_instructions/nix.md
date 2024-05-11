# Building and developing on Nix based systems

The openage repository is a [nix flake](https://wiki.nixos.org/wiki/Flakes) that
allow Nix users to easily build, install and develop openage.

To build openage using nix

1. make sure you have nix with flakes enabled, either by permanent system
   configuration or by using command line flags, as described on
   [the wiki](https://wiki.nixos.org/wiki/Flakes);
2. clone this repository and `cd` into it;
3. run `nix build .#openage` to start the build process;
4. the built artifact will be in `./result`.

Nix will configure and build the source code automatically via the
`configurePhase` and `buildPhase` scripts, automatically provided by nix.

If you want to build the derivation and run it immediately, you can use

```
nix run .#openage
```

instead of

```
nix build .#openage
./result/bin/openage
```

## Development

You can get a development shell, with the required dependencies, by running

```
nix shell
```

This will download the same dependencies used for build and pop you in a
shell ready for use. You can call `configurePhase` and `buildPhase` to run
cmake configuration and building.

Please note that nyan is downloaded from github as defined in `nix/nyan.nix`,
so if you want to provide nyan source tree, you'll need to modify that file
to use a path (such as `../../nyan`, relative to `nyan.nix`) instead of the
`fetchFromGitHub` function, for example:

```
# Clone both
git clone https://github.com/SFTtech/nyan
git clone https://github.com/SFTtech/openage
```

Then edit `openage/nix/nyan.nix` to use `../../nyan`, the result will be like

```
$ head -n18 openage/nix/nyan.nix

```
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

  src = ../../nyan;

  nativeBuildInputs = [
```
