#include "PluginEditor.h"
#include <chrono>
#include <random>

//==============================================================================
// Matrix character set – hex digits, symbols and some ASCII art chars
const juce::String MultiEffectProcessorEditor::matrixChars =
    "01234567890ABCDEF+-=<>?!#$%^&*abcdefgh[]{}|~";

// Shared random engine, seeded from hardware entropy
static std::mt19937& getRng()
{
    static std::mt19937 rng(
        static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    return rng;
}

static int randInt(int lo, int hi)   // inclusive [lo, hi]
{
    return std::uniform_int_distribution<int>(lo, hi)(getRng());
}

static float randFloat(float lo, float hi)
{
    return std::uniform_real_distribution<float>(lo, hi)(getRng());
}

//==============================================================================
static constexpr int kHeaderH  = 58;   // height of the matrix-rain header
static constexpr int kPanelW   = 300;  // width of one effect panel
static constexpr int kPanelH   = 200;  // height of one effect panel
static constexpr int kPanelPad = 2;    // gap between panels

static constexpr int kEditorW  = kPanelW * 3 + kPanelPad * 4;
static constexpr int kEditorH  = kHeaderH + kPanelH * 4 + kPanelPad * 5;

//==============================================================================
// Layout helpers

juce::Rectangle<int> MultiEffectProcessorEditor::panelBounds(int col, int row) const
{
    const int x = kPanelPad + col * (kPanelW + kPanelPad);
    const int y = kHeaderH + kPanelPad + row * (kPanelH + kPanelPad);
    return { x, y, kPanelW, kPanelH };
}

void MultiEffectProcessorEditor::setupRotarySlider(juce::Slider& slider, juce::Label& label)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 15);
    slider.setLookAndFeel(&cyberpunkLF);
    addAndMakeVisible(slider);
    label.setFont(CyberpunkLookAndFeel::getCustomFont().withHeight(11.0f));
    label.setJustificationType(juce::Justification::centred);
    label.setLookAndFeel(&cyberpunkLF);
    addAndMakeVisible(label);
}

