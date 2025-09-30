cmake_minimum_required(VERSION 3.15)

project(DSP4Guitar VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(JUCE)

# Create the plugin target
juce_add_plugin(DSP4Guitar
    COMPANY_NAME "GizzZmo"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Gzzm
    PLUGIN_CODE Dsp4
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "DSP4Guitar"
)

# Add source files
target_sources(DSP4Guitar
    PRIVATE
        MultiEffectProcessor.cpp
        MultiEffectProcessor.h
        PluginEditor.h
        Delay.cpp
        Delay.h
        Distortion.cpp
        Distortion.h
        Modulation.cpp
        Modulation.h
        PresetManager.cpp
        PresetManager.h
        StereoWidening.cpp
        StereoWidening.h
        DSP4GuitarApp.h
)

# Include current directory for headers
target_include_directories(DSP4Guitar
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link JUCE modules
target_link_libraries(DSP4Guitar
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions
target_compile_definitions(DSP4Guitar
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
