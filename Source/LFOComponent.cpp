#include "LFOComponent.h"

//==============================================================================
// LFO Drawable Waveform Component
//==============================================================================



void LFOComponent::paint(juce::Graphics& g)
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

void LFOComponent::resized()
{
    waveformArea = getLocalBounds().reduced(12, 12);
}

void LFOComponent::mouseDown(const juce::MouseEvent& event)
{
    if (!waveformArea.contains(event.getPosition()))
        return;
        
    isDragging = true;
    dragStartIndex = getWaveformIndexFromX(event.x);
    updateWaveformPoint(event.x, event.y);
    repaint();
}

void LFOComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging)
        return;
        
    updateWaveformPoint(event.x, event.y);
    repaint();
}

void LFOComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
    dragStartIndex = -1;
    repaint();
}

void LFOComponent::updateWaveformPoint(int x, int y)
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

int LFOComponent::getWaveformIndexFromX(int x) const
{
    if (waveformArea.getWidth() <= 0)
        return 0;
        
    float normalizedX = (x - waveformArea.getX()) / float(waveformArea.getWidth());
    normalizedX = juce::jlimit(0.0f, 1.0f, normalizedX);
    return static_cast<int>(normalizedX * (waveformPoints.size() - 1));
}

float LFOComponent::getWaveformValueFromY(int y) const
{
    if (waveformArea.getHeight() <= 0)
        return 0.5f;
        
    float normalizedY = (y - waveformArea.getY()) / float(waveformArea.getHeight());
    normalizedY = juce::jlimit(0.0f, 1.0f, normalizedY);
    return 1.0f - normalizedY; // Invert Y axis (top = 1.0, bottom = 0.0)
}

juce::Point<int> LFOComponent::getPointFromWaveformData(int index) const
{
    if (index < 0 || index >= waveformPoints.size() || waveformArea.getWidth() <= 0)
        return juce::Point<int>(0, 0);
        
    float normalizedX = index / float(waveformPoints.size() - 1);
    float normalizedY = 1.0f - waveformPoints[index]; // Invert Y axis
    
    int x = waveformArea.getX() + static_cast<int>(normalizedX * waveformArea.getWidth());
    int y = waveformArea.getY() + static_cast<int>(normalizedY * waveformArea.getHeight());
    
    return juce::Point<int>(x, y);
}

void LFOComponent::setWaveformData(const std::vector<float>& data)
{
    if (data.size() == waveformPoints.size())
    {
        waveformPoints = data;
        repaint();
    }
}


//==============================================================================
// LFO Module Component (Container)
//==============================================================================



void LFOModuleComponent::paint(juce::Graphics& g)
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

void LFOModuleComponent::resized()
{
    auto bounds = getLocalBounds().reduced(12, 12);
    
    // Title at the top
    auto titleArea = bounds.removeFromTop(20);
    lfoTitleLabel.setBounds(titleArea);
    
    bounds.removeFromTop(5); // Small gap
    
    // Main waveform display area (takes most of the space)
    auto waveformArea = bounds.removeFromTop(120);
    lfoWaveform.setBounds(waveformArea);
    
    bounds.removeFromTop(8); // Gap
    
    // Preset buttons row
    auto presetButtonsArea = bounds.removeFromTop(25);
    int buttonWidth = presetButtonsArea.getWidth() / 4;
    
    lfoSineButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 0));
    lfoSawButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 0));
    lfoSquareButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 0));
    lfoTriangleButton.setBounds(presetButtonsArea.removeFromLeft(buttonWidth).reduced(2, 0));
    
    bounds.removeFromTop(8); // Gap
    
    // Rate knob and controls
    auto controlsArea = bounds.removeFromTop(80);
    
    // Rate knob on the left
    auto rateKnobArea = controlsArea.removeFromLeft(80);
    auto rateLabelArea = rateKnobArea.removeFromBottom(15);
    lfoRateLabel.setBounds(rateLabelArea);
    lfoRateKnob.setBounds(rateKnobArea);
    
    controlsArea.removeFromLeft(10); // Gap
    
    // Rate type buttons (Hz/BPM) in the middle
    auto rateTypeArea = controlsArea.removeFromLeft(80);
    auto hzButtonArea = rateTypeArea.removeFromTop(25);
    rateTypeArea.removeFromTop(5); // Small gap
    auto bpmButtonArea = rateTypeArea.removeFromTop(25);
    
    lfoHzButton.setBounds(hzButtonArea);
    lfoBpmButton.setBounds(bpmButtonArea);
    
    controlsArea.removeFromLeft(10); // Gap
    
    // Trigger button on the right
    auto triggerArea = controlsArea.removeFromTop(30);
    lfoTriggerButton.setBounds(triggerArea);
}

void LFOModuleComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &lfoRateKnob)
    {
        // Placeholder - rate value changed
        float rate = static_cast<float>(lfoRateKnob.getValue());
        // TODO: Update LFO rate when implementation is added
    }
}

void LFOModuleComponent::buttonClicked(juce::Button* button)
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
}