//==============================================================================
MultiEffectProcessorEditor::MultiEffectProcessorEditor(MultiEffectProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&cyberpunkLF);

    // ------------------------------------------------------------------
    // Seed & build matrix rain drops (time-based seed for varied patterns)
    const int numDrops = (kEditorW / 18) + 5;
    for (int i = 0; i < numDrops; ++i)
    {
        MatrixDrop d;
        d.x          = static_cast<float>(i * 18 + randInt(0, 7));
        d.y          = static_cast<float>(randInt(0, kHeaderH - 1));
        d.speed      = 0.4f + static_cast<float>(randInt(0, 12)) * 0.06f;
        d.length     = 4 + randInt(0, 9);
        d.charOffset = randInt(0, matrixChars.length() - 1);
        d.opacity    = randFloat(0.4f, 1.0f);
        matrixDrops.push_back(d);
    }

    // ------------------------------------------------------------------
    // Bitcrusher
    addAndMakeVisible(bitcrusherOnButton);
    bitcrusherOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(bitcrusherDepthSlider, bitcrusherDepthLabel);
    setupRotarySlider(bitcrusherRateSlider,  bitcrusherRateLabel);

    // Ring Mod
    addAndMakeVisible(ringModOnButton);
    ringModOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(ringModRateSlider,  ringModRateLabel);
    setupRotarySlider(ringModDepthSlider, ringModDepthLabel);

    // Phaser
    addAndMakeVisible(phaserOnButton);
    phaserOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(phaserRateSlider,     phaserRateLabel);
    setupRotarySlider(phaserDepthSlider,    phaserDepthLabel);
    setupRotarySlider(phaserFeedbackSlider, phaserFeedbackLabel);
    setupRotarySlider(phaserMixSlider,      phaserMixLabel);

    // Chorus
    addAndMakeVisible(chorusOnButton);
    chorusOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(chorusRateSlider,  chorusRateLabel);
    setupRotarySlider(chorusDepthSlider, chorusDepthLabel);
    setupRotarySlider(chorusMixSlider,   chorusMixLabel);

    // Tremolo
    addAndMakeVisible(tremoloOnButton);
    tremoloOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(tremoloRateSlider,  tremoloRateLabel);
    setupRotarySlider(tremoloDepthSlider, tremoloDepthLabel);

    // Delay
    addAndMakeVisible(delayOnButton);
    delayOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(delayTimeSlider,     delayTimeLabel);
    setupRotarySlider(delayFeedbackSlider, delayFeedbackLabel);
    setupRotarySlider(delayMixSlider,      delayMixLabel);

    // Reverb
    addAndMakeVisible(reverbOnButton);
    reverbOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(reverbRoomSizeSlider, reverbRoomSizeLabel);
    setupRotarySlider(reverbDampingSlider,  reverbDampingLabel);
    setupRotarySlider(reverbWetLevelSlider, reverbWetLevelLabel);
    setupRotarySlider(reverbDryLevelSlider, reverbDryLevelLabel);
    setupRotarySlider(reverbWidthSlider,    reverbWidthLabel);

    // Multiband Compressor
    addAndMakeVisible(compressorOnButton);
    compressorOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(compressorLowThreshSlider,  compressorLowThreshLabel);
    setupRotarySlider(compressorMidThreshSlider,  compressorMidThreshLabel);
    setupRotarySlider(compressorHighThreshSlider, compressorHighThreshLabel);
    setupRotarySlider(compressorRatioSlider,      compressorRatioLabel);
    setupRotarySlider(compressorAttackSlider,     compressorAttackLabel);
    setupRotarySlider(compressorReleaseSlider,    compressorReleaseLabel);
    setupRotarySlider(compressorMakeupSlider,     compressorMakeupLabel);

    // Wah Wah
    addAndMakeVisible(wahOnButton);
    wahOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(wahRateSlider,      wahRateLabel);
    setupRotarySlider(wahDepthSlider,     wahDepthLabel);
    setupRotarySlider(wahFreqSlider,      wahFreqLabel);
    setupRotarySlider(wahResonanceSlider, wahResonanceLabel);
    setupRotarySlider(wahMixSlider,       wahMixLabel);

    // Fuzz
    addAndMakeVisible(fuzzOnButton);
    fuzzOnButton.setLookAndFeel(&cyberpunkLF);
    setupRotarySlider(fuzzDriveSlider, fuzzDriveLabel);
    setupRotarySlider(fuzzToneSlider,  fuzzToneLabel);
    setupRotarySlider(fuzzLevelSlider, fuzzLevelLabel);
    setupRotarySlider(fuzzMixSlider,   fuzzMixLabel);

    // ------------------------------------------------------------------
    // Parameter attachments
    auto attach = [&](const juce::String& id, juce::Slider& s) {
        sliderAttachments.push_back(
            std::make_unique<SliderAttachment>(audioProcessor.apvts, id, s));
    };
    auto attachBtn = [&](const juce::String& id, juce::ToggleButton& b) {
        buttonAttachments.push_back(
            std::make_unique<ButtonAttachment>(audioProcessor.apvts, id, b));
    };

    attach("bitcrusherDepth",  bitcrusherDepthSlider);
    attach("bitcrusherRate",   bitcrusherRateSlider);
    attachBtn("bitcrusherOn",  bitcrusherOnButton);

    attach("ringModRate",      ringModRateSlider);
    attach("ringModDepth",     ringModDepthSlider);
    attachBtn("ringModOn",     ringModOnButton);

    attach("phaserRate",       phaserRateSlider);
    attach("phaserDepth",      phaserDepthSlider);
    attach("phaserFeedback",   phaserFeedbackSlider);
    attach("phaserMix",        phaserMixSlider);
    attachBtn("phaserOn",      phaserOnButton);

    attach("chorusRate",       chorusRateSlider);
    attach("chorusDepth",      chorusDepthSlider);
    attach("chorusMix",        chorusMixSlider);
    attachBtn("chorusOn",      chorusOnButton);

    attach("tremoloRate",      tremoloRateSlider);
    attach("tremoloDepth",     tremoloDepthSlider);
    attachBtn("tremoloOn",     tremoloOnButton);

    attach("delayTime",        delayTimeSlider);
    attach("delayFeedback",    delayFeedbackSlider);
    attach("delayMix",         delayMixSlider);
    attachBtn("delayOn",       delayOnButton);

    attach("reverbRoomSize",   reverbRoomSizeSlider);
    attach("reverbDamping",    reverbDampingSlider);
    attach("reverbWetLevel",   reverbWetLevelSlider);
    attach("reverbDryLevel",   reverbDryLevelSlider);
    attach("reverbWidth",      reverbWidthSlider);
    attachBtn("reverbOn",      reverbOnButton);

    attach("compressorLowThresh",  compressorLowThreshSlider);
    attach("compressorMidThresh",  compressorMidThreshSlider);
    attach("compressorHighThresh", compressorHighThreshSlider);
    attach("compressorRatio",      compressorRatioSlider);
    attach("compressorAttack",     compressorAttackSlider);
    attach("compressorRelease",    compressorReleaseSlider);
    attach("compressorMakeup",     compressorMakeupSlider);
    attachBtn("compressorOn",      compressorOnButton);

    attach("wahRate",      wahRateSlider);
    attach("wahDepth",     wahDepthSlider);
    attach("wahFreq",      wahFreqSlider);
    attach("wahResonance", wahResonanceSlider);
    attach("wahMix",       wahMixSlider);
    attachBtn("wahOn",     wahOnButton);

    attach("fuzzDrive", fuzzDriveSlider);
    attach("fuzzTone",  fuzzToneSlider);
    attach("fuzzLevel", fuzzLevelSlider);
    attach("fuzzMix",   fuzzMixSlider);
    attachBtn("fuzzOn", fuzzOnButton);

    setSize(kEditorW, kEditorH);
    startTimerHz(30);
}

