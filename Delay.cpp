#include "Delay.h"

Delay::Delay(int maxDelaySamples) {
    delayBuffer.resize(maxDelaySamples, 0.0f);
}

void Delay::process(juce::AudioBuffer<float>& buffer, float feedback, float delayTime, float sampleRate) {
    int delaySamples = static_cast<int>(delayTime * sampleRate);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* data = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            float delayedSample = delayBuffer[(bufferIndex - delaySamples + delayBuffer.size()) % delayBuffer.size()];
            delayBuffer[bufferIndex] = data[i] + delayedSample * feedback;
            data[i] += delayedSample;
            bufferIndex = (bufferIndex + 1) % delayBuffer.size();
        }
    }
}
