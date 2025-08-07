This project, "DSP4Guitar", is a comprehensive exploration and implementation of Digital Signal Processing (DSP) effects, primarily targeting guitar audio. It serves a dual purpose:

1.  **A Conceptual JUCE-based VST Host Application**: One part of the code (`DSP4GuitarApp.h`) outlines a minimalist standalone application. This application is designed to set up audio input/output (preferring ASIO on Windows), scan for available VST/VST3 plugins on the system, load a selected plugin, and route live audio through it. It acts as a basic testbed or a simple utility for experimenting with external VSTs.

2.  **A Multi-Effect VST/AU Guitar Plugin Development Compendium**: The core of the project's more advanced code and the extensive `Documentation` folder (`MultiEffectProcessor.*`, `PluginEditor.h`, and various Markdown files) focuses on the detailed design, implementation, and optimization of a versatile multi-effect guitar plugin. This aspect functions as a rich educational resource, covering:
    *   **DSP Theory**: In-depth explanations of various audio effects (distortion, dynamics, filters, modulation, time-based effects, pitch shifting).
    *   **C++ Implementations**: Practical code examples for building these effects from scratch or utilizing JUCE's DSP module.
    *   **Plugin Development Best Practices**: Guidance on JUCE's `AudioProcessor` and `AudioProcessorEditor` architecture, parameter management with `AudioProcessorValueTreeState`, MIDI integration, preset systems, and GUI design.
    *   **Performance Optimization**: Techniques for efficient real-time audio processing, including SIMD, denormal handling, and parameter smoothing.
    *   **Deployment**: Strategies for packaging and distributing the VST/AU plugin.

In essence, the project provides both a functional, albeit simple, VST host and a detailed, educational blueprint for building a sophisticated multi-effect plugin, complete with theoretical background and practical C++ implementations.

---

## Architecture

The project conceptually comprises two main architectural components: a standalone host application and a multi-effect VST plugin.

### 1. Standalone Host Application (outlined in `DSP4GuitarApp.h`)

This component provides a basic framework for a desktop audio application:

*   **`DSP4GuitarApplication`**: The top-level JUCE `JUCEApplication` class, responsible for the application's lifecycle (initialisation, shutdown, quit requests).
*   **`MainHostWindow`**: A `juce::DocumentWindow` that acts as the application's main window, containing the primary audio processing and UI component.
*   **`MainAudioComponent`**: Inherits `juce::AudioAppComponent` and `juce::Button::Listener`, `juce::ComboBox::Listener`, `juce::Timer`. This is the heart of the host application. It manages the `juce::AudioDeviceManager` for audio input/output, scans the system for VST/VST3 plugins using `juce::AudioPluginFormatManager`, loads a selected plugin, and routes audio through it. It also provides a minimal GUI for audio settings and plugin selection.

### 2. Multi-Effect VST/AU Plugin (primary focus of `MultiEffectProcessor` and `Documentation` files)

This is the main DSP development focus, detailed across many files:

*   **Core DSP Classes (Modular Examples)**: `Delay`, `Distortion`, `Modulation`, `StereoWidening`. These are independent C++ classes demonstrating how to implement specific DSP effects. While not directly linked to the `DSP4GuitarApp` host's plugin loading, they represent the building blocks and conceptual approaches used for effects.
*   **`MultiEffectProcessor` (Plugin Core)**: Inherits `juce::AudioProcessor`. This is the central class for the actual VST/AU plugin.
    *   **Parameter Management**: Uses `juce::AudioProcessorValueTreeState` to define and manage all plugin parameters (e.g., enable toggles, drive, rate, depth, mix for various effects). This enables DAW automation and preset saving.
    *   **Effect Chain**: Employs `juce::dsp::ProcessorChain` to organize multiple DSP effects in a defined processing order. It integrates both custom-implemented effects (like `Bitcrusher`, `RingModulator`, `Tremolo` defined within its own header) and JUCE's built-in DSP modules (e.g., `Phaser`, `Flanger`, `Chorus`, `DelayLine`, `Reverb`).
    *   **Audio Processing**: The `processBlock` method orchestrates the audio flow through the chained effects, updating effect parameters dynamically from the `AudioProcessorValueTreeState`.
