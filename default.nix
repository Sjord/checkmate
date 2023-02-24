# To run this on nix/nixos, just run `nix-build` (or `nix build`) in the directory containing this file
# and then run any executable in the result/bin directory,
# e.g. `./result/bin/mpck`.
# Or, if there is a corresponding flake.nix file and you have "flakes" enabled,
# you can just run "nix run" and it will run the default executable ("mpck").
# If you want to pass arguments to it or run a different executable, run it these ways:
# nix run .#mpck -- arg  # put the arguments to be passed to the executable after the --

{ pkgs ? import <nixpkgs> {}}:
with pkgs;
# fastStdenv.mkDerivation { # for faster running times (8-12%) BUT... nondeterministic builds :(
fastStdenv.mkDerivation {
  name = "checkmate";
  src = ./.;
  
  enableParallelBuilding = true;

  # any dependencies/build tools needed at compilation/build time here
  nativeBuildInputs = [ pkg-config gcc clang autoconf automake libtool ];

  # any runtime dependencies here
  buildInputs = [ ];

  # the bash shell steps to build it
  buildPhase = ''
    ./autogen.sh
    ./configure
    make
  '';

  # for a generic copy of all compiled executables:
  # cp $(find * -maxdepth 1 -executable -type f) $out/bin/
  # to copy specific build outputs:
  # cp keyconv oclvanitygen++ oclvanityminer vanitygen++ $out/bin/
  installPhase = ''
    mkdir -p $out/bin
    cp mpck/mpck $out/bin/
  '';
}
