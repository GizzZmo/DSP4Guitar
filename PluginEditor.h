#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
*/
class MultiEffectProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer // Inherit from Timer for animations
{
public:
    MultiEffectProcessorEditor(MultiEffectProcessor&);
    ~MultiEffectProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override; // For animations

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultiEffectProcessor& audioProcessor;

    // --- UI Elements ---
    // We'll declare sliders for continuous parameters and buttons (toggles) for On/Off.
    // Grouping them by effect can help keep things organised.

    // Bitcrusher
    juce::ToggleButton bitcrusherOnButton { "On" };
    juce::Slider bitcrusherDepthSlider;
    juce::Slider bitcrusherRateSlider;
    juce::Label bitcrusherDepthLabel { {}, "Depth" };
    juce::Label bitcrusherRateLabel { {}, "Rate" };

    // Ring Mod
    juce::ToggleButton ringModOnButton { "On" };
    juce::Slider ringModRateSlider;
    juce::Slider ringModDepthSlider;
    juce::Label ringModRateLabel { {}, "Rate" };
    juce::Label ringModDepthLabel { {}, "Depth" };

    // Phaser
    juce::ToggleButton phaserOnButton { "On" };
    juce::Slider phaserRateSlider;
    juce::Slider phaserDepthSlider;
    juce::Slider phaserFeedbackSlider;
    juce::Slider phaserMixSlider;
    juce::Label phaserRateLabel { {}, "Rate" };
    juce::Label phaserDepthLabel { {}, "Depth" };
    juce::Label phaserFeedbackLabel { {}, "Fbk" };
    juce::Label phaserMixLabel { {}, "Mix" };

    // Flanger
    juce::ToggleButton flangerOnButton { "On" };
    juce::Slider flangerRateSlider;
    juce::Slider flangerDepthSlider;
    juce::Slider flangerFeedbackSlider;
    juce::Slider flangerMixSlider;
    juce::Label flangerRateLabel { {}, "Rate" };
    juce::Label flangerDepthLabel { {}, "Depth" };
    juce::Label flangerFeedbackLabel { {}, "Fbk" };
    juce::Label flangerMixLabel { {}, "Mix" };

    // Chorus
    juce::ToggleButton chorusOnButton { "On" };
    juce::Slider chorusRateSlider;
    juce::Slider chorusDepthSlider;
    juce::Slider chorusMixSlider;
    juce::Label chorusRateLabel { {}, "Rate" };
    juce::Label chorusDepthLabel { {}, "Depth" };
    juce::Label chorusMixLabel { {}, "Mix" };

    // Tremolo
    juce::ToggleButton tremoloOnButton { "On" };
    juce::Slider tremoloRateSlider;
    juce::Slider tremoloDepthSlider;
    juce::Label tremoloRateLabel { {}, "Rate" };
    juce::Label tremoloDepthLabel { {}, "Depth" };

    // Delay
    juce::ToggleButton delayOnButton { "On" };
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Slider delayMixSlider;
    juce::Label delayTimeLabel { {}, "Time" };
    juce::Label delayFeedbackLabel { {}, "Fbk" };
    juce::Label delayMixLabel { {}, "Mix" };

    // Reverb
    juce::ToggleButton reverbOnButton { "On" };
    juce::Slider reverbRoomSizeSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWetLevelSlider;
    juce::Slider reverbDryLevelSlider;
    juce::Slider reverbWidthSlider;
    juce::Label reverbRoomSizeLabel { {}, "Size" };
    juce::Label reverbDampingLabel { {}, "Damp" };
    juce::Label reverbWetLevelLabel { {}, "Wet" };
    juce::Label reverbDryLevelLabel { {}, "Dry" };
    juce::Label reverbWidthLabel { {}, "Width" };

    // --- Parameter Attachments ---
    // These link the UI elements to the APVTS in the processor.
    // We use std::unique_ptr to manage their lifetime automatically.
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;

    // Function to add a label to a slider
    void addLabelAndMakeVisible(juce::Slider& slider, juce::Label& label);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiEffectProcessorEditor)
};
