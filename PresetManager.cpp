#include "PresetManager.h"

PresetManager::PresetManager() {}

void PresetManager::savePreset(const juce::String& presetName) {
    auto preset = presetStore.getOrCreateChildWithName(presetName, nullptr);
    preset.setProperty("drive", 1.0f, nullptr);
    preset.setProperty("delay", 0.5f, nullptr);
}

void PresetManager::loadPreset(const juce::String& presetName) {
    auto preset = presetStore.getChildWithName(presetName);
    if (preset.isValid()) {
        float drive = preset.getProperty("drive");
        float delay = preset.getProperty("delay");
    }
}
