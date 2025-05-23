#pragma once
#include <JuceHeader.h>

class Delay {
public:
    Delay(int maxDelaySamples);
    void process(juce::AudioBuffer<float>& buffer, float feedback, float delayTime, float sampleRate);

private:
    std::vector<float> delayBuffer;
    int bufferIndex = 0;
};
