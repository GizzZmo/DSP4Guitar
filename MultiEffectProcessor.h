#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
// Simple Ring Modulator DSP class
class RingModulator
{
public:
    RingModulator() { lfo.initialise([](float x) { return std::sin(x); }, 128); }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        lfo.prepare(spec);
        sampleRate = spec.sampleRate;
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;

        lfo.setFrequency(rate);
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = inputBlock.getNumSamples();
        auto numChannels = inputBlock.getNumChannels();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float lfoSample = (lfo.processSample(0.0f) * depth) + (1.0f - depth); // Mix LFO
            for (int channel = 0; channel < numChannels; ++channel)
            {
                float inputSample = inputBlock.getSample(channel, sample);
                outputBlock.setSample(channel, sample, inputSample * lfoSample);
            }
        }
    }

    void reset() { lfo.reset(); }
    void setRate(float newRate) { rate = newRate; }
    void setDepth(float newDepth) { depth = newDepth; }

private:
    juce::dsp::Oscillator<float> lfo;
    float rate = 500.0f;
    float depth = 1.0f;
    double sampleRate = 44100.0;
};

//==============================================================================
// Simple Bitcrusher DSP class
class Bitcrusher
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) { }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;

        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = inputBlock.getNumSamples();
        auto numChannels = inputBlock.getNumChannels();

        float step = 1.0f / (std::pow(2.0f, bitDepth) - 1.0f);
        float invStep = 1.0f / step;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            holdCounter--;
            if (holdCounter <= 0)
            {
                holdCounter = rate;
                for (int channel = 0; channel < numChannels; ++channel)
                {
                   float inputSample = inputBlock.getSample(channel, sample);
                   // Quantize (Bit Depth)
                   float crushedSample = step * std::floor(inputSample * invStep + 0.5f);
                   lastSample[channel] = crushedSample;
                }
            }

            for (int channel = 0; channel < numChannels; ++channel)
            {
                outputBlock.setSample(channel, sample, lastSample[channel]);
            }
        }
    }

    void reset() { std::fill(lastSample.begin(), lastSample.end(), 0.0f); }
    void setBitDepth(float newDepth) { bitDepth = juce::jlimit(1.0f, 16.0f, newDepth); }
    void setRate(float newRate) { rate = juce::jlimit(1.0f, 100.0f, newRate); } // Downsampling Factor

private:
    float bitDepth = 8.0f;
    float rate = 1.0f; // 1 = no downsampling, >1 = downsampling
    int holdCounter = 1;
    std::array<float, 2> lastSample = {0.0f, 0.0f};
};

//==============================================================================
// Simple Tremolo DSP class
class Tremolo
{
public:
    Tremolo() { lfo.initialise([](float x) { return std::sin(x); }, 128); }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        lfo.prepare(spec);
        gain.prepare(spec);
        gain.setRampDurationSeconds(0.02); // Avoid clicks
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
         if (context.isBypassed) return;

        lfo.setFrequency(rate);
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = inputBlock.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float lfoSample = (lfo.processSample(0.0f) + 1.0f) * 0.5f; // 0 to 1
            float gainValue = 1.0f - (depth * lfoSample);
            gain.setGainLinear(gainValue);
            gain.processSample(inputBlock.getSample(0, sample)); // Just to advance gain processor
        }
        
        juce::dsp::ProcessContextReplacing<float> gainContext(outputBlock);
        gainContext.isBypassed = context.isBypassed;
        outputBlock.copyFrom(inputBlock); // Copy input to output first
        gain.process(gainContext); // Apply gain

    }

    void reset() { lfo.reset(); gain.reset(); }
    void setRate(float newRate) { rate = newRate; }
    void setDepth(float newDepth) { depth = newDepth; }

private:
    juce::dsp::Oscillator<float> lfo;
    juce::dsp::Gain<float> gain;
    float rate = 5.0f;
    float depth = 0.5f;
};

//==============================================================================
class MultiEffectProcessor : public juce::AudioProcessor
{
public:
    MultiEffectProcessor();
    ~MultiEffectProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:
    // Define Effect Chain Order
    enum ChainPositions
    {
        BitcrusherIndex,
        RingModIndex,
        PhaserIndex,
        ChorusIndex,
        TremoloIndex,
        DelayIndex,
        ReverbIndex,
        // Add more if needed
        NumEffects
    };

    // Use ProcessorChain for easier management
    using EffectChain = juce::dsp::ProcessorChain<
        Bitcrusher,
        RingModulator,
        juce::dsp::Phaser<float>,
        juce::dsp::Chorus<float>,
        Tremolo,
        juce::dsp::DelayLine<float>, // Simplified Tape Delay
        juce::dsp::Reverb>;         // Basic Reverb

    EffectChain effectChain;

    // Parameters (pointers for quick access in processBlock)
    juce::AudioParameterBool* bitcrusherOn = nullptr;
    juce::AudioParameterFloat* bitcrusherDepth = nullptr;
    juce::AudioParameterFloat* bitcrusherRate = nullptr;

    juce::AudioParameterBool* ringModOn = nullptr;
    juce::AudioParameterFloat* ringModRate = nullptr;
    juce::AudioParameterFloat* ringModDepth = nullptr;

    juce::AudioParameterBool* phaserOn = nullptr;
    juce::AudioParameterFloat* phaserRate = nullptr;
    juce::AudioParameterFloat* phaserDepth = nullptr;
    juce::AudioParameterFloat* phaserFeedback = nullptr;
    juce::AudioParameterFloat* phaserMix = nullptr;

    juce::AudioParameterBool* chorusOn = nullptr;
    juce::AudioParameterFloat* chorusRate = nullptr;
    juce::AudioParameterFloat* chorusDepth = nullptr;
    juce::AudioParameterFloat* chorusMix = nullptr;

    juce::AudioParameterBool* tremoloOn = nullptr;
    juce::AudioParameterFloat* tremoloRate = nullptr;
    juce::AudioParameterFloat* tremoloDepth = nullptr;
    
    juce::AudioParameterBool* delayOn = nullptr;
    juce::AudioParameterFloat* delayTime = nullptr;
    juce::AudioParameterFloat* delayFeedback = nullptr;
    juce::AudioParameterFloat* delayMix = nullptr;

    juce::AudioParameterBool* reverbOn = nullptr;
    juce::AudioParameterFloat* reverbRoomSize = nullptr;
    juce::AudioParameterFloat* reverbDamping = nullptr;
    juce::AudioParameterFloat* reverbWetLevel = nullptr;
    juce::AudioParameterFloat* reverbDryLevel = nullptr;
    juce::AudioParameterFloat* reverbWidth = nullptr;

    void updateParameters(); // Function to update DSP based on APVTS

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiEffectProcessor)
};
