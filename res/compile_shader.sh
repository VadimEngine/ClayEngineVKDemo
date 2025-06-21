#!/bin/bash

# Exit on error
set -e

# Define paths (relative to script's location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_SRC="$SCRIPT_DIR/shaders"
SHADER_OUT="$SCRIPT_DIR/shaders"

# List of shader base names
SHADERS=("Flat" "Solid" "Text" "Texture")

# Create output directory if it doesn't exist
mkdir -p "$SHADER_OUT"

# Loop over each shader name and compile
for S in "${SHADERS[@]}"; do
    echo "Compiling $S.vert.glsl ..."
    glslc -fshader-stage=vert "$SHADER_SRC/$S.vert.glsl" -g -o "$SHADER_OUT/$S.vert.spv"

    echo "Compiling $S.frag.glsl ..."
    glslc -fshader-stage=frag "$SHADER_SRC/$S.frag.glsl" -g -o "$SHADER_OUT/$S.frag.spv"
done

echo
echo "Shader compilation complete."
