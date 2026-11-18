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
// 3-Band Multiband Compressor DSP class
class MultibandCompressor
{
public:
    using StereoFilter = juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        lowLP.prepare(spec);
        midHP.prepare(spec);
        midLP.prepare(spec);
        highHP.prepare(spec);

        lowComp.prepare(spec);
        midComp.prepare(spec);
        highComp.prepare(spec);

        makeupGain.prepare(spec);
        makeupGain.setRampDurationSeconds(0.01);

        int maxSamples = static_cast<int>(spec.maximumBlockSize);
        int numCh      = static_cast<int>(spec.numChannels);
        lowBuf.setSize(numCh, maxSamples);
        midBuf.setSize(numCh, maxSamples);
        highBuf.setSize(numCh, maxSamples);

        updateFilters();
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;

        const auto& inBlock = context.getInputBlock();
        auto& outBlock      = context.getOutputBlock();
        const int numSamples  = static_cast<int>(inBlock.getNumSamples());
        const int numChannels = static_cast<int>(inBlock.getNumChannels());

        // Copy input into the three band buffers
        for (int ch = 0; ch < numChannels; ++ch)
        {
            lowBuf.copyFrom(ch, 0, inBlock.getChannelPointer(ch), numSamples);
            midBuf.copyFrom(ch, 0, inBlock.getChannelPointer(ch), numSamples);
            highBuf.copyFrom(ch, 0, inBlock.getChannelPointer(ch), numSamples);
        }

        juce::dsp::AudioBlock<float> lowBlock(lowBuf.getArrayOfWritePointers(),
                                               static_cast<size_t>(numChannels),
                                               static_cast<size_t>(numSamples));
        juce::dsp::AudioBlock<float> midBlock(midBuf.getArrayOfWritePointers(),
                                               static_cast<size_t>(numChannels),
                                               static_cast<size_t>(numSamples));
        juce::dsp::AudioBlock<float> highBlock(highBuf.getArrayOfWritePointers(),
                                                static_cast<size_t>(numChannels),
                                                static_cast<size_t>(numSamples));

        juce::dsp::ProcessContextReplacing<float> lowCtx(lowBlock);
        juce::dsp::ProcessContextReplacing<float> midCtx(midBlock);
        juce::dsp::ProcessContextReplacing<float> highCtx(highBlock);

        // Crossover filtering
        lowLP.process(lowCtx);
        midHP.process(midCtx);
        midLP.process(midCtx);
        highHP.process(highCtx);

        // Compression per band
        lowComp.process(lowCtx);
        midComp.process(midCtx);
        highComp.process(highCtx);

        // Sum bands into output
        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float* low  = lowBuf.getReadPointer(ch);
            const float* mid  = midBuf.getReadPointer(ch);
            const float* high = highBuf.getReadPointer(ch);
            float* out        = outBlock.getChannelPointer(ch);
            for (int i = 0; i < numSamples; ++i)
                out[i] = low[i] + mid[i] + high[i];
        }

        // Makeup gain
        juce::dsp::ProcessContextReplacing<float> outCtx(outBlock);
        makeupGain.process(outCtx);
    }

    void reset()
    {
        lowLP.reset();  midHP.reset();  midLP.reset();  highHP.reset();
        lowComp.reset(); midComp.reset(); highComp.reset();
        makeupGain.reset();
    }

    void setLowThreshold(float dB)  { lowComp.setThreshold(dB); }
    void setMidThreshold(float dB)  { midComp.setThreshold(dB); }
    void setHighThreshold(float dB) { highComp.setThreshold(dB); }
    void setRatio(float r)          { lowComp.setRatio(r);  midComp.setRatio(r);  highComp.setRatio(r); }
    void setAttack(float ms)        { lowComp.setAttack(ms); midComp.setAttack(ms); highComp.setAttack(ms); }
    void setRelease(float ms)       { lowComp.setRelease(ms); midComp.setRelease(ms); highComp.setRelease(ms); }
    void setMakeupGain(float dB)    { makeupGain.setGainDecibels(dB); }

