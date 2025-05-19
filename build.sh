#!/bin/bash

# Flags
should_run=false
should_clean=false
run_latest=false

# Parse flags
for arg in "$@"; do
    case "$arg" in
        --run|-r)
            should_run=true
            ;;
        --clean|-c)
            should_clean=true
            ;;
        cdrun)
            run_latest=true
            ;;
    esac
done

# Handle running latest version
if [ "$run_latest" = true ]; then
    echo "🔍 Looking for latest version..."
    
    # Check if build directory exists
    if [ ! -d "build" ]; then
        echo "❌ No build directory found. Please build first."
        exit 1
    fi
    
    # Get the latest version
    latest_version=$(ls build 2>/dev/null | grep -E '^v[0-9]+\.[0-9]+$' | sort -V | tail -n 1)
    
    if [ -z "$latest_version" ]; then
        echo "❌ No versions found in build directory."
        exit 1
    fi
    
    latest_executable="build/$latest_version/main"
    
    if [ ! -f "$latest_executable" ]; then
        echo "❌ Executable not found for version $latest_version"
        exit 1
    fi
    
    echo "🚀 Running latest version ($latest_version)..."
    "$latest_executable"
    exit 0
fi

# Handle cleaning
if [ "$should_clean" = true ]; then
    echo "🧹 Cleaning build directory..."
    rm -rf build
    echo "✅ Build directory cleaned."
    exit 0
fi

# Ensure the build directory exists
mkdir -p build

# Get the latest version
latest_version=$(ls build 2>/dev/null | grep -E '^v[0-9]+\.[0-9]+$' | sort -V | tail -n 1)

# Generate a suggestion for the next version
if [[ $latest_version =~ ^v([0-9]+)\.([0-9]+)$ ]]; then
    major="${BASH_REMATCH[1]}"
    minor="${BASH_REMATCH[2]}"
    next_minor=$((minor + 1))
    suggested_version="v$major.$next_minor"
else
    suggested_version="v1.0"
fi

# Ask for version name
read -p "Enter version name (default: $suggested_version): " version
if [ -z "$version" ]; then
    version="$suggested_version"
fi

# Normalize version
if [[ ! $version =~ ^v ]]; then
    version="v$version"
fi

if [[ ! $version =~ \.[0-9]+$ ]]; then
    version="${version}.0"
fi

# Create build directory
build_dir="build/$version"
mkdir -p "$build_dir"

# Compile
g++ -std=c++23 src/*.cpp -o "$build_dir/main"

# Check success
if [ $? -eq 0 ]; then
    echo "✅ Build succeeded! Executable is in $build_dir/main"
    if [ "$should_run" = true ]; then
        echo "🚀 Running program..."
        "$build_dir/main"
    else
        echo "👉 Run manually with: $build_dir/main"
        echo "👉 Or run latest version with: ./$(basename "$0") cdrun"
    fi
else
    echo "❌ Build failed."
fi