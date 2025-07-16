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
    LFOModuleComponent() : knobLookAndFeel(nullptr), buttonLookAndFeel(nullptr), labelLookAndFeel(nullptr) 
    {
        // Set up title label
        lfoTitleLabel.setText("LFO", juce::dontSendNotification);
        lfoTitleLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
        lfoTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        lfoTitleLabel.setJustificationType(juce::Justification::centred);
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
        lfoRateLabel.setFont(juce::Font("Press Start 2P", 8.0f, juce::Font::plain));
        lfoRateLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        lfoRateLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(lfoRateLabel);
        
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
        lfoTriggerButton.setButtonText("TRIGGER");
        lfoTriggerButton.setClickingTogglesState(true);
        lfoTriggerButton.setToggleState(false, juce::dontSendNotification);
        lfoTriggerButton.addListener(this);
        addAndMakeVisible(lfoTriggerButton);
        
        // Set up draw button
        lfoDrawButton.setButtonText("DRAW");
        lfoDrawButton.setClickingTogglesState(true);
        lfoDrawButton.setToggleState(false, juce::dontSendNotification);
        lfoDrawButton.addListener(this);
        addAndMakeVisible(lfoDrawButton);
        
        // Set up preset waveform buttons
        lfoSineButton.setButtonText("SIN");
        lfoSineButton.addListener(this);
        addAndMakeVisible(lfoSineButton);
        
        lfoSawButton.setButtonText("SAW");
        lfoSawButton.addListener(this);
        addAndMakeVisible(lfoSawButton);
        
        lfoSquareButton.setButtonText("SQR");
        lfoSquareButton.addListener(this);
        addAndMakeVisible(lfoSquareButton);
        
        lfoTriangleButton.setButtonText("TRI");
        lfoTriangleButton.addListener(this);
        addAndMakeVisible(lfoTriangleButton);
        
        lfoChaosButton.setButtonText("CHAOS");
        lfoChaosButton.addListener(this);
        addAndMakeVisible(lfoChaosButton);
    }
    ~LFOModuleComponent() override {}
    
    void setLookAndFeel(CustomKnobLookAndFeel* knobLookAndFeel, WaveButtonLookAndFeel* buttonLookAndFeel, LEDLabelLookAndFeel* labelLookAndFeel)
    {
        this->knobLookAndFeel = knobLookAndFeel;
        this->buttonLookAndFeel = buttonLookAndFeel;
        this->labelLookAndFeel = labelLookAndFeel;
        
        // Apply look and feel to existing components
        if (this->knobLookAndFeel) lfoRateKnob.setLookAndFeel(this->knobLookAndFeel);
        if (this->labelLookAndFeel) 
        {
            lfoTitleLabel.setLookAndFeel(this->labelLookAndFeel);
            lfoRateLabel.setLookAndFeel(this->labelLookAndFeel);
        }
        if (this->buttonLookAndFeel)
        {
            lfoHzButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoBpmButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoTriggerButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoDrawButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSineButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSawButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoSquareButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoTriangleButton.setLookAndFeel(this->buttonLookAndFeel);
            lfoChaosButton.setLookAndFeel(this->buttonLookAndFeel);
        }
    }
    
    void paint(juce::Graphics& g) override 
    {
        auto bounds = getLocalBounds();
        
        // Draw main background with metallic outline (matching other components)
        g.setColour(juce::Colour(0xff0f0f0f));
        g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
        
        // Inner metallic frame
        auto metalBounds = bounds.toFloat().reduced(3.0f);
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillRoundedRectangle(metalBounds, 6.0f);
        
        // Metallic highlight
        g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
        g.drawRoundedRectangle(metalBounds.reduced(1.0f), 5.0f, 1.0f);
        
        // Draw background inside the frames
        auto innerBounds = bounds.toFloat().reduced(8.0f);
        g.setColour(juce::Colour(0xff0f0f1f).withAlpha(0.7f));
        g.fillRoundedRectangle(innerBounds, 4.0f);
    }
    
    void resized() override 
    {
        auto bounds = getLocalBounds().reduced(20, 20);
        
        // Title at the top - centered
        auto titleArea = bounds.removeFromTop(25);
        lfoTitleLabel.setBounds(titleArea);
        
        bounds.removeFromTop(10); // Gap
        
        // Main waveform display area - centered and larger
        auto waveformArea = bounds.removeFromTop(100);
        waveformArea = waveformArea.reduced(20, 0); // Add some side padding
        lfoWaveform.setBounds(waveformArea);
        
        bounds.removeFromTop(15); // Gap
        
        // Preset buttons row - centered
        auto presetButtonsArea = bounds.removeFromTop(30);
        presetButtonsArea = presetButtonsArea.reduced(20, 0); // Center the buttons
        int buttonWidth = presetButtonsArea.getWidth() / 5;
        
        lfoSineButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 2));
        lfoSawButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 2));
        lfoSquareButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 2));
        lfoTriangleButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 2));
        lfoChaosButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 2));
        
        bounds.removeFromTop(15); // Gap
        
        // Rate knob and controls - centered horizontally
        auto controlsArea = bounds.removeFromTop(70);
        int totalControlsWidth = 60 + 10 + 60 + 10 + 80 + 10 + 60; // knob + gap + hz/bpm + gap + trigger + gap + draw
        int startX = (controlsArea.getWidth() - totalControlsWidth) / 2;
        
        // Rate knob
        auto rateKnobArea = juce::Rectangle<int>(startX, 0, 60, 70);
        auto rateLabelArea = rateKnobArea.removeFromBottom(18);
        lfoRateLabel.setBounds(rateLabelArea);
        lfoRateKnob.setBounds(rateKnobArea);
        
        // Rate type buttons (Hz/BPM)
        auto rateTypeArea = juce::Rectangle<int>(startX + 70, 10, 60, 60);
        auto hzButtonArea = rateTypeArea.removeFromTop(25);
        rateTypeArea.removeFromTop(5); // Small gap
        auto bpmButtonArea = rateTypeArea.removeFromTop(25);
        
        lfoHzButton.setBounds(hzButtonArea);
        lfoBpmButton.setBounds(bpmButtonArea);
        
        // Trigger button
        auto triggerArea = juce::Rectangle<int>(startX + 140, 15, 80, 30);
        lfoTriggerButton.setBounds(triggerArea);
        
        // Draw button
        auto drawArea = juce::Rectangle<int>(startX + 230, 15, 60, 30);
        lfoDrawButton.setBounds(drawArea);
    }
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override {}
    
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
            // Toggle draw mode
            bool isDrawMode = lfoDrawButton.getToggleState();
            // TODO: Enable/disable draw mode when implementation is added
            // This could switch between different drawing behaviors
        }
        else if (button == &lfoSineButton)
        {
            lfoWaveform.resetToSineWave();
        }
        else if (button == &lfoSawButton)
        {
            lfoWaveform.resetToSawWave();
        }
        else if (button == &lfoSquareButton)
        {
            lfoWaveform.resetToSquareWave();
        }
        else if (button == &lfoTriangleButton)
        {
            lfoWaveform.resetToTriangleWave();
        }
        else if (button == &lfoChaosButton)
        {
            // TODO: Implement chaos LFO mode - for now just placeholder
            // This could generate a random/chaotic waveform or enable the current smooth drawing behavior
        }
    }
    
private:
    // LFO waveform drawing component
    LFOComponent lfoWaveform;
    
    // Rate control
    juce::Slider lfoRateKnob;
    juce::Label lfoRateLabel;
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOModuleComponent)
};