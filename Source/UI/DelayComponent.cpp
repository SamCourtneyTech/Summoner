#include "DelayComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

DelayComponent::DelayComponent()
{
    // DELAY EFFECT CONTROLS
    // Feedback knob
    delayFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFeedbackKnob.setRange(0.0, 0.95, 0.01);
    delayFeedbackKnob.setValue(0.3);
    delayFeedbackKnob.addListener(this);
    addAndMakeVisible(delayFeedbackKnob);
    
    delayFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    delayFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFeedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayFeedbackLabel);
    
    // BPM button
    delayBpmButton.setButtonText("BPM");
    delayBpmButton.setClickingTogglesState(true);
    delayBpmButton.setToggleState(true, juce::dontSendNotification);
    delayBpmButton.addListener(this);
    addAndMakeVisible(delayBpmButton);
    
    // Link button
    delayLinkButton.setButtonText("LINK");
    delayLinkButton.setClickingTogglesState(true);
    delayLinkButton.setToggleState(true, juce::dontSendNotification);
    delayLinkButton.addListener(this);
    addAndMakeVisible(delayLinkButton);
    
    // Left delay time slider
    delayLeftTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayLeftTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayLeftTimeSlider.setRange(1.0, 2000.0, 1.0);
    delayLeftTimeSlider.setValue(250.0);
    delayLeftTimeSlider.addListener(this);
    addAndMakeVisible(delayLeftTimeSlider);
    
    delayLeftTimeLabel.setText("L TIME", juce::dontSendNotification);
    delayLeftTimeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayLeftTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayLeftTimeLabel);
    
    // Right delay time slider
    delayRightTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayRightTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayRightTimeSlider.setRange(1.0, 2000.0, 1.0);
    delayRightTimeSlider.setValue(375.0);
    delayRightTimeSlider.addListener(this);
    addAndMakeVisible(delayRightTimeSlider);
    
    delayRightTimeLabel.setText("R TIME", juce::dontSendNotification);
    delayRightTimeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayRightTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayRightTimeLabel);
    
    // Left triplet button
    delayLeftTripletButton.setButtonText("L 3");
    delayLeftTripletButton.setClickingTogglesState(true);
    delayLeftTripletButton.setToggleState(false, juce::dontSendNotification);
    delayLeftTripletButton.addListener(this);
    addAndMakeVisible(delayLeftTripletButton);
    
    // Left dot timing button
    delayLeftDotButton.setButtonText("L .");
    delayLeftDotButton.setClickingTogglesState(true);
    delayLeftDotButton.setToggleState(false, juce::dontSendNotification);
    delayLeftDotButton.addListener(this);
    addAndMakeVisible(delayLeftDotButton);
    
    // Right triplet button
    delayRightTripletButton.setButtonText("R 3");
    delayRightTripletButton.setClickingTogglesState(true);
    delayRightTripletButton.setToggleState(false, juce::dontSendNotification);
    delayRightTripletButton.addListener(this);
    addAndMakeVisible(delayRightTripletButton);
    
    // Right dot timing button
    delayRightDotButton.setButtonText("R .");
    delayRightDotButton.setClickingTogglesState(true);
    delayRightDotButton.setToggleState(false, juce::dontSendNotification);
    delayRightDotButton.addListener(this);
    addAndMakeVisible(delayRightDotButton);
    
    // Filter frequency knob
    delayFilterFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFilterFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFilterFreqKnob.setRange(20.0, 20000.0, 1.0);
    delayFilterFreqKnob.setValue(8000.0);
    delayFilterFreqKnob.addListener(this);
    addAndMakeVisible(delayFilterFreqKnob);
    
    delayFilterFreqLabel.setText("FREQ", juce::dontSendNotification);
    delayFilterFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFilterFreqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayFilterFreqLabel);
    
    // Filter Q knob
    delayFilterQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFilterQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFilterQKnob.setRange(0.1, 10.0, 0.1);
    delayFilterQKnob.setValue(1.0);
    delayFilterQKnob.addListener(this);
    addAndMakeVisible(delayFilterQKnob);
    
    delayFilterQLabel.setText("Q", juce::dontSendNotification);
    delayFilterQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFilterQLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayFilterQLabel);
    
    // Normal button
    delayNormalButton.setButtonText("NORMAL");
    delayNormalButton.setClickingTogglesState(true);
    delayNormalButton.setToggleState(true, juce::dontSendNotification);
    delayNormalButton.addListener(this);
    addAndMakeVisible(delayNormalButton);
    
    // Ping-pong button
    delayPingPongButton.setButtonText("PING-PONG");
    delayPingPongButton.setClickingTogglesState(true);
    delayPingPongButton.setToggleState(false, juce::dontSendNotification);
    delayPingPongButton.addListener(this);
    addAndMakeVisible(delayPingPongButton);
    
    // Mix knob
    delayMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayMixKnob.setRange(0.0, 1.0, 0.01);
    delayMixKnob.setValue(0.25);
    delayMixKnob.addListener(this);
    addAndMakeVisible(delayMixKnob);
    
    delayMixLabel.setText("MIX", juce::dontSendNotification);
    delayMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayMixLabel);
    
    // Power button
    delayPowerButton.setButtonText("DELAY ON");
    delayPowerButton.setClickingTogglesState(true);
    delayPowerButton.setToggleState(false, juce::dontSendNotification);
    delayPowerButton.addListener(this);
    addAndMakeVisible(delayPowerButton);
}

