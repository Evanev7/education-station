{
  description = "C FLAKE C FLAKE";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
  };

  outputs = inputs: let
    system = "x86_64-linux";
    pkgs = inputs.nixpkgs.legacyPackages.${system};
  in
    with pkgs; {
      devShells.${system}.default = mkShell {
        buildInputs = [
          raylib
          gcc
          libGL
          xorg.libX11
        ];
        LD_LIBRARY_PATH = "/run/opengl-driver/lib:/run/opengl-driver-32/lib";
      };
    };
}
