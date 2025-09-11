#include "CompressorComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

CompressorComponent::CompressorComponent()
{
    // COMPRESSOR EFFECT CONTROLS
    // Threshold knob
    compressorThresholdKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorThresholdKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorThresholdKnob.setRange(-60.0, 0.0, 0.1);
    compressorThresholdKnob.setValue(-20.0);
    compressorThresholdKnob.addListener(this);
    addAndMakeVisible(compressorThresholdKnob);
    
    compressorThresholdLabel.setText("THRESH", juce::dontSendNotification);
    compressorThresholdLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorThresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorThresholdLabel);
    
    // Ratio knob
    compressorRatioKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorRatioKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorRatioKnob.setRange(1.0, 20.0, 0.1);
    compressorRatioKnob.setValue(4.0);
    compressorRatioKnob.addListener(this);
    addAndMakeVisible(compressorRatioKnob);
    
    compressorRatioLabel.setText("RATIO", juce::dontSendNotification);
    compressorRatioLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorRatioLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorRatioLabel);
    
    // Attack knob
    compressorAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorAttackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorAttackKnob.setRange(0.1, 100.0, 0.1);
    compressorAttackKnob.setValue(5.0);
    compressorAttackKnob.addListener(this);
    addAndMakeVisible(compressorAttackKnob);
    
    compressorAttackLabel.setText("ATTACK", juce::dontSendNotification);
    compressorAttackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorAttackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorAttackLabel);
    
    // Release knob
    compressorReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorReleaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorReleaseKnob.setRange(10.0, 1000.0, 1.0);
    compressorReleaseKnob.setValue(100.0);
    compressorReleaseKnob.addListener(this);
    addAndMakeVisible(compressorReleaseKnob);
    
    compressorReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    compressorReleaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorReleaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorReleaseLabel);
    
    // Gain knob
    compressorGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorGainKnob.setRange(0.0, 30.0, 0.1);
    compressorGainKnob.setValue(0.0);
    compressorGainKnob.addListener(this);
    addAndMakeVisible(compressorGainKnob);
    
    compressorGainLabel.setText("GAIN", juce::dontSendNotification);
    compressorGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorGainLabel);
    
    // Multiband button
    compressorMultibandButton.setButtonText("MULTIBAND");
    compressorMultibandButton.setClickingTogglesState(true);
    compressorMultibandButton.setToggleState(false, juce::dontSendNotification);
    compressorMultibandButton.addListener(this);
    addAndMakeVisible(compressorMultibandButton);
    
    // Mix knob
    compressorMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorMixKnob.setRange(0.0, 1.0, 0.01);
    compressorMixKnob.setValue(1.0);
    compressorMixKnob.addListener(this);
    addAndMakeVisible(compressorMixKnob);
    
    compressorMixLabel.setText("MIX", juce::dontSendNotification);
    compressorMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorMixLabel);
    
    // Power button
    compressorPowerButton.setButtonText("COMP ON");
    compressorPowerButton.setClickingTogglesState(true);
    compressorPowerButton.setToggleState(false, juce::dontSendNotification);
    compressorPowerButton.addListener(this);
    addAndMakeVisible(compressorPowerButton);
    
    // Compressor value displays
    compressorRatioValueLabel.setText("4.0:1", juce::dontSendNotification);
    compressorRatioValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorRatioValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorRatioValueLabel);
    
    compressorAttackValueLabel.setText("5.0ms", juce::dontSendNotification);
    compressorAttackValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorAttackValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorAttackValueLabel);
    
    compressorReleaseValueLabel.setText("100ms", juce::dontSendNotification);
    compressorReleaseValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorReleaseValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(compressorReleaseValueLabel);
}

void CompressorComponent::paint(juce::Graphics& g)
{
    // Component background handled by parent
}

void CompressorComponent::resized()
{
    layoutControls(getLocalBounds());
}

void CompressorComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Layout with same large knobs and symmetrical spacing as chorus
    auto knobSize = 62; // Same size as chorus for consistency
    auto labelHeight = 22; // Same as chorus
    auto knobSpacing = 25; // Same spacing as chorus
    auto rowSpacing = 18; // Same row spacing as chorus
    auto buttonWidth = 100; // Wider button for "MULTIBAND" text
    auto buttonHeight = 32; // Same height as chorus power button
    
    // Calculate positions for perfectly centered layout
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto startX = (bounds.getWidth() - totalKnobWidth) / 2;
    auto startY = 35; // Same positioning as chorus
    
    // Top row: Power button (left) and Mix knob (right)
    auto topRowY = startY;
    
    // Power button (moved 28 pixels to the right)
    auto powerButtonX = startX + 28;
    compressorPowerButton.setBounds(powerButtonX, topRowY + 60, buttonWidth, buttonHeight);
    
    // Mix knob (positioned on right side, moved left 37 pixels with row)
    auto mixX = startX + 2 * (knobSize + knobSpacing) - 37; // Right side position, moved left 37px
    auto mixKnobY = topRowY + 60 + (buttonHeight - knobSize) / 2; // Center with power button
    compressorMixKnob.setBounds(mixX, mixKnobY, knobSize, knobSize);
    compressorMixLabel.setBounds(mixX, mixKnobY + knobSize + 3, knobSize, labelHeight);
    
    // First knob row: Threshold, Ratio, Attack (moved right 7 pixels and down 60 pixels)
    auto row1Y = topRowY + std::max(buttonHeight, knobSize) + 25 + 60; // Space after top row + 60 pixels down
    auto row2StartX = startX + 7; // Moved right 7 pixels
    
    // Threshold knob and label
    compressorThresholdKnob.setBounds(row2StartX, row1Y, knobSize, knobSize);
    compressorThresholdLabel.setBounds(row2StartX, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Ratio knob and label
    auto ratio_x = row2StartX + knobSize + knobSpacing;
    compressorRatioKnob.setBounds(ratio_x, row1Y, knobSize, knobSize);
    compressorRatioLabel.setBounds(ratio_x, row1Y + knobSize + 3, knobSize, labelHeight);
    compressorRatioValueLabel.setBounds(ratio_x, row1Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Attack knob and label
    auto attack_x = row2StartX + 2 * (knobSize + knobSpacing);
    compressorAttackKnob.setBounds(attack_x, row1Y, knobSize, knobSize);
    compressorAttackLabel.setBounds(attack_x, row1Y + knobSize + 3, knobSize, labelHeight);
    compressorAttackValueLabel.setBounds(attack_x, row1Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Second knob row: Release, Gain, and Multiband button (moved right 7 pixels and down 60 pixels)
    auto row2Y = row1Y + knobSize + labelHeight + rowSpacing;
    auto row3StartX = startX + 7; // Moved right 7 pixels
    
    // Release knob and label
    compressorReleaseKnob.setBounds(row3StartX, row2Y, knobSize, knobSize);
    compressorReleaseLabel.setBounds(row3StartX, row2Y + knobSize + 3, knobSize, labelHeight);
    compressorReleaseValueLabel.setBounds(row3StartX, row2Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Gain knob and label (using updated ratio_x from row 2)
    auto gainX = row3StartX + knobSize + knobSpacing;
    compressorGainKnob.setBounds(gainX, row2Y, knobSize, knobSize);
    compressorGainLabel.setBounds(gainX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // Multiband button (moved 20 pixels to the left)
    auto multibandY = row2Y + (knobSize - buttonHeight) / 2; // Center with knobs
    compressorMultibandButton.setBounds(attack_x - 20, multibandY, buttonWidth, buttonHeight);
}

void CompressorComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    // Set look and feel for knobs
    compressorThresholdKnob.setLookAndFeel(digitalKnobLAF);
    compressorRatioKnob.setLookAndFeel(digitalKnobLAF);
    compressorAttackKnob.setLookAndFeel(digitalKnobLAF);
    compressorReleaseKnob.setLookAndFeel(digitalKnobLAF);
    compressorGainKnob.setLookAndFeel(digitalKnobLAF);
    compressorMixKnob.setLookAndFeel(digitalKnobLAF);
    
    // Set look and feel for labels
    compressorThresholdLabel.setLookAndFeel(digitalKnobLAF);
    compressorRatioLabel.setLookAndFeel(digitalKnobLAF);
    compressorAttackLabel.setLookAndFeel(digitalKnobLAF);
    compressorReleaseLabel.setLookAndFeel(digitalKnobLAF);
    compressorGainLabel.setLookAndFeel(digitalKnobLAF);
    compressorMixLabel.setLookAndFeel(digitalKnobLAF);
    
    // Set look and feel for value display labels
    compressorRatioValueLabel.setLookAndFeel(digitalKnobLAF);
    compressorAttackValueLabel.setLookAndFeel(digitalKnobLAF);
    compressorReleaseValueLabel.setLookAndFeel(digitalKnobLAF);
    
    // Set look and feel for buttons
    compressorMultibandButton.setLookAndFeel(digitalButtonLAF);
    compressorPowerButton.setLookAndFeel(digitalButtonLAF);
}

void CompressorComponent::sliderValueChanged(juce::Slider* slider)
{
    if (!parentSynthesizer) return;
    
    auto newValue = slider->getValue();
    
    if (slider == &compressorThresholdKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorThreshold(static_cast<float>(newValue));
    }
    else if (slider == &compressorRatioKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorRatio(static_cast<float>(newValue));
        // Update value display
        compressorRatioValueLabel.setText(juce::String(newValue, 1) + ":1", juce::dontSendNotification);
    }
    else if (slider == &compressorAttackKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorAttack(static_cast<float>(newValue));
        // Update value display
        compressorAttackValueLabel.setText(juce::String(newValue, 1) + "ms", juce::dontSendNotification);
    }
    else if (slider == &compressorReleaseKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorRelease(static_cast<float>(newValue));
        // Update value display
        compressorReleaseValueLabel.setText(juce::String(static_cast<int>(newValue)) + "ms", juce::dontSendNotification);
    }
    else if (slider == &compressorGainKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorGain(static_cast<float>(newValue));
    }
    else if (slider == &compressorMixKnob)
    {
        parentSynthesizer->audioProcessor.setCompressorMix(static_cast<float>(newValue));
    }
}

void CompressorComponent::buttonClicked(juce::Button* button)
{
    if (!parentSynthesizer) return;
    
    if (button == &compressorPowerButton)
    {
        parentSynthesizer->audioProcessor.setCompressorEnabled(compressorPowerButton.getToggleState());
    }
    else if (button == &compressorMultibandButton)
    {
        parentSynthesizer->audioProcessor.setCompressorMultiband(compressorMultibandButton.getToggleState());
    }
}

void CompressorComponent::syncWithDSPState()
{
    if (!parentSynthesizer) return;
    
    // Sync all compressor parameters with DSP state
    compressorThresholdKnob.setValue(parentSynthesizer->audioProcessor.getCompressorThreshold(), juce::dontSendNotification);
    compressorRatioKnob.setValue(parentSynthesizer->audioProcessor.getCompressorRatio(), juce::dontSendNotification);
    compressorAttackKnob.setValue(parentSynthesizer->audioProcessor.getCompressorAttack(), juce::dontSendNotification);
    compressorReleaseKnob.setValue(parentSynthesizer->audioProcessor.getCompressorRelease(), juce::dontSendNotification);
    compressorGainKnob.setValue(parentSynthesizer->audioProcessor.getCompressorGain(), juce::dontSendNotification);
    compressorMixKnob.setValue(parentSynthesizer->audioProcessor.getCompressorMix(), juce::dontSendNotification);
    compressorPowerButton.setToggleState(parentSynthesizer->audioProcessor.getCompressorEnabled(), juce::dontSendNotification);
    compressorMultibandButton.setToggleState(parentSynthesizer->audioProcessor.getCompressorMultiband(), juce::dontSendNotification);
    
    // Update value displays with current values
    auto ratioValue = parentSynthesizer->audioProcessor.getCompressorRatio();
    compressorRatioValueLabel.setText(juce::String(ratioValue, 1) + ":1", juce::dontSendNotification);
    
    auto attackValue = parentSynthesizer->audioProcessor.getCompressorAttack();
    compressorAttackValueLabel.setText(juce::String(attackValue, 1) + "ms", juce::dontSendNotification);
    
    auto releaseValue = parentSynthesizer->audioProcessor.getCompressorRelease();
    compressorReleaseValueLabel.setText(juce::String(static_cast<int>(releaseValue)) + "ms", juce::dontSendNotification);
}