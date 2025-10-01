#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class StereoWidening {
public:
    StereoWidening();
    void process(juce::AudioBuffer<float>& buffer, float widthAmount);
};
