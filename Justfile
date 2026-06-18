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

[arg("all", long="all", short="a", value="true")]
[arg("mode", long="release", short="r", value="release")]
format mode="debug" all="false":
    @{{ if os() == "windows" { "just _format-windows " + mode + " " + all } else { "just _format-linux " + mode + " " + all } }}

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

_lint-windows mode all:
    @$all_flag = "{{ all }}"; \
    if ($all_flag -eq "true") { \
        $files = git ls-files | Where-Object { $_ -match '\.(cpp|hpp|h|cc|cxx)$' } \
    } else { \
        $changed = @(git diff --diff-filter=d --name-only) + @(git diff --cached --diff-filter=d --name-only) + @(git ls-files --others --exclude-standard) | Select-Object -Unique; \
        $files = $changed | Where-Object { $_ -match '\.(cpp|hpp|h|cc|cxx)$' } \
    }; \
    if ($files) { \
        Write-Host "Formatting C++ files..."; \
        $files | ForEach-Object { clang-format -i $_ }; \
        Write-Host "Running clang-tidy..."; \
        $src_files = $files | Where-Object { $_ -match '\.(cpp|cc|cxx)$' }; \
        if ($src_files) { \
            clang-tidy -p build/{{ if mode == "release" { "Release" } else { "Debug" } }} --quiet $src_files \
        } else { \
            Write-Host "No source files to lint with clang-tidy." \
        } \
    } else { \
        Write-Host "No files to format." \
    }

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

_format-linux mode all:
    #!/usr/bin/env bash
    set -euo pipefail
    if [ "{{ all }}" = "true" ]; then
        files=$(git ls-files | grep -E "\.(cpp|hpp|h|cc|cxx)$" || true)
    else
        changed=$( (git diff --diff-filter=d --name-only; git diff --cached --diff-filter=d --name-only; git ls-files --others --exclude-standard) | sort -u )
        files=$(echo "$changed" | grep -E "\.(cpp|hpp|h|cc|cxx)$" || true)
    fi
    if [ -n "$files" ]; then
        echo "Formatting C++ files..."
        echo "$files" | xargs -r clang-format -i
        echo "Running clang-tidy..."
        src_files=$(echo "$files" | grep -E "\.(cpp|cc|cxx)$" || true)
        if [ -n "$src_files" ]; then
            echo "$src_files" | xargs -r clang-tidy -p build/{{ if mode == "release" { "Release" } else { "Debug" } }} --quiet
        else
            echo "No source files to lint with clang-tidy."
        fi
    else
        echo "No files to format."
    fi
