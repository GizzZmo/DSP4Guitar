#include "MultiEffectProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultiEffectProcessor::MultiEffectProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Get raw pointers for quick access
    bitcrusherOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bitcrusherOn"));
    bitcrusherDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("bitcrusherDepth"));
    bitcrusherRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("bitcrusherRate"));

    ringModOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("ringModOn"));
    ringModRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("ringModRate"));
    ringModDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("ringModDepth"));
    
    phaserOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("phaserOn"));
    phaserRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("phaserRate"));
    phaserDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("phaserDepth"));
    phaserFeedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("phaserFeedback"));
    phaserMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("phaserMix"));
    
    chorusOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("chorusOn"));
    chorusRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("chorusRate"));
    chorusDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("chorusDepth"));
    chorusMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("chorusMix"));

    tremoloOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("tremoloOn"));
    tremoloRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("tremoloRate"));
    tremoloDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("tremoloDepth"));

    delayOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("delayOn"));
    delayTime = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("delayTime"));
    delayFeedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("delayFeedback"));
    delayMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("delayMix"));

    reverbOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("reverbOn"));
    reverbRoomSize = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbRoomSize"));
    reverbDamping = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbDamping"));
    reverbWetLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbWetLevel"));
    reverbDryLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbDryLevel"));
    reverbWidth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbWidth"));

    compressorOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("compressorOn"));
    compressorLowThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorLowThresh"));
    compressorMidThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorMidThresh"));
    compressorHighThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorHighThresh"));
    compressorRatio = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorRatio"));
    compressorAttack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorAttack"));
    compressorRelease = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorRelease"));
    compressorMakeup = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("compressorMakeup"));

    wahOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("wahOn"));
    wahRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("wahRate"));
    wahDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("wahDepth"));
    wahFreq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("wahFreq"));
    wahResonance = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("wahResonance"));
    wahMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("wahMix"));

    fuzzOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("fuzzOn"));
    fuzzDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fuzzDrive"));
    fuzzTone = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fuzzTone"));
    fuzzLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fuzzLevel"));
    fuzzMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("fuzzMix"));
}

MultiEffectProcessor::~MultiEffectProcessor() {}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MultiEffectProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // --- Bitcrusher ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("bitcrusherOn", "Bitcrusher On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bitcrusherDepth", "Bit Depth", juce::NormalisableRange<float>(2.0f, 16.0f, 1.0f), 8.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bitcrusherRate", "Downsample", juce::NormalisableRange<float>(1.0f, 100.0f, 1.0f), 1.0f));

    // --- Ring Mod ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("ringModOn", "Ring Mod On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ringModRate", "RM Rate", juce::NormalisableRange<float>(20.0f, 5000.0f, 1.0f, 0.3f), 440.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ringModDepth", "RM Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // --- Phaser ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("phaserOn", "Phaser On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("phaserRate", "Phaser Rate", juce::NormalisableRange<float>(0.05f, 8.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("phaserDepth", "Phaser Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("phaserFeedback", "Phaser Fbk", juce::NormalisableRange<float>(-0.9f, 0.9f, 0.01f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("phaserMix", "Phaser Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // --- Chorus ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("chorusOn", "Chorus On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // --- Tremolo ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("tremoloOn", "Tremolo On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tremoloRate", "Tremolo Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tremoloDepth", "Tremolo Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // --- Delay ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("delayOn", "Delay On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time (ms)", juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f), 300.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Delay Fbk", juce::NormalisableRange<float>(0.0f, 0.98f, 0.01f), 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // --- Reverb ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("reverbOn", "Reverb On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "Room Size", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Damping", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbWetLevel", "Wet Level", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.33f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbDryLevel", "Dry Level", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbWidth", "Width", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // --- Multiband Compressor ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("compressorOn", "Compressor On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorLowThresh", "Lo Thresh", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorMidThresh", "Mid Thresh", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorHighThresh", "Hi Thresh", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorRatio", "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorAttack", "Attack (ms)", juce::NormalisableRange<float>(1.0f, 200.0f, 1.0f, 0.4f), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorRelease", "Release (ms)", juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compressorMakeup", "Makeup (dB)", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));

    // --- Wah Wah ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("wahOn", "Wah On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wahRate", "Wah Rate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wahDepth", "Wah Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wahFreq", "Wah Freq", juce::NormalisableRange<float>(300.0f, 3000.0f, 1.0f), 1500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wahResonance", "Wah Q", juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wahMix", "Wah Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // --- Fuzz ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("fuzzOn", "Fuzz On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fuzzDrive", "Fuzz Drive", juce::NormalisableRange<float>(1.0f, 100.0f, 0.1f, 0.3f), 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fuzzTone", "Fuzz Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fuzzLevel", "Fuzz Level", juce::NormalisableRange<float>(-20.0f, 20.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fuzzMix", "Fuzz Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
void MultiEffectProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    effectChain.prepare(spec);

    // Prepare Delay Line (needs specific setup)
    auto& delayLine = effectChain.get<DelayIndex>();
    delayLine.setMaximumDelayInSamples(int(sampleRate * 2.0)); // Max 2 seconds delay
    delayLine.prepare(spec);

    updateParameters(); // Set initial values
}

void MultiEffectProcessor::releaseResources() {}

bool MultiEffectProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    return mainInput.size() > 0 && mainOutput.size() > 0 && mainInput == mainOutput;
}

