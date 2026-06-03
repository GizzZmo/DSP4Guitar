#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 * CyberpunkLookAndFeel
 *
 * A custom JUCE LookAndFeel_V4 that applies a cyberpunk / Matrix-terminal
 * aesthetic to the DSP4Guitar plugin UI.
 *
 * Colour palette
 * --------------
 *  Matrix Green  #00FF41  – primary active / highlight
 *  Dark Green    #00B300  – secondary / track
 *  Near-Black    #050505  – component background
 *  Dark BG       #0D0D1A  – window / panel background
 *  Cyan          #00FFFF  – accent for special labels
 *  Purple        #FF00FF  – alternate highlight
 *  Grey          #333333  – disabled / inactive elements
 */
class CyberpunkLookAndFeel : public juce::LookAndFeel_V4
{
public:
    struct ThemePalette
    {
        juce::Colour primary;
        juce::Colour secondary;
        juce::Colour background;
        juce::Colour surface;
        juce::Colour accent;
        juce::Colour inactive;
    };

    // -------------------------------------------------------------------------
    // Colour constants (defined inline in the class body, C++17)
    static inline const juce::Colour matrixGreen    { 0xFF00FF41 };
    static inline const juce::Colour matrixDarkGreen{ 0xFF00B300 };
    static inline const juce::Colour matrixBlack    { 0xFF050505 };
    static inline const juce::Colour matrixDarkBG   { 0xFF0D0D1A };
    static inline const juce::Colour matrixCyan     { 0xFF00FFFF };
    static inline const juce::Colour matrixPurple   { 0xFFFF00FF };
    static inline const juce::Colour matrixGray     { 0xFF444444 };

    // -------------------------------------------------------------------------
    CyberpunkLookAndFeel()
    {
        applyTheme({
            matrixGreen,
            matrixDarkGreen,
            matrixDarkBG,
            matrixBlack,
            matrixCyan,
            matrixGray
        });
    }

    void applyTheme(const ThemePalette& newTheme)
    {
        theme = newTheme;

        // Window
        setColour(juce::ResizableWindow::backgroundColourId, theme.background);
        setColour(juce::DocumentWindow::backgroundColourId,  theme.surface);

        // Sliders
        setColour(juce::Slider::backgroundColourId,           theme.surface);
        setColour(juce::Slider::thumbColourId,                theme.primary);
        setColour(juce::Slider::trackColourId,                theme.secondary);
        setColour(juce::Slider::rotarySliderFillColourId,     theme.primary);
        setColour(juce::Slider::rotarySliderOutlineColourId,  theme.secondary);
        setColour(juce::Slider::textBoxTextColourId,          theme.primary);
        setColour(juce::Slider::textBoxBackgroundColourId,    theme.surface);
        setColour(juce::Slider::textBoxHighlightColourId,     theme.secondary);
        setColour(juce::Slider::textBoxOutlineColourId,       theme.secondary);

        // Labels
        setColour(juce::Label::textColourId,       theme.primary);
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::outlineColourId,    juce::Colours::transparentBlack);

        // Toggle buttons
        setColour(juce::ToggleButton::textColourId,         theme.primary);
        setColour(juce::ToggleButton::tickColourId,         theme.primary);
        setColour(juce::ToggleButton::tickDisabledColourId, theme.inactive);

        // Text buttons
        setColour(juce::TextButton::buttonColourId,    theme.background);
        setColour(juce::TextButton::buttonOnColourId,  theme.secondary);
        setColour(juce::TextButton::textColourOffId,   theme.primary);
        setColour(juce::TextButton::textColourOnId,    theme.primary);

        // ComboBox
        setColour(juce::ComboBox::backgroundColourId,      theme.background);
        setColour(juce::ComboBox::textColourId,            theme.primary);
        setColour(juce::ComboBox::outlineColourId,         theme.secondary);
        setColour(juce::ComboBox::buttonColourId,          theme.secondary);
        setColour(juce::ComboBox::arrowColourId,           theme.primary);
        setColour(juce::ComboBox::focusedOutlineColourId,  theme.primary);

