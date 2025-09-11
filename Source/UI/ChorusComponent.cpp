#include "ChorusComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

ChorusComponent::ChorusComponent()
{
    // Initialize chorus controls
    
    // Rate knob
    chorusRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusRateKnob.setRange(0.1, 10.0, 0.1);
    chorusRateKnob.setValue(2.0);
    addAndMakeVisible(chorusRateKnob);
    
    chorusRateLabel.setText("RATE", juce::dontSendNotification);
    chorusRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusRateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusRateLabel);
    
    // Delay 1 knob
    chorusDelay1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDelay1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDelay1Knob.setRange(1.0, 50.0, 1.0);
    chorusDelay1Knob.setValue(20.0);
    addAndMakeVisible(chorusDelay1Knob);
    
    chorusDelay1Label.setText("DELAY 1", juce::dontSendNotification);
    chorusDelay1Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDelay1Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusDelay1Label);
    
    // Delay 2 knob
    chorusDelay2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDelay2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDelay2Knob.setRange(1.0, 50.0, 1.0);
    chorusDelay2Knob.setValue(30.0);
    addAndMakeVisible(chorusDelay2Knob);
    
    chorusDelay2Label.setText("DELAY 2", juce::dontSendNotification);
    chorusDelay2Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDelay2Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusDelay2Label);
    
    // Depth knob
    chorusDepthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDepthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDepthKnob.setRange(0.0, 1.0, 0.01);
    chorusDepthKnob.setValue(0.5);
    addAndMakeVisible(chorusDepthKnob);
    
    chorusDepthLabel.setText("DEPTH", juce::dontSendNotification);
    chorusDepthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDepthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusDepthLabel);
    
    // Feed knob
    chorusFeedKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusFeedKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusFeedKnob.setRange(0.0, 0.9, 0.01);
    chorusFeedKnob.setValue(0.3);
    addAndMakeVisible(chorusFeedKnob);
    
    chorusFeedLabel.setText("FEED", juce::dontSendNotification);
    chorusFeedLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusFeedLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusFeedLabel);
    
    // LPF knob
    chorusLpfKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusLpfKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusLpfKnob.setRange(0.0, 100.0, 1.0);
    chorusLpfKnob.setValue(0.0);
    addAndMakeVisible(chorusLpfKnob);
    
    chorusLpfLabel.setText("LPF", juce::dontSendNotification);
    chorusLpfLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusLpfLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusLpfLabel);
    
    // Mix knob
    chorusMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusMixKnob.setRange(0.0, 1.0, 0.01);
    chorusMixKnob.setValue(0.5);
    addAndMakeVisible(chorusMixKnob);
    
    chorusMixLabel.setText("MIX", juce::dontSendNotification);
    chorusMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusMixLabel);
    
    // Power button
    chorusPowerButton.setButtonText("CHORUS ON");
    chorusPowerButton.setClickingTogglesState(true);
    chorusPowerButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(chorusPowerButton);
    
    // Add listeners for chorus controls
    chorusRateKnob.addListener(this);
    chorusDelay1Knob.addListener(this);
    chorusDelay2Knob.addListener(this);
    chorusDepthKnob.addListener(this);
    chorusFeedKnob.addListener(this);
    chorusLpfKnob.addListener(this);
    chorusMixKnob.addListener(this);
    chorusPowerButton.addListener(this);
}

void ChorusComponent::paint(juce::Graphics& g)
{
    // Component background is handled by parent
}

void ChorusComponent::resized()
{
    auto bounds = getLocalBounds();
    layoutControls(bounds);
}

void ChorusComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parentSynthesizer == nullptr) return;
    
    if (slider == &chorusRateKnob)
    {
        parentSynthesizer->audioProcessor.setChorusRate(static_cast<float>(chorusRateKnob.getValue()));
    }
    else if (slider == &chorusDelay1Knob)
    {
        parentSynthesizer->audioProcessor.setChorusDelay1(static_cast<float>(chorusDelay1Knob.getValue()));
    }
    else if (slider == &chorusDelay2Knob)
    {
        parentSynthesizer->audioProcessor.setChorusDelay2(static_cast<float>(chorusDelay2Knob.getValue()));
    }
    else if (slider == &chorusDepthKnob)
    {
        parentSynthesizer->audioProcessor.setChorusDepth(static_cast<float>(chorusDepthKnob.getValue()));
    }
    else if (slider == &chorusFeedKnob)
    {
        parentSynthesizer->audioProcessor.setChorusFeedback(static_cast<float>(chorusFeedKnob.getValue()));
    }
    else if (slider == &chorusLpfKnob)
    {
        parentSynthesizer->audioProcessor.setChorusLPF(static_cast<float>(chorusLpfKnob.getValue()));
    }
    else if (slider == &chorusMixKnob)
    {
        parentSynthesizer->audioProcessor.setChorusMix(static_cast<float>(chorusMixKnob.getValue()));
    }
}

void ChorusComponent::buttonClicked(juce::Button* button)
{
    if (parentSynthesizer == nullptr) return;
    
    if (button == &chorusPowerButton)
    {
        parentSynthesizer->audioProcessor.setChorusEnabled(chorusPowerButton.getToggleState());
    }
}

