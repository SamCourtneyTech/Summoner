#pragma once
#include <JuceHeader.h>

// Custom look and feel for digital screen effects
class DigitalScreenLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        auto bounds = button.getLocalBounds();

        // Digital screen tab styling
        if (button.isFrontTab())
        {
            // Active tab - bright green
            g.setColour(juce::Colour(0xff00ff00).withAlpha(0.9f));
            g.fillRect(bounds);
            g.setColour(juce::Colour(0xff000000));
        }
        else
        {
            // Inactive tab - darker green
            g.setColour(juce::Colour(0xff003300).withAlpha(0.7f));
            g.fillRect(bounds);
            g.setColour(juce::Colour(0xff00aa00));
        }

        // Draw text with digital font style
        g.setFont(juce::Font("Courier New", 9.7f, juce::Font::bold)); // 10.8f * 0.9 = 9.72f
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred);

        // Add scan line effect
        for (int y = bounds.getY(); y < bounds.getBottom(); y += 3)
        {
            g.setColour(juce::Colour(0xff000000).withAlpha(0.1f));
            g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
        }
    }
};
