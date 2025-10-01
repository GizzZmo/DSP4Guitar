#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

class Distortion {
public:
    Distortion();
    void process(juce::AudioBuffer<float>& buffer, float drive);
};
