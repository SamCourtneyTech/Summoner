#pragma once
#include <JuceHeader.h>

// Engraved label look and feel for macro knobs
class EngravedLabelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = label.getFont();

        // Draw inset shadow (darker text offset down and right)
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.setFont(font);
        g.drawText(text, bounds.translated(1.0f, 1.0f), label.getJustificationType());

        // Draw highlight (lighter text offset up and left)
        g.setColour(juce::Colour(0xff909090).withAlpha(0.6f));
        g.drawText(text, bounds.translated(-0.5f, -0.5f), label.getJustificationType());

        // Draw main text (medium dark gray)
        g.setColour(juce::Colour(0xff505050));
        g.drawText(text, bounds, label.getJustificationType());
    }
};