MultiEffectProcessorEditor::~MultiEffectProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

//==============================================================================
void MultiEffectProcessorEditor::timerCallback()
{
    ++matrixAnimCounter;

    for (auto& drop : matrixDrops)
    {
        drop.y += drop.speed;
        if (drop.y > static_cast<float>(kHeaderH + drop.length * 12))
        {
            drop.y          = -static_cast<float>(drop.length * 12);
            drop.charOffset = randInt(0, matrixChars.length() - 1);
            drop.speed      = 0.4f + static_cast<float>(randInt(0, 12)) * 0.06f;
        }
        // Slowly shift the chars
        if ((matrixAnimCounter % 6) == 0)
            drop.charOffset = (drop.charOffset + 1) % matrixChars.length();
    }
    repaint(0, 0, getWidth(), kHeaderH);
}

//==============================================================================
// Paint
//==============================================================================
void MultiEffectProcessorEditor::paint(juce::Graphics& g)
{
    using CP = CyberpunkLookAndFeel;

    // ------------------------------------------------------------------ body
    g.fillAll(CP::matrixDarkBG);

    // Subtle scan-line grid across the whole background
    g.setColour(CP::matrixGreen.withAlpha(0.03f));
    for (int y = kHeaderH; y < getHeight(); y += 4)
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(getWidth()));
    for (int x = 0; x < getWidth(); x += 40)
        g.drawVerticalLine(x, static_cast<float>(kHeaderH), static_cast<float>(getHeight()));

    // ------------------------------------------------------------------ header
    {
        juce::Rectangle<float> header(0.0f, 0.0f, static_cast<float>(getWidth()),
                                      static_cast<float>(kHeaderH));

        // Header background gradient
        g.setGradientFill(juce::ColourGradient(
            CP::matrixBlack,
            0.0f, 0.0f,
            juce::Colour(0xFF050820),
            0.0f, static_cast<float>(kHeaderH),
            false));
        g.fillRect(header);

        // Matrix rain drops
        auto monoFont = CyberpunkLookAndFeel::getCustomFont().withHeight(11.0f);
        g.setFont(monoFont);

        const int charH = 12;
        for (const auto& drop : matrixDrops)
        {
            const int numChars = matrixChars.length();
            for (int i = 0; i < drop.length; ++i)
            {
                const float cy = drop.y - static_cast<float>(i * charH);
                if (cy < -charH || cy > static_cast<float>(kHeaderH + charH))
                    continue;

                const float brightness = 1.0f - static_cast<float>(i) / static_cast<float>(drop.length);
                const juce::Colour charColour =
                    (i == 0) ? juce::Colour(0xFFCCFFCC) // leading char is near-white
                             : CP::matrixGreen.withAlpha(drop.opacity * brightness);
                g.setColour(charColour);

                const int charIdx = (drop.charOffset + i) % numChars;
                g.drawText(juce::String::charToString(matrixChars[charIdx]),
                           static_cast<int>(drop.x), static_cast<int>(cy),
                           14, charH,
                           juce::Justification::centred, false);
            }
        }

        // Horizontal separator line (glowing)
        g.setColour(CP::matrixGreen.withAlpha(0.6f));
        g.drawHorizontalLine(kHeaderH - 1, 0.0f, static_cast<float>(getWidth()));
        g.setColour(CP::matrixGreen.withAlpha(0.15f));
        g.drawHorizontalLine(kHeaderH - 3, 0.0f, static_cast<float>(getWidth()));

        // Plugin title
        g.setFont(CyberpunkLookAndFeel::getCustomFont().withHeight(22.0f).boldened());
        g.setColour(CP::matrixGreen);
        g.drawText("DSP4GUITAR",
                   10, 0, 200, kHeaderH,
                   juce::Justification::centredLeft);

        // Subtitle
        g.setFont(CyberpunkLookAndFeel::getCustomFont().withHeight(10.0f));
        g.setColour(CP::matrixCyan.withAlpha(0.8f));
        g.drawText("MULTI-EFFECT PROCESSOR",
                   10, 26, 240, kHeaderH - 26,
                   juce::Justification::centredLeft);

        // Version tag (right side)
        g.setFont(CyberpunkLookAndFeel::getCustomFont().withHeight(10.0f));
        g.setColour(CP::matrixGray);
        g.drawText("v1.0  //  GizzZmo",
                   getWidth() - 160, 0, 155, kHeaderH,
                   juce::Justification::centredRight);
    }

    // ------------------------------------------------------------------ effect panels
    // Panel draw order mirrors resized() layout (row, col):
    //  Row 0: Bitcrusher [0,0] | RingMod     [1,0] | Tremolo   [2,0]
    //  Row 1: Phaser     [0,1] | Chorus      [1,1] | Compressor[2,1]
    //  Row 2: Delay      [0,2] | Reverb      [1,2] | WahWah    [2,2]
    //  Row 3: Fuzz       [0,3] | [empty]            | [empty]

    auto* apvts = &audioProcessor.apvts;
    auto isOn   = [&](const juce::String& id) -> bool
    {
        if (auto* p = apvts->getParameter(id))
            return p->getValue() > 0.5f;
        return false;
    };

    drawEffectPanel(g, panelBounds(0, 0), "BITCRUSHER", isOn("bitcrusherOn"));
    drawEffectPanel(g, panelBounds(1, 0), "RING MOD",   isOn("ringModOn"));
    drawEffectPanel(g, panelBounds(2, 0), "TREMOLO",    isOn("tremoloOn"));
    drawEffectPanel(g, panelBounds(0, 1), "PHASER",     isOn("phaserOn"));
    drawEffectPanel(g, panelBounds(1, 1), "CHORUS",     isOn("chorusOn"));
    drawEffectPanel(g, panelBounds(2, 1), "COMPRESSOR", isOn("compressorOn"));
    drawEffectPanel(g, panelBounds(0, 2), "DELAY",      isOn("delayOn"));
    drawEffectPanel(g, panelBounds(1, 2), "REVERB",     isOn("reverbOn"));
    drawEffectPanel(g, panelBounds(2, 2), "WAH-WAH",    isOn("wahOn"));
    drawEffectPanel(g, panelBounds(0, 3), "FUZZ",       isOn("fuzzOn"));
}

