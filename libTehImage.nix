{
    stdenv,
    fetchgit,
}:


stdenv.mkDerivation {
    pname = "libTehImage";
    version = "0.0.1";

    #outputs = [ "out" "dev" ];

    src = fetchgit {
        url = "https://git.tehbox.org/cgit/boss/tehimage.git";
        rev = "v0.0.1";
        hash = "sha256-S3daS6seroaQJDFbcar+RjVXVV3RciHSx9e0iC+i6+0=";
    };

    preInstall = "mkdir -p $out/include";
}