void MultiEffectProcessor::updateParameters()
{
    // --- Bitcrusher ---
    auto& bitcrusher = effectChain.get<BitcrusherIndex>();
    bitcrusher.setBitDepth(bitcrusherDepth->get());
    bitcrusher.setRate(bitcrusherRate->get());
    effectChain.setBypassed<BitcrusherIndex>(!bitcrusherOn->get());

    // --- Ring Mod ---
    auto& ringMod = effectChain.get<RingModIndex>();
    ringMod.setRate(ringModRate->get());
    ringMod.setDepth(ringModDepth->get());
    effectChain.setBypassed<RingModIndex>(!ringModOn->get());

    // --- Phaser ---
    auto& phaser = effectChain.get<PhaserIndex>();
    phaser.setRate(phaserRate->get());
    phaser.setDepth(phaserDepth->get());
    phaser.setFeedback(phaserFeedback->get());
    phaser.setMix(phaserMix->get());
    effectChain.setBypassed<PhaserIndex>(!phaserOn->get());

    // --- Chorus ---
    auto& chorus = effectChain.get<ChorusIndex>();
    chorus.setRate(chorusRate->get());
    chorus.setDepth(chorusDepth->get());
    chorus.setMix(chorusMix->get());
    effectChain.setBypassed<ChorusIndex>(!chorusOn->get());

    // --- Tremolo ---
    auto& tremolo = effectChain.get<TremoloIndex>();
    tremolo.setRate(tremoloRate->get());
    tremolo.setDepth(tremoloDepth->get());
    effectChain.setBypassed<TremoloIndex>(!tremoloOn->get());

    // --- Delay ---
    // Delay needs a more complex update (handle feedback, mix) - this is simplified
    auto& delayLine = effectChain.get<DelayIndex>();
    // We can't directly set params on DelayLine. We need a custom DSP class for full delay.
    // This is a placeholder - a real delay needs feedback and mix logic here.
    effectChain.setBypassed<DelayIndex>(!delayOn->get());


    // --- Reverb ---
    auto& reverb = effectChain.get<ReverbIndex>();
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = reverbRoomSize->get();
    reverbParams.damping = reverbDamping->get();
    reverbParams.wetLevel = reverbWetLevel->get();
    reverbParams.dryLevel = reverbDryLevel->get();
    reverbParams.width = reverbWidth->get();
    reverb.setParameters(reverbParams);
    effectChain.setBypassed<ReverbIndex>(!reverbOn->get());

    // --- Multiband Compressor ---
    auto& comp = effectChain.get<CompressorIndex>();
    comp.setLowThreshold(compressorLowThresh->get());
    comp.setMidThreshold(compressorMidThresh->get());
    comp.setHighThreshold(compressorHighThresh->get());
    comp.setRatio(compressorRatio->get());
    comp.setAttack(compressorAttack->get());
    comp.setRelease(compressorRelease->get());
    comp.setMakeupGain(compressorMakeup->get());
    effectChain.setBypassed<CompressorIndex>(!compressorOn->get());

    // --- Wah Wah ---
    auto& wah = effectChain.get<WahIndex>();
    wah.setRate(wahRate->get());
    wah.setDepth(wahDepth->get());
    wah.setCenterFreq(wahFreq->get());
    wah.setResonance(wahResonance->get());
    wah.setMix(wahMix->get());
    effectChain.setBypassed<WahIndex>(!wahOn->get());

    // --- Fuzz ---
    auto& fuzzProc = effectChain.get<FuzzIndex>();
    fuzzProc.setDrive(fuzzDrive->get());
    fuzzProc.setTone(fuzzTone->get());
    fuzzProc.setLevel(fuzzLevel->get());
    fuzzProc.setMix(fuzzMix->get());
    effectChain.setBypassed<FuzzIndex>(!fuzzOn->get());

}

void MultiEffectProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateParameters(); // Update DSPs before processing

    // --- Simple Delay Implementation (Manual within processBlock for now) ---
    // A proper implementation would use a custom DSP class inside the chain.
    float dTime = delayTime->get();
    float dFbk = delayFeedback->get();
    float dMix = delayMix->get();
    bool dOn = delayOn->get();
    auto& delayLine = effectChain.get<DelayIndex>();
    int delaySamples = int(getSampleRate() * dTime / 1000.0);
    
    // --- Create DSP Context ---
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // --- Process through the chain ---
    effectChain.process(context);

    // --- Manual Delay Processing (After chain for now) ---
    if (dOn)
    {
       auto numSamples = buffer.getNumSamples();
       for (int channel = 0; channel < totalNumInputChannels; ++channel)
       {
           auto* channelData = buffer.getWritePointer(channel);
           for (int sample = 0; sample < numSamples; ++sample)
           {
               float delayedSample = delayLine.popSample(channel);
               float inputSample = channelData[sample];
               delayLine.pushSample(channel, inputSample + (delayedSample * dFbk));
               channelData[sample] = (inputSample * (1.0f - dMix)) + (delayedSample * dMix);
           }
       }
    }
}

//==============================================================================
// Standard JUCE boilerplate (getName, acceptsMidi, etc.)
const juce::String MultiEffectProcessor::getName() const { return JucePlugin_Name; }
bool MultiEffectProcessor::acceptsMidi() const { return false; }
bool MultiEffectProcessor::producesMidi() const { return false; }
bool MultiEffectProcessor::isMidiEffect() const { return false; }
double MultiEffectProcessor::getTailLengthSeconds() const { return 2.0; } // Account for Delay/Reverb
int MultiEffectProcessor::getNumPrograms() { return 1; }
int MultiEffectProcessor::getCurrentProgram() { return 0; }
void MultiEffectProcessor::setCurrentProgram(int index) {}
const juce::String MultiEffectProcessor::getProgramName(int index) { return {}; }
void MultiEffectProcessor::changeProgramName(int index, const juce::String& newName) {}
bool MultiEffectProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* MultiEffectProcessor::createEditor()
{
    return new MultiEffectProcessorEditor(*this);
}
void MultiEffectProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
void MultiEffectProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiEffectProcessor();
}