*   **`MultiEffectProcessorEditor` (Plugin GUI)**: Inherits `juce::AudioProcessorEditor`. This class provides the graphical user interface for the VST plugin. It contains numerous UI elements (sliders, toggle buttons, labels) for each effect parameter and uses JUCE's attachment classes (`SliderAttachment`, `ButtonAttachment`) to link these UI controls directly to the `AudioProcessorValueTreeState` in the `MultiEffectProcessor`, ensuring real-time interaction and DAW compatibility.
*   **`PresetManager`**: A utility class that uses JUCE's `juce::ValueTree` to handle the saving and loading of plugin presets, allowing users to store and recall their preferred effect configurations.
*   **`Documentation`**: This extensive folder of Markdown files functions as a detailed compendium. It systematically explains DSP theory, C++ implementation details for various effects, strategies for performance optimization (e.g., SIMD, denormal handling), advanced features like MIDI integration and dynamic effect blending, and practical steps for VST plugin packaging and distribution.

---

## File-by-File Breakdown

Here's a breakdown of each file's role in the project:

### `DSP4GuitarApp.h`

*   **Role**: This file defines the main classes for a **standalone JUCE application** that functions as a simple VST host.
*   **Key Components**:
    *   `DSP4GuitarApplication`: The entry point for the entire application. It inherits `juce::JUCEApplication` and handles core application lifecycle events (initialization, shutdown, quit requests). It creates the main application window.
    *   `MainHostWindow`: A nested class inheriting `juce::DocumentWindow`. This represents the application's main top-level window. It sets up the window's properties (resizable, visible) and is responsible for holding the `MainAudioComponent`.
    *   `MainAudioComponent`: A nested class inheriting `juce::AudioAppComponent`, `juce::Button::Listener`, `juce::ComboBox::Listener`, and `juce::Timer`. This is the core audio engine and UI for the *host*.
        *   It initializes and manages `juce::AudioDeviceManager` to interact with audio hardware (e.g., setting up ASIO on Windows).
        *   It uses `juce::AudioPluginFormatManager` to scan the system for VST/VST3 plugins and `juce::KnownPluginList` to store discovered plugins.
        *   It provides UI elements (buttons, combobox, label) to open audio settings, trigger plugin scanning, select a loaded plugin, and display status messages.
        *   Its `prepareToPlay` and `getNextAudioBlock` methods handle the audio buffer processing, routing input audio through a currently loaded `juce::AudioPluginInstance`.

### `Delay.h` / `Delay.cpp`

*   **Role**: Defines and implements a **basic audio delay effect**.
*   **Key Components**:
    *   `Delay` class: Manages a `std::vector<float>` as a circular buffer for delay.
    *   `process` method: Applies a simple delay effect to an audio buffer, including a feedback mechanism.
    *   Note: This is a standalone, simplified DSP class, not directly integrated into the `MultiEffectProcessor`'s chain as a JUCE DSP object, but it serves as an example of such a component.

### `Distortion.h` / `Distortion.cpp`

*   **Role**: Defines and implements a **basic soft-clipping distortion effect**.
*   **Key Components**:
    *   `Distortion` class: Provides a `process` method that applies `std::tanh()` to incoming audio samples, simulating soft-clipping distortion based on a `drive` parameter.
    *   Note: Similar to `Delay`, this is a standalone example DSP class.

### `Modulation.h` / `Modulation.cpp`

*   **Role**: Defines and implements a **simple amplitude modulation (tremolo) effect**.
*   **Key Components**:
    *   `Modulation` class: Contains a phase variable to generate a sine wave for an LFO.
    *   `process` method: Multiplies audio samples by an LFO-generated value to create a tremolo effect, controllable by `depth` and `rate` parameters.
    *   Note: Another standalone example DSP class.