void DelayComponent::paint(juce::Graphics& g)
{
    // Component background handled by parent
}

void DelayComponent::resized()
{
    layoutControls(getLocalBounds());
}

void DelayComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Layout with same large knobs and green styling as other effects
    auto knobSize = 62; // Same size as other effects for consistency
    auto labelHeight = 22; // Same as other effects
    auto knobSpacing = 20; // Slightly tighter spacing due to more controls
    auto rowSpacing = 15; // Tighter row spacing
    auto buttonWidth = 70; // Standard button width
    auto buttonHeight = 28; // Standard button height
    auto smallButtonWidth = 35; // For triplet/dot buttons
    
    // Calculate positions for layout
    auto startY = 25; // Same as other effects but slightly higher due to more rows
    
    // Row 1: Delay ON button and Mix knob (top row, moved right 3 pixels)
    auto row1Y = startY;
    auto powerButtonX = (bounds.getWidth() - buttonWidth - knobSpacing - knobSize) / 2 + 3;
    delayPowerButton.setBounds(powerButtonX, row1Y, buttonWidth, buttonHeight);
    
    // Mix knob in same row as power button
    auto mixRow1X = powerButtonX + buttonWidth + knobSpacing;
    delayMixKnob.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2, knobSize, knobSize);
    delayMixLabel.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2 + knobSize + 3, knobSize, labelHeight);
    
    // Row 2: BPM and LINK buttons (moved right 7 pixels)
    auto row2Y = row1Y + std::max(buttonHeight, knobSize) + 20;
    auto bpmX = bounds.getWidth() / 2 - buttonWidth - 10 + 7;
    delayBpmButton.setBounds(bpmX, row2Y, buttonWidth, buttonHeight);
    
    auto linkX = bounds.getWidth() / 2 + 10 + 7;
    delayLinkButton.setBounds(linkX, row2Y, buttonWidth, buttonHeight);
    
    // Row 3: Left Time, Feedback, Right Time knobs (moved right 6 pixels)
    auto row3Y = row2Y + buttonHeight + 20;
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto knobStartX = (bounds.getWidth() - totalKnobWidth) / 2 + 6;
    
    // Left time
    delayLeftTimeSlider.setBounds(knobStartX, row3Y, knobSize, knobSize);
    delayLeftTimeLabel.setBounds(knobStartX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Feedback (center)
    auto feedbackX = knobStartX + knobSize + knobSpacing;
    delayFeedbackKnob.setBounds(feedbackX, row3Y, knobSize, knobSize);
    delayFeedbackLabel.setBounds(feedbackX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Right time
    auto rightTimeX = knobStartX + 2 * (knobSize + knobSpacing);
    delayRightTimeSlider.setBounds(rightTimeX, row3Y, knobSize, knobSize);
    delayRightTimeLabel.setBounds(rightTimeX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Row 4: Triplet and dot buttons under time knobs (moved right 3 pixels with row 3)
    auto row4Y = row3Y + knobSize + labelHeight + 10;
    
    // Left triplet and dot buttons (under left time) - positioned relative to knobStartX which already moved 3px
    auto leftTripletX = knobStartX + (knobSize - 2 * smallButtonWidth - 5) / 2;
    delayLeftTripletButton.setBounds(leftTripletX, row4Y, smallButtonWidth, buttonHeight);
    delayLeftDotButton.setBounds(leftTripletX + smallButtonWidth + 5, row4Y, smallButtonWidth, buttonHeight);
    
    // Right triplet and dot buttons (under right time) - positioned relative to rightTimeX which moved with knobStartX
    auto rightTripletX = rightTimeX + (knobSize - 2 * smallButtonWidth - 5) / 2;
    delayRightTripletButton.setBounds(rightTripletX, row4Y, smallButtonWidth, buttonHeight);
    delayRightDotButton.setBounds(rightTripletX + smallButtonWidth + 5, row4Y, smallButtonWidth, buttonHeight);
    
    // Row 5: Filter Freq and Filter Q knobs (separate row, moved right 7 pixels)
    auto row5Y = row4Y + buttonHeight + 15;
    auto filterKnobWidth = (2 * knobSize) + knobSpacing;
    auto filterKnobStartX = (bounds.getWidth() - filterKnobWidth) / 2 + 7;
    
    delayFilterFreqKnob.setBounds(filterKnobStartX, row5Y, knobSize, knobSize);
    delayFilterFreqLabel.setBounds(filterKnobStartX, row5Y + knobSize + 3, knobSize, labelHeight);
    
    auto filterQX = filterKnobStartX + knobSize + knobSpacing;
    delayFilterQKnob.setBounds(filterQX, row5Y, knobSize, knobSize);
    delayFilterQLabel.setBounds(filterQX, row5Y + knobSize + 3, knobSize, labelHeight);
    
    // Row 6: Normal and Ping-Pong buttons (separate row, moved right 6 pixels)
    auto row6Y = row5Y + knobSize + labelHeight + 15;
    auto buttonStartX = (bounds.getWidth() - buttonWidth) / 2 + 6;
    delayNormalButton.setBounds(buttonStartX, row6Y, buttonWidth, buttonHeight);
    delayPingPongButton.setBounds(buttonStartX, row6Y + buttonHeight + 5, buttonWidth, buttonHeight);
}

void DelayComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    // Set look and feel for knobs
    delayFeedbackKnob.setLookAndFeel(digitalKnobLAF);
    delayLeftTimeSlider.setLookAndFeel(digitalKnobLAF);
    delayRightTimeSlider.setLookAndFeel(digitalKnobLAF);
    delayFilterFreqKnob.setLookAndFeel(digitalKnobLAF);
    delayFilterQKnob.setLookAndFeel(digitalKnobLAF);
    delayMixKnob.setLookAndFeel(digitalKnobLAF);
    
    // Set look and feel for labels
    delayFeedbackLabel.setLookAndFeel(digitalKnobLAF);
    delayLeftTimeLabel.setLookAndFeel(digitalKnobLAF);
    delayRightTimeLabel.setLookAndFeel(digitalKnobLAF);
    delayFilterFreqLabel.setLookAndFeel(digitalKnobLAF);
    delayFilterQLabel.setLookAndFeel(digitalKnobLAF);
    delayMixLabel.setLookAndFeel(digitalKnobLAF);
    
    // Set look and feel for buttons
    delayBpmButton.setLookAndFeel(digitalButtonLAF);
    delayLinkButton.setLookAndFeel(digitalButtonLAF);
    delayLeftTripletButton.setLookAndFeel(digitalButtonLAF);
    delayLeftDotButton.setLookAndFeel(digitalButtonLAF);
    delayRightTripletButton.setLookAndFeel(digitalButtonLAF);
    delayRightDotButton.setLookAndFeel(digitalButtonLAF);
    delayNormalButton.setLookAndFeel(digitalButtonLAF);
    delayPingPongButton.setLookAndFeel(digitalButtonLAF);
    delayPowerButton.setLookAndFeel(digitalButtonLAF);
}

void DelayComponent::sliderValueChanged(juce::Slider* slider)
{
    if (!parentSynthesizer) return;
    
    auto newValue = slider->getValue();
    
    if (slider == &delayFeedbackKnob)
    {
        parentSynthesizer->audioProcessor.setDelayFeedback(static_cast<float>(newValue));
    }
    else if (slider == &delayLeftTimeSlider)
    {
        float leftTime = static_cast<float>(newValue);
        parentSynthesizer->audioProcessor.setDelayLeftTime(leftTime);
        
        // If link is enabled, also update right time
        if (delayLinkButton.getToggleState())
        {
            delayRightTimeSlider.setValue(leftTime, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDelayRightTime(leftTime);
        }
    }
    else if (slider == &delayRightTimeSlider)
    {
        float rightTime = static_cast<float>(newValue);
        parentSynthesizer->audioProcessor.setDelayRightTime(rightTime);
        
        // If link is enabled, also update left time
        if (delayLinkButton.getToggleState())
        {
            delayLeftTimeSlider.setValue(rightTime, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDelayLeftTime(rightTime);
        }
    }
    else if (slider == &delayFilterFreqKnob)
    {
        parentSynthesizer->audioProcessor.setDelayFilterFreq(static_cast<float>(newValue));
    }
    else if (slider == &delayFilterQKnob)
    {
        parentSynthesizer->audioProcessor.setDelayFilterQ(static_cast<float>(newValue));
    }
    else if (slider == &delayMixKnob)
    {
        parentSynthesizer->audioProcessor.setDelayMix(static_cast<float>(newValue));
    }
}

void DelayComponent::buttonClicked(juce::Button* button)
{
    if (!parentSynthesizer) return;
    
    if (button == &delayPowerButton)
    {
        parentSynthesizer->audioProcessor.setDelayEnabled(delayPowerButton.getToggleState());
    }
    else if (button == &delayBpmButton)
    {
        parentSynthesizer->audioProcessor.setDelaySync(delayBpmButton.getToggleState());
    }
    else if (button == &delayLinkButton)
    {
        // Link functionality is handled in the UI logic, not sent to processor
        // When linking, sync right time to left time
        if (delayLinkButton.getToggleState())
        {
            float leftTime = delayLeftTimeSlider.getValue();
            delayRightTimeSlider.setValue(leftTime, juce::sendNotification);
        }
    }
    else if (button == &delayLeftTripletButton)
    {
        parentSynthesizer->audioProcessor.setDelayTriplet(delayLeftTripletButton.getToggleState());
    }
    else if (button == &delayLeftDotButton)
    {
        parentSynthesizer->audioProcessor.setDelayDotted(delayLeftDotButton.getToggleState());
    }
    else if (button == &delayRightTripletButton)
    {
        parentSynthesizer->audioProcessor.setDelayRTriplet(delayRightTripletButton.getToggleState());
    }
    else if (button == &delayRightDotButton)
    {
        parentSynthesizer->audioProcessor.setDelayRDotted(delayRightDotButton.getToggleState());
    }
    else if (button == &delayNormalButton)
    {
        if (delayNormalButton.getToggleState())
        {
            delayPingPongButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDelayPingPong(false); // Normal mode
        }
        else
        {
            delayNormalButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    else if (button == &delayPingPongButton)
    {
        if (delayPingPongButton.getToggleState())
        {
            delayNormalButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDelayPingPong(true); // Ping-pong mode
        }
        else
        {
            delayPingPongButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
}

void DelayComponent::syncWithDSPState()
{
    if (!parentSynthesizer) return;
    
    // Sync all delay parameters with DSP state
    delayFeedbackKnob.setValue(parentSynthesizer->audioProcessor.getDelayFeedback(), juce::dontSendNotification);
    delayLeftTimeSlider.setValue(parentSynthesizer->audioProcessor.getDelayLeftTime(), juce::dontSendNotification);
    delayRightTimeSlider.setValue(parentSynthesizer->audioProcessor.getDelayRightTime(), juce::dontSendNotification);
    delayFilterFreqKnob.setValue(parentSynthesizer->audioProcessor.getDelayFilterFreq(), juce::dontSendNotification);
    delayFilterQKnob.setValue(parentSynthesizer->audioProcessor.getDelayFilterQ(), juce::dontSendNotification);
    delayMixKnob.setValue(parentSynthesizer->audioProcessor.getDelayMix(), juce::dontSendNotification);
    delayPowerButton.setToggleState(parentSynthesizer->audioProcessor.getDelayEnabled(), juce::dontSendNotification);
    delayBpmButton.setToggleState(parentSynthesizer->audioProcessor.getDelaySync(), juce::dontSendNotification);
    // Link button state is managed locally in the UI, not synced with DSP
    delayLeftTripletButton.setToggleState(parentSynthesizer->audioProcessor.getDelayTriplet(), juce::dontSendNotification);
    delayLeftDotButton.setToggleState(parentSynthesizer->audioProcessor.getDelayDotted(), juce::dontSendNotification);
    delayRightTripletButton.setToggleState(parentSynthesizer->audioProcessor.getDelayRTriplet(), juce::dontSendNotification);
    delayRightDotButton.setToggleState(parentSynthesizer->audioProcessor.getDelayRDotted(), juce::dontSendNotification);
    
    // Sync delay mode buttons
    bool isPingPong = parentSynthesizer->audioProcessor.getDelayPingPong();
    delayNormalButton.setToggleState(!isPingPong, juce::dontSendNotification);
    delayPingPongButton.setToggleState(isPingPong, juce::dontSendNotification);
}