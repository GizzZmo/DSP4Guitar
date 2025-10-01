#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class Modulation {
public:
    Modulation();
    void process(juce::AudioBuffer<float>& buffer, float depth, float rate, float sampleRate);

private:
    float phase = 0.0f;
};