### `StereoWidening.h` / `StereoWidening.cpp`

*   **Role**: Defines and implements a **stereo widening effect** using Mid-Side processing.
*   **Key Components**:
    *   `StereoWidening` class: Provides a `process` method that transforms stereo audio into Mid (mono sum) and Side (stereo difference) components, scales the Side component by a `widthAmount`, and then converts back to Left/Right, creating a wider stereo image.
    *   Note: Another standalone example DSP class.

### `MultiEffectProcessor.h`

*   **Role**: This is the header file for the **core of the Multi-Effect VST/AU plugin**. It defines the main audio processor class and custom DSP components used within it.
*   **Key Components**:
    *   `RingModulator` class: A custom DSP class implementing a basic ring modulator effect using a sine wave oscillator.
    *   `Bitcrusher` class: A custom DSP class implementing a bitcrushing effect (quantization and downsampling).
    *   `Tremolo` class: A custom DSP class implementing a tremolo effect using `juce::dsp::Oscillator` and `juce::dsp::Gain`.
    *   `MultiEffectProcessor` class: Inherits `juce::AudioProcessor`.
        *   It declares a `juce::AudioProcessorValueTreeState` (`apvts`) to manage all plugin parameters.
        *   It defines an `enum ChainPositions` to specify the order of effects in the processing chain.
        *   It uses `juce::dsp::ProcessorChain` (`effectChain`) to neatly organize and process audio through a series of various effects. This chain includes both the custom DSP classes defined above and built-in JUCE DSP classes (e.g., `juce::dsp::Phaser`, `juce::dsp::Flanger`, `juce::dsp::Chorus`, `juce::dsp::DelayLine`, `juce::dsp::Reverb`).
        *   It declares raw pointers to `juce::AudioParameterBool` and `juce::AudioParameterFloat` for quick access to parameter values in the `processBlock` method.
        *   Declares `createParameterLayout()` to define the plugin's parameters and `updateParameters()` to sync parameter values with the DSP objects in the chain.

### `MultiEffectProcessor.cpp`

*   **Role**: This is the implementation file for the **core of the Multi-Effect VST/AU plugin**. It provides the actual DSP logic and parameter handling.
*   **Key Components**:
    *   `MultiEffectProcessor` constructor: Initializes the `AudioProcessorValueTreeState` and retrieves raw pointers to all defined parameters for efficient access.
    *   `createParameterLayout()`: A static method that defines all the plugin's parameters (e.g., "Bitcrusher On", "Bit Depth", "RM Rate", "Phaser Mix", "Delay Time", "Room Size" for Reverb) with their IDs, names, ranges, and default values.
    *   `prepareToPlay()`: Prepares the `juce::dsp::ProcessorChain` and its individual effects for processing based on the sample rate and block size. It also calls `updateParameters()` to set initial values.
    *   `updateParameters()`: A helper method that retrieves the current values from the `AudioProcessorValueTreeState` and applies them to the corresponding DSP objects within the `effectChain`, setting their internal parameters and bypass states.
    *   `processBlock()`: The main audio processing callback. It handles denormal prevention, clears unused output channels, calls `updateParameters()` before processing each block, and then processes the audio through the `effectChain`. It includes a temporary, manual implementation for the Delay effect for demonstration purposes.
    *   Standard JUCE boilerplate methods: `getName`, `acceptsMidi`, `createEditor`, `getStateInformation`, `setStateInformation`, etc., which are required for a functional JUCE audio plugin.

### `PluginEditor.h`