private:
    void updateFilters()
    {
        *lowLP.state  = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 300.0f);
        *midHP.state  = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 300.0f);
        *midLP.state  = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 3000.0f);
        *highHP.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 3000.0f);
    }

    double sampleRate = 44100.0;

    StereoFilter lowLP, midHP, midLP, highHP;
    juce::dsp::Compressor<float> lowComp, midComp, highComp;
    juce::dsp::Gain<float> makeupGain;

    juce::AudioBuffer<float> lowBuf, midBuf, highBuf;
};

//==============================================================================
// Wah-Wah (auto-wah) DSP class
class WahWah
{
public:
    WahWah() { lfo.initialise([](float x) { return std::sin(x); }, 128); }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        lfo.prepare(spec);
        for (auto& f : filters)
        {
            f.prepare(spec);
            f.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
                sampleRate, centerFreq, resonance);
        }
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;

        lfo.setFrequency(rate);
        const auto& inBlock  = context.getInputBlock();
        auto& outBlock       = context.getOutputBlock();
        const int numSamples  = static_cast<int>(inBlock.getNumSamples());
        const int numChannels = static_cast<int>(inBlock.getNumChannels());

        for (int s = 0; s < numSamples; ++s)
        {
            const float lfoVal = lfo.processSample(0.0f); // -1 to +1

            // Update filter coefficients every 8 samples to reduce CPU load
            if ((coeffCounter++ & 7) == 0)
            {
                const float freq = juce::jlimit(200.0f, 4000.0f,
                                                centerFreq + lfoVal * depth * sweepRange);
                const auto newCoeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass(
                    sampleRate, freq, resonance);
                for (auto& f : filters)
                    f.coefficients = newCoeffs;
            }

            for (int ch = 0; ch < numChannels; ++ch)
            {
                const float in       = inBlock.getSample(ch, s);
                const float filtered = filters[ch].processSample(in);
                outBlock.setSample(ch, s, (1.0f - mix) * in + mix * filtered);
            }
        }
    }

    void reset()
    {
        lfo.reset();
        for (auto& f : filters) f.reset();
        coeffCounter = 0;
    }

    void setRate(float newRate)       { rate = newRate; }
    void setDepth(float newDepth)     { depth = newDepth; }
    void setCenterFreq(float freq)    { centerFreq = freq; }
    void setResonance(float q)        { resonance = juce::jlimit(0.1f, 20.0f, q); }
    void setMix(float newMix)         { mix = newMix; }

private:
    static constexpr float sweepRange = 1500.0f;

    juce::dsp::Oscillator<float> lfo;
    std::array<juce::dsp::IIR::Filter<float>, 2> filters;

    double sampleRate = 44100.0;
    float rate        = 2.0f;
    float depth       = 0.8f;
    float centerFreq  = 1500.0f;
    float resonance   = 4.0f;
    float mix         = 1.0f;
    int   coeffCounter = 0;
};

//==============================================================================
// Fuzz DSP class
class Fuzz
{
public:
    using StereoFilter = juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        toneFilter.prepare(spec);
        outputGain.prepare(spec);
        outputGain.setRampDurationSeconds(0.01);
        updateToneFilter();
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;

        const auto& inBlock  = context.getInputBlock();
        auto& outBlock       = context.getOutputBlock();
        const int numSamples  = static_cast<int>(inBlock.getNumSamples());
        const int numChannels = static_cast<int>(inBlock.getNumChannels());

        // Apply drive + hard clip + wet/dry mix
        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float* in = inBlock.getChannelPointer(ch);
            float* out      = outBlock.getChannelPointer(ch);
            for (int s = 0; s < numSamples; ++s)
            {
                const float driven = in[s] * drive;
                const float fuzzed = juce::jlimit(-1.0f, 1.0f, driven);
                out[s] = (1.0f - mix) * in[s] + mix * fuzzed;
            }
        }

        // Tone filter + output level applied to the output block
        juce::dsp::ProcessContextReplacing<float> outCtx(outBlock);
        toneFilter.process(outCtx);
        outputGain.process(outCtx);
    }

    void reset()
    {
        toneFilter.reset();
        outputGain.reset();
    }

    void setDrive(float newDrive)   { drive = juce::jlimit(1.0f, 100.0f, newDrive); }
    void setTone(float newTone)     { tone = juce::jlimit(0.0f, 1.0f, newTone); updateToneFilter(); }
    void setLevel(float newLevelDb) { outputGain.setGainDecibels(newLevelDb); }
    void setMix(float newMix)       { mix = newMix; }

