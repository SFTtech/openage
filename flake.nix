{
  # This is a nix flake that contains a declarative definition of the openage
  # and nyan packages, providing convenient and reproducible builds and
  # development shells.

  description = "Free (as in freedom) open source clone of the Age of Empires II engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in
      {
        # This output is to build the derivation with `nix build` as well as to
        # get development shells using `nix develop`.
        # These are the packages provided by this flake: nyan and openage.
        packages = rec {
          # `nix build .#nyan` to build this
          nyan = pkgs.callPackage ./nix/nyan.nix { };
          # `nix build .#openage` to build this
          openage = pkgs.callPackage ./nix/openage.nix {
            # Nyan is not provided by nixpkgs, but it comes from this flake
            inherit (self.packages.${system}) nyan;
          };
          # If no path is specified, openage is the default
          default = openage;
        };

        # This output is to run the application directly with `nix run`
        # (or `nix run .#openage` if you want to be explicit)
        apps = rec {
          openage = flake-utils.lib.mkApp { drv = self.packages.${system}.openage; };
          default = openage;
        };
      });
}