*   **Role**: This file defines the **GUI for the Multi-Effect VST/AU plugin**.
*   **Key Components**:
    *   `MultiEffectProcessorEditor` class: Inherits `juce::AudioProcessorEditor` and `juce::Timer`.
        *   It holds a reference to the `MultiEffectProcessor` to interact with its parameters.
        *   It declares a large number of UI elements, primarily `juce::ToggleButton` (for on/off switches) and `juce::Slider` (for continuous parameters), organized by effect (Bitcrusher, Ring Mod, Phaser, Flanger, Chorus, Tremolo, Delay, Reverb). Labels (`juce::Label`) are also declared for parameter names.
        *   It uses `std::vector<std::unique_ptr<SliderAttachment>>` and `std::vector<std::unique_ptr<ButtonAttachment>>` to automatically manage the lifetime and connections between the UI elements and the plugin's `AudioProcessorValueTreeState` parameters.
        *   Declares `paint()` and `resized()` methods for rendering and layout, and `timerCallback()` for potential animations or real-time updates.
        *   It includes a `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR` macro for memory debugging.

### `PresetManager.h` / `PresetManager.cpp`

*   **Role**: Defines and implements a basic **preset management system** for the plugin.
*   **Key Components**:
    *   `PresetManager` class: Contains a `juce::ValueTree` named `presetStore`.
    *   `savePreset()`: Stores current parameter values into a `ValueTree` child representing a preset. (The provided example is simplified, only storing "drive" and "delay").
    *   `loadPreset()`: Retrieves parameter values from a named preset `ValueTree` child and could apply them to the plugin's parameters. (The provided example retrieves but doesn't apply).

### `LICENSE`

*   **Role**: Specifies the **MIT License** under which the project's code is distributed. This permits broad use, modification, and distribution, provided the original copyright notice is included.

### `Modulation.cpp`

*   **Role**: Implements the `Modulation` class defined in `Modulation.h`.

### `README.md` (Top-level)

*   **Role**: This is the main project overview README file that would be visible on a GitHub repository.
*   **Content**: Provides a high-level overview of the "Multi-Effect VST Plugin", lists its key features (distortion, stereo widening, delay, modulation blending, MIDI control, waveform visualization), and gives basic installation and usage instructions. It references the MIT License.

### `StereoWidening.cpp`

*   **Role**: Implements the `StereoWidening` class defined in `StereoWidening.h`.

### `filelist.md`

*   **Role**: Provides a **directory structure and list of source files** within the project. It helps in understanding the organization of the codebase.

### `Documentation/DeploymentOptimization.md`

*   **Role**: Discusses **final optimization steps** for a VST plugin before deployment.
*   **Content**: Covers CPU efficiency (pre-allocated buffers), seamless parameter automation, MIDI/DAW sync, and low-latency processing. It provides a *complete code example* of a simplified `MultiEffectProcessor` with basic distortion, delay, and stereo widening integrated directly into the `processBlock`.

### `Documentation/Multi-effect blending.md`

*   **Role**: Explores techniques for **blending multiple effects** within a plugin.
*   **Content**: Details parallel processing (splitting signals for simultaneous effects), cross-blending (dynamic modulation of wet/dry mix), and dynamic effect interaction (e.g., delay feedback linked to distortion saturation). Includes C++ code snippets for each concept.

### `Documentation/VST_Multi_Guitar_FX.md`

*   **Role**: Lays out the **architecture for a multi-effect VST plugin**.
*   **Content**: Describes the plugin's components (processing core, parameter management, GUI, buffer handling), emphasizes JUCE as the framework. Provides C++ snippets for a `MultiEffectProcessor` integrating basic distortion, compression, chorus, and delay, and a `MultiEffectEditor` for GUI. Also touches on performance optimizations like SIMD and parameter smoothing.

### `Documentation/addingmorefx.md`

*   **Role**: Expands the plugin's capabilities by **adding more effects** and detailing **preset system enhancements** and **advanced GUI refinements**.
*   **Content**: Provides C++ examples for Pitch Shifter (octave-up), Tremolo, Bitcrusher, and Reverb (Schroeder algorithm). Discusses using `juce::ValueTree` for preset management (saving/loading). Introduces GUI enhancements like a VU Meter and custom knobs.

### `Documentation/additionalfx.md`

