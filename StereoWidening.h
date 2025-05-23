#pragma once
#include <JuceHeader.h>

class StereoWidening {
public:
    StereoWidening();
    void process(juce::AudioBuffer<float>& buffer, float widthAmount);
};
