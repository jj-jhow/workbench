#!/usr/bin/env bash
# Build OpenUSD WASM bindings using Emscripten.
#
# All paths are configurable via environment variables. Defaults assume the
# dependencies live next to the workbench repo root. Override them or create
# a local env file at scripts/build-wasm.env to match your setup:
#
#   OPENUSD_PREFIX  – wasm64 OpenUSD install     (default: ../../openusd_wasm64)
#   TBB_PREFIX      – wasm64 oneTBB install      (default: ../../tbb_wasm64)
#   EMSDK           – Emscripten SDK root         (optional, sourced if set)
#   BUILD_TYPE      – "debug" for -O0 -g, else -O2
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
REPO_ROOT="$(cd "${PROJECT_DIR}/../../.." && pwd)"

# Source an optional local env file for overrides
if [[ -f "${SCRIPT_DIR}/build-wasm.env" ]]; then
    # shellcheck source=/dev/null
    source "${SCRIPT_DIR}/build-wasm.env"
fi

# Resolve dependency prefixes with sensible defaults
OPENUSD_PREFIX="${OPENUSD_PREFIX:-${REPO_ROOT}/../openusd_wasm64}"
TBB_PREFIX="${TBB_PREFIX:-${REPO_ROOT}/../tbb_wasm64}"

# Activate Emscripten SDK if EMSDK is set and em++ isn't already on PATH
if ! command -v em++ &>/dev/null; then
    if [[ -n "${EMSDK:-}" && -f "${EMSDK}/emsdk_env.sh" ]]; then
        source "${EMSDK}/emsdk_env.sh"
    else
        echo "Error: em++ not found. Either activate the Emscripten SDK or set EMSDK." >&2
        exit 1
    fi
fi

# Validate that the dependency dirs exist
for dir_var in OPENUSD_PREFIX TBB_PREFIX; do
    dir="${!dir_var}"
    if [[ ! -d "$dir" ]]; then
        echo "Error: ${dir_var} directory not found: ${dir}" >&2
        echo "       Set ${dir_var} to the correct path or create scripts/build-wasm.env" >&2
        exit 1
    fi
done

# Resolve to absolute paths for the compiler
OPENUSD_PREFIX="$(cd "$OPENUSD_PREFIX" && pwd)"
TBB_PREFIX="$(cd "$TBB_PREFIX" && pwd)"

# Choose optimisation level
if [[ "${BUILD_TYPE:-release}" == "debug" ]]; then
    OPT_FLAGS="-O0 -g"
else
    OPT_FLAGS="-O2"
fi

echo "=== build-wasm ==="
echo "  OPENUSD_PREFIX : ${OPENUSD_PREFIX}"
echo "  TBB_PREFIX     : ${TBB_PREFIX}"
echo "  em++           : $(command -v em++)"
echo "  Output         : ${PROJECT_DIR}/public/openusd.js"
echo ""

em++ ${OPT_FLAGS} \
    -s WASM=1 -sMEMORY64=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="OpenUSDModule" \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    --use-port=zlib \
    -pthread \
    -I"${OPENUSD_PREFIX}/include" \
    -I"${TBB_PREFIX}/include" \
    -L"${OPENUSD_PREFIX}/lib" \
    -L"${TBB_PREFIX}/lib" \
    -lusd_usd -lusd_usdGeom -lusd_usdShade -lusd_sdf -lusd_tf \
    -lusd_arch -lusd_vt -lusd_gf -lusd_ar -lusd_pcp -lusd_plug \
    -lusd_work -lusd_trace -lusd_js -lusd_kind -lusd_sdr -lusd_ts \
    -ltbb \
    --bind \
    -o "${PROJECT_DIR}/public/openusd.js" \
    "${PROJECT_DIR}/src/wasm/usd_bindings.cpp"

echo "Build complete: public/openusd.js + public/openusd.wasm"
