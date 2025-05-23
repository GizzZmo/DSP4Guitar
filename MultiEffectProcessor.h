#pragma once
#include <JuceHeader.h>

class MultiEffectProcessor : public juce::AudioProcessor {
public:
    MultiEffectProcessor();
    ~MultiEffectProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

private:
    juce::AudioProcessorValueTreeState parameters;
    void applyDistortion(juce::AudioBuffer<float>& buffer, float drive);
    void applyDelay(juce::AudioBuffer<float>& buffer, float feedback);
    void processStereoWidening(juce::AudioBuffer<float>& buffer, float widthAmount);
    void processModulatedBlend(juce::AudioBuffer<float>& buffer, float modulationDepth, float modulationRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiEffectProcessor)
};
