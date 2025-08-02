#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class LFOComponent : public juce::Component
{
public:
    LFOComponent() 
    {
        waveformPoints.resize(numWaveformPoints);
        resetToSineWave();
    }
    ~LFOComponent() override {}
    
    void paint(juce::Graphics& g) override 
    {
        auto bounds = getLocalBounds();
        
        // Draw background with metallic frame (matching knob style)
        g.setColour(juce::Colour(0xff0f0f0f));
        g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
        
        // Inner metallic frame
        auto metalBounds = bounds.toFloat().reduced(3.0f);
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillRoundedRectangle(metalBounds, 6.0f);
        
        // Metallic highlight
        g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
        g.drawRoundedRectangle(metalBounds.reduced(1.0f), 5.0f, 1.0f);
        
        // Draw dark background inside the frames
        waveformArea = bounds.reduced(12, 12);
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRoundedRectangle(waveformArea.toFloat(), 4.0f);
        
        if (waveformArea.getWidth() <= 0 || waveformArea.getHeight() <= 0) 
            return;
        
        // Draw grid lines
        g.setColour(juce::Colour(0xff202020));
        
        // Horizontal center line
        int centerY = waveformArea.getCentreY();
        g.drawLine(waveformArea.getX(), centerY, waveformArea.getRight(), centerY, 1.0f);
        
        // Vertical grid lines (4 divisions)
        for (int i = 1; i < 4; ++i)
        {
            int x = waveformArea.getX() + (waveformArea.getWidth() * i / 4);
            g.setColour(juce::Colour(0xff151515));
            g.drawLine(x, waveformArea.getY(), x, waveformArea.getBottom(), 0.5f);
        }
        
        // Horizontal grid lines (top and bottom quarter lines)
        int quarterY1 = waveformArea.getY() + waveformArea.getHeight() / 4;
        int quarterY2 = waveformArea.getY() + (waveformArea.getHeight() * 3 / 4);
        g.setColour(juce::Colour(0xff151515));
        g.drawLine(waveformArea.getX(), quarterY1, waveformArea.getRight(), quarterY1, 0.5f);
        g.drawLine(waveformArea.getX(), quarterY2, waveformArea.getRight(), quarterY2, 0.5f);
        
        // Draw the waveform
        if (waveformPoints.size() >= 2)
        {
            juce::Path waveformPath;
            
            // Start the path
            auto firstPoint = getPointFromWaveformData(0);
            waveformPath.startNewSubPath(firstPoint.x, firstPoint.y);
            
            // Add lines to all points
            for (int i = 1; i < waveformPoints.size(); ++i)
            {
                auto point = getPointFromWaveformData(i);
                waveformPath.lineTo(point.x, point.y);
            }
            
            // Draw the waveform with LED glow effect
            // Outer glow layers
            for (float glowRadius = 3.0f; glowRadius >= 1.0f; glowRadius -= 0.5f)
            {
                auto alpha = 0.02f + (0.06f * (3.5f - glowRadius) / 2.5f);
                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.strokePath(waveformPath, juce::PathStrokeType(2.0f + glowRadius));
            }
            
            // Inner glow
            g.setColour(juce::Colours::white.withAlpha(0.3f));
            g.strokePath(waveformPath, juce::PathStrokeType(3.0f));
            
            // Core bright white waveform
            g.setColour(juce::Colours::white);
            g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
            
            // Draw control points as small circles when editing
            if (isDragging)
            {
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                for (int i = 0; i < waveformPoints.size(); i += 8) // Show every 8th point
                {
                    auto point = getPointFromWaveformData(i);
                    g.fillEllipse(point.x - 2, point.y - 2, 4, 4);
                }
            }
        }
    }
    
    void resized() override 
    {
        waveformArea = getLocalBounds().reduced(12, 12);
    }
    
    // Mouse events for drawing the waveform
    void mouseDown(const juce::MouseEvent& event) override
    {
        if (!waveformArea.contains(event.getPosition()))
            return;
            
        isDragging = true;
        dragStartIndex = getWaveformIndexFromX(event.x);
        updateWaveformPoint(event.x, event.y);
        repaint();
    }
    
    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (!isDragging)
            return;
            
        updateWaveformPoint(event.x, event.y);
        repaint();
    }
    
    void mouseUp(const juce::MouseEvent& event) override
    {
        isDragging = false;
        dragStartIndex = -1;
        repaint();
    }
    
    // Get/set waveform data
    const std::vector<float>& getWaveformData() const { return waveformPoints; }
    void setWaveformData(const std::vector<float>& data) {}
    
    // Reset to default waveform
    void resetToSineWave()
    {
        for (int i = 0; i < numWaveformPoints; ++i)
        {
            float phase = (i / float(numWaveformPoints - 1)) * 2.0f * juce::MathConstants<float>::pi;
            waveformPoints[i] = 0.5f + 0.4f * std::sin(phase);
        }
        repaint();
    }
    
    void resetToSawWave()
    {
        for (int i = 0; i < numWaveformPoints; ++i)
        {
            float phase = i / float(numWaveformPoints - 1);
            waveformPoints[i] = 0.1f + 0.8f * phase;
        }
        repaint();
    }
    
    void resetToSquareWave()
    {
        for (int i = 0; i < numWaveformPoints; ++i)
        {
            float phase = i / float(numWaveformPoints - 1);
            waveformPoints[i] = (phase < 0.5f) ? 0.8f : 0.2f;
        }
        repaint();
    }
    
    void resetToTriangleWave()
    {
        for (int i = 0; i < numWaveformPoints; ++i)
        {
            float phase = i / float(numWaveformPoints - 1);
            if (phase < 0.5f)
                waveformPoints[i] = 0.1f + 0.8f * (phase * 2.0f);
            else
                waveformPoints[i] = 0.9f - 0.8f * ((phase - 0.5f) * 2.0f);
        }
        repaint();
    }
    
