#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;

class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(5);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // Draw the block arc based on slider position
        if (sliderPos > 0.0f)
        {
            // Create evenly spaced white blocks with LED glow
            auto arcRadius = radius - 1;
            auto blockSize = 0.08f; // Smaller blocks for rounder appearance
            auto gapSize = 0.02f; // Smaller gaps for smoother look
            auto currentAngle = rotaryStartAngle;
            
            while (currentAngle < angle)
            {
                auto blockEndAngle = juce::jmin(currentAngle + blockSize, angle);
                
                auto startX = centreX + arcRadius * std::cos(currentAngle - juce::MathConstants<float>::halfPi);
                auto startY = centreY + arcRadius * std::sin(currentAngle - juce::MathConstants<float>::halfPi);
                auto endX = centreX + arcRadius * std::cos(blockEndAngle - juce::MathConstants<float>::halfPi);
                auto endY = centreY + arcRadius * std::sin(blockEndAngle - juce::MathConstants<float>::halfPi);
                
                // Create the block path
                juce::Path blockPath;
                blockPath.addLineSegment(juce::Line<float>(startX, startY, endX, endY), 5.0f);
                
                // Draw LED glow effect for each block
                // Outer glow layers
                for (float glowRadius = 3.0f; glowRadius >= 1.0f; glowRadius -= 0.5f)
                {
                    auto alpha = 0.02f + (0.06f * (3.5f - glowRadius) / 2.5f);
                    g.setColour(juce::Colours::white.withAlpha(alpha));
                    
                    juce::Path glowPath;
                    glowPath.addLineSegment(juce::Line<float>(startX, startY, endX, endY), 5.0f + glowRadius * 2);
                    g.fillPath(glowPath);
                }
                
                // Inner glow
                g.setColour(juce::Colours::white.withAlpha(0.3f));
                juce::Path innerGlowPath;
                innerGlowPath.addLineSegment(juce::Line<float>(startX, startY, endX, endY), 6.0f);
                g.fillPath(innerGlowPath);
                
                // Core bright white block
                g.setColour(juce::Colours::white);
                g.fillPath(blockPath);
                
                currentAngle += blockSize + gapSize;
            }
        }
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = label.getFont();
        auto justification = label.getJustificationType();
        
        g.setFont(font);
        
        // Text glow
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

class WaveButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto font = juce::Font("Press Start 2P", 10.0f, juce::Font::plain);
        g.setFont(font);
        auto bounds = button.getLocalBounds();
        auto text = button.getButtonText();
        
        // LED glow effect using the same technique as knob labels
        if (button.getToggleState())
        {
            // Determine color based on button text when toggled (selected)
            juce::Colour ledColour;
            if (text == "PNK")
            {
                ledColour = juce::Colours::pink;
            }
            else
            {
                ledColour = juce::Colours::white; // All buttons use white LED except pink
            }
            
            // Outer glow - multiple layers using 4-directional technique like knob labels
            for (float i = 2.5f; i >= 1.0f; i -= 0.5f)
            {
                auto alpha = 0.03f + (0.05f * (3.0f - i) / 2.0f);
                g.setColour(ledColour.withAlpha(alpha));
                
                // Draw in 4 directions for each glow layer
                g.drawText(text, bounds.translated(-i, 0), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(i, 0), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(0, -i), juce::Justification::centred, true);
                g.drawText(text, bounds.translated(0, i), juce::Justification::centred, true);
            }
            
            // Inner glow - more intense
            g.setColour(ledColour.withAlpha(0.15f));
            g.drawText(text, bounds.translated(-1, 0), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(1, 0), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(0, -1), juce::Justification::centred, true);
            g.drawText(text, bounds.translated(0, 1), juce::Justification::centred, true);
            
            // Core bright text
            g.setColour(ledColour);
            g.drawText(text, bounds, juce::Justification::centred, true);
        }
        else
        {
            // Normal state - dim LED effect
            juce::Colour dimColour = juce::Colour(0xff404040);
            if (isMouseOverButton)
                dimColour = dimColour.brighter(0.1f);
            if (isButtonDown)
                dimColour = dimColour.darker(0.1f);
                
            g.setColour(dimColour);
            g.drawText(text, bounds, juce::Justification::centred, true);
        }
    }
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        
        // Check if button is toggled (selected)
        auto* textButton = dynamic_cast<juce::TextButton*>(&button);
        bool isToggled = textButton && textButton->getToggleState();
        
        // 3D metallic button styling similar to knob backgrounds
        float cornerRadius = 3.0f;
        
        if (isToggled)
        {
            // Pressed/selected state - darker and inset look
            g.setColour(juce::Colour(0xff0a0a0a)); // Very dark base
            g.fillRoundedRectangle(bounds, cornerRadius);
            
            // Dark border for inset effect
            g.setColour(juce::Colour(0xff000000).withAlpha(0.9f));
            g.drawRoundedRectangle(bounds, cornerRadius, 2.0f);
            
            // Inner highlight for metallic effect
            g.setColour(juce::Colour(0xff2a2a2a).withAlpha(0.6f));
            g.drawRoundedRectangle(bounds.reduced(1.5f), cornerRadius - 1.0f, 1.0f);
        }
        else
        {
            // Normal state - raised 3D button look
            juce::Colour baseColour = isButtonDown ? juce::Colour(0xff0d0d0d) 
                : isMouseOverButton ? juce::Colour(0xff181818)
                : juce::Colour(0xff0f0f0f); // Same as knob backgrounds
            
            // Base fill
            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, cornerRadius);
            
            // Outer border for depth
            g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
            g.drawRoundedRectangle(bounds, cornerRadius, 2.0f);
            
            // Inner metallic highlight
            g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
            g.drawRoundedRectangle(bounds.reduced(1.0f), cornerRadius - 0.5f, 1.0f);
            
            // Top highlight for 3D raised effect
            if (!isButtonDown)
            {
                g.setColour(juce::Colour(0xff606060).withAlpha(0.3f));
                auto topHighlight = bounds.reduced(2.0f);
                topHighlight.setHeight(topHighlight.getHeight() * 0.3f);
                g.fillRoundedRectangle(topHighlight, cornerRadius - 1.0f);
            }
        }
    }
};

class LEDLabelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = label.getFont();
        auto justification = label.getJustificationType();
        
        g.setFont(font);
        
        // Subtle outer glow - much more restrained
        for (float i = 2.5f; i >= 1.0f; i -= 0.5f)
        {
            auto alpha = 0.03f + (0.05f * (3.0f - i) / 2.0f); // Very low alpha
            g.setColour(juce::Colours::white.withAlpha(alpha));
            
            // Only 4 directions for cleaner look
            g.drawText(text, bounds.translated(-i, 0), justification, true);
            g.drawText(text, bounds.translated(i, 0), justification, true);
            g.drawText(text, bounds.translated(0, -i), justification, true);
            g.drawText(text, bounds.translated(0, i), justification, true);
        }
        
        // Very subtle inner glow
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawText(text, bounds.translated(-1, 0), justification, true);
        g.drawText(text, bounds.translated(1, 0), justification, true);
        g.drawText(text, bounds.translated(0, -1), justification, true);
        g.drawText(text, bounds.translated(0, 1), justification, true);
        
        // Core bright white text - the main readable text
        g.setColour(juce::Colours::white);
        g.drawText(text, bounds, justification, true);
    }
};

class LEDNumberLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds().toFloat();
        auto text = label.getText();
        auto font = label.getFont();
        auto justification = label.getJustificationType();
        
        g.setFont(font);
        
        // Text glow
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

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;
    
    // Custom look and feel for wave buttons
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;
    
    // Custom look and feel for LED labels
    LEDLabelLookAndFeel ledLabelLookAndFeel;
    LEDNumberLookAndFeel ledNumberLookAndFeel;
    
    // ADSR envelope visualization
    ADSREnvelopeComponent envelopeComponent;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider detuneSlider;
    juce::Label detuneLabel;
    juce::Slider stereoWidthSlider;
    juce::Label stereoWidthLabel;
    juce::Slider panSlider;
    juce::Label panLabel;
    juce::Slider phaseSlider;
    juce::Label phaseLabel;
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    
    // Pulse width control
    juce::Slider pulseWidthSlider;
    juce::Label pulseWidthLabel;
    
    // Octave control
    juce::Label octaveLabel;
    juce::Label octaveValueLabel;
    
    // Semitone control
    juce::Label semitoneLabel;
    juce::Label semitoneValueLabel;
    
    // Fine tune control
    juce::Label fineTuneLabel;
    juce::Label fineTuneValueLabel;
    
    // Voice count control
    juce::Label voiceCountLabel;
    juce::Label voiceCountValueLabel;
    
    // Oscillator type buttons
    juce::TextButton sineWaveButton;
    juce::TextButton sawWaveButton;
    juce::TextButton squareWaveButton;
    juce::TextButton triangleWaveButton;
    juce::TextButton noiseWaveButton;
    juce::TextButton pinkNoiseButton;
    
    // Random phase button
    juce::TextButton randomPhaseButton;
    
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};