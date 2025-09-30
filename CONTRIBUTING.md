# Contributing to DSP4Guitar

Thank you for considering contributing to DSP4Guitar! This document outlines the process and guidelines.

## Development Setup

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler
- Git

### Building Locally

1. Clone the repository:
```bash
git clone https://github.com/GizzZmo/DSP4Guitar.git
cd DSP4Guitar
```

2. Download JUCE framework:
```bash
git clone --depth 1 --branch 7.0.9 https://github.com/juce-framework/JUCE.git
```

3. Build the project:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

## Continuous Integration

The project uses GitHub Actions for CI/CD:

### CI Workflow
- **Triggers**: Push to `main`/`develop` branches, pull requests
- **Platforms**: Ubuntu, macOS, Windows
- **Actions**:
  - Build VST3 and AU (macOS) plugins
  - Run code quality checks
  - Verify documentation
  - Upload build artifacts

### Release Workflow
- **Triggers**: New release creation, manual dispatch
- **Actions**:
  - Build optimized release versions
  - Package plugins with documentation
  - Attach to GitHub releases

## Making Changes

1. **Fork the repository** and create a new branch
2. **Make your changes** with clear, focused commits
3. **Test locally** - ensure the plugin builds and works in a DAW
4. **Push to your fork** and create a pull request
5. **Wait for CI** - GitHub Actions will automatically:
   - Build your changes on all platforms
   - Run code quality checks
   - Report results on your PR

### Code Quality Standards

- Use C++17 features appropriately
- Follow existing code style and formatting
- Avoid trailing whitespace
- Comment complex DSP algorithms
- Ensure thread-safety in audio processing code

### Pull Request Guidelines

- Fill out the PR template completely
- Reference any related issues
- Ensure all CI checks pass
- Respond to review feedback promptly

## Testing

### Local Testing
- Test plugin in at least one DAW (Reaper, Ableton, etc.)
- Verify VST3 format works correctly
- Test MIDI functionality if applicable
- Check for CPU usage and audio artifacts

### CI Testing
- All PRs must pass CI checks
- Address any build failures or warnings
- Code quality checks should pass

## Questions?

If you have questions about contributing, feel free to:
- Open an issue for discussion
- Check existing documentation in the `Documentation/` folder
- Review closed PRs for examples

Thank you for contributing! 🎸
