# CI/CD System Documentation

## Overview

DSP4Guitar uses GitHub Actions for continuous integration and deployment. The system automatically builds, tests, and packages the VST/AU plugin for multiple platforms.

## Workflows

### 1. CI Workflow (`.github/workflows/ci.yml`)

**Triggers:**
- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches  
- Manual workflow dispatch

**Jobs:**

#### Build Job
- **Platforms**: Ubuntu, macOS (Apple Silicon via `macos-14`), Windows
- **Actions**:
  - Checks out code with submodules
  - Sets up platform-specific dependencies (Linux: ALSA, JACK, X11, etc.)
  - Caches JUCE framework (`actions/cache@v4`) for faster builds
  - Downloads JUCE 7.0.9 on cache miss
  - Configures CMake with Release settings
  - Builds VST3 plugin (and AU on macOS)
  - Uploads build artifacts

**Artifacts:**
- `DSP4Guitar-Linux-VST3`: Linux VST3 plugin
- `DSP4Guitar-macOS`: macOS VST3 and AU plugins
- `DSP4Guitar-Windows-VST3`: Windows VST3 plugin

#### Code Quality Job
- Checks for trailing whitespace in `.cpp`/`.h`/`.hpp` files
- Checks for tabs in source files
- Scans for debug print statements (`printf`, `cout`)
- Warns if `#include <iostream>` is found (prefer JUCE's `DBG()` macro)
- Runs on Ubuntu only

#### Documentation Job
- Verifies `README.md` exists
- Checks for `LICENSE` file
- Checks for `CONTRIBUTING.md`
- Checks for `Documentation/effects-reference.md`
- Verifies `CMakeLists.txt` exists
- Checks that all source files referenced in `CMakeLists.txt` are present on disk
- Runs on Ubuntu only

### 2. Release Workflow (`.github/workflows/release.yml`)

**Triggers:**
- New GitHub release created
- Manual workflow dispatch

**Actions:**
- Checks out code with submodules
- Sets up platform-specific dependencies (Linux)
- Caches JUCE (`actions/cache@v4`) — same cache key as CI, so builds after CI runs benefit from warm cache
- Downloads JUCE 7.0.9 on cache miss
- Builds optimised release versions for all platforms
- Packages plugins with README and LICENSE
- Creates platform-specific archives:
  - Linux: `.tar.gz`
  - macOS: `.zip`
  - Windows: `.zip`
- Attaches archives to the GitHub release using `softprops/action-gh-release@v2`

## Build System

### CMakeLists.txt

The project uses CMake 3.15+ with the following configuration:

- **C++ Standard**: C++17
- **JUCE Version**: 7.0.9
- **Plugin Formats**: VST3, AU (macOS), Standalone
- **Linked JUCE Modules**:
  - `juce_audio_basics`
  - `juce_audio_devices`
  - `juce_audio_formats`
  - `juce_audio_plugin_client`
  - `juce_audio_processors`
  - `juce_audio_utils`
  - `juce_core`
  - `juce_data_structures`
  - `juce_dsp`
  - `juce_events`
  - `juce_graphics`
  - `juce_gui_basics`
  - `juce_gui_extra`

### Source Files Included
- MultiEffectProcessor.cpp/h — plugin entry point and all DSP classes
- PluginEditor.cpp/h — GUI editor
- CyberpunkLookAndFeel.h — custom JUCE LookAndFeel
- Delay.cpp/h, Distortion.cpp/h, Modulation.cpp/h — legacy effect helpers
- PresetManager.cpp/h — preset save/load
- StereoWidening.cpp/h — stereo widening utility
- DSP4GuitarApp.h — standalone app entry point

## Local Development

### Prerequisites
```bash
# Install CMake (3.15+)
cmake --version

# Install platform dependencies
# Ubuntu/Debian:
sudo apt-get update
sudo apt-get install libasound2-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libfreetype6-dev libx11-dev libxcomposite-dev libxcursor-dev \
  libxinerama-dev libxrandr-dev libxrender-dev \
  libglu1-mesa-dev mesa-common-dev

# macOS: Xcode Command Line Tools
xcode-select --install

# Windows: Visual Studio 2019 or later
```

### Building
```bash
# Clone JUCE framework
git clone --depth 1 --branch 7.0.9 https://github.com/juce-framework/JUCE.git

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --config Debug

# For release builds:
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Build Output Locations
- **VST3**: `build/*_artefacts/Release/VST3/`
- **AU** (macOS): `build/*_artefacts/Release/AU/`
- **Standalone**: `build/*_artefacts/Release/Standalone/`

## Status Badge

The README includes a CI status badge that shows the current build status:

```markdown
[![CI](https://github.com/GizzZmo/DSP4Guitar/actions/workflows/ci.yml/badge.svg)](https://github.com/GizzZmo/DSP4Guitar/actions/workflows/ci.yml)
```

## Accessing Build Artifacts

### From Pull Requests
1. Navigate to your PR on GitHub
2. Scroll to the checks section
3. Click "Show all checks"
4. Find the CI workflow run
5. Click "Details" → "Summary"
6. Download artifacts from the "Artifacts" section

### From Releases
1. Navigate to the Releases page
2. Download the platform-specific archive
3. Extract and install the plugin

## Troubleshooting

### Build Failures

**JUCE not found:**
- Ensure JUCE is cloned in the project root
- Check JUCE version is 7.0.9

**Missing dependencies (Linux):**
- Install all required dev packages listed above
- Update package manager: `sudo apt-get update`

**CMake version too old:**
- Upgrade to CMake 3.15 or later

### Workflow Failures

**Artifact upload fails:**
- Check build actually produced plugin files
- Verify paths in workflow match actual build output

**Code quality checks fail:**
- Remove trailing whitespace: `sed -i 's/[[:space:]]*$//' <file>`
- Address any warnings in the CI logs

**Release asset upload fails:**
- Ensure `GITHUB_TOKEN` has write access to releases (default for `release` events)
- Check that the archive was created successfully in the Package step

## Extending the CI

### Adding New Tests

Add test steps to the `build` job in `ci.yml`:

```yaml
- name: Run Tests
  run: |
    cd build
    ctest --output-on-failure
```

### Adding New Platforms

Add to the matrix in both workflows:

```yaml
strategy:
  matrix:
    os: [ubuntu-latest, macos-latest, windows-latest, ubuntu-20.04]
```

### Adding Code Coverage

Add a new job to `ci.yml`:

```yaml
coverage:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v4
    # ... build with coverage flags
    - name: Upload coverage
      uses: codecov/codecov-action@v3
```

## Best Practices

1. **Always test locally** before pushing — run `./scripts/pre-commit-check.sh`
2. **Keep workflows fast** — JUCE caching is already configured; avoid removing it
3. **Monitor artifact sizes** — keep plugins reasonably sized
4. **Review CI logs** for warnings even if the build succeeds
5. **Update JUCE version** carefully — update the version string in CMakeLists.txt, the `git clone` commands in both workflows, and the `actions/cache` key in both workflows

## Support

For issues with the CI system:
1. Check the workflow run logs on GitHub
2. Review this documentation
3. Open an issue with "CI:" prefix in the title
4. Include relevant log excerpts

---

Last updated: 2025

