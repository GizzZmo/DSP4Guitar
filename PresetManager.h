#pragma once
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

class PresetManager {
public:
    PresetManager();
    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);

private:
    juce::ValueTree presetStore{"Presets"};
};
