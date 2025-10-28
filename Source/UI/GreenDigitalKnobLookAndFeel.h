#pragma once
#include <JuceHeader.h>

// Green digital knob look and feel for chorus module
class GreenDigitalKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                        juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height);
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();

        // Digital knob with even larger radius for maximum prominence
        auto radius = 15.0f;
        auto knobBounds = juce::Rectangle<float>(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // Outer digital bezel
        g.setColour(juce::Colour(0xff000000));
        g.fillEllipse(knobBounds.expanded(2.5f));

        // Inner digital panel with green gradient
        juce::ColourGradient digitalGradient(
            juce::Colour(0xff1a2a1a), centreX, centreY - radius,
            juce::Colour(0xff0d150d), centreX, centreY + radius,
            false
        );
        digitalGradient.addColour(0.3, juce::Colour(0xff152015));
        digitalGradient.addColour(0.7, juce::Colour(0xff0a100a));
        g.setGradientFill(digitalGradient);
        g.fillEllipse(knobBounds);

        // Digital rim with bright green tint
        g.setColour(juce::Colour(0xff003000));
        g.drawEllipse(knobBounds, 2.0f);

        // Inner digital highlight
        g.setColour(juce::Colour(0xff004000).withAlpha(0.6f));
        g.drawEllipse(knobBounds.reduced(1.5f), 1.5f);

        // Digital position indicator - bright green glow line
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineLength = radius * 0.7f;
        auto lineStartRadius = radius * 0.2f;

        juce::Point<float> lineStart(centreX + lineStartRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                   centreY + lineStartRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
        juce::Point<float> lineEnd(centreX + lineLength * std::cos(angle - juce::MathConstants<float>::halfPi),
                                 centreY + lineLength * std::sin(angle - juce::MathConstants<float>::halfPi));

        // Glow effect for the indicator line
        for (float i = 4.0f; i >= 1.0f; i -= 0.5f)
        {
            auto alpha = 0.1f + (0.3f * (5.0f - i) / 4.0f);
            g.setColour(juce::Colour(0xff00ff00).withAlpha(alpha));
            g.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, i);
        }

        // Core bright green indicator
        g.setColour(juce::Colour(0xff00ff00));
        g.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, 2.0f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), label.getFont().getHeight() * 1.3f, juce::Font::bold);

        g.setFont(font);

        // Digital text with green glow
        juce::Colour textColour = juce::Colour(0xff00cc00);

        // Subtle outer glow
        for (float i = 2.0f; i >= 0.5f; i -= 0.5f)
        {
            auto alpha = 0.08f + (0.15f * (2.5f - i) / 2.0f);
            g.setColour(textColour.withAlpha(alpha));
            g.drawText(text, bounds.translated(-i, 0), label.getJustificationType(), true);
            g.drawText(text, bounds.translated(i, 0), label.getJustificationType(), true);
            g.drawText(text, bounds.translated(0, -i), label.getJustificationType(), true);
            g.drawText(text, bounds.translated(0, i), label.getJustificationType(), true);
        }

        // Core bright text
        g.setColour(textColour);
        g.drawText(text, bounds, label.getJustificationType(), true);
    }
};