//==============================================================================
void MultiEffectProcessorEditor::drawEffectPanel(juce::Graphics& g,
                                                  juce::Rectangle<int> bounds,
                                                  const juce::String& name,
                                                  bool isActive)
{
    using CP = CyberpunkLookAndFeel;

    const auto bf = bounds.toFloat();

    // Panel background
    g.setColour(juce::Colour(0xFF080812));
    g.fillRoundedRectangle(bf, 5.0f);

    // Outer glow when active
    if (isActive)
    {
        g.setColour(CP::matrixGreen.withAlpha(0.08f));
        g.fillRoundedRectangle(bf.expanded(3.0f), 7.0f);
    }

    // Border
    const juce::Colour borderCol = isActive ? CP::matrixGreen : CP::matrixGray.withAlpha(0.5f);
    g.setColour(borderCol);
    g.drawRoundedRectangle(bf.reduced(0.5f), 5.0f, 1.2f);

    // Header strip
    const auto headerStrip = bounds.removeFromTop(28).toFloat();
    g.setColour(isActive ? CP::matrixDarkGreen.withAlpha(0.35f)
                         : juce::Colour(0xFF0A0A14));
    g.fillRoundedRectangle(headerStrip, 4.0f);

    // Effect name text
    g.setFont(CyberpunkLookAndFeel::getCustomFont().withHeight(12.0f).boldened());
    g.setColour(isActive ? CP::matrixGreen : CP::matrixGray);
    g.drawText(name,
               static_cast<int>(headerStrip.getX()) + 30,
               static_cast<int>(headerStrip.getY()),
               static_cast<int>(headerStrip.getWidth()) - 60,
               static_cast<int>(headerStrip.getHeight()),
               juce::Justification::centred);

    // Corner brackets (decorative)
    if (isActive)
    {
        g.setColour(CP::matrixCyan.withAlpha(0.5f));
        const float bx = bf.getX();
        const float by = bf.getY();
        const float bw = bf.getWidth();
        const float bh = bf.getHeight();
        const float cs = 8.0f; // corner size
        // top-left
        g.drawLine(bx + cs, by + 1.0f, bx + 1.0f, by + 1.0f, 1.5f);
        g.drawLine(bx + 1.0f, by + 1.0f, bx + 1.0f, by + cs, 1.5f);
        // top-right
        g.drawLine(bx + bw - cs, by + 1.0f, bx + bw - 1.0f, by + 1.0f, 1.5f);
        g.drawLine(bx + bw - 1.0f, by + 1.0f, bx + bw - 1.0f, by + cs, 1.5f);
        // bottom-left
        g.drawLine(bx + 1.0f, by + bh - cs, bx + 1.0f, by + bh - 1.0f, 1.5f);
        g.drawLine(bx + 1.0f, by + bh - 1.0f, bx + cs, by + bh - 1.0f, 1.5f);
        // bottom-right
        g.drawLine(bx + bw - 1.0f, by + bh - cs, bx + bw - 1.0f, by + bh - 1.0f, 1.5f);
        g.drawLine(bx + bw - cs, by + bh - 1.0f, bx + bw - 1.0f, by + bh - 1.0f, 1.5f);
    }
}

