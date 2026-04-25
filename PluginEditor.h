#pragma once

#include "MultiEffectProcessor.h"
#include "CyberpunkLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <vector>

//==============================================================================
/**
 * MultiEffectProcessorEditor
 *
 * Cyberpunk / Matrix-terminal themed plugin UI.
 * Renders a scrolling Matrix-rain animation in the header, neon-bordered
 * effect panels, and custom rotary knobs / LED-style toggle buttons.
 */
class MultiEffectProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
public:
    MultiEffectProcessorEditor(MultiEffectProcessor&);
    ~MultiEffectProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MultiEffectProcessor& audioProcessor;

    // ------------------------------------------------------------------
    // Cyberpunk Look-and-Feel (must outlive all child components)
    CyberpunkLookAndFeel cyberpunkLF;

    // ------------------------------------------------------------------
    // Matrix-rain animation state
    struct MatrixDrop
    {
        float x, y;
        float speed;
        int   length;
        int   charOffset;
        float opacity;
    };
    std::vector<MatrixDrop> matrixDrops;
    static const juce::String matrixChars;
    int matrixAnimCounter = 0;

    // ------------------------------------------------------------------
    // UI Elements – grouped by effect

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

    // ------------------------------------------------------------------
    // Parameter Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;

    // ------------------------------------------------------------------
    // Layout helpers
    /** Draws one cyberpunk effect panel (border, title, active indicator). */
    void drawEffectPanel (juce::Graphics& g,
                          juce::Rectangle<int> bounds,
                          const juce::String& name,
                          bool isActive);

    /** Configures a rotary slider and makes both it and its label visible. */
    void setupRotarySlider (juce::Slider& slider, juce::Label& label);

    /** Returns the bounds rectangle for effect panel at column col, row row. */
    juce::Rectangle<int> panelBounds (int col, int row) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiEffectProcessorEditor)
};
