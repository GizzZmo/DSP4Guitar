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
    delaySync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("delaySync"));
    delayNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("delayNote"));
    tremoloSync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("tremoloSync"));
    tremoloNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("tremoloNote"));
    phaserSync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("phaserSync"));
    phaserNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("phaserNote"));
    chorusSync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("chorusSync"));
    chorusNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("chorusNote"));
    flangerSync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("flangerSync"));
    flangerNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("flangerNote"));
    wahSync = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("wahSync"));
    wahNote = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("wahNote"));

    noiseGateOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("noiseGateOn"));
    noiseGateThreshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("noiseGateThreshold"));

    overdriveOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("overdriveOn"));
    overdriveDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("overdriveDrive"));
    overdriveLevel = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("overdriveLevel"));
    overdriveMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("overdriveMix"));

    flangerOn = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("flangerOn"));
    flangerRate = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("flangerRate"));
    flangerDepth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("flangerDepth"));
    flangerFeedback = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("flangerFeedback"));
    flangerMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("flangerMix"));

    // Default chain order
    chainOrder = { NoiseGateIndex, OverdriveIndex, BitcrusherIndex, FuzzIndex,
                   CompressorIndex, RingModIndex, WahIndex, PhaserIndex,
                   ChorusIndex, TremoloIndex, FlangerIndex, DelayIndex, ReverbIndex };

}

