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
        
        // Draw ultra-HD 3D knob center circle (always visible, doesn't indicate position)
        auto knobRadius = radius * 0.55f; // Smaller for better proportions
        auto knobBounds = juce::Rectangle<float>(centreX - knobRadius, centreY - knobRadius, 
                                                knobRadius * 2.0f, knobRadius * 2.0f);
        
        // Multi-layer drop shadow for realistic depth
        for (int i = 4; i >= 1; --i)
        {
            auto shadowAlpha = 0.15f - (i * 0.03f);
            g.setColour(juce::Colour(0xff000000).withAlpha(shadowAlpha));
            g.fillEllipse(knobBounds.translated(i * 0.8f, i * 0.8f));
        }
        
        // Create radial gradient for main knob body (darker colors)
        juce::ColourGradient knobGradient(
            juce::Colour(0xff1a1a1a),    // Darker top
            centreX - knobRadius * 0.4f, centreY - knobRadius * 0.4f,
            juce::Colour(0xff080808),    // Much darker bottom
            centreX + knobRadius * 0.3f, centreY + knobRadius * 0.3f,
            true
        );
        knobGradient.addColour(0.3, juce::Colour(0xff141414));
        knobGradient.addColour(0.7, juce::Colour(0xff0a0a0a));
        g.setGradientFill(knobGradient);
        g.fillEllipse(knobBounds);
        
        // Outer metallic rim with gradient (darker)
        juce::ColourGradient rimGradient(
            juce::Colour(0xff303030),    // Darker top rim
            centreX, centreY - knobRadius,
            juce::Colour(0xff050505),    // Much darker bottom rim
            centreX, centreY + knobRadius,
            false
        );
        g.setGradientFill(rimGradient);
        g.drawEllipse(knobBounds, 2.0f);
        
        // Inner rim for more definition
        g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
        g.drawEllipse(knobBounds.reduced(1.5f), 1.0f);
        
        // Primary highlight - large soft glow from top-left (darker)
        auto primaryHighlight = knobBounds.reduced(knobRadius * 0.15f);
        primaryHighlight.translate(-knobRadius * 0.12f, -knobRadius * 0.18f);
        juce::ColourGradient primaryGradient(
            juce::Colour(0xff505050).withAlpha(0.7f),    // Dimmer center
            primaryHighlight.getCentreX(), primaryHighlight.getCentreY(),
            juce::Colour(0xff303030).withAlpha(0.0f),    // Fade to transparent
            primaryHighlight.getCentreX() + primaryHighlight.getWidth() * 0.4f,
            primaryHighlight.getCentreY() + primaryHighlight.getHeight() * 0.4f,
            true
        );
        g.setGradientFill(primaryGradient);
        g.fillEllipse(primaryHighlight);
        
        // Secondary highlight - smaller, more intense (darker)
        auto secondaryHighlight = knobBounds.reduced(knobRadius * 0.3f);
        secondaryHighlight.translate(-knobRadius * 0.08f, -knobRadius * 0.12f);
        juce::ColourGradient secondaryGradient(
            juce::Colour(0xff606060).withAlpha(0.6f),
            secondaryHighlight.getCentreX(), secondaryHighlight.getCentreY(),
            juce::Colour(0xff404040).withAlpha(0.0f),
            secondaryHighlight.getRight(), secondaryHighlight.getBottom(),
            true
        );
        g.setGradientFill(secondaryGradient);
        g.fillEllipse(secondaryHighlight);
        
        // Subtle bottom shadow inside knob
        auto bottomShadow = knobBounds.reduced(knobRadius * 0.1f);
        bottomShadow.translate(knobRadius * 0.05f, knobRadius * 0.08f);
        bottomShadow.setHeight(bottomShadow.getHeight() * 0.6f);
        bottomShadow.setY(bottomShadow.getY() + bottomShadow.getHeight() * 0.4f);
        juce::ColourGradient shadowGradient(
            juce::Colour(0xff000000).withAlpha(0.0f),
            bottomShadow.getCentreX(), bottomShadow.getY(),
            juce::Colour(0xff000000).withAlpha(0.4f),
            bottomShadow.getCentreX(), bottomShadow.getBottom(),
            false
        );
        g.setGradientFill(shadowGradient);
        g.fillEllipse(bottomShadow);
        
        
        // Subtle reflective ring around the knob (darker)
        auto reflectiveRing = knobBounds.reduced(knobRadius * 0.05f);
        g.setColour(juce::Colour(0xff404040).withAlpha(0.1f));
        g.drawEllipse(reflectiveRing, 0.5f);
        
        // Draw the block arc based on slider position (around the outer edge)
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
    
    // Helper methods for grouped component positioning and background drawing
    void layoutWaveTypeButtons(juce::Rectangle<int>& bounds);
    void layoutADSREnvelope(juce::Rectangle<int>& bounds);
    void layoutADSRKnobs(juce::Rectangle<int>& bounds);
    void layoutVolumeKnobs(juce::Rectangle<int>& bounds);
    void layoutOctaveControls(juce::Rectangle<int>& bounds);
    void layoutPhaseControls(juce::Rectangle<int>& bounds);
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

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;
    
    // Custom look and feel for wave buttons
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;
    
    // Custom look and feel for LED labels
    LEDLabelLookAndFeel ledLabelLookAndFeel;
    LEDNumberLookAndFeel ledNumberLookAndFeel;
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    ADSREnvelopeComponent adsrEnvelopeVisualizer;
    
    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    ADSREnvelopeComponent osc2AdsrEnvelopeVisualizer;
    
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