*   **Role**: Further expands the effects palette and preset system with **more sophisticated effect variations**.
*   **Content**: Introduces Dynamic Distortion (playing-responsive overdrive), Analog Warmth (saturation/tape emulation), and Adaptive Pitch Shifting (responsive harmonization). Continues to detail the `ValueTree`-based preset system for full parameter recall and suggests category-based presets.

### `Documentation/interactivemidi.md`

*   **Role**: Focuses on **enhancing the user experience** through visual feedback and external control.
*   **Content**: Explains how to implement an `Interactive Waveform Display` (oscilloscope) for real-time audio visualization using JUCE components. Details MIDI control integration, mapping MIDI CC messages to plugin parameters within the `processBlock`.

### `Documentation/midiswitch.md`

*   **Role**: Expands on **MIDI integration**, specifically for **preset switching** and **dynamic control**.
*   **Content**: Describes how to assign MIDI Program Change messages to recall specific presets. Discusses defining expanded preset categories and implementing adaptive envelope-based effect modulation and MIDI expression pedal mapping for dynamic control.

### `Documentation/performance.md`

*   **Role**: Delves into **advanced performance optimizations** and **multi-channel processing**.
*   **Content**: Covers crucial techniques for low-latency DSP: avoiding denormalization issues (flush-to-zero), utilizing SIMD processing (SSE/AVX intrinsics) for parallel computation, and oversampling to reduce aliasing. Also explains how to handle stereo, surround, and multi-bus processing efficiently.

### `Documentation/presetvisualization.md`

*   **Role**: Details **GUI refinements specifically for preset management** and **custom effect chaining options**.
*   **Content**: Describes UI elements like preset selection dropdowns, visual preset indicators, and interactive save/load buttons. Introduces the concept of dynamic effect bypass toggles and modular effect ordering within the plugin's signal flow, allowing user customization of the effects chain.

### `Documentation/readme.md` (in Documentation folder)

*   **Role**: This is a very extensive **compendium on DSP theory and C++ implementation of guitar effects**, structured as chapters.
*   **Content**:
    *   **Chapter 1: Gain, Distortion, Overdrive & Fuzz**: Deep dive into non-linear waveshaping, mathematical models (hard, soft, asymmetrical clipping), structured C++ implementations, optimization strategies (lookup tables, piecewise approximations, parameter smoothing, SIMD), and creative variations (dynamic, multi-band, octave-up, bitcrusher, tube emulation).
    *   **Chapter 2: Dynamics Processing (Compressor, Noise Gate)**: DSP concepts (envelope following, gain reduction, attack/release) and basic compressor implementation.
    *   **Chapter 3: Filter Effects (Wah, EQ)**: DSP concepts (Biquad IIR filters, sweeping resonance) and a Wah filter implementation.
    *   **Chapter 4: Modulation Effects (Chorus, Flanger, Phaser)**: DSP concepts (delay modulation, all-pass filters, interpolation) and a Chorus implementation.
    *   **Chapter 5: Time-Based Effects (Delay, Reverb)**: DSP concepts (circular buffers, feedback paths) and a simple Delay implementation.
    *   This file provides the most in-depth theoretical and practical DSP knowledge within the project.

### `Documentation/vstplugin.md`

*   **Role**: Provides a **structured guide for building a VST plugin in C++ using JUCE**, from setup to deployment.
*   **Content**: Covers environment preparation, core VST plugin architecture (`AudioProcessor`, `AudioProcessorEditor`, `AudioProcessorValueTreeState`), implementing a distortion effect, creating a basic GUI, and plugin packaging/testing. It then expands on optimizing VST integration, including efficient parameter management, buffer processing, host communication (parameter smoothing, DAW sync), and real-time performance optimizations (denormal handling, SIMD).

### `packaging.md`

*   **Role**: Details the process of **packaging the VST plugin for distribution**.
*   **Content**: Guides through building in Release mode, structuring the plugin folder for distribution, creating installers for Windows (using Inno Setup) and macOS (using Packages), and including necessary documentation. It also repeats simplified C++ code for `MultiEffectProcessor.h` and `.cpp` for context within the packaging guide.