        // PopupMenu
        setColour(juce::PopupMenu::backgroundColourId,            theme.background);
        setColour(juce::PopupMenu::textColourId,                  theme.primary);
        setColour(juce::PopupMenu::headerTextColourId,            theme.accent);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.secondary);
        setColour(juce::PopupMenu::highlightedTextColourId,       theme.surface);

        // ScrollBar
        setColour(juce::ScrollBar::backgroundColourId, theme.background);
        setColour(juce::ScrollBar::thumbColourId,      theme.secondary);
        setColour(juce::ScrollBar::trackColourId,      theme.surface);
    }

    const ThemePalette& getTheme() const noexcept
    {
        return theme;
    }

    // =========================================================================
    // Rotary slider
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& /*slider*/) override
    {
        const float radius   = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        const float centreX  = (float)x + (float)width  * 0.5f;
        const float centreY  = (float)y + (float)height * 0.5f;
        const float rx       = centreX - radius;
        const float ry       = centreY - radius;
        const float rw       = radius * 2.0f;
        const float angle    = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Outer glow ring
        g.setColour(theme.primary.withAlpha(0.12f));
        g.fillEllipse(rx - 4.0f, ry - 4.0f, rw + 8.0f, rw + 8.0f);

        // Background disc
        g.setColour(theme.surface);
        g.fillEllipse(rx, ry, rw, rw);

        // Track arc (full range, dim)
        {
            juce::Path track;
            track.addArc(rx + 2.0f, ry + 2.0f, rw - 4.0f, rw - 4.0f,
                         rotaryStartAngle, rotaryEndAngle, true);
            g.setColour(theme.secondary.withAlpha(0.4f));
            g.strokePath(track, juce::PathStrokeType(2.5f));
        }

        // Value arc (filled, neon green)
        {
            juce::Path arc;
            arc.addArc(rx + 2.0f, ry + 2.0f, rw - 4.0f, rw - 4.0f,
                       rotaryStartAngle, angle, true);
            g.setColour(theme.primary);
            g.strokePath(arc, juce::PathStrokeType(3.0f,
                juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Outer ring
        g.setColour(theme.secondary);
        g.drawEllipse(rx, ry, rw, rw, 1.2f);

        // Thumb pointer
        {
            juce::Path thumb;
            const float thumbW  = 3.0f;
            const float thumbH  = radius * 0.55f;
            thumb.addRectangle(-thumbW * 0.5f, -(radius - 2.0f), thumbW, thumbH);
            const auto xform = juce::AffineTransform::rotation(angle)
                                   .translated(centreX, centreY);
            // Soft glow
            g.setColour(theme.primary.withAlpha(0.25f));
            g.fillPath(thumb, xform.scaled(1.8f, 1.0f, centreX, centreY));
            // Sharp pointer
            g.setColour(theme.primary);
            g.fillPath(thumb, xform);
        }

        // Centre dot
        g.setColour(theme.primary);
        g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);
    }

    // =========================================================================
    // Toggle button (LED-style)
    void drawToggleButton (juce::Graphics& g,
                           juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override
    {
        auto bounds  = button.getLocalBounds().toFloat();
        const float ledSize = juce::jmin(bounds.getHeight() * 0.65f, 16.0f);
        const float ledX    = bounds.getX() + 3.0f;
        const float ledY    = bounds.getCentreY() - ledSize * 0.5f;
        juce::Rectangle<float> ledBox(ledX, ledY, ledSize, ledSize);

        if (button.getToggleState())
        {
            // Glow
            g.setColour(theme.primary.withAlpha(0.25f));
            g.fillEllipse(ledBox.expanded(4.0f));
            // Fill
            g.setColour(theme.primary);
            g.fillEllipse(ledBox);
            // Border
            g.setColour(theme.primary.brighter(0.4f));
            g.drawEllipse(ledBox, 1.5f);
        }
        else
        {
            g.setColour(theme.inactive.withAlpha(0.5f));
            g.fillEllipse(ledBox);
            g.setColour(theme.inactive);
            g.drawEllipse(ledBox, 1.2f);
        }

        // Text
        auto textBounds = bounds.withLeft(ledBox.getRight() + 5.0f);
        g.setFont(getCustomFont().withHeight(12.0f));
        auto textColour = button.getToggleState() ? theme.primary : theme.inactive;
        if (shouldDrawButtonAsHighlighted)
            textColour = textColour.brighter (0.2f);
        g.setColour (textColour);
        g.drawFittedText(button.getButtonText(), textBounds.toNearestInt(),
                         juce::Justification::centredLeft, 1);
    }

    // =========================================================================
    // Text button background
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& /*backgroundColour*/,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

        juce::Colour base = button.getToggleState() ? theme.secondary : theme.background;
        if (shouldDrawButtonAsDown)      base = base.darker(0.35f);
        else if (shouldDrawButtonAsHighlighted) base = base.brighter(0.12f);

        g.setColour(base);
        g.fillRoundedRectangle(bounds, 4.0f);

        juce::Colour outline = button.getToggleState() ? theme.primary : theme.secondary;
        if (shouldDrawButtonAsHighlighted) outline = outline.brighter(0.3f);
        g.setColour(outline);
        g.drawRoundedRectangle(bounds, 4.0f, 1.5f);
    }

    void drawButtonText (juce::Graphics& g,
                         juce::TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/,
                         bool /*shouldDrawButtonAsDown*/) override
    {
        g.setFont(getCustomFont().withHeight(13.0f));
        g.setColour(button.getToggleState() ? theme.primary : theme.secondary.brighter(0.5f));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(),
                         juce::Justification::centred, 1);
    }

    // =========================================================================
    // Label
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll(label.findColour(juce::Label::backgroundColourId));

        if (!label.isBeingEdited())
        {
            const float alpha = label.isEnabled() ? 1.0f : 0.5f;
            g.setFont(getCustomFont().withHeight(label.getFont().getHeight()));
            g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
            const auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());
            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                             juce::jmax(1, (int)((float)textArea.getHeight() /
                                                  label.getFont().getHeight())),
                             label.getMinimumHorizontalScale());
        }
    }

    // =========================================================================
    // Slider text box
    void drawLinearSlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                           const juce::Slider::SliderStyle style,
                           juce::Slider& slider) override
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height,
                                         sliderPos, 0.0f, 0.0f, style, slider);
    }

    // =========================================================================
    // Monospace font for the terminal feel
    static juce::Font getCustomFont()
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain);
    }

private:
    ThemePalette theme {};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CyberpunkLookAndFeel)
};
