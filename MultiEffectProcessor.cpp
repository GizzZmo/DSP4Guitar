#include "MultiEffectProcessor.h"

MultiEffectProcessor::MultiEffectProcessor() : parameters(*this, nullptr, "PARAMS", {
    std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.0f, 5.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("delay", "Delay", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterBool>("distortionOn", "Distortion", false),
    std::make_unique<juce::AudioParameterBool>("delayOn", "Delay", false),
    std::make_unique<juce::AudioParameterBool>("wideningOn", "Stereo Widening", false)
}) {}

void MultiEffectProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    if (*parameters.getRawParameterValue("distortionOn")) 
        applyDistortion(buffer, *parameters.getRawParameterValue("drive"));

    if (*parameters.getRawParameterValue("delayOn")) 
        applyDelay(buffer, *parameters.getRawParameterValue("delay"));

    if (*parameters.getRawParameterValue("wideningOn")) 
        processStereoWidening(buffer, *parameters.getRawParameterValue("width"));
}

void MultiEffectProcessor::applyDistortion(juce::AudioBuffer<float>& buffer, float drive) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            data[i] = std::tanh(drive * data[i]);
        }
    }
}

// Implement Delay, StereoWidening, and ModulatedBlend similarly...
