#!/bin/bash
# Pre-commit validation script for DSP4Guitar
# Run this before committing to catch issues early

set -e

echo "🔍 Running pre-commit validation..."
echo ""

# Check for trailing whitespace
echo "✓ Checking for trailing whitespace..."
if grep -r --include='*.cpp' --include='*.h' --include='*.hpp' '[[:space:]]$' . 2>/dev/null; then
    echo "⚠️  Warning: Trailing whitespace found in source files"
    echo "   Fix with: sed -i 's/[[:space:]]*$//' <file>"
else
    echo "✓ No trailing whitespace found"
fi
echo ""

# Check for tabs in source files
echo "✓ Checking for tabs in source files..."
if grep -r --include='*.cpp' --include='*.h' $'\t' . 2>/dev/null; then
    echo "⚠️  Warning: Tabs found in source files (consider using spaces)"
else
    echo "✓ No tabs found"
fi
echo ""

# Check for debug print statements
echo "✓ Checking for debug print statements..."
if grep -r --include='*.cpp' --include='*.h' 'printf\|cout' . 2>/dev/null; then
    echo "⚠️  Warning: Debug print statements found (printf/cout)"
else
    echo "✓ No debug statements found"
fi
echo ""

# Verify required files exist
echo "✓ Checking required files..."
required_files=("README.md" "LICENSE" "CMakeLists.txt")
for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Error: Required file $file not found"
        exit 1
    fi
done
echo "✓ All required files present"
echo ""

# Try to configure with CMake
echo "✓ Testing CMake configuration..."
if [ -d "JUCE" ]; then
    if cmake -B build-test -DCMAKE_BUILD_TYPE=Debug > /dev/null 2>&1; then
        echo "✓ CMake configuration successful"
        rm -rf build-test
    else
        echo "❌ Error: CMake configuration failed"
        echo "   Run: cmake -B build-test -DCMAKE_BUILD_TYPE=Debug"
        exit 1
    fi
else
    echo "⚠️  Warning: JUCE not found, skipping CMake test"
    echo "   Clone JUCE: git clone --depth 1 --branch 7.0.9 https://github.com/juce-framework/JUCE.git"
fi
echo ""

echo "✅ All validation checks passed!"
echo ""
echo "You can now commit your changes with confidence."
echo "The CI will run similar checks on push."
