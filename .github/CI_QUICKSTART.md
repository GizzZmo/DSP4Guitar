# CI Quick Start Guide

## What is CI/CD?

Continuous Integration (CI) automatically builds and tests your code every time you push changes. This project uses GitHub Actions to:

- ✅ Build the plugin for Linux, macOS, and Windows
- ✅ Run code quality checks
- ✅ Package releases automatically
- ✅ Provide downloadable artifacts

## How It Works

### When You Push Code

1. **Push to main/develop branch** → CI workflow triggers
2. **Create a Pull Request** → CI workflow triggers
3. **CI builds on 3 platforms** → Linux, macOS, Windows
4. **Artifacts uploaded** → Download built plugins from Actions tab

### When You Create a Release

1. **Create a GitHub release** → Release workflow triggers
2. **Plugins built and packaged** → For all platforms
3. **Archives attached to release** → Ready for distribution

## Using the CI System

### Check Build Status

1. Go to the [Actions tab](https://github.com/GizzZmo/DSP4Guitar/actions)
2. See all workflow runs
3. Click any run to see details
4. Green ✓ = passed, Red ✗ = failed

### Download Build Artifacts

**From a PR or commit:**
1. Find the workflow run in Actions tab
2. Click on the run
3. Scroll to "Artifacts" section
4. Download platform-specific builds

**From a release:**
1. Go to [Releases](https://github.com/GizzZmo/DSP4Guitar/releases)
2. Download the archive for your platform
3. Extract and install

### Run Local Checks

Before pushing, validate your changes locally:

**Linux/macOS:**
```bash
./scripts/pre-commit-check.sh
```

**Windows (PowerShell):**
```powershell
.\scripts\pre-commit-check.ps1
```

This catches issues before CI runs.

## Understanding the Workflows

### CI Workflow (`.github/workflows/ci.yml`)

```
Push/PR → Checkout code → Install dependencies → Build → Upload artifacts
```

**Runs on:** Ubuntu, macOS, Windows  
**Time:** ~10-15 minutes per platform  
**Artifacts:** VST3 (all), AU (macOS)

### Release Workflow (`.github/workflows/release.yml`)

```
Release created → Build all platforms → Package → Attach to release
```

**Runs on:** Ubuntu, macOS, Windows  
**Produces:** .tar.gz (Linux), .zip (macOS/Windows)

## Build Configuration

The build uses CMake with these settings:

- **CMake version:** 3.15+
- **C++ standard:** C++17
- **JUCE version:** 7.0.9
- **Build type:** Release (optimized)
- **Plugin formats:** VST3, AU, Standalone

## Troubleshooting

### ❌ Build Failed

**Check the logs:**
1. Go to Actions tab
2. Click the failed run
3. Expand the failed step
4. Read error messages

**Common issues:**
- Missing dependencies (Linux)
- JUCE download failed (network)
- CMake configuration error (check CMakeLists.txt)

### ⚠️ Code Quality Failed

**Fix:**
- Remove trailing whitespace: `sed -i 's/[[:space:]]*$//' <file>`
- Check for debug print statements (printf/cout)
- Ensure consistent formatting

### 📦 Artifacts Missing

**Check:**
- Build completed successfully
- Plugin files were created
- Paths in workflow match build output

## Pro Tips

1. **Enable Actions** in your fork's settings
2. **Watch workflow runs** to catch issues early
3. **Use draft PRs** to test CI before review
4. **Cache JUCE** to speed up builds (already configured)
5. **Run local checks** before every push

## Getting Help

- 📖 Read [CI_DOCUMENTATION.md](.github/CI_DOCUMENTATION.md) for details
- 📝 Check [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines
- 🐛 Open an issue with "CI:" prefix for CI problems
- 💬 Include workflow run logs in bug reports

---

**Next steps:**
1. Make a small change
2. Push to a branch
3. Watch CI run in Actions tab
4. Download and test the artifact

Happy building! 🎸
