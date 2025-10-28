#pragma once
#include <JuceHeader.h>
#include "SimpleKnobLookAndFeel.h"

// Digital filter knob look and feel with glow text and background
class DigitalFilterKnobLookAndFeel : public SimpleKnobLookAndFeel
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = label.getFont();
        auto justification = label.getJustificationType();

        // Draw background matching oscillator knob colors
        auto bgBounds = bounds.reduced(1.0f);

        // Multi-layer drop shadow (same as oscillator knobs)
        for (int i = 4; i >= 1; --i)
        {
            auto shadowAlpha = 0.15f - (i * 0.03f);
            g.setColour(juce::Colour(0xff000000).withAlpha(shadowAlpha));
            g.fillRoundedRectangle(bgBounds.translated(i * 0.8f, i * 0.8f), 3.0f);
        }

        // Create gradient matching oscillator knobs
        juce::ColourGradient bgGradient(
            juce::Colour(0xff1a1a1a),    // Same as knob top color
            bgBounds.getX(), bgBounds.getY(),
            juce::Colour(0xff080808),    // Same as knob bottom color
            bgBounds.getX(), bgBounds.getBottom(), false);
        bgGradient.addColour(0.3, juce::Colour(0xff141414)); // Same mid-highlight
        bgGradient.addColour(0.7, juce::Colour(0xff0a0a0a)); // Same mid-tone
        g.setGradientFill(bgGradient);
        g.fillRoundedRectangle(bgBounds, 3.0f);

        // Outer metallic rim with gradient (matching knobs exactly)
        juce::ColourGradient rimGradient(
            juce::Colour(0xff303030),    // Darker top rim (same as knobs)
            bgBounds.getCentreX(), bgBounds.getY(),
            juce::Colour(0xff050505),    // Much darker bottom rim (same as knobs)
            bgBounds.getCentreX(), bgBounds.getBottom(),
            false
        );
        g.setGradientFill(rimGradient);
        g.drawRoundedRectangle(bgBounds, 3.0f, 2.0f);

        // Inner rim for more definition (same as knobs)
        g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
        g.drawRoundedRectangle(bgBounds.reduced(1.5f), 2.0f, 1.0f);

        g.setFont(font);

        // Text glow effect (same as oscillator knobs)
        // Outer glow for text
        for (float i = 2.5f; i >= 1.0f; i -= 0.5f)
        {
            auto alpha = 0.04f + (0.08f * (3.0f - i) / 2.0f);
            g.setColour(juce::Colours::white.withAlpha(alpha));
            g.drawText(text, bounds.translated(-i, 0), justification, true);
            g.drawText(text, bounds.translated(i, 0), justification, true);
            g.drawText(text, bounds.translated(0, -i), justification, true);
            g.drawText(text, bounds.translated(0, i), justification, true);
        }

        // Inner glow for text
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawText(text, bounds.translated(-1, 0), justification, true);
        g.drawText(text, bounds.translated(1, 0), justification, true);
        g.drawText(text, bounds.translated(0, -1), justification, true);
        g.drawText(text, bounds.translated(0, 1), justification, true);

        // Core bright white text
        g.setColour(juce::Colours::white);
        g.drawText(text, bounds, justification, true);
    }
};
