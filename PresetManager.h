#pragma once
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <vector>
#include <map>
#include <tuple>

class PresetManager {
public:
    PresetManager();
    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);

    static const std::vector<std::tuple<juce::String, juce::String, std::map<juce::String, float>>>& getFactoryPresets();

private:
    juce::ValueTree presetStore{"Presets"};
};
