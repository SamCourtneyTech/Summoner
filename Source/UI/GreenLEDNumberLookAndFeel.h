#pragma once
#include <JuceHeader.h>

// Green LED number look and feel for distortion type selector
class GreenLEDNumberLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();

        // Draw dark background panel
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRoundedRectangle(bounds, 3.0f);

        // Draw green glowing border
        g.setColour(juce::Colour(0xff00ff00).withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, 3.0f, 2.0f);

        // Inner green glow
        g.setColour(juce::Colour(0xff00ff00).withAlpha(0.2f));
        g.drawRoundedRectangle(bounds.reduced(2.0f), 2.0f, 1.0f);

        // Draw digital-style text with green glow
        auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::bold);
        g.setFont(font);

        // Text glow effect
        for (float i = 2.0f; i >= 0.5f; i -= 0.5f)
        {
            auto alpha = 0.1f + (0.2f * (2.5f - i) / 2.0f);
            g.setColour(juce::Colour(0xff00ff00).withAlpha(alpha));
            g.drawText(text, bounds.translated(-i, 0), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(i, 0), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(0, -i), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(0, i), juce::Justification::centred, true);
        }

        // Core bright green text
        g.setColour(juce::Colour(0xff00ff00));
        g.drawText(text, bounds, juce::Justification::centred, true);

        // Add scan lines effect for digital look
        for (int y = (int)bounds.getY(); y < (int)bounds.getBottom(); y += 2)
        {
            g.setColour(juce::Colour(0xff000000).withAlpha(0.3f));
            g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
        }
    }
};
