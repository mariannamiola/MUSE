#!/usr/bin/env bash
set -euo pipefail

# Install the system dependencies required to build MUSE.
# Each package is installed only if it is missing (cmake: any cmake already in PATH is accepted).

# Detect OS
OS="$(uname)"

MISSING=()

if [[ "$OS" == "Darwin" ]]; then
    # macOS (Homebrew)
    if ! command -v brew >/dev/null 2>&1; then
        echo "ERROR: Homebrew not found. Install it from https://brew.sh and re-run this script."
        exit 1
    fi

    PACKAGES=(
        cmake        # build system
        pkgconf      # pkg-config (required by apps/CMakeLists.txt)
        lz4          # FLANN / GeoStatsLib
        armadillo    # GeoStatsLib
        eigen        # linear algebra
        sqlite       # PROJ, muse_compute
        libtiff      # PROJ
        libomp       # OpenMP (apps, FLANN)
        boost        # cinolib
        gnuplot      # Matplot++ (runtime, plots)
    )

    for pkg in "${PACKAGES[@]}"; do
        if [[ "$pkg" == "cmake" ]] && command -v cmake >/dev/null 2>&1; then
            echo "[ok]      cmake ($(command -v cmake))"
        elif brew list --formula "$pkg" >/dev/null 2>&1; then
            echo "[ok]      $pkg"
        else
            echo "[missing] $pkg"
            MISSING+=("$pkg")
        fi
    done

    if [[ ${#MISSING[@]} -gt 0 ]]; then
        brew update
        brew install "${MISSING[@]}"
    fi
else
    # Linux (Debian/Ubuntu, apt)
    PACKAGES=(
        build-essential   # compilers
        cmake             # build system
        pkg-config        # required by apps/CMakeLists.txt
        liblz4-dev        # FLANN / GeoStatsLib
        libarmadillo-dev  # GeoStatsLib
        libeigen3-dev     # linear algebra
        sqlite3           # PROJ (build)
        libsqlite3-dev    # PROJ, muse_compute
        libtiff-dev       # PROJ
        libboost-dev      # cinolib
        gnuplot           # Matplot++ (runtime, plots)
    )

    for pkg in "${PACKAGES[@]}"; do
        if [[ "$pkg" == "cmake" ]] && command -v cmake >/dev/null 2>&1; then
            echo "[ok]      cmake ($(command -v cmake))"
        elif dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q "install ok installed"; then
            echo "[ok]      $pkg"
        else
            echo "[missing] $pkg"
            MISSING+=("$pkg")
        fi
    done

    if [[ ${#MISSING[@]} -gt 0 ]]; then
        sudo apt-get update
        sudo apt-get install -y "${MISSING[@]}"
    fi
fi

if [[ ${#MISSING[@]} -eq 0 ]]; then
    echo "All system dependencies are already installed."
else
    echo "Installed: ${MISSING[*]}"
fi
