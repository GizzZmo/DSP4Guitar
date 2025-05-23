#pragma once
#include <JuceHeader.h>

class Modulation {
public:
    Modulation();
    void process(juce::AudioBuffer<float>& buffer, float depth, float rate, float sampleRate);

private:
    float phase = 0.0f;
};