private:
    // Waveform drawing area
    juce::Rectangle<int> waveformArea;
    
    // Waveform data (normalized 0.0 to 1.0, where 0.5 is center)
    std::vector<float> waveformPoints;
    static const int numWaveformPoints = 128; // Resolution of drawable waveform
    
    // Drawing state
    bool isDragging = false;
    int dragStartIndex = -1;
    
    // Helper methods
    void updateWaveformPoint(int x, int y)
    {
        if (waveformArea.getWidth() <= 0)
            return;
            
        int index = getWaveformIndexFromX(x);
        float value = getWaveformValueFromY(y);
        
        if (index >= 0 && index < waveformPoints.size())
        {
            waveformPoints[index] = value;
            
            // Smooth neighboring points for natural curves
            int range = 3; // How many neighboring points to affect
            for (int offset = -range; offset <= range; ++offset)
            {
                int neighborIndex = index + offset;
                if (neighborIndex >= 0 && neighborIndex < waveformPoints.size() && neighborIndex != index)
                {
                    float distance = std::abs(offset) / float(range);
                    float influence = 1.0f - (distance * distance); // Quadratic falloff
                    float targetValue = waveformPoints[neighborIndex];
                    waveformPoints[neighborIndex] = targetValue + (value - targetValue) * influence * 0.3f;
                }
            }
        }
    }
    
    int getWaveformIndexFromX(int x) const
    {
        if (waveformArea.getWidth() <= 0)
            return 0;
            
        float normalizedX = (x - waveformArea.getX()) / float(waveformArea.getWidth());
        normalizedX = juce::jlimit(0.0f, 1.0f, normalizedX);
        return static_cast<int>(normalizedX * (waveformPoints.size() - 1));
    }
    
    float getWaveformValueFromY(int y) const
    {
        if (waveformArea.getHeight() <= 0)
            return 0.5f;
            
        float normalizedY = (y - waveformArea.getY()) / float(waveformArea.getHeight());
        normalizedY = juce::jlimit(0.0f, 1.0f, normalizedY);
        return 1.0f - normalizedY; // Invert Y axis (top = 1.0, bottom = 0.0)
    }
    juce::Point<int> getPointFromWaveformData(int index) const 
    {
        if (index < 0 || index >= waveformPoints.size() || waveformArea.getWidth() <= 0)
            return juce::Point<int>(0, 0);
            
        float normalizedX = index / float(waveformPoints.size() - 1);
        float normalizedY = 1.0f - waveformPoints[index]; // Invert Y axis
        
        int x = waveformArea.getX() + static_cast<int>(normalizedX * waveformArea.getWidth());
        int y = waveformArea.getY() + static_cast<int>(normalizedY * waveformArea.getHeight());
        
        return juce::Point<int>(x, y);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOComponent)
};

class LFOModuleComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    // Simple knob look and feel for rate knob (no LED lighting)
    class SimpleKnobLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider& slider) override
        {
            auto bounds = juce::Rectangle<float>(x, y, width, height);
            auto centreX = bounds.getCentreX();
            auto centreY = bounds.getCentreY() + 2.0f; // Lower the visual knob by 2 pixels (moved down 1px from +1)
            
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

    // Custom look and feel for smaller LFO buttons
    class SmallLFOButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool) override
        {
            // Use much smaller font for trigger button, normal for others
            float fontSize = (button.getButtonText() == "TRIGGER") ? 2.0f : 7.0f;
            auto font = juce::Font("Press Start 2P", fontSize, juce::Font::plain);
            g.setFont(font);
            g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                                   : juce::TextButton::textColourOffId)
                              .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

            const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
            const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

            const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
            const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
            const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

            g.drawFittedText(button.getButtonText(),
                           leftIndent, yIndent, button.getWidth() - leftIndent - rightIndent,
                           button.getHeight() - yIndent * 2,
                           juce::Justification::centred, 1);
        }
    };
    
    // Engraved label look and feel for LFO title
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

    // Trigger button look and feel - same as main button but smaller font
    class TriggerButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    private:
        WaveButtonLookAndFeel* mainButtonLookAndFeel;
        
    public:
        TriggerButtonLookAndFeel(WaveButtonLookAndFeel* mainLookAndFeel) 
            : mainButtonLookAndFeel(mainLookAndFeel) {}
        
        void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            if (mainButtonLookAndFeel)
                mainButtonLookAndFeel->drawButtonBackground(g, button, backgroundColour, 
                                                          shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            else
                juce::LookAndFeel_V4::drawButtonBackground(g, button, backgroundColour, 
                                                         shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        }
        
        void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool) override
        {
            // Use smaller font specifically for trigger button
            float fontSize = 8.5f; // Good size for "TRIG"
            auto font = juce::Font("Press Start 2P", fontSize, juce::Font::plain);
            g.setFont(font);
            g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                                   : juce::TextButton::textColourOffId)
                              .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

            const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
            const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

            const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
            const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
            const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

            g.drawFittedText(button.getButtonText(),
                           leftIndent, yIndent, button.getWidth() - leftIndent - rightIndent,
                           button.getHeight() - yIndent * 2,
                           juce::Justification::centred, 1);
        }
        
        void setMainButtonLookAndFeel(WaveButtonLookAndFeel* newMainLookAndFeel)
        {
            mainButtonLookAndFeel = newMainLookAndFeel;
        }
    };

    LFOModuleComponent() : knobLookAndFeel(nullptr), buttonLookAndFeel(nullptr), labelLookAndFeel(nullptr), triggerButtonLookAndFeel(nullptr) 
    {
        // Set up title label with engraved effect
        lfoTitleLabel.setText("LFO", juce::dontSendNotification);
        lfoTitleLabel.setFont(juce::Font("Times New Roman", 11.0f, juce::Font::bold));
        lfoTitleLabel.setJustificationType(juce::Justification::centred);
        lfoTitleLabel.setLookAndFeel(&engravedLabelLookAndFeel);
        addAndMakeVisible(lfoTitleLabel);
        
        // Set up LFO waveform drawing area
        addAndMakeVisible(lfoWaveform);
        
        // Set up rate knob
        lfoRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        lfoRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        lfoRateKnob.setRange(0.1, 20.0, 0.1);
        lfoRateKnob.setValue(1.0);
        lfoRateKnob.addListener(this);
        addAndMakeVisible(lfoRateKnob);
        
        lfoRateLabel.setText("RATE", juce::dontSendNotification);
        lfoRateLabel.setFont(juce::Font("Press Start 2P", 7.2f, juce::Font::plain)); // 8.0f * 0.9
        lfoRateLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        lfoRateLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(lfoRateLabel);
        
        // Set up rate value display label
        lfoRateValueLabel.setText("1.000 Hz", juce::dontSendNotification);
        lfoRateValueLabel.setFont(juce::Font("Press Start 2P", 7.5f, juce::Font::plain));
        lfoRateValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        lfoRateValueLabel.setJustificationType(juce::Justification::centred);
        lfoRateValueLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xff1a1a1a));
        lfoRateValueLabel.setColour(juce::Label::outlineColourId, juce::Colour(0xff404040));
        addAndMakeVisible(lfoRateValueLabel);
        
        // Set up rate type buttons (Hz/BPM)
        lfoHzButton.setButtonText("Hz");
        lfoHzButton.setClickingTogglesState(true);
        lfoHzButton.setToggleState(true, juce::dontSendNotification);
        lfoHzButton.addListener(this);
        addAndMakeVisible(lfoHzButton);
        
        lfoBpmButton.setButtonText("BPM");
        lfoBpmButton.setClickingTogglesState(true);
        lfoBpmButton.setToggleState(false, juce::dontSendNotification);
        lfoBpmButton.addListener(this);
        addAndMakeVisible(lfoBpmButton);
        
        // Set up trigger button
        lfoTriggerButton.setButtonText("TRIG");
        lfoTriggerButton.setClickingTogglesState(true);
        lfoTriggerButton.setToggleState(false, juce::dontSendNotification);
        lfoTriggerButton.addListener(this);
        addAndMakeVisible(lfoTriggerButton);
        
        // Set up draw button
        lfoDrawButton.setButtonText("DRAW");
        lfoDrawButton.setClickingTogglesState(true);
        lfoDrawButton.setToggleState(true, juce::dontSendNotification);
        lfoDrawButton.addListener(this);
        addAndMakeVisible(lfoDrawButton);
        
        // Set up preset waveform buttons
        // lfoSineButton.setButtonText("SIN");
        // lfoSineButton.addListener(this);
        // addAndMakeVisible(lfoSineButton);
        
        // lfoSawButton.setButtonText("SAW");
        // lfoSawButton.addListener(this);
        // addAndMakeVisible(lfoSawButton);
        
        // lfoSquareButton.setButtonText("SQR");
        // lfoSquareButton.addListener(this);
        // addAndMakeVisible(lfoSquareButton);
        
        // lfoTriangleButton.setButtonText("TRI");
        // lfoTriangleButton.addListener(this);
        // addAndMakeVisible(lfoTriangleButton);
        
        lfoChaosButton.setButtonText("CHAOS");
        lfoChaosButton.setClickingTogglesState(true);
        lfoChaosButton.setToggleState(false, juce::dontSendNotification);
        lfoChaosButton.addListener(this);
        addAndMakeVisible(lfoChaosButton);
        
        // Apply smaller font look and feel to all buttons
        lfoHzButton.setLookAndFeel(&smallButtonLookAndFeel);
        lfoBpmButton.setLookAndFeel(&smallButtonLookAndFeel);
        lfoTriggerButton.setLookAndFeel(&smallButtonLookAndFeel);
        lfoDrawButton.setLookAndFeel(&smallButtonLookAndFeel);
        // lfoSineButton.setLookAndFeel(&smallButtonLookAndFeel);
        // lfoSawButton.setLookAndFeel(&smallButtonLookAndFeel);
        // lfoSquareButton.setLookAndFeel(&smallButtonLookAndFeel);
        // lfoTriangleButton.setLookAndFeel(&smallButtonLookAndFeel);
        lfoChaosButton.setLookAndFeel(&smallButtonLookAndFeel);
        
        // Initialize rate display
        updateRateDisplay();
    }
    ~LFOModuleComponent() override {}
    
    void setLookAndFeel(CustomKnobLookAndFeel* knobLookAndFeel, WaveButtonLookAndFeel* buttonLookAndFeel, LEDLabelLookAndFeel* labelLookAndFeel)
    {
        this->knobLookAndFeel = knobLookAndFeel;
        this->buttonLookAndFeel = buttonLookAndFeel;
        this->labelLookAndFeel = labelLookAndFeel;
        
        // Apply simple look and feel to rate knob (no LED lighting)
        lfoRateKnob.setLookAndFeel(&simpleKnobLookAndFeel);
        if (this->labelLookAndFeel) 
        {
            lfoTitleLabel.setLookAndFeel(this->labelLookAndFeel);
            lfoRateLabel.setLookAndFeel(this->labelLookAndFeel);
        }
        if (this->buttonLookAndFeel)
        {
            lfoHzButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoBpmButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoDrawButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSineButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSawButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSquareButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoTriangleButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoChaosButton.setLookAndFeel(this->buttonLookAndFeel);
            
            // Set up trigger button with custom look and feel that uses same background but smaller font
            triggerButtonLookAndFeel.setMainButtonLookAndFeel(this->buttonLookAndFeel);
            lfoTriggerButton.setLookAndFeel(&triggerButtonLookAndFeel);
        }
    }
    
    void paint(juce::Graphics& g) override 
    {
        auto bounds = getLocalBounds().toFloat();
        
        // EXACT COPY of oscillator 1 background styling
        
        // Shadow layer (bottom-right offset for depth)
        auto shadowBounds = bounds.translated(3.0f, 3.0f);
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(shadowBounds, 10.0f);

        // Dark base layer (for depth)
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillRoundedRectangle(bounds, 10.0f);

        // Main raised surface - metallic modular synth module look
        auto raisedBounds = bounds.reduced(2.0f);

        // Much darker metallic grey base
        g.setColour(juce::Colour(0xff202020));
        g.fillRoundedRectangle(raisedBounds, 8.0f);

        // Add very dark metallic gradient for depth
        juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                             juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
        g.setGradientFill(metallicGradient);
        g.fillRoundedRectangle(raisedBounds, 8.0f);

        // Add subtle metallic texture
        juce::Random metallicRandom(42);
        g.setColour(juce::Colour(0xff202020).withAlpha(0.3f));
        for (int i = 0; i < 200; ++i)
        {
            auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
            auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
            g.fillRect(x, y, 1.0f, 1.0f);
        }

        // Add lighter metallic highlights
        metallicRandom.setSeed(123);
        g.setColour(juce::Colour(0xff606060).withAlpha(0.2f));
        for (int i = 0; i < 150; ++i)
        {
            auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
            auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
            g.fillRect(x, y, 1.0f, 1.0f);
        }

        // Top and left highlights (simulating light from top-left)
        g.setColour(juce::Colour(0xff707070).withAlpha(0.8f));
        // Top highlight
        g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
                   raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
        // Left highlight  
        g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
                   raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);

        // Bottom and right shadows (simulating shadow from top-left light)
        g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
        // Bottom shadow
        g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
                   raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
        // Right shadow
        g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
                   raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);

        // Add corner screws for modular synth module look
        auto screwRadius = 6.0f;
        auto screwInset = 15.0f;

        // Top-left screw
        auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(tlScrew);
        g.setColour(juce::Colour(0xff808080));
        g.drawEllipse(tlScrew, 1.0f);
        // Screw slot
        g.setColour(juce::Colour(0xff101010));
        g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);

        // Top-right screw
        auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(trScrew);
        g.setColour(juce::Colour(0xff808080));
        g.drawEllipse(trScrew, 1.0f);
        // Screw slot
        g.setColour(juce::Colour(0xff101010));
        g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);

        // Bottom-left screw
        auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(blScrew);
        g.setColour(juce::Colour(0xff808080));
        g.drawEllipse(blScrew, 1.0f);
        // Screw slot
        g.setColour(juce::Colour(0xff101010));
        g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);

        // Bottom-right screw
        auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(brScrew);
        g.setColour(juce::Colour(0xff808080));
        g.drawEllipse(brScrew, 1.0f);
        // Screw slot
        g.setColour(juce::Colour(0xff101010));
        g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    }
    
    void resized() override 
    {
        auto bounds = getLocalBounds().reduced(14, 14);
        
        // Top row: Buttons split to corners with title in center
        auto topArea = bounds.removeFromTop(22);
        
        // Left side buttons (preset waveforms) - commented out
        // auto leftButtonArea = topArea.removeFromLeft(160);
        // int leftButtonWidth = leftButtonArea.getWidth() / 3; // Only 3 buttons on left
        
        // lfoSineButton.setBounds(leftButtonArea.removeFromLeft(leftButtonWidth).reduced(2, 1));
        // lfoSawButton.setBounds(leftButtonArea.removeFromLeft(leftButtonWidth).reduced(2, 1));
        // lfoSquareButton.setBounds(leftButtonArea.removeFromLeft(leftButtonWidth).reduced(2, 1));
        
        // Right side buttons - CHAOS button and Hz/BPM controls
        auto rightButtonArea = topArea.removeFromRight(120);
        
        // CHAOS button on right (moved 70 pixels to the left)
        auto chaosArea = rightButtonArea.removeFromLeft(60);
        auto chaosRect = chaosArea.reduced(2, 1);
        chaosRect = chaosRect.translated(-70, 0);
        lfoChaosButton.setBounds(chaosRect);
        
        // Hz/BPM buttons stacked vertically (moved 20 pixels to the left)
        auto hzBpmArea = rightButtonArea.removeFromLeft(60).translated(-20, 0);
        auto hzArea = hzBpmArea.removeFromTop(11);
        auto bpmArea = hzBpmArea.translated(0, 2);
        lfoHzButton.setBounds(hzArea.reduced(2, 0));
        lfoBpmButton.setBounds(bpmArea.reduced(2, 0));
        
        // Center title area (moved 20 pixels to the left)
        lfoTitleLabel.setBounds(topArea.translated(-20, 0));
        
        bounds.removeFromTop(5); // Small gap
        
        // Main waveform display area
        auto waveformArea = bounds.removeFromTop(90);
        waveformArea = waveformArea.reduced(14, 0);
        lfoWaveform.setBounds(waveformArea);
        
        bounds.removeFromTop(5); // Small gap
        
        // Bottom controls: Rate knob centered, action buttons spread out
        auto bottomControlsArea = bounds.removeFromTop(40);
        
        // Rate knob in center (moved 10 pixels to the left)
        auto rateKnobArea = juce::Rectangle<int>((bottomControlsArea.getWidth() - 40) / 2 - 10, 0, 40, 40);
        auto rateLabelArea = rateKnobArea.removeFromBottom(12);
        lfoRateLabel.setBounds(rateLabelArea);
        lfoRateKnob.setBounds(rateKnobArea);
        
        // Rate value display box next to rate knob (moved 118 pixels to the left and 3 pixels down)
        auto rateValueArea = juce::Rectangle<int>(rateKnobArea.getRight() + 5 - 118, rateKnobArea.getY() + 11, 75, 20);
        lfoRateValueLabel.setBounds(rateValueArea);
        
        // Trigger button on left (moved 25 pixels right and 3 pixels down) - same size as chaos button
        auto triggerArea = juce::Rectangle<int>(35, 13, 58, 18);
        lfoTriggerButton.setBounds(triggerArea);
        
        // Draw button on right (moved 55 pixels to the left and 5 pixels down)
        auto drawArea = juce::Rectangle<int>(bottomControlsArea.getWidth() - 115, 15, 50, 20);
        lfoDrawButton.setBounds(drawArea);
    }
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override 
    {
        if (slider == &lfoRateKnob)
        {
            updateRateDisplay();
        }
    }
    
    // Button listener
    void buttonClicked(juce::Button* button) override
    {
        if (button == &lfoHzButton)
        {
            // Radio button behavior for Hz/BPM
            if (!lfoHzButton.getToggleState())
            {
                lfoHzButton.setToggleState(true, juce::dontSendNotification);
                return;
            }
            
            lfoBpmButton.setToggleState(false, juce::dontSendNotification);
            updateRateDisplay();
            // TODO: Switch to Hz mode when implementation is added
        }
        else if (button == &lfoBpmButton)
        {
            // Radio button behavior for Hz/BPM
            if (!lfoBpmButton.getToggleState())
            {
                lfoBpmButton.setToggleState(true, juce::dontSendNotification);
                return;
            }
            
            lfoHzButton.setToggleState(false, juce::dontSendNotification);
            updateRateDisplay();
            // TODO: Switch to BPM mode when implementation is added
        }
        else if (button == &lfoTriggerButton)
        {
            // Toggle trigger mode
            bool isTriggered = lfoTriggerButton.getToggleState();
            // TODO: Enable/disable trigger mode when implementation is added
        }
        else if (button == &lfoDrawButton)
        {
            // Toggle draw mode - mutually exclusive with chaos mode
            bool isDrawMode = lfoDrawButton.getToggleState();
            if (isDrawMode)
            {
                lfoChaosButton.setToggleState(false, juce::dontSendNotification);
            }
            // TODO: Enable/disable draw mode when implementation is added
            // This could switch between different drawing behaviors
        }
        // else if (button == &lfoSineButton)
        // {
        //     lfoWaveform.resetToSineWave();
        // }
        // else if (button == &lfoSawButton)
        // {
        //     lfoWaveform.resetToSawWave();
        // }
        // else if (button == &lfoSquareButton)
        // {
        //     lfoWaveform.resetToSquareWave();
        // }
        // else if (button == &lfoTriangleButton)
        // {
        //     lfoWaveform.resetToTriangleWave();
        // }
        else if (button == &lfoChaosButton)
        {
            // Toggle chaos mode - mutually exclusive with draw mode
            bool isChaosMode = lfoChaosButton.getToggleState();
            if (isChaosMode)
            {
                lfoDrawButton.setToggleState(false, juce::dontSendNotification);
            }
            // TODO: Implement chaos LFO mode - for now just placeholder
            // This could generate a random/chaotic waveform or enable the current smooth drawing behavior
        }
    }
    
    void updateRateDisplay()
    {
        double rateValue = lfoRateKnob.getValue();
        
        if (lfoHzButton.getToggleState())
        {
            // Hz mode: slightly compressed logarithmic mapping from 0.001 to 100 Hz
            // Map knob range 0.1-20.0 to logarithmic scale 0.001-100 Hz with reduced granularity
            double normalizedValue = (rateValue - 0.1) / (20.0 - 0.1); // 0.0 to 1.0
            
            // Apply power curve to compress the lower end even more
            double compressedValue = std::pow(normalizedValue, 0.25); // 0.25 creates more even distribution
            
            double logMin = std::log(0.001); // ln(0.001) ≈ -6.91
            double logMax = std::log(100.0);  // ln(100) ≈ 4.61
            double logValue = logMin + compressedValue * (logMax - logMin);
            double displayHz = std::exp(logValue);
            
            juce::String displayText = juce::String(displayHz, 3) + " Hz";
            lfoRateValueLabel.setText(displayText, juce::dontSendNotification);
        }
        else if (lfoBpmButton.getToggleState())
        {
            // BPM mode: Convert knob value to musical note values
            // Map 0.1-20.0 to different note divisions
            const char* noteValues[] = {
                "32 bars", "16 bars", "8 bars", "4 bars", "2 bars", "1 bar",
                "1/2", "1/3", "1/4", "1/6", "1/8", "1/12", "1/16", "1/24", 
                "1/32", "1/48", "1/64", "1/96", "1/128", "1/192", "1/256"
            };
            
            int numValues = sizeof(noteValues) / sizeof(noteValues[0]);
            int index = static_cast<int>((rateValue - 0.1) / (20.0 - 0.1) * (numValues - 1));
            index = juce::jlimit(0, numValues - 1, index);
            
            lfoRateValueLabel.setText(noteValues[index], juce::dontSendNotification);
        }
    }
    
