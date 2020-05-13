with import <nixpkgs> {};
mkShell {
    buildInputs = [
        clang_10
        glibcLocales
        linuxPackages.perf
        python3
        shellcheck
        valgrind
    ];
    shellHook = ''
        . .shellhook
    '';
}