private:
    void updateToneFilter()
    {
        if (sampleRate <= 0.0) return;
        const float cutoff = 500.0f + tone * 8000.0f;
        *toneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);
    }

    double sampleRate = 44100.0;
    float drive = 20.0f;
    float tone  = 0.5f;
    float mix   = 1.0f;

    StereoFilter toneFilter;
    juce::dsp::Gain<float> outputGain;
};

//==============================================================================
//==============================================================================
// Noise Gate
class NoiseGate
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope = 0.0f;
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;
        auto& in = context.getInputBlock();
        auto& out = context.getOutputBlock();
        const int n = (int)in.getNumSamples();
        const int chs = (int)in.getNumChannels();
        const float thresh = juce::Decibels::decibelsToGain(thresholdDb);
        const float coeff = std::exp(-1.0f / (sampleRate * 0.01f)); // ~10 ms

        for (int i = 0; i < n; ++i)
        {
            float peak = 0.0f;
            for (int c = 0; c < chs; ++c)
                peak = std::max(peak, std::abs(in.getSample(c, i)));
            envelope = peak > envelope ? peak : envelope * coeff + peak * (1.0f - coeff);
            float g = envelope > thresh ? 1.0f : 0.0f;
            for (int c = 0; c < chs; ++c)
                out.setSample(c, i, in.getSample(c, i) * g);
        }
    }

    void reset() { envelope = 0.0f; }
    void setThreshold(float db) { thresholdDb = db; }

private:
    double sampleRate = 44100.0;
    float thresholdDb = -40.0f;
    float envelope = 0.0f;
};

//==============================================================================
// Soft Overdrive
class Overdrive
{
public:
    void prepare(const juce::dsp::ProcessSpec&) {}

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;
        auto& in = context.getInputBlock();
        auto& out = context.getOutputBlock();
        const int n = (int)in.getNumSamples();
        const int chs = (int)in.getNumChannels();

        for (int c = 0; c < chs; ++c)
        {
            for (int i = 0; i < n; ++i)
            {
                float x = in.getSample(c, i) * drive;
                float y = std::tanh(x);
                out.setSample(c, i, (1.0f - mix) * in.getSample(c, i) + mix * y * level);
            }
        }
    }

    void reset() {}
    void setDrive(float d) { drive = juce::jlimit(1.0f, 40.0f, d); }
    void setLevel(float l) { level = juce::Decibels::decibelsToGain(l); }
    void setMix(float m) { mix = m; }

private:
    float drive = 5.0f;
    float level = 1.0f;
    float mix = 1.0f;
};

//==============================================================================
// Simple Flanger
class Flanger
{
public:
    Flanger() { lfo.initialise([](float x) { return std::sin(x); }, 128); }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        lfo.prepare(spec);
        delayLine.prepare(spec);
        delayLine.setMaximumDelayInSamples((int)(sampleRate * 0.02)); // 20 ms
    }

    template <typename ProcessContext>
    void process(const ProcessContext& context)
    {
        if (context.isBypassed) return;
        lfo.setFrequency(rate);
        auto& in = context.getInputBlock();
        auto& out = context.getOutputBlock();
        const int n = (int)in.getNumSamples();
        const int chs = (int)in.getNumChannels();

        for (int i = 0; i < n; ++i)
        {
            float lfoVal = (lfo.processSample(0.0f) + 1.0f) * 0.5f;
            float delaySamps = 1.0f + lfoVal * depth * sampleRate * 0.005f; // 0–5 ms
            for (int c = 0; c < chs; ++c)
            {
                float input = in.getSample(c, i);
                float delayed = delayLine.popSample(c, delaySamps);
                delayLine.pushSample(c, input + delayed * feedback);
                out.setSample(c, i, (1.0f - mix) * input + mix * (input + delayed));
            }
        }
    }

    void reset() { lfo.reset(); delayLine.reset(); }
    void setRate(float r) { rate = r; }
    void setDepth(float d) { depth = d; }
    void setFeedback(float f) { feedback = juce::jlimit(-0.9f, 0.9f, f); }
    void setMix(float m) { mix = m; }

