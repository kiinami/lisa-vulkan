# Settings ####################################################################

set dotenv-load

set windows-shell := ["powershell.exe", "-NoProfile", "-NoLogo", "-File", "./vsdev-shell.ps1", "-Command"]

# Public recipes ##############################################################

[arg("mode", long="release", short="r", value="release")]
deps mode="debug":
    @{{ if os() == "windows" { "just _deps-windows " + mode } else { "just _deps-linux " + mode } }}

[arg("mode", long="release", short="r", value="release")]
build mode="debug":
    @{{ if os() == "windows" { "just _build-windows " + mode } else { "just _build-linux " + mode } }}

[arg("mode", long="release", short="r", value="release")]
run mode="debug" *args="":
    @{{ if os() == "windows" { "just _run-windows " + mode + " " + args } else { "just _run-linux " + mode + " " + args } }}

clean:
    @{{ if os() == "windows" { "just _clean-windows" } else { "just _clean-linux" } }}

# Windows recipes #############################################################

_deps-windows mode:
        conan install . \
            --build=missing \
            --profile=conan/profiles/{{ if mode == "release" { "release" } else { "debug" } }}-windows

_build-windows mode:
    cmake -S . \
        --preset {{ if mode == "release" { "release-configure" } else { "debug-configure" } }}
    cmake --build build/{{ if mode == "release" { "Release" } else { "Debug" } }} \
        --config {{ if mode == "release" { "Release" } else { "Debug" } }}

_run-windows mode *args="":
    & 'build/{{ if mode == "release" { "Release" } else { "Debug" } }}/lisa.exe' {{ args }}

_clean-windows:
    powershell -Command "Remove-Item -Recurse -Force -ErrorAction SilentlyContinue build"

# Linux recipes ###############################################################

_deps-linux mode:
    conan install . \
        --build=missing \
        --profile=conan/profiles/{{ if mode == "release" { "release" } else { "debug" } }}

_build-linux mode:
    cmake -S . \
        --preset {{ if mode == "release" { "release-configure" } else { "debug-configure" } }}
    cmake --build build/{{ if mode == "release" { "Release" } else { "Debug" } }} \
        --config {{ if mode == "release" { "Release" } else { "Debug" } }}

_run-linux mode *args="":
    build/{{ if mode == "release" { "Release" } else { "Debug" } }}/lisa {{ args }}

_clean-linux:
    rm -rf build
