#pragma once
#include <JuceHeader.h>

class PresetManager {
public:
    PresetManager();
    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);

private:
    juce::ValueTree presetStore{"Presets"};
};
