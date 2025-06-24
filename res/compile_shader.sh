#!/bin/bash

# Exit on error
set -e

# Define paths (relative to script's location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_SRC="$SCRIPT_DIR/shaders"
SHADER_OUT="$SCRIPT_DIR/shaders"

# List of shader base names
SHADERS=("Flat" "Solid" "Text" "Texture" "Sprite")

# Ensure output directory exists
if [ ! -d "$SHADER_OUT" ]; then
    echo "Error: Output directory '$SHADER_OUT' does not exist."
    exit 1
fi

# Loop over each shader name and compile
for S in "${SHADERS[@]}"; do
    VERT_SRC="$SHADER_SRC/$S.vert.glsl"
    VERT_OUT="$SHADER_OUT/$S.vert.spv"
    FRAG_SRC="$SHADER_SRC/$S.frag.glsl"
    FRAG_OUT="$SHADER_OUT/$S.frag.spv"

    echo "Compiling $VERT_SRC ..."
    echo "  > glslc -fshader-stage=vert \"$VERT_SRC\" -g -o \"$VERT_OUT\""
    glslc -fshader-stage=vert "$VERT_SRC" -g -o "$VERT_OUT"

    echo "Compiling $FRAG_SRC ..."
    echo "  > glslc -fshader-stage=frag \"$FRAG_SRC\" -g -o \"$FRAG_OUT\""
    glslc -fshader-stage=frag "$FRAG_SRC" -g -o "$FRAG_OUT"
done

echo
echo "Shader compilation complete."
