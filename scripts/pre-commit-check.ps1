# Pre-commit validation script for DSP4Guitar (Windows)
# Run this before committing to catch issues early

Write-Host "🔍 Running pre-commit validation..." -ForegroundColor Cyan
Write-Host ""

$ErrorActionPreference = "Stop"
$hasErrors = $false

# Check for trailing whitespace
Write-Host "✓ Checking for trailing whitespace..." -ForegroundColor Yellow
$trailingWhitespace = Select-String -Path "*.cpp","*.h","*.hpp" -Pattern '\s+$' -Recurse -ErrorAction SilentlyContinue
if ($trailingWhitespace) {
    Write-Host "❌ Error: Trailing whitespace found in source files" -ForegroundColor Red
    $hasErrors = $true
} else {
    Write-Host "✓ No trailing whitespace found" -ForegroundColor Green
}
Write-Host ""

# Check for tabs in source files
Write-Host "✓ Checking for tabs in source files..." -ForegroundColor Yellow
$tabs = Select-String -Path "*.cpp","*.h" -Pattern "`t" -Recurse -ErrorAction SilentlyContinue
if ($tabs) {
    Write-Host "⚠️  Warning: Tabs found in source files (consider using spaces)" -ForegroundColor Yellow
} else {
    Write-Host "✓ No tabs found" -ForegroundColor Green
}
Write-Host ""

# Check for debug print statements
Write-Host "✓ Checking for debug print statements..." -ForegroundColor Yellow
$debugPrints = Select-String -Path "*.cpp","*.h" -Pattern "(printf|cout)" -Recurse -ErrorAction SilentlyContinue
if ($debugPrints) {
    Write-Host "⚠️  Warning: Debug print statements found (printf/cout)" -ForegroundColor Yellow
} else {
    Write-Host "✓ No debug statements found" -ForegroundColor Green
}
Write-Host ""

# Verify required files exist
Write-Host "✓ Checking required files..." -ForegroundColor Yellow
$requiredFiles = @("README.md", "LICENSE", "CMakeLists.txt")
foreach ($file in $requiredFiles) {
    if (-not (Test-Path $file)) {
        Write-Host "❌ Error: Required file $file not found" -ForegroundColor Red
        $hasErrors = $true
    }
}
Write-Host "✓ All required files present" -ForegroundColor Green
Write-Host ""

# Try to configure with CMake
Write-Host "✓ Testing CMake configuration..." -ForegroundColor Yellow
if (Test-Path "JUCE") {
    try {
        cmake -B build-test -DCMAKE_BUILD_TYPE=Debug 2>&1 | Out-Null
        Write-Host "✓ CMake configuration successful" -ForegroundColor Green
        Remove-Item -Recurse -Force build-test -ErrorAction SilentlyContinue
    } catch {
        Write-Host "❌ Error: CMake configuration failed" -ForegroundColor Red
        Write-Host "   Run: cmake -B build-test -DCMAKE_BUILD_TYPE=Debug" -ForegroundColor Yellow
        $hasErrors = $true
    }
} else {
    Write-Host "⚠️  Warning: JUCE not found, skipping CMake test" -ForegroundColor Yellow
    Write-Host "   Clone JUCE: git clone --depth 1 --branch 7.0.9 https://github.com/juce-framework/JUCE.git" -ForegroundColor Yellow
}
Write-Host ""

if ($hasErrors) {
    Write-Host "❌ Validation failed with errors!" -ForegroundColor Red
    exit 1
} else {
    Write-Host "✅ All validation checks passed!" -ForegroundColor Green
    Write-Host ""
    Write-Host "You can now commit your changes with confidence." -ForegroundColor Cyan
    Write-Host "The CI will run similar checks on push." -ForegroundColor Cyan
}