MultiEffectProcessor::~MultiEffectProcessor() {}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout MultiEffectProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray noteDivisions { "1/1", "1/2", "1/2.", "1/4", "1/4.", "1/8", "1/8.", "1/16", "1/16.", "1/32" };


    
    // --- Noise Gate ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("noiseGateOn", "Noise Gate On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("noiseGateThreshold", "Gate Threshold", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -40.0f));

    // --- Overdrive ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("overdriveOn", "Overdrive On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("overdriveDrive", "OD Drive", juce::NormalisableRange<float>(1.0f, 40.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("overdriveLevel", "OD Level", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("overdriveMix", "OD Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // --- Flanger ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("flangerOn", "Flanger On", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("flangerRate", "Flanger Rate", juce::NormalisableRange<float>(0.05f, 10.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("flangerDepth", "Flanger Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("flangerFeedback", "Flanger Fbk", juce::NormalisableRange<float>(-0.9f, 0.9f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("flangerMix", "Flanger Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
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

    
    // --- Tempo Sync ---
    params.push_back(std::make_unique<juce::AudioParameterBool>("delaySync", "Delay Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("delayNote", "Delay Note", noteDivisions, 5)); // default 1/8

    params.push_back(std::make_unique<juce::AudioParameterBool>("tremoloSync", "Tremolo Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("tremoloNote", "Tremolo Note", noteDivisions, 5));

    params.push_back(std::make_unique<juce::AudioParameterBool>("phaserSync", "Phaser Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("phaserNote", "Phaser Note", noteDivisions, 5));

    params.push_back(std::make_unique<juce::AudioParameterBool>("chorusSync", "Chorus Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("chorusNote", "Chorus Note", noteDivisions, 5));

    params.push_back(std::make_unique<juce::AudioParameterBool>("flangerSync", "Flanger Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("flangerNote", "Flanger Note", noteDivisions, 5));

    params.push_back(std::make_unique<juce::AudioParameterBool>("wahSync", "Wah Sync", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("wahNote", "Wah Note", noteDivisions, 5));
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

    noiseGate.prepare(spec);
    overdrive.prepare(spec);
    bitcrusher.prepare(spec);
    fuzz.prepare(spec);
    compressor.prepare(spec);
    ringMod.prepare(spec);
    wah.prepare(spec);
    phaser.prepare(spec);
    chorus.prepare(spec);
    tremolo.prepare(spec);
    flanger.prepare(spec);
    delayLine.setMaximumDelayInSamples((int)(sampleRate * 2.0));
    delayLine.prepare(spec);
    reverb.prepare(spec);
}

void MultiEffectProcessor::releaseResources() {}

bool MultiEffectProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    return mainInput.size() > 0 && mainOutput.size() > 0 && mainInput == mainOutput;
}

void MultiEffectProcessor::updateParameters() { /* parameters applied in processEffect */ }

void MultiEffectProcessor::setChainOrder(const std::vector<int>& order)
{
    chainOrder = order;
}


void MultiEffectProcessor::updateTempoFromHost()
{
    if (auto* playHead = getPlayHead())
    {
        if (auto pos = playHead->getPosition())
        {
            if (pos->getBpm().hasValue())
                currentBpm = *pos->getBpm();
        }
    }
}

float MultiEffectProcessor::noteDivisionToHz(int noteIndex) const
{
    // 1/1, 1/2, 1/2., 1/4, 1/4., 1/8, 1/8., 1/16, 1/16., 1/32
    static const float beats[] = { 4.0f, 2.0f, 3.0f, 1.0f, 1.5f, 0.5f, 0.75f, 0.25f, 0.375f, 0.125f };
    if (noteIndex < 0 || noteIndex >= 10) noteIndex = 5;
    float beatLengthSec = 60.0f / (float)currentBpm;
    return 1.0f / (beatLengthSec * beats[noteIndex]);
}

float MultiEffectProcessor::noteDivisionToMs(int noteIndex) const
{
    static const float beats[] = { 4.0f, 2.0f, 3.0f, 1.0f, 1.5f, 0.5f, 0.75f, 0.25f, 0.375f, 0.125f };
    if (noteIndex < 0 || noteIndex >= 10) noteIndex = 5;
    float beatLengthMs = 60000.0f / (float)currentBpm;
    return beatLengthMs * beats[noteIndex];
}

void MultiEffectProcessor::processEffect(int index, juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);

    switch (index)
    {
        case NoiseGateIndex:
            if (noiseGateOn && noiseGateOn->get())
            {
                noiseGate.setThreshold(noiseGateThreshold->get());
                noiseGate.process(ctx);
            }
            break;
        case OverdriveIndex:
            if (overdriveOn && overdriveOn->get())
            {
                overdrive.setDrive(overdriveDrive->get());
                overdrive.setLevel(overdriveLevel->get());
                overdrive.setMix(overdriveMix->get());
                overdrive.process(ctx);
            }
            break;
        case BitcrusherIndex:
            if (bitcrusherOn && bitcrusherOn->get())
            {
                bitcrusher.setBitDepth(bitcrusherDepth->get());
                bitcrusher.setRate(bitcrusherRate->get());
                bitcrusher.process(ctx);
            }
            break;
        case FuzzIndex:
            if (fuzzOn && fuzzOn->get())
            {
                fuzz.setDrive(fuzzDrive->get());
                fuzz.setTone(fuzzTone->get());
                fuzz.setLevel(fuzzLevel->get());
                fuzz.setMix(fuzzMix->get());
                fuzz.process(ctx);
            }
            break;
        case CompressorIndex:
            if (compressorOn && compressorOn->get())
            {
                compressor.setLowThreshold(compressorLowThresh->get());
                compressor.setMidThreshold(compressorMidThresh->get());
                compressor.setHighThreshold(compressorHighThresh->get());
                compressor.setRatio(compressorRatio->get());
                compressor.setAttack(compressorAttack->get());
                compressor.setRelease(compressorRelease->get());
                compressor.setMakeupGain(compressorMakeup->get());
                compressor.process(ctx);
            }
            break;
        case RingModIndex:
            if (ringModOn && ringModOn->get())
            {
                ringMod.setRate(ringModRate->get());
                ringMod.setDepth(ringModDepth->get());
                ringMod.process(ctx);
            }
            break;
        case WahIndex:
            if (wahOn && wahOn->get())
            {
                wah.setRate(wahSync && wahSync->get() ? noteDivisionToHz(wahNote->getIndex()) : wahRate->get());
                wah.setDepth(wahDepth->get());
                wah.setCenterFreq(wahFreq->get());
                wah.setResonance(wahResonance->get());
                wah.setMix(wahMix->get());
                wah.process(ctx);
            }
            break;
        case PhaserIndex:
            if (phaserOn && phaserOn->get())
            {
                phaser.setRate(phaserSync && phaserSync->get() ? noteDivisionToHz(phaserNote->getIndex()) : phaserRate->get());
                phaser.setDepth(phaserDepth->get());
                phaser.setFeedback(phaserFeedback->get());
                phaser.setMix(phaserMix->get());
                phaser.process(ctx);
            }
            break;
        case ChorusIndex:
            if (chorusOn && chorusOn->get())
            {
                chorus.setRate(chorusSync && chorusSync->get() ? noteDivisionToHz(chorusNote->getIndex()) : chorusRate->get());
                chorus.setDepth(chorusDepth->get());
                chorus.setMix(chorusMix->get());
                chorus.process(ctx);
            }
            break;
        case TremoloIndex:
            if (tremoloOn && tremoloOn->get())
            {
                tremolo.setRate(tremoloSync && tremoloSync->get() ? noteDivisionToHz(tremoloNote->getIndex()) : tremoloRate->get());
                tremolo.setDepth(tremoloDepth->get());
                tremolo.process(ctx);
            }
            break;
        case FlangerIndex:
            if (flangerOn && flangerOn->get())
            {
                flanger.setRate(flangerSync && flangerSync->get() ? noteDivisionToHz(flangerNote->getIndex()) : flangerRate->get());
                flanger.setDepth(flangerDepth->get());
                flanger.setFeedback(flangerFeedback->get());
                flanger.setMix(flangerMix->get());
                flanger.process(ctx);
            }
            break;
        case DelayIndex:
            // Manual delay still used for now
            if (delayOn && delayOn->get())
            {
                float dTime = (delaySync && delaySync->get()) ? noteDivisionToMs(delayNote->getIndex()) : delayTime->get();
                float dFbk = delayFeedback->get();
                float dMix = delayMix->get();
                int delaySamples = (int)(getSampleRate() * dTime / 1000.0);
                auto numSamples = buffer.getNumSamples();
                for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                {
                    auto* data = buffer.getWritePointer(ch);
                    for (int s = 0; s < numSamples; ++s)
                    {
                        float delayed = delayLine.popSample(ch);
                        float in = data[s];
                        delayLine.pushSample(ch, in + delayed * dFbk);
                        data[s] = in * (1.0f - dMix) + delayed * dMix;
                    }
                }
            }
            break;
        case ReverbIndex:
            if (reverbOn && reverbOn->get())
            {
                juce::dsp::Reverb::Parameters p;
                p.roomSize = reverbRoomSize->get();
                p.damping = reverbDamping->get();
                p.wetLevel = reverbWetLevel->get();
                p.dryLevel = reverbDryLevel->get();
                p.width = reverbWidth->get();
                reverb.setParameters(p);
                reverb.process(ctx);
            }
            break;
        default: break;
    }
}

void MultiEffectProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    updateTempoFromHost();
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Process in current chain order
    for (int idx : chainOrder)
        processEffect(idx, buffer);
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
    // Persist chain order as a child
    juce::ValueTree orderTree("ChainOrder");
    for (size_t i = 0; i < chainOrder.size(); ++i)
        orderTree.setProperty("e" + juce::String(i), chainOrder[i], nullptr);
    state.addChild(orderTree, -1, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
void MultiEffectProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        auto vt = juce::ValueTree::fromXml(*xmlState);
        apvts.replaceState(vt);
        auto orderTree = vt.getChildWithName("ChainOrder");
        if (orderTree.isValid())
        {
            chainOrder.clear();
            for (int i = 0; i < orderTree.getNumProperties(); ++i)
                chainOrder.push_back((int)orderTree.getProperty("e" + juce::String(i)));
        }
    }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiEffectProcessor();
}
