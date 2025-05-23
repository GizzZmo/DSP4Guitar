#pragma once
#include <JuceHeader.h>

class Distortion {
public:
    Distortion();
    void process(juce::AudioBuffer<float>& buffer, float drive);
};