//==============================================================================
// Layout helpers for placing sliders inside a panel
namespace
{
    // Place n knobs (each w×h) in a horizontal row centred within `area`
    void placeKnobRow(const juce::Rectangle<int>& area,
                      int n,
                      std::initializer_list<std::pair<juce::Slider*, juce::Label*>> items,
                      int knobW = 72, int knobH = 72, int labelH = 16)
    {
        const int totalW = n * knobW;
        int startX = area.getX() + (area.getWidth() - totalW) / 2;
        int startY = area.getY();

        int idx = 0;
        for (auto& [sl, lb] : items)
        {
            if (sl)
                sl->setBounds(startX + idx * knobW, startY, knobW, knobH);
            if (lb)
                lb->setBounds(startX + idx * knobW, startY + knobH, knobW, labelH);
            ++idx;
        }
    }
} // namespace

//==============================================================================
void MultiEffectProcessorEditor::resized()
{
    // ------------------------------------------------------------------
    // Helper: place a toggle button in the left portion of a panel header
    auto placeToggle = [](juce::ToggleButton& btn, juce::Rectangle<int> panel)
    {
        btn.setBounds(panel.getX() + 4,
                      panel.getY() + 6,
                      52, 18);
    };

    // ------------------------------------------------------------------
    // Row 0 – Bitcrusher | Ring Mod | Tremolo

    {
        auto p = panelBounds(0, 0);           // Bitcrusher
        placeToggle(bitcrusherOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 2,
            { {&bitcrusherDepthSlider, &bitcrusherDepthLabel},
              {&bitcrusherRateSlider,  &bitcrusherRateLabel} });
    }
    {
        auto p = panelBounds(1, 0);           // Ring Mod
        placeToggle(ringModOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 2,
            { {&ringModRateSlider,  &ringModRateLabel},
              {&ringModDepthSlider, &ringModDepthLabel} });
    }
    {
        auto p = panelBounds(2, 0);           // Tremolo
        placeToggle(tremoloOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 2,
            { {&tremoloRateSlider,  &tremoloRateLabel},
              {&tremoloDepthSlider, &tremoloDepthLabel} });
    }

    // ------------------------------------------------------------------
    // Row 1 – Phaser | Chorus

    {
        auto p = panelBounds(0, 1);           // Phaser  (4 knobs: 2 rows of 2)
        placeToggle(phaserOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 2);
        const int halfH = sliderArea.getHeight() / 2;
        placeKnobRow(sliderArea.withHeight(halfH), 2,
            { {&phaserRateSlider,     &phaserRateLabel},
              {&phaserDepthSlider,    &phaserDepthLabel} },
            72, 58, 14);
        placeKnobRow(sliderArea.withTrimmedTop(halfH), 2,
            { {&phaserFeedbackSlider, &phaserFeedbackLabel},
              {&phaserMixSlider,      &phaserMixLabel} },
            72, 58, 14);
    }
    {
        auto p = panelBounds(1, 1);           // Chorus  (3 knobs)
        placeToggle(chorusOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 3,
            { {&chorusRateSlider,  &chorusRateLabel},
              {&chorusDepthSlider, &chorusDepthLabel},
              {&chorusMixSlider,   &chorusMixLabel} },
            72, 72, 16);
    }

    // ------------------------------------------------------------------
    // Row 2 – Delay | Reverb

    {
        auto p = panelBounds(0, 2);           // Delay  (3 knobs)
        placeToggle(delayOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 3,
            { {&delayTimeSlider,     &delayTimeLabel},
              {&delayFeedbackSlider, &delayFeedbackLabel},
              {&delayMixSlider,      &delayMixLabel} },
            72, 72, 16);
    }
    {
        auto p = panelBounds(1, 2);           // Reverb  (5 knobs: 3 + 2)
        placeToggle(reverbOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 2);
        const int halfH = sliderArea.getHeight() / 2;
        placeKnobRow(sliderArea.withHeight(halfH), 3,
            { {&reverbRoomSizeSlider, &reverbRoomSizeLabel},
              {&reverbDampingSlider,  &reverbDampingLabel},
              {&reverbWetLevelSlider, &reverbWetLevelLabel} },
            72, 58, 14);
        placeKnobRow(sliderArea.withTrimmedTop(halfH), 2,
            { {&reverbDryLevelSlider, &reverbDryLevelLabel},
              {&reverbWidthSlider,    &reverbWidthLabel} },
            72, 58, 14);
    }

    // ------------------------------------------------------------------
    // Row 1 (cont.) – Compressor | Row 2 (cont.) – WahWah | Row 3 – Fuzz

    {
        auto p = panelBounds(2, 1);           // Compressor  (7 knobs: 3 + 4)
        placeToggle(compressorOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 2);
        const int halfH = sliderArea.getHeight() / 2;
        placeKnobRow(sliderArea.withHeight(halfH), 3,
            { {&compressorLowThreshSlider,  &compressorLowThreshLabel},
              {&compressorMidThreshSlider,  &compressorMidThreshLabel},
              {&compressorHighThreshSlider, &compressorHighThreshLabel} },
            72, 58, 14);
        placeKnobRow(sliderArea.withTrimmedTop(halfH), 4,
            { {&compressorRatioSlider,   &compressorRatioLabel},
              {&compressorAttackSlider,  &compressorAttackLabel},
              {&compressorReleaseSlider, &compressorReleaseLabel},
              {&compressorMakeupSlider,  &compressorMakeupLabel} },
            54, 54, 14);
    }
    {
        auto p = panelBounds(2, 2);           // WahWah  (5 knobs: 3 + 2)
        placeToggle(wahOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 2);
        const int halfH = sliderArea.getHeight() / 2;
        placeKnobRow(sliderArea.withHeight(halfH), 3,
            { {&wahRateSlider,  &wahRateLabel},
              {&wahDepthSlider, &wahDepthLabel},
              {&wahMixSlider,   &wahMixLabel} },
            72, 58, 14);
        placeKnobRow(sliderArea.withTrimmedTop(halfH), 2,
            { {&wahFreqSlider,      &wahFreqLabel},
              {&wahResonanceSlider, &wahResonanceLabel} },
            72, 58, 14);
    }
    {
        auto p = panelBounds(0, 3);           // Fuzz  (4 knobs)
        placeToggle(fuzzOnButton, p);
        const auto sliderArea = p.withTrimmedTop(32).reduced(6, 4);
        placeKnobRow(sliderArea, 4,
            { {&fuzzDriveSlider, &fuzzDriveLabel},
              {&fuzzToneSlider,  &fuzzToneLabel},
              {&fuzzLevelSlider, &fuzzLevelLabel},
              {&fuzzMixSlider,   &fuzzMixLabel} },
            54, 72, 16);
    }
}
