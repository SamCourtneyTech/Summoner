#pragma once
#include <JuceHeader.h>

// Simple knob look and feel for macro knobs (no LED lighting)
class SimpleKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                        juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height);
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY() + 2.0f; // Lower the visual knob by 2 pixels

        // Draw smaller visual knob within the same bounds
        auto radius = 8.0f; // Thinner fixed radius
        auto knobBounds = juce::Rectangle<float>(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // Draw deeper shadow beneath knob for more 3D depth
        auto shadowBounds = knobBounds.translated(1.5f, 2.0f);
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillEllipse(shadowBounds);

        // Additional soft outer shadow
        auto outerShadow = knobBounds.translated(0.5f, 1.0f).expanded(1.0f);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillEllipse(outerShadow);

        // Draw main knob body with darker radial gradient for more dramatic 3D effect
        juce::ColourGradient knobGradient(
            juce::Colour(0xff303030), centreX - radius * 0.5f, centreY - radius * 0.5f, // Darker highlight from top-left
            juce::Colour(0xff0a0a0a), centreX + radius * 0.7f, centreY + radius * 0.7f, // Much darker bottom-right
            true // radial gradient
        );
        knobGradient.addColour(0.3, juce::Colour(0xff252525)); // Darker mid-highlight
        knobGradient.addColour(0.6, juce::Colour(0xff1a1a1a)); // Darker mid-tone
        knobGradient.addColour(0.9, juce::Colour(0xff050505)); // Very dark edge
        g.setGradientFill(knobGradient);
        g.fillEllipse(knobBounds);

        // Draw more pronounced beveled edge with multiple layers
        // Bright top-left highlight (simulating strong light)
        g.setColour(juce::Colour(0xff909090).withAlpha(0.9f));
        g.drawEllipse(knobBounds.reduced(0.3f), 0.8f);

        // Secondary highlight ring
        g.setColour(juce::Colour(0xff606060).withAlpha(0.7f));
        g.drawEllipse(knobBounds.reduced(0.8f), 0.6f);

        // Deep bottom-right shadow edge
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        auto shadowEdgeBounds = knobBounds.reduced(0.3f).translated(0.8f, 0.8f);
        g.drawEllipse(shadowEdgeBounds, 1.2f);

        // Inner shadow ring for depth
        auto innerShadowRim = knobBounds.reduced(1.8f);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(innerShadowRim, 0.8f);

        // Metallic outer rim with gradient
        g.setColour(juce::Colour(0xff505050));
        g.drawEllipse(knobBounds, 1.5f);

        // Bright metallic edge highlight
        g.setColour(juce::Colour(0xff808080).withAlpha(0.8f));
        g.drawEllipse(knobBounds.reduced(0.2f), 0.5f);

        // Draw realistic position indicator line (slit) with depth
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineLength = radius * 0.6f; // Proportional to the smaller radius
        auto lineStartRadius = radius * 0.2f; // Proportional to the smaller radius

        juce::Point<float> lineStart(centreX + lineStartRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                   centreY + lineStartRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
        juce::Point<float> lineEnd(centreX + lineLength * std::cos(angle - juce::MathConstants<float>::halfPi),
                                 centreY + lineLength * std::sin(angle - juce::MathConstants<float>::halfPi));

        // Draw slit shadow for depth
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.drawLine(lineStart.x + 0.5f, lineStart.y + 0.5f, lineEnd.x + 0.5f, lineEnd.y + 0.5f, 1.5f);

        // Draw main slit indicator
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, 1.5f);

        // Add highlight to slit for metallic appearance
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawLine(lineStart.x - 0.3f, lineStart.y - 0.3f, lineEnd.x - 0.3f, lineEnd.y - 0.3f, 0.5f);
    }
};
