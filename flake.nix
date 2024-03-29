{
  description = "Free (as in freedom) open source clone of the Age of Empires II engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; }; in
      {
        packages = {
          nyan = pkgs.callPackage ./nix/nyan.nix { };
          openage = pkgs.callPackage ./nix/openage.nix {
            inherit (self.packages.${system}) nyan;
          };
        };
        apps = rec {
          openage = flake-utils.lib.mkApp { drv = self.packages.${system}.openage; };
          default = openage;
        };
      });
}