private:
    juce::dsp::Oscillator<float> lfo;
    juce::dsp::DelayLine<float> delayLine { 96000 };
    double sampleRate = 44100.0;
    float rate = 0.5f;
    float depth = 0.7f;
    float feedback = 0.5f;
    float mix = 0.5f;
};

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
        NoiseGateIndex,
        OverdriveIndex,
        BitcrusherIndex,
        FuzzIndex,
        CompressorIndex,
        RingModIndex,
        WahIndex,
        PhaserIndex,
        ChorusIndex,
        TremoloIndex,
        FlangerIndex,
        DelayIndex,
        ReverbIndex,
        NumEffects
    };

    // Runtime-ordered chain (indices into ChainPositions)
    std::vector<int> chainOrder;

    // Individual DSP instances
    NoiseGate noiseGate;
    Overdrive overdrive;
    Bitcrusher bitcrusher;
    Fuzz fuzz;
    MultibandCompressor compressor;
    RingModulator ringMod;
    WahWah wah;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::Chorus<float> chorus;
    Tremolo tremolo;
    Flanger flanger;
    juce::dsp::DelayLine<float> delayLine { 96000 * 2 };
    juce::dsp::Reverb reverb;

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

    juce::AudioParameterBool*  compressorOn = nullptr;
    juce::AudioParameterFloat* compressorLowThresh = nullptr;
    juce::AudioParameterFloat* compressorMidThresh = nullptr;
    juce::AudioParameterFloat* compressorHighThresh = nullptr;
    juce::AudioParameterFloat* compressorRatio = nullptr;
    juce::AudioParameterFloat* compressorAttack = nullptr;
    juce::AudioParameterFloat* compressorRelease = nullptr;
    juce::AudioParameterFloat* compressorMakeup = nullptr;

    juce::AudioParameterBool*  wahOn = nullptr;
    juce::AudioParameterFloat* wahRate = nullptr;
    juce::AudioParameterFloat* wahDepth = nullptr;
    juce::AudioParameterFloat* wahFreq = nullptr;
    juce::AudioParameterFloat* wahResonance = nullptr;
    juce::AudioParameterFloat* wahMix = nullptr;

    
    juce::AudioParameterBool*  noiseGateOn = nullptr;
    juce::AudioParameterFloat* noiseGateThreshold = nullptr;

    juce::AudioParameterBool*  overdriveOn = nullptr;
    juce::AudioParameterFloat* overdriveDrive = nullptr;
    juce::AudioParameterFloat* overdriveLevel = nullptr;
    juce::AudioParameterFloat* overdriveMix = nullptr;

    juce::AudioParameterBool*  flangerOn = nullptr;
    juce::AudioParameterFloat* flangerRate = nullptr;
    juce::AudioParameterFloat* flangerDepth = nullptr;
    juce::AudioParameterFloat* flangerFeedback = nullptr;
    juce::AudioParameterFloat* flangerMix = nullptr;
    juce::AudioParameterBool*  fuzzOn = nullptr;
    juce::AudioParameterFloat* fuzzDrive = nullptr;
    juce::AudioParameterFloat* fuzzTone = nullptr;
    juce::AudioParameterFloat* fuzzLevel = nullptr;
    juce::AudioParameterFloat* fuzzMix = nullptr;

    void updateParameters();
    void processEffect(int index, juce::AudioBuffer<float>& buffer);
    void setChainOrder(const std::vector<int>& order);
    const std::vector<int>& getChainOrder() const { return chainOrder; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiEffectProcessor)
};
