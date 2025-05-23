#include "Distortion.h"

Distortion::Distortion() {}

void Distortion::process(juce::AudioBuffer<float>& buffer, float drive) {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            data[i] = std::tanh(drive * data[i]); // Soft-clipping distortion
        }
    }
}
