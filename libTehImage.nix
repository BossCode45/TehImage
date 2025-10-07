{
    stdenv,
    fetchgit,
    pkgs
}:


stdenv.mkDerivation {
    pname = "libTehImage";
    version = "0.0.2";

    #outputs = [ "out" "dev" ];

    buildInputs = [
        pkgs.gcc
        pkgs.gnumake
    ];

    src = fetchgit {
        url = "https://git.tehbox.org/boss/tehimage.git";
        rev = "v0.0.2";
        hash = "sha256-GrCtLFFa3wKHzXuqziobWz4w0tWzbiShkbXS0Obs+rw=";
    };

    preInstall = "mkdir -p $out/include";
}
