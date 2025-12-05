#!/bin/bash
set -e

INSTALL_DIR="${INSTALL_DIR:-/usr/local/bin}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check for Release build only
if [ ! -d "$SCRIPT_DIR/build/Release" ]; then
    echo "No Release build directory found. Build the project in Release mode first:"
    echo "  conan install . --build=missing -s build_type=Release"
    echo "  conan build . -s build_type=Release"
    exit 1
fi

BUILD_DIR="$SCRIPT_DIR/build/Release"

# Check write permissions
if [ ! -w "$INSTALL_DIR" ]; then
    echo "$INSTALL_DIR is not writable. Run with sudo or set INSTALL_DIR."
    exit 1
fi

mkdir -p "$INSTALL_DIR"

INSTALLED=0
for tool_dir in "$SCRIPT_DIR/src/tools"/*/; do
    tool_name=$(basename "$tool_dir")
    tool_exe="$BUILD_DIR/src/tools/$tool_name/$tool_name"
    
    if [ -f "$tool_exe" ]; then
        echo "Installing $tool_name..."
        cp "$tool_exe" "$INSTALL_DIR/"
        chmod +x "$INSTALL_DIR/$tool_name"
        INSTALLED=$((INSTALLED + 1))
    fi
done

if [ $INSTALLED -gt 0 ]; then
    echo "Installed $INSTALLED tool(s) to $INSTALL_DIR"
else
    echo "No tools found to install."
    exit 1
fi
