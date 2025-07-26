#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "LFOComponent.h"

class SummonerXSerum2AudioProcessor;


class ADSREnvelopeComponent : public juce::Component
{
public:
    ADSREnvelopeComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void updateEnvelope(float attack, float decay, float sustain, float release);
    
private:
    float attackTime = 0.1f;
    float decayTime = 0.2f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.3f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSREnvelopeComponent)
};


class SynthesizerComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // Mouse events for octave control
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    // Helper method to update envelope display
    void updateEnvelopeDisplay();
    
    // Helper methods for grouped component positioning and background drawing
    void layoutWaveTypeButtons(juce::Rectangle<int>& bounds);
    void layoutADSREnvelope(juce::Rectangle<int>& bounds);
    void layoutADSRKnobs(juce::Rectangle<int>& bounds);
    void layoutVolumeKnobs(juce::Rectangle<int>& bounds);
    void layoutOctaveControls(juce::Rectangle<int>& bounds);
    void layoutPhaseControls(juce::Rectangle<int>& bounds);
    void layoutLFOModule(juce::Rectangle<int>& bounds);
    void layoutEffectsModule(juce::Rectangle<int>& bounds);
    void layoutChorusControls(juce::Rectangle<int>& bounds);
    void layoutCompressorControls(juce::Rectangle<int>& bounds);
    void layoutDelayControls(juce::Rectangle<int>& bounds);
    void layoutDistortionControls(juce::Rectangle<int>& bounds);
    void layoutSecondOscillator(juce::Rectangle<int>& bounds);
    
    void drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawSecondOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PitchControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawFilterSectionBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawMacroKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;
    
    // Custom look and feel for wave buttons
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;
    
    // Custom look and feel for LED labels
    LEDLabelLookAndFeel ledLabelLookAndFeel;
    LEDNumberLookAndFeel ledNumberLookAndFeel;
    
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
    
    DigitalScreenLookAndFeel digitalScreenLookAndFeel;
    
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
    
    DigitalButtonLookAndFeel digitalButtonLookAndFeel;
    
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
    
    DigitalKnobLookAndFeel digitalKnobLookAndFeel;
    
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
    
    GreenDigitalKnobLookAndFeel greenDigitalKnobLookAndFeel;
    
    // Green digital button look and feel for chorus module
    class GreenDigitalButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonText(juce::Graphics& g, juce::TextButton& button,
            bool isMouseOverButton, bool isButtonDown) override
        {
            auto font = juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold);
            g.setFont(font);
            auto bounds = button.getLocalBounds().toFloat();
            auto text = button.getButtonText();
            
            // Digital display text styling
            if (button.getToggleState())
            {
                // Active state - bright green glow
                juce::Colour textColour = juce::Colour(0xff00ff00);
                
                // Subtle outer glow
                for (float i = 2.0f; i >= 0.5f; i -= 0.5f)
                {
                    auto alpha = 0.1f + (0.15f * (2.5f - i) / 2.0f);
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
            float cornerRadius = 3.0f;
            
            if (isToggled)
            {
                // Active state - bright green digital panel
                // Outer dark bezel
                g.setColour(juce::Colour(0xff000000));
                g.fillRoundedRectangle(bounds, cornerRadius);
                
                // Inner screen glow
                auto screenBounds = bounds.reduced(2.0f);
                juce::ColourGradient screenGradient(
                    juce::Colour(0xff003300), screenBounds.getCentreX(), screenBounds.getY(),
                    juce::Colour(0xff001a00), screenBounds.getCentreX(), screenBounds.getBottom(),
                    false
                );
                g.setGradientFill(screenGradient);
                g.fillRoundedRectangle(screenBounds, cornerRadius - 1.0f);
                
                // Screen highlight
                g.setColour(juce::Colour(0xff00ff00).withAlpha(0.15f));
                g.fillRoundedRectangle(screenBounds.reduced(1.0f), cornerRadius - 2.0f);
                
                // Scan lines effect
                for (int y = (int)screenBounds.getY(); y < (int)screenBounds.getBottom(); y += 3)
                {
                    g.setColour(juce::Colour(0xff000000).withAlpha(0.2f));
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
                auto panelBounds = bounds.reduced(2.0f);
                g.setColour(baseColour);
                g.fillRoundedRectangle(panelBounds, cornerRadius - 1.0f);
                
                // Subtle inner border
                g.setColour(juce::Colour(0xff303030).withAlpha(0.3f));
                g.drawRoundedRectangle(panelBounds.reduced(0.5f), cornerRadius - 1.5f, 0.5f);
                
                // Dim scan lines
                for (int y = (int)panelBounds.getY(); y < (int)panelBounds.getBottom(); y += 3)
                {
                    g.setColour(juce::Colour(0xff000000).withAlpha(0.15f));
                    g.drawHorizontalLine(y, panelBounds.getX(), panelBounds.getRight());
                }
            }
        }
    };
    
    GreenDigitalButtonLookAndFeel greenDigitalButtonLookAndFeel;
    
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
    
    GreenLEDNumberLookAndFeel greenLEDNumberLookAndFeel;
    
    // Simple knob look and feel instance for macro knobs
    SimpleKnobLookAndFeel simpleKnobLookAndFeel;
    
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
    
    DigitalFilterKnobLookAndFeel digitalFilterKnobLookAndFeel;
    
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
    
    EngravedLabelLookAndFeel engravedLabelLookAndFeel;
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    ADSREnvelopeComponent adsrEnvelopeVisualizer;
    
    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    ADSREnvelopeComponent osc2AdsrEnvelopeVisualizer;
    
    // LFO MODULE
    LFOModuleComponent lfoModule;
    
    // EFFECTS MODULE
    juce::TabbedComponent effectsModule { juce::TabbedButtonBar::TabsAtTop };
    
    // Effects preset controls
    juce::TextButton effectsPresetPrevButton;
    juce::TextButton effectsPresetNextButton;
    juce::Label effectsPresetNameLabel;
    juce::TextButton effectsPresetSaveButton;
    juce::TextButton effectsPresetLoadButton;
    
    // Effects module border component
    class EffectsBorderComponent : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds();
            
            // Draw realistic 3D metallic border around the effects module (even larger border)
            // Outermost dark frame (thicker)
            g.setColour(juce::Colour(0xff1a1a1a));
            g.drawRect(bounds, 6);
            
            // Second layer - dark gray
            g.setColour(juce::Colour(0xff303030));
            g.drawRect(bounds.reduced(6), 3);
            
            // Third layer - medium gray
            g.setColour(juce::Colour(0xff505050));
            g.drawRect(bounds.reduced(9), 2);
            
            // Inner light highlight (metallic shine)
            g.setColour(juce::Colour(0xff909090));
            g.drawRect(bounds.reduced(11), 2);
            
            // Inner shadow
            g.setColour(juce::Colour(0xff0a0a0a));
            g.drawRect(bounds.reduced(13), 1);
        }
    };
    
    EffectsBorderComponent effectsBorder;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;
    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    juce::Slider volumeControlsVolumeKnob;
    juce::Label volumeControlsVolumeLabel;
    juce::Slider volumeControlsDetuneKnob;
    juce::Label volumeControlsDetuneLabel;
    juce::Slider volumeControlsStereoWidthKnob;
    juce::Label volumeControlsStereoWidthLabel;
    juce::Slider volumeControlsPanKnob;
    juce::Label volumeControlsPanLabel;
    // PHASE CONTROLS GROUP - (STATIONARY)
    juce::Slider phaseControlsPhaseKnob;
    juce::Label phaseControlsPhaseLabel;
    // ADSR KNOBS GROUP - Row 3 (MOVEABLE)
    juce::Slider adsrAttackKnob;
    juce::Label adsrAttackLabel;
    juce::Slider adsrDecayKnob;
    juce::Label adsrDecayLabel;
    juce::Slider adsrSustainKnob;
    juce::Label adsrSustainLabel;
    juce::Slider adsrReleaseKnob;
    juce::Label adsrReleaseLabel;
    
    // Pulse width control
    juce::Slider pulseWidthSlider;
    juce::Label pulseWidthLabel;
    
    // PITCH CONTROLS GROUP - Row 5 (MOVEABLE)
    juce::Label pitchControlsOctaveLabel;
    juce::Label pitchControlsOctaveValueLabel;
    juce::Label pitchControlsSemitoneLabel;
    juce::Label pitchControlsSemitoneValueLabel;
    juce::Label pitchControlsFineTuneLabel;
    juce::Label pitchControlsFineTuneValueLabel;
    juce::Label pitchControlsVoiceCountLabel;
    juce::Label pitchControlsVoiceCountValueLabel;
    
    // WAVE TYPE BUTTONS GROUP - Row 1 (MOVEABLE)
    juce::TextButton waveTypeSineButton;
    juce::TextButton waveTypeSawButton;
    juce::TextButton waveTypeSquareButton;
    juce::TextButton waveTypeTriangleButton;
    juce::TextButton waveTypeWhiteNoiseButton;
    juce::TextButton waveTypePinkNoiseButton;
    juce::TextButton waveTypeRandomPhaseButton;
    
    // SECOND OSCILLATOR CONTROLS - Row 6 (MOVEABLE)
    juce::TextButton osc2SineButton;
    juce::TextButton osc2SawButton;
    juce::TextButton osc2SquareButton;
    juce::TextButton osc2TriangleButton;
    juce::TextButton osc2WhiteNoiseButton;
    juce::TextButton osc2PinkNoiseButton;
    juce::Slider osc2VolumeKnob;
    juce::Label osc2VolumeLabel;
    juce::Slider osc2DetuneKnob;
    juce::Label osc2DetuneLabel;
    juce::Slider osc2StereoKnob;
    juce::Label osc2StereoLabel;
    juce::Label osc2VoicesLabel;
    juce::Label osc2VoicesValueLabel;
    juce::Slider osc2PanKnob;
    juce::Label osc2PanLabel;
    juce::Label osc2OctaveLabel;
    juce::Label osc2OctaveValueLabel;
    juce::Label osc2SemitoneLabel;
    juce::Label osc2SemitoneValueLabel;
    juce::Label osc2FineTuneLabel;
    juce::Label osc2FineTuneValueLabel;
    juce::TextButton osc2RandomPhaseButton;
    juce::Slider osc2PhaseKnob;
    juce::Label osc2PhaseLabel;
    juce::Label osc2TitleLabel;
    juce::Slider osc2AttackKnob;
    juce::Label osc2AttackLabel;
    juce::Slider osc2DecayKnob;
    juce::Label osc2DecayLabel;
    juce::Slider osc2SustainKnob;
    juce::Label osc2SustainLabel;
    juce::Slider osc2ReleaseKnob;
    juce::Label osc2ReleaseLabel;
    juce::TextButton osc2AdsrLinkButton;
    
    // Filter controls
    juce::Slider filterCutoffKnob;
    juce::Label filterCutoffLabel;
    juce::Slider filterResonanceKnob;
    juce::Label filterResonanceLabel;
    juce::TextButton osc1FilterEnableButton;
    juce::TextButton osc2FilterEnableButton;
    juce::TextButton filterLPButton;
    juce::TextButton filterHPButton;
    juce::TextButton filterBPButton;
    juce::TextButton filterNotchButton;
    juce::TextButton filterCombButton;
    juce::TextButton filterFormantButton;
    juce::TextButton filter12dBButton;
    juce::TextButton filter24dBButton;
    
    // Macro controls
    juce::Slider macro1Knob;
    juce::Label macro1Label;
    juce::Slider macro2Knob;
    juce::Label macro2Label;
    juce::Slider macro3Knob;
    juce::Label macro3Label;
    juce::Slider macro4Knob;
    juce::Label macro4Label;
    juce::Slider macro5Knob;
    juce::Label macro5Label;
    juce::Slider macro6Knob;
    juce::Label macro6Label;
    juce::Slider macro7Knob;
    juce::Label macro7Label;
    juce::Slider macro8Knob;
    juce::Label macro8Label;
    
    // Chorus effect controls
    juce::Slider chorusRateKnob;
    juce::Label chorusRateLabel;
    juce::Slider chorusDelay1Knob;
    juce::Label chorusDelay1Label;
    juce::Slider chorusDelay2Knob;
    juce::Label chorusDelay2Label;
    juce::Slider chorusDepthKnob;
    juce::Label chorusDepthLabel;
    juce::Slider chorusFeedKnob;
    juce::Label chorusFeedLabel;
    juce::Slider chorusLpfKnob;
    juce::Label chorusLpfLabel;
    juce::Slider chorusMixKnob;
    juce::Label chorusMixLabel;
    juce::TextButton chorusPowerButton;
    
    // Compressor effect controls
    juce::Slider compressorThresholdKnob;
    juce::Label compressorThresholdLabel;
    juce::Slider compressorRatioKnob;
    juce::Label compressorRatioLabel;
    juce::Slider compressorAttackKnob;
    juce::Label compressorAttackLabel;
    juce::Slider compressorReleaseKnob;
    juce::Label compressorReleaseLabel;
    juce::Slider compressorGainKnob;
    juce::Label compressorGainLabel;
    juce::TextButton compressorMultibandButton;
    juce::Slider compressorMixKnob;
    juce::Label compressorMixLabel;
    juce::TextButton compressorPowerButton;
    
    // Delay effect controls
    juce::Slider delayFeedbackKnob;
    juce::Label delayFeedbackLabel;
    juce::TextButton delayBpmButton;
    juce::TextButton delayLinkButton;
    juce::Slider delayLeftTimeSlider;
    juce::Label delayLeftTimeLabel;
    juce::Slider delayRightTimeSlider;
    juce::Label delayRightTimeLabel;
    juce::TextButton delayLeftTripletButton;
    juce::TextButton delayLeftDotButton;
    juce::TextButton delayRightTripletButton;
    juce::TextButton delayRightDotButton;
    juce::Slider delayFilterFreqKnob;
    juce::Label delayFilterFreqLabel;
    juce::Slider delayFilterQKnob;
    juce::Label delayFilterQLabel;
    juce::TextButton delayNormalButton;
    juce::TextButton delayPingPongButton;
    juce::Slider delayMixKnob;
    juce::Label delayMixLabel;
    juce::TextButton delayPowerButton;
    
    // Distortion effect controls
    juce::Label distortionTypeLabel;
    juce::Label distortionTypeValueLabel;
    juce::Slider distortionDriveKnob;
    juce::Label distortionDriveLabel;
    juce::Slider distortionMixKnob;
    juce::Label distortionMixLabel;
    juce::TextButton distortionPowerButton;
    
    // Distortion filter controls
    juce::TextButton distortionFilterOffButton;
    juce::TextButton distortionFilterPreButton;
    juce::TextButton distortionFilterPostButton;
    juce::TextButton distortionFilterLPButton;
    juce::TextButton distortionFilterBPButton;
    juce::TextButton distortionFilterHPButton;
    juce::Slider distortionFilterFreqKnob;
    juce::Label distortionFilterFreqLabel;
    juce::Slider distortionFilterQKnob;
    juce::Label distortionFilterQLabel;
    
    // Octave control state
    int octaveValue = 0;
    bool isDraggingOctave = false;
    int dragStartY = 0;
    int dragStartOctave = 0;
    
    // Semitone control state
    int semitoneValue = 0;
    bool isDraggingSemitone = false;
    int dragStartSemitone = 0;
    
    // Fine tune control state
    int fineTuneValue = 0;
    bool isDraggingFineTune = false;
    int dragStartFineTune = 0;
    
    // Voice count control state
    int voiceCountValue = 1;
    bool isDraggingVoiceCount = false;
    int dragStartVoiceCount = 0;
    
    // Oscillator 2 ADSR original values (stored when linking is enabled)
    float osc2OriginalAttack = 0.1f;
    float osc2OriginalDecay = 0.2f;
    float osc2OriginalSustain = 0.7f;
    float osc2OriginalRelease = 0.3f;
    
    // Oscillator 2 pitch control state
    int osc2OctaveValue = 0;
    bool isDraggingOsc2Octave = false;
    int dragStartOsc2Octave = 0;
    
    int osc2SemitoneValue = 0;
    bool isDraggingOsc2Semitone = false;
    int dragStartOsc2Semitone = 0;
    
    int osc2FineTuneValue = 0;
    bool isDraggingOsc2FineTune = false;
    int dragStartOsc2FineTune = 0;
    
    int osc2VoiceCountValue = 1;
    bool isDraggingOsc2VoiceCount = false;
    int dragStartOsc2VoiceCount = 0;
    
    // Distortion type control state
    int distortionTypeValue = 1; // 1-16 for different distortion types
    bool isDraggingDistortionType = false;
    int dragStartDistortionType = 0;
    
    // Cached bounds for each section to share between resized() and paint()
    juce::Rectangle<int> waveButtonsBounds;
    juce::Rectangle<int> adsrEnvelopeBounds;
    juce::Rectangle<int> adsrKnobsBounds;
    juce::Rectangle<int> volumeKnobsBounds;
    juce::Rectangle<int> octaveControlsBounds;
    juce::Rectangle<int> phaseControlsBounds;
    juce::Rectangle<int> secondOscillatorBounds;
    juce::Rectangle<int> osc2PitchControlsBounds;
    juce::Rectangle<int> osc2PhaseControlsBounds;
    juce::Rectangle<int> filterSectionBounds;
    juce::Rectangle<int> macroKnobsBounds;
    
    // MOVEABLE COMPONENT GROUPS - Offset controls for easy repositioning
    // Wave Type Buttons Group (Row 1 - Top row with SIN, SAW, SQR, TRI, WHT, PNK, RND PHASE)
    float waveTypeButtonsGroupOffsetX = 0.0f;
    float waveTypeButtonsGroupOffsetY = 25.0f;    // MOVED DOWN 25 pixels (reduced from 40)
    
    // ADSR Envelope Visualizer Group (Row 2 - Visual envelope display)
    float adsrEnvelopeGroupOffsetX = 0.0f;
    float adsrEnvelopeGroupOffsetY = 25.0f;       // MOVED DOWN 25 pixels (reduced from 40)
    
    // ADSR Knobs Group (Row 3 - Attack, Decay, Sustain, Release knobs)
    float adsrKnobsGroupOffsetX = 0.0f;
    float adsrKnobsGroupOffsetY = 25.0f;          // MOVED DOWN 25 pixels (reduced from 40)
    
    // Volume Controls Group (Row 4 - Volume, Detune, Stereo Width, Pan knobs)
    float volumeControlsGroupOffsetX = 0.0f;
    float volumeControlsGroupOffsetY = 25.0f;     // MOVED DOWN 25 pixels (reduced from 40)
    
    // Pitch Controls Group (Row 5 - Octave, Semitone, Fine Tune, Voice Count sliders)
    float pitchControlsGroupOffsetX = 0.0f;
    float pitchControlsGroupOffsetY = 18.0f;      // MOVED DOWN 18 pixels (tiny adjustment)
    
    // Second Oscillator Group (Row 6 - Simple sine wave and volume)
    float secondOscillatorGroupOffsetX = 0.0f;
    float secondOscillatorGroupOffsetY = 0.0f;   // Reset to 0 for top-right positioning
    
    // STATIONARY COMPONENTS - These remain in place
    // Phase Controls Group (Phase knob + button - stays put)
    float phaseControlsGroupOffsetX = 0.0f;
    float phaseControlsGroupOffsetY = 0.0f;
    
    // Oscillator background paint (main background - stays put)
    float oscillatorBackgroundOffsetX = 10.0f;    // X offset for background alignment with wood border
    float oscillatorBackgroundOffsetY = -10.0f;   // Y offset for background alignment with wood border

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};
