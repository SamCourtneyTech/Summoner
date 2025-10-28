#pragma once
#include <JuceHeader.h>

// Digital knob look and feel for chorus module
class DigitalKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                        juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height);
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();

        // Digital knob with smaller radius for more compact appearance
        auto radius = 8.0f;
        auto knobBounds = juce::Rectangle<float>(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // Outer digital bezel
        g.setColour(juce::Colour(0xff000000));
        g.fillEllipse(knobBounds.expanded(2.0f));

        // Inner digital panel with gradient
        juce::ColourGradient digitalGradient(
            juce::Colour(0xff1a2a2a), centreX, centreY - radius,
            juce::Colour(0xff0d1515), centreX, centreY + radius,
            false
        );
        digitalGradient.addColour(0.3, juce::Colour(0xff152020));
        digitalGradient.addColour(0.7, juce::Colour(0xff0a1010));
        g.setGradientFill(digitalGradient);
        g.fillEllipse(knobBounds);

        // Digital rim with cyan tint
        g.setColour(juce::Colour(0xff003030));
        g.drawEllipse(knobBounds, 1.5f);

        // Inner digital highlight
        g.setColour(juce::Colour(0xff004040).withAlpha(0.6f));
        g.drawEllipse(knobBounds.reduced(1.0f), 1.0f);

        // Digital position indicator - cyan glow line
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineLength = radius * 0.7f;
        auto lineStartRadius = radius * 0.2f;

        juce::Point<float> lineStart(centreX + lineStartRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                   centreY + lineStartRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
        juce::Point<float> lineEnd(centreX + lineLength * std::cos(angle - juce::MathConstants<float>::halfPi),
                                 centreY + lineLength * std::sin(angle - juce::MathConstants<float>::halfPi));

        // Glow effect for the indicator line
        for (float i = 3.0f; i >= 1.0f; i -= 0.5f)
        {
            auto alpha = 0.1f + (0.2f * (4.0f - i) / 3.0f);
            g.setColour(juce::Colour(0xff00ffff).withAlpha(alpha));
            g.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, i);
        }

        // Core bright cyan indicator
        g.setColour(juce::Colour(0xff00ffff));
        g.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, 1.5f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), label.getFont().getHeight(), juce::Font::bold);

        g.setFont(font);

        // Digital text with cyan glow
        juce::Colour textColour = juce::Colour(0xff00cccc);

        // Subtle outer glow
        for (float i = 1.5f; i >= 0.5f; i -= 0.5f)
        {
            auto alpha = 0.08f + (0.1f * (2.0f - i) / 1.5f);
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
