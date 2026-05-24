{
    stdenv,
    fetchgit,
    pkgs
}:


stdenv.mkDerivation {
    pname = "libTehImage";
    version = "0.0.3";

    #outputs = [ "out" "dev" ];

    buildInputs = [
        pkgs.gcc
        pkgs.gnumake
    ];

    src = fetchgit {
        url = "https://git.tehbox.org/boss/tehimage.git";
        rev = "v0.0.3";
        hash = "sha256-ujEVV0uIH67nl7SQcYdHc0BFyiUoIpvvyo+Qp3mF5lw=";
    };

    preInstall = "mkdir -p $out/include";
}
