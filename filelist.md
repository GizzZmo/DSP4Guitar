# DSP4Guitar Directory Structure

## Root

- `CMakeLists.txt` — CMake build configuration (JUCE 7.0.9, C++17, VST3/AU/Standalone)
- `README.md` — Project overview, installation, and usage guide
- `CONTRIBUTING.md` — Contribution guidelines and development setup
- `LICENSE` — MIT licence
- `filelist.md` — This file
- `CI_SETUP_SUMMARY.md` — CI implementation summary
- `packaging.md` — Plugin packaging and distribution guide

## Source Files

### Plugin Core
- `MultiEffectProcessor.h` / `.cpp` — `AudioProcessor` subclass; contains all DSP helper classes (Bitcrusher, Fuzz, MultibandCompressor, RingModulator, WahWah, Tremolo) and the 10-effect `ProcessorChain`
- `PluginEditor.h` / `.cpp` — `AudioProcessorEditor` subclass; GUI panels, knobs, toggles, waveform display

### GUI Theme
- `CyberpunkLookAndFeel.h` — Custom JUCE `LookAndFeel` (neon-green cyberpunk aesthetic)

### Preset Management
- `PresetManager.h` / `.cpp` — Save and load XML presets

### Legacy / Utility Effect Helpers
- `Delay.h` / `.cpp` — Standalone delay utility
- `Distortion.h` / `.cpp` — Standalone distortion utility
- `Modulation.h` / `.cpp` — Standalone modulation utility
- `StereoWidening.h` / `.cpp` — Stereo widening utility

### Standalone App
- `DSP4GuitarApp.h` — Entry point for the standalone application format

## Scripts

- `scripts/pre-commit-check.sh` — Bash validation script (Linux/macOS)
- `scripts/pre-commit-check.ps1` — PowerShell validation script (Windows)

## GitHub Actions

- `.github/workflows/ci.yml` — CI workflow: multi-platform build, code quality, documentation checks
- `.github/workflows/release.yml` — Release workflow: build, package, attach to GitHub release
- `.github/CI_DOCUMENTATION.md` — Full CI/CD reference
- `.github/CI_QUICKSTART.md` — Quick-start guide for CI
- `.github/pull_request_template.md` — PR description template

## Documentation

- `Documentation/effects-reference.md` — Complete parameter reference for all 10 effects
- `Documentation/readme.md` — DSP theory compendium (distortion, dynamics, filters, modulation)
- `Documentation/vstplugin.md` — VST plugin architecture and JUCE integration guide
- `Documentation/performance.md` — Real-time performance and multi-channel processing
- `Documentation/interactivemidi.md` — Waveform display and MIDI control integration
- `Documentation/additionalfx.md` — Additional effect variations and preset system
- `Documentation/addingmorefx.md` — Guide for adding new effects
- `Documentation/midiswitch.md` — MIDI preset switching documentation
- `Documentation/presetvisualization.md` — Preset visualisation guide
- `Documentation/Multi-effect blending.md` — Multi-effect blending documentation
- `Documentation/DeploymentOptimization.md` — Deployment optimisation guide
- `Documentation/VST_Multi_Guitar_FX.md` — VST multi-guitar FX reference

## Assets

- `assets/screenshots/` — UI screenshots and SVG mockup
- `assets/icons/` — Plugin logo and icon files

## Build Output (generated, not committed)

- `build/` — CMake build directory
- `build/*_artefacts/Release/VST3/` — Built VST3 plugin
- `build/*_artefacts/Release/AU/` — Built AU plugin (macOS)
- `build/*_artefacts/Release/Standalone/` — Built standalone app