void ChorusComponent::syncWithDSPState()
{
    if (parentSynthesizer == nullptr) return;
    
    chorusRateKnob.setValue(parentSynthesizer->audioProcessor.getChorusRate(), juce::dontSendNotification);
    chorusDelay1Knob.setValue(parentSynthesizer->audioProcessor.getChorusDelay1(), juce::dontSendNotification);
    chorusDelay2Knob.setValue(parentSynthesizer->audioProcessor.getChorusDelay2(), juce::dontSendNotification);
    chorusDepthKnob.setValue(parentSynthesizer->audioProcessor.getChorusDepth(), juce::dontSendNotification);
    chorusFeedKnob.setValue(parentSynthesizer->audioProcessor.getChorusFeedback(), juce::dontSendNotification);
    chorusLpfKnob.setValue(parentSynthesizer->audioProcessor.getChorusLPF(), juce::dontSendNotification);
    chorusMixKnob.setValue(parentSynthesizer->audioProcessor.getChorusMix(), juce::dontSendNotification);
    chorusPowerButton.setToggleState(parentSynthesizer->audioProcessor.getChorusEnabled(), juce::dontSendNotification);
}

void ChorusComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF)
{
    chorusRateKnob.setLookAndFeel(digitalKnobLAF);
    chorusDelay1Knob.setLookAndFeel(digitalKnobLAF);
    chorusDelay2Knob.setLookAndFeel(digitalKnobLAF);
    chorusDepthKnob.setLookAndFeel(digitalKnobLAF);
    chorusFeedKnob.setLookAndFeel(digitalKnobLAF);
    chorusLpfKnob.setLookAndFeel(digitalKnobLAF);
    chorusMixKnob.setLookAndFeel(digitalKnobLAF);
    chorusPowerButton.setLookAndFeel(digitalButtonLAF);
    
    chorusRateLabel.setLookAndFeel(digitalKnobLAF);
    chorusDelay1Label.setLookAndFeel(digitalKnobLAF);
    chorusDelay2Label.setLookAndFeel(digitalKnobLAF);
    chorusDepthLabel.setLookAndFeel(digitalKnobLAF);
    chorusFeedLabel.setLookAndFeel(digitalKnobLAF);
    chorusLpfLabel.setLookAndFeel(digitalKnobLAF);
    chorusMixLabel.setLookAndFeel(digitalKnobLAF);
}

void ChorusComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Layout with even larger knobs and more symmetrical spacing to match FX unit style
    auto knobSize = 62; // Further increased from 55 for even more prominence
    auto labelHeight = 22; // Increased for better proportion with larger knobs
    auto knobSpacing = 25; // Increased spacing for better symmetry
    auto rowSpacing = 18; // Increased row spacing
    auto buttonWidth = 80; // Power button width
    auto buttonHeight = 32; // Taller power button to match larger knobs
    
    // Calculate positions for perfectly centered layout
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto startX = (bounds.getWidth() - totalKnobWidth) / 2;
    auto startY = 35; // Moved down a bit more for better spacing
    
    // Top row: Power button and Mix knob closer together
    auto topRowY = startY;
    
    // Calculate closer positioning for power button and mix knob (moved left 20 pixels)
    auto topRowWidth = buttonWidth + 25 + knobSize; // Button + smaller gap + knob
    auto topRowStartX = (bounds.getWidth() - topRowWidth) / 2 - 3; // Center the top row, moved left 3px (2 + 1)
    
    // Power button (moved 12 pixels to the right from new position)
    auto powerButtonX = topRowStartX + 12;
    chorusPowerButton.setBounds(powerButtonX, topRowY + 60, buttonWidth, buttonHeight);
    
    // Mix knob (positioned closer to power button)
    auto mixX = topRowStartX + buttonWidth + 25; // 25px gap instead of 40px
    auto mixKnobY = topRowY + (buttonHeight - knobSize) / 2; // Center with power button
    chorusMixKnob.setBounds(mixX, mixKnobY + 60, knobSize, knobSize);
    chorusMixLabel.setBounds(mixX, mixKnobY + 60 + knobSize + 3, knobSize, labelHeight);
    
    // First knob row: Rate, Delay 1, Delay 2 (moved right 7 pixels and down 60 pixels)
    auto row1Y = topRowY + std::max(buttonHeight, knobSize) + 25 + 60; // Space after top row + 60 pixels
    auto row2StartX = startX + 7; // Moved right 7 pixels
    
    // Rate knob and label
    chorusRateKnob.setBounds(row2StartX, row1Y, knobSize, knobSize);
    chorusRateLabel.setBounds(row2StartX, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Delay 1 knob and label
    auto delay1_x = row2StartX + knobSize + knobSpacing;
    chorusDelay1Knob.setBounds(delay1_x, row1Y, knobSize, knobSize);
    chorusDelay1Label.setBounds(delay1_x, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Delay 2 knob and label
    auto delay2_x = row2StartX + 2 * (knobSize + knobSpacing);
    chorusDelay2Knob.setBounds(delay2_x, row1Y, knobSize, knobSize);
    chorusDelay2Label.setBounds(delay2_x, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Second knob row: Depth, Feed, LPF (moved right 7 pixels and down 60 pixels)
    auto row2Y = row1Y + knobSize + labelHeight + rowSpacing;
    auto row3StartX = startX + 7; // Moved right 7 pixels
    
    // Depth knob and label
    chorusDepthKnob.setBounds(row3StartX, row2Y, knobSize, knobSize);
    chorusDepthLabel.setBounds(row3StartX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // Feed knob and label
    auto feedX = row3StartX + knobSize + knobSpacing;
    chorusFeedKnob.setBounds(feedX, row2Y, knobSize, knobSize);
    chorusFeedLabel.setBounds(feedX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // LPF knob and label
    auto lpfX = row3StartX + 2 * (knobSize + knobSpacing);
    chorusLpfKnob.setBounds(lpfX, row2Y, knobSize, knobSize);
    chorusLpfLabel.setBounds(lpfX, row2Y + knobSize + 3, knobSize, labelHeight);
}