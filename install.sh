#!/bin/bash
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default install directory
INSTALL_DIR="${INSTALL_DIR:-/usr/local/bin}"

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build/Release"

echo -e "${GREEN}cpp-cli Installation Script${NC}"
echo "=============================="
echo ""

# Check if running with sudo if needed
check_permissions() {
    if [ ! -w "$INSTALL_DIR" ]; then
        if [ "$EUID" -ne 0 ]; then
            echo -e "${YELLOW}Warning: $INSTALL_DIR is not writable.${NC}"
            echo "Please run with sudo or set INSTALL_DIR to a writable location."
            echo "Example: INSTALL_DIR=~/.local/bin ./install.sh"
            exit 1
        fi
    fi
}

# Ensure conan is installed
ensure_conan() {
    echo -e "${GREEN}Checking Conan installation...${NC}"
    
    if [ ! -d "$SCRIPT_DIR/.venv" ]; then
        echo "Creating virtual environment..."
        python3 -m venv "$SCRIPT_DIR/.venv"
    fi
    
    source "$SCRIPT_DIR/.venv/bin/activate"
    
    if ! command -v conan &> /dev/null; then
        echo "Installing Conan..."
        pip install --upgrade pip
        pip install conan
    fi
    
    echo "Conan version: $(conan --version)"
}

# Build release
build_release() {
    echo ""
    echo -e "${GREEN}Building Release...${NC}"
    
    source "$SCRIPT_DIR/.venv/bin/activate"
    
    # Install conan dependencies
    echo "Installing Conan dependencies..."
    conan install "$SCRIPT_DIR" --output-folder="$BUILD_DIR" --build=missing -s build_type=Release
    
    # Configure and build
    echo "Configuring CMake..."
    cmake --preset conan-release
    
    echo "Building..."
    cmake --build --preset conan-release
}

# Install tools
install_tools() {
    echo ""
    echo -e "${GREEN}Installing tools to $INSTALL_DIR...${NC}"
    
    check_permissions
    
    # Create install directory if it doesn't exist
    mkdir -p "$INSTALL_DIR"
    
    # Find and install all tool executables
    TOOLS_DIR="$SCRIPT_DIR/src/tools"
    INSTALLED=0
    
    for tool_dir in "$TOOLS_DIR"/*/; do
        if [ -d "$tool_dir" ]; then
            tool_name=$(basename "$tool_dir")
            tool_exe="$BUILD_DIR/src/tools/$tool_name/$tool_name"
            
            if [ -f "$tool_exe" ]; then
                echo "  Installing $tool_name..."
                cp "$tool_exe" "$INSTALL_DIR/"
                chmod +x "$INSTALL_DIR/$tool_name"
                ((INSTALLED++))
            else
                echo -e "${YELLOW}  Warning: $tool_name not found at $tool_exe${NC}"
            fi
        fi
    done
    
    echo ""
    if [ $INSTALLED -gt 0 ]; then
        echo -e "${GREEN}Successfully installed $INSTALLED tool(s) to $INSTALL_DIR${NC}"
    else
        echo -e "${RED}No tools were installed. Please run 'Build Debug All Tools' task first.${NC}"
        exit 1
    fi
}

# List installed tools
list_tools() {
    echo ""
    echo "Installed tools:"
    for tool_dir in "$SCRIPT_DIR/src/tools"/*/; do
        if [ -d "$tool_dir" ]; then
            tool_name=$(basename "$tool_dir")
            if [ -f "$INSTALL_DIR/$tool_name" ]; then
                echo -e "  ${GREEN}✓${NC} $tool_name"
            fi
        fi
    done
}

# Main
main() {
    case "${1:-install}" in
        install)
            ensure_conan
            build_release
            install_tools
            list_tools
            ;;
        build)
            ensure_conan
            build_release
            ;;
        list)
            list_tools
            ;;
        *)
            echo "Usage: $0 [install|build|list]"
            echo ""
            echo "Commands:"
            echo "  install  Build and install tools (default)"
            echo "  build    Build release only"
            echo "  list     List installed tools"
            echo ""
            echo "Environment variables:"
            echo "  INSTALL_DIR  Installation directory (default: /usr/local/bin)"
            exit 1
            ;;
    esac
}

main "$@"

