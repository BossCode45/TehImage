{
    description = "default c++ flake";
    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
        flake-utils.url = "github:numtide/flake-utils";
    };
    outputs = { self, nixpkgs, ... }@inputs:
        inputs.flake-utils.lib.eachDefaultSystem (system: let
            pkgs = nixpkgs.legacyPackages.${system};
        in {
            packages.libTehImage = (pkgs.callPackage ./libTehImage.nix {} );
            packages.default = self.packages.${system}.libTehImage;
            devShells.default = pkgs.mkShell {
                nativeBuildInputs = [
                    pkgs.gcc
                    pkgs.gnumake
                    pkgs.clang-tools
                ];
            };
        });
}
