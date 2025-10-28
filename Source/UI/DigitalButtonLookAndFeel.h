#pragma once
#include <JuceHeader.h>

// Digital button look and feel for smooth digital display appearance
class DigitalButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.0f, juce::Font::bold);
        g.setFont(font);
        auto bounds = button.getLocalBounds().toFloat();
        auto text = button.getButtonText();

        // Digital display text styling
        if (button.getToggleState())
        {
            // Active state - bright cyan glow
            juce::Colour textColour = juce::Colour(0xff00ffff);

            // Subtle outer glow
            for (float i = 1.5f; i >= 0.5f; i -= 0.5f)
            {
                auto alpha = 0.1f + (0.1f * (2.0f - i) / 1.5f);
                g.setColour(textColour.withAlpha(alpha));
                g.drawText(text, bounds.translated(-i, 0), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(i, 0), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(0, -i), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(0, i), juce::Justification::centred, true);
            }

            // Core bright text
            g.setColour(textColour);
            g.drawText(text, bounds, juce::Justification::centred, true);
        }
        else
        {
            // Inactive state - dim display
            juce::Colour dimColour = juce::Colour(0xff404040);
            if (isMouseOverButton)
                dimColour = juce::Colour(0xff606060);
            if (isButtonDown)
                dimColour = juce::Colour(0xff808080);

            g.setColour(dimColour);
            g.drawText(text, bounds, juce::Justification::centred, true);
        }
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto* textButton = dynamic_cast<juce::TextButton*>(&button);
        bool isToggled = textButton && textButton->getToggleState();

        // Digital display panel styling
        float cornerRadius = 2.0f;

        if (isToggled)
        {
            // Active state - bright digital panel
            // Outer dark bezel
            g.setColour(juce::Colour(0xff000000));
            g.fillRoundedRectangle(bounds, cornerRadius);

            // Inner screen glow
            auto screenBounds = bounds.reduced(1.5f);
            juce::ColourGradient screenGradient(
                juce::Colour(0xff003333), screenBounds.getCentreX(), screenBounds.getY(),
                juce::Colour(0xff001a1a), screenBounds.getCentreX(), screenBounds.getBottom(),
                false
            );
            g.setGradientFill(screenGradient);
            g.fillRoundedRectangle(screenBounds, cornerRadius - 1.0f);

            // Screen highlight
            g.setColour(juce::Colour(0xff00ffff).withAlpha(0.1f));
            g.fillRoundedRectangle(screenBounds.reduced(0.5f), cornerRadius - 1.5f);

            // Scan lines effect
            for (int y = (int)screenBounds.getY(); y < (int)screenBounds.getBottom(); y += 2)
            {
                g.setColour(juce::Colour(0xff000000).withAlpha(0.15f));
                g.drawHorizontalLine(y, screenBounds.getX(), screenBounds.getRight());
            }
        }
        else
        {
            // Inactive state - dark digital panel
            juce::Colour baseColour = isButtonDown ? juce::Colour(0xff0a0a0a)
                : isMouseOverButton ? juce::Colour(0xff151515)
                : juce::Colour(0xff101010);

            // Outer bezel
            g.setColour(juce::Colour(0xff000000));
            g.fillRoundedRectangle(bounds, cornerRadius);

            // Inner panel
            auto panelBounds = bounds.reduced(1.5f);
            g.setColour(baseColour);
            g.fillRoundedRectangle(panelBounds, cornerRadius - 1.0f);

            // Subtle inner border
            g.setColour(juce::Colour(0xff303030).withAlpha(0.3f));
            g.drawRoundedRectangle(panelBounds.reduced(0.5f), cornerRadius - 1.5f, 0.5f);

            // Dim scan lines
            for (int y = (int)panelBounds.getY(); y < (int)panelBounds.getBottom(); y += 2)
            {
                g.setColour(juce::Colour(0xff000000).withAlpha(0.1f));
                g.drawHorizontalLine(y, panelBounds.getX(), panelBounds.getRight());
            }
        }
    }
};