private:
    // LFO waveform drawing component
    LFOComponent lfoWaveform;
    
    // Rate control
    juce::Slider lfoRateKnob;
    juce::Label lfoRateLabel;
    juce::Label lfoRateValueLabel;
    
    // Rate type buttons (Hz/BPM)
    juce::TextButton lfoHzButton;
    juce::TextButton lfoBpmButton;
    
    // Trigger mode button
    juce::TextButton lfoTriggerButton;
    
    // Draw mode button
    juce::TextButton lfoDrawButton;
    
    // Preset waveform buttons
    juce::TextButton lfoSineButton;
    juce::TextButton lfoSawButton;
    juce::TextButton lfoSquareButton;
    juce::TextButton lfoTriangleButton;
    juce::TextButton lfoChaosButton;
    
    // Title label
    juce::Label lfoTitleLabel;
    
    // Look and feel references (not owned)
    CustomKnobLookAndFeel* knobLookAndFeel;
    WaveButtonLookAndFeel* buttonLookAndFeel;
    LEDLabelLookAndFeel* labelLookAndFeel;
    
    // Custom small button look and feel (owned)
    SmallLFOButtonLookAndFeel smallButtonLookAndFeel;
    
    // Simple knob look and feel for rate knob (owned)
    SimpleKnobLookAndFeel simpleKnobLookAndFeel;
    
    // Engraved label look and feel for LFO title (owned)
    EngravedLabelLookAndFeel engravedLabelLookAndFeel;
    
    // Trigger button look and feel (owned)
    TriggerButtonLookAndFeel triggerButtonLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOModuleComponent)
};