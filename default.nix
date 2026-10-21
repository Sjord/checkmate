{
  pkgs ? import <nixpkgs> { },
}:

let
  inherit (pkgs) lib stdenv autoreconfHook;
in
stdenv.mkDerivation {
  pname = "checkmate";
  version = "0.21";

  # The repository does not ship a generated configure script, so
  # autoreconfHook runs autoconf/automake before the configure phase.
  src = lib.cleanSourceWith {
    src = ./.;
    filter =
      path: _type:
      let
        name = baseNameOf path;
      in
      name != ".git"
      && name != "result"
      && !(lib.hasSuffix ".mp3" name);
  };

  nativeBuildInputs = [ autoreconfHook ];

  enableParallelBuilding = true;
  doCheck = true;

  meta = with lib; {
    description = "Check MP3 files for errors";
    homepage = "https://github.com/Sjord/checkmate";
    license = licenses.gpl2Plus;
    platforms = platforms.unix;
    mainProgram = "mpck";
  };
}
