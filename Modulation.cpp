#include "Modulation.h"

Modulation::Modulation() {}

void Modulation::process(juce::AudioBuffer<float>& buffer, float depth, float rate, float sampleRate) {
    float phaseIncrement = (2.0f * M_PI * rate) / sampleRate;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            float lfoValue = (1.0f - depth) + depth * std::sin(phase);
            data[i] *= lfoValue;
            phase += phaseIncrement;
            if (phase > 2.0f * M_PI) phase -= 2.0f * M_PI;
        }
    }
}
