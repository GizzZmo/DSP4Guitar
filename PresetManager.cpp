#include "PresetManager.h"
#include "MultiEffectProcessor.h"

PresetManager::PresetManager()
{
    // Factory presets stored as ValueTree properties (parameter ID -> value)
    // These are loaded by the editor into the APVTS.
}

void PresetManager::savePreset(const juce::String& presetName)
{
    // Existing implementation placeholder
}

void PresetManager::loadPreset(const juce::String& presetName)
{
    // Existing implementation placeholder
}

// Static factory presets (name, category, map of paramID -> float)
const std::vector<std::tuple<juce::String, juce::String, std::map<juce::String, float>>>& PresetManager::getFactoryPresets()
{
    static const std::vector<std::tuple<juce::String, juce::String, std::map<juce::String, float>>> presets = {
        { "Clean Sparkle", "Clean", {
            {"noiseGateOn", 1.0f}, {"noiseGateThreshold", -35.0f},
            {"overdriveOn", 0.0f}, {"fuzzOn", 0.0f},
            {"chorusOn", 1.0f}, {"chorusRate", 1.2f}, {"chorusDepth", 0.4f}, {"chorusMix", 0.35f},
            {"reverbOn", 1.0f}, {"reverbRoomSize", 0.4f}, {"reverbWetLevel", 0.25f}
        }},
        { "Crunch Rhythm", "Drive", {
            {"overdriveOn", 1.0f}, {"overdriveDrive", 8.0f}, {"overdriveMix", 0.8f},
            {"compressorOn", 1.0f}, {"compressorRatio", 4.0f},
            {"delayOn", 1.0f}, {"delayTime", 375.0f}, {"delayFeedback", 0.35f}, {"delayMix", 0.3f}
        }},
        { "Lead Sustain", "Lead", {
            {"noiseGateOn", 1.0f}, {"overdriveOn", 1.0f}, {"overdriveDrive", 18.0f},
            {"fuzzOn", 0.0f}, {"compressorOn", 1.0f}, {"compressorMakeup", 3.0f},
            {"delayOn", 1.0f}, {"delayTime", 450.0f}, {"delayMix", 0.4f},
            {"reverbOn", 1.0f}, {"reverbWetLevel", 0.3f}
        }},
        { "Fuzz Face", "Fuzz", {
            {"fuzzOn", 1.0f}, {"fuzzDrive", 35.0f}, {"fuzzTone", 0.4f}, {"fuzzMix", 1.0f},
            {"noiseGateOn", 1.0f}, {"noiseGateThreshold", -30.0f}
        }},
        { "Ambient Wash", "Ambient", {
            {"reverbOn", 1.0f}, {"reverbRoomSize", 0.85f}, {"reverbWetLevel", 0.6f}, {"reverbDamping", 0.3f},
            {"delayOn", 1.0f}, {"delayTime", 600.0f}, {"delayFeedback", 0.55f}, {"delayMix", 0.45f},
            {"chorusOn", 1.0f}, {"chorusMix", 0.5f}, {"flangerOn", 1.0f}, {"flangerMix", 0.3f}
        }},
        { "Wah Solo", "Modulation", {
            {"wahOn", 1.0f}, {"wahRate", 3.5f}, {"wahDepth", 0.9f}, {"wahFreq", 1200.0f}, {"wahResonance", 6.0f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 12.0f}
        }},
        { "Phaser Sweep", "Modulation", {
            {"phaserOn", 1.0f}, {"phaserRate", 0.8f}, {"phaserDepth", 0.7f}, {"phaserFeedback", 0.4f}, {"phaserMix", 0.6f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 6.0f}
        }},
        { "Flanger Jet", "Modulation", {
            {"flangerOn", 1.0f}, {"flangerRate", 0.3f}, {"flangerDepth", 0.8f}, {"flangerFeedback", 0.7f}, {"flangerMix", 0.6f}
        }},
        { "Lo-Fi Crush", "Lo-Fi", {
            {"bitcrusherOn", 1.0f}, {"bitcrusherDepth", 6.0f}, {"bitcrusherRate", 4.0f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 10.0f}, {"overdriveMix", 0.7f}
        }},
        { "Tremolo Pulse", "Modulation", {
            {"tremoloOn", 1.0f}, {"tremoloRate", 5.0f}, {"tremoloDepth", 0.6f},
            {"reverbOn", 1.0f}, {"reverbWetLevel", 0.2f}
        }},
        { "Gate & Boost", "Utility", {
            {"noiseGateOn", 1.0f}, {"noiseGateThreshold", -25.0f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 3.0f}, {"overdriveLevel", 6.0f}, {"overdriveMix", 1.0f}
        }},
        { "Wide Chorus", "Clean", {
            {"chorusOn", 1.0f}, {"chorusRate", 0.8f}, {"chorusDepth", 0.5f}, {"chorusMix", 0.6f},
            {"reverbOn", 1.0f}, {"reverbWidth", 1.0f}, {"reverbWetLevel", 0.35f}
        }},
        { "Ring Mod Sci-Fi", "Experimental", {
            {"ringModOn", 1.0f}, {"ringModRate", 220.0f}, {"ringModDepth", 0.8f},
            {"reverbOn", 1.0f}, {"reverbWetLevel", 0.4f}
        }},
        { "Multiband Glue", "Dynamics", {
            {"compressorOn", 1.0f}, {"compressorLowThresh", -18.0f}, {"compressorMidThresh", -22.0f},
            {"compressorHighThresh", -20.0f}, {"compressorRatio", 3.0f}, {"compressorMakeup", 2.0f}
        }},
        { "Slapback Delay", "Time", {
            {"delayOn", 1.0f}, {"delayTime", 120.0f}, {"delayFeedback", 0.2f}, {"delayMix", 0.4f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 7.0f}
        }},
        { "Big Room", "Ambient", {
            {"reverbOn", 1.0f}, {"reverbRoomSize", 0.95f}, {"reverbWetLevel", 0.55f}, {"reverbDryLevel", 0.5f},
            {"delayOn", 1.0f}, {"delayTime", 800.0f}, {"delayFeedback", 0.6f}, {"delayMix", 0.35f}
        }},
        { "Vintage Fuzz", "Fuzz", {
            {"fuzzOn", 1.0f}, {"fuzzDrive", 25.0f}, {"fuzzTone", 0.6f}, {"fuzzLevel", -3.0f},
            {"noiseGateOn", 1.0f}
        }},
        { "Auto Wah Funk", "Modulation", {
            {"wahOn", 1.0f}, {"wahRate", 4.0f}, {"wahDepth", 0.75f}, {"wahResonance", 5.0f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 9.0f}
        }},
        { "Clean Delay", "Clean", {
            {"delayOn", 1.0f}, {"delayTime", 500.0f}, {"delayFeedback", 0.3f}, {"delayMix", 0.35f},
            {"reverbOn", 1.0f}, {"reverbWetLevel", 0.2f}, {"chorusOn", 1.0f}, {"chorusMix", 0.25f}
        }},
        { "High Gain Lead", "Lead", {
            {"noiseGateOn", 1.0f}, {"noiseGateThreshold", -28.0f},
            {"overdriveOn", 1.0f}, {"overdriveDrive", 22.0f}, {"overdriveMix", 1.0f},
            {"compressorOn", 1.0f}, {"compressorRatio", 6.0f}, {"compressorMakeup", 4.0f},
            {"delayOn", 1.0f}, {"delayTime", 400.0f}, {"delayMix", 0.35f},
            {"reverbOn", 1.0f}, {"reverbWetLevel", 0.25f}
        }}
    };
    return presets;
}
