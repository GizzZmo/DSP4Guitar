# DSP4Guitar — Cyberpunk Multi-Effect Processor

[![CI](https://github.com/GizzZmo/DSP4Guitar/actions/workflows/ci.yml/badge.svg)](https://github.com/GizzZmo/DSP4Guitar/actions/workflows/ci.yml)

> *"Plug in. Drop out. Enter the Matrix."*

![DSP4Guitar Banner](assets/screenshots/banner.png)

---

### **Overview**
A multi-effect **VST/AU plugin** built using **JUCE** wrapped in a full **cyberpunk / Matrix-terminal aesthetic** — neon-green-on-black UI, scrolling Matrix-rain header animation, glowing LED toggles, and custom rotary knobs.  It integrates **distortion, delay, stereo widening, modulation blending, MIDI-controlled preset switching**, and **dynamic effect interaction**.

### **UI Preview**

| Plugin UI | Logo |
|-----------|------|
| ![Plugin UI](assets/screenshots/plugin_ui.png) | ![Logo](assets/icons/dsp4guitar_logo.png) |

> The SVG vector mockup is at [`assets/screenshots/plugin_ui.svg`](assets/screenshots/plugin_ui.svg).

---

### **Features**
✅ **Cyberpunk / Matrix terminal UI** — neon green `#00FF41`, dark-panel aesthetic, scrolling rain animation  
✅ **Distortion with dynamic response**  
✅ **Stereo widening with Mid-Side processing**  
✅ **Delay with feedback linked to distortion**  
✅ **Modulation depth controlled by signal dynamics**  
✅ **MIDI-based preset switching**  
✅ **Custom effect routing configurations**  
✅ **Real-time waveform visualization**  

### **Cyberpunk Colour Palette**

| Token | Hex | Usage |
|-------|-----|-------|
| Matrix Green | `#00FF41` | Active knobs, borders, LEDs |
| Dark Green | `#00B300` | Track fills, secondary labels |
| Near-Black | `#050505` | Knob backgrounds |
| Dark BG | `#0D0D1A` | Window background |
| Cyan | `#00FFFF` | Accent labels, corner brackets |
| Gray | `#444444` | Inactive / disabled elements |

---

### **Installation**
1. Clone the repository:
   ```sh
   git clone https://github.com/GizzZmo/DSP4Guitar.git
   cd DSP4Guitar
   ```

2. The project uses CMake and JUCE framework:
   ```sh
   # Clone JUCE (if not using CI auto-download)
   git clone --depth 1 --branch 7.0.9 https://github.com/juce-framework/JUCE.git
   
   # Configure and build
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

3. Copy the .vst3 or .component file to your VST/AU plugin directory:
   - **Linux**: `~/.vst3/`
   - **macOS**: `/Library/Audio/Plug-Ins/VST3/` or `~/Library/Audio/Plug-Ins/Components/`
   - **Windows**: `C:\Program Files\Common Files\VST3\`

4. Open your DAW, scan for plugins, and test.

### **Development**
- The project includes a CI/CD pipeline that automatically builds for Linux, macOS, and Windows
- Artifacts are available from successful CI runs
- Code quality checks are performed on each push/PR
- 📖 See [CI Quick Start Guide](.github/CI_QUICKSTART.md) and [CI Documentation](.github/CI_DOCUMENTATION.md) for details
- 🔍 Run `./scripts/pre-commit-check.sh` (or `.ps1` on Windows) to validate changes locally

### **How to Use**
- Adjust effects via GUI sliders
- Save/Load custom presets
- Use MIDI program change messages to switch presets
- Enable/disable effects dynamically via toggle buttons
- Monitor processed signal in the interactive waveform display

### **License**
MIT License - Open-source for personal and commercial use.

