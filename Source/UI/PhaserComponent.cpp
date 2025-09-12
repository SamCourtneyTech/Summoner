#include "PhaserComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

PhaserComponent::PhaserComponent()
{
    // Initialize phaser controls
    
    // Power button
    phaserPowerButton.setButtonText("PHASER ON");
    phaserPowerButton.setClickingTogglesState(true);
    phaserPowerButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(phaserPowerButton);
    
    // Mix knob
    phaserMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserMixKnob.setRange(0.0, 100.0, 1.0);
    phaserMixKnob.setValue(50.0);
    addAndMakeVisible(phaserMixKnob);
    
    phaserMixLabel.setText("MIX", juce::dontSendNotification);
    phaserMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserMixLabel);
    
    // Rate knob
    phaserRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserRateKnob.setRange(0.1, 10.0, 0.1);
    phaserRateKnob.setValue(1.0);
    addAndMakeVisible(phaserRateKnob);
    
    phaserRateLabel.setText("RATE", juce::dontSendNotification);
    phaserRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserRateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserRateLabel);
    
    // BPM Sync button
    phaserBpmButton.setButtonText("BPM SYNC");
    phaserBpmButton.setClickingTogglesState(true);
    phaserBpmButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(phaserBpmButton);
    
    // Poles value label (interactive)
    phaserPolesValueLabel.setText("4 POLES", juce::dontSendNotification);
    phaserPolesValueLabel.setInterceptsMouseClicks(true, true);
    phaserPolesValueLabel.addMouseListener(this, false);
    addAndMakeVisible(phaserPolesValueLabel);
    
    phaserPolesLabel.setText("POLES", juce::dontSendNotification);
    phaserPolesLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserPolesLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserPolesLabel);
    
    // Depth 1 knob
    phaserDepth1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserDepth1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserDepth1Knob.setRange(0.0, 100.0, 1.0);
    phaserDepth1Knob.setValue(50.0);
    addAndMakeVisible(phaserDepth1Knob);
    
    phaserDepth1Label.setText("DEPTH 1", juce::dontSendNotification);
    phaserDepth1Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserDepth1Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserDepth1Label);
    
    // Depth 2 knob
    phaserDepth2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserDepth2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserDepth2Knob.setRange(0.0, 100.0, 1.0);
    phaserDepth2Knob.setValue(30.0);
    addAndMakeVisible(phaserDepth2Knob);
    
    phaserDepth2Label.setText("DEPTH 2", juce::dontSendNotification);
    phaserDepth2Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserDepth2Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserDepth2Label);
    
    // Frequency knob
    phaserFrequencyKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserFrequencyKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserFrequencyKnob.setRange(20.0, 2000.0, 1.0);
    phaserFrequencyKnob.setValue(500.0);
    addAndMakeVisible(phaserFrequencyKnob);
    
    phaserFrequencyLabel.setText("FREQ", juce::dontSendNotification);
    phaserFrequencyLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserFrequencyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserFrequencyLabel);
    
    // Feedback knob
    phaserFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserFeedbackKnob.setRange(0.0, 100.0, 1.0);
    phaserFeedbackKnob.setValue(25.0);
    addAndMakeVisible(phaserFeedbackKnob);
    
    phaserFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    phaserFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserFeedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserFeedbackLabel);
    
    // Phase knob
    phaserPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserPhaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserPhaseKnob.setRange(0.0, 360.0, 1.0);
    phaserPhaseKnob.setValue(0.0);
    addAndMakeVisible(phaserPhaseKnob);
    
    phaserPhaseLabel.setText("PHASE", juce::dontSendNotification);
    phaserPhaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserPhaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(phaserPhaseLabel);
    
    // Add listeners for phaser controls
    phaserPowerButton.addListener(this);
    phaserMixKnob.addListener(this);
    phaserRateKnob.addListener(this);
    phaserBpmButton.addListener(this);
    phaserDepth1Knob.addListener(this);
    phaserDepth2Knob.addListener(this);
    phaserFrequencyKnob.addListener(this);
    phaserFeedbackKnob.addListener(this);
    phaserPhaseKnob.addListener(this);
}

void PhaserComponent::paint(juce::Graphics& g)
{
    // Component background is handled by parent
}

void PhaserComponent::resized()
{
    auto bounds = getLocalBounds();
    layoutControls(bounds);
}

void PhaserComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parentSynthesizer == nullptr) return;
    
    if (slider == &phaserMixKnob)
    {
        parentSynthesizer->audioProcessor.setPhaserMix(static_cast<float>(phaserMixKnob.getValue()));
    }
    else if (slider == &phaserRateKnob)
    {
        parentSynthesizer->audioProcessor.setPhaserRate(static_cast<float>(phaserRateKnob.getValue()));
    }
    else if (slider == &phaserDepth1Knob)
    {
        parentSynthesizer->audioProcessor.setPhaserDepth1(static_cast<float>(phaserDepth1Knob.getValue()));
    }
    else if (slider == &phaserDepth2Knob)
    {
        parentSynthesizer->audioProcessor.setPhaserDepth2(static_cast<float>(phaserDepth2Knob.getValue()));
    }
    else if (slider == &phaserFrequencyKnob)
    {
        parentSynthesizer->audioProcessor.setPhaserFrequency(static_cast<float>(phaserFrequencyKnob.getValue()));
    }
    else if (slider == &phaserFeedbackKnob)
    {
        parentSynthesizer->audioProcessor.setPhaserFeedback(static_cast<float>(phaserFeedbackKnob.getValue()));
    }
    else if (slider == &phaserPhaseKnob)
    {
        parentSynthesizer->audioProcessor.setPhaserPhase(static_cast<float>(phaserPhaseKnob.getValue()));
    }
}

void PhaserComponent::buttonClicked(juce::Button* button)
{
    if (parentSynthesizer == nullptr) return;
    
    if (button == &phaserPowerButton)
    {
        parentSynthesizer->audioProcessor.setPhaserEnabled(phaserPowerButton.getToggleState());
    }
    else if (button == &phaserBpmButton)
    {
        // TODO: Implement BPM sync for phaser if needed
        // For now, just toggle the button state
    }
}

void PhaserComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check if mouse click is on the phaser poles value label
    if (event.eventComponent == &phaserPolesValueLabel)
    {
        cyclePhaserPoles();
    }
}

void PhaserComponent::cyclePhaserPoles()
{
    // Cycle from 2 to 8 poles, then back to 2
    currentPhaserPoles++;
    if (currentPhaserPoles > 8)
        currentPhaserPoles = 2;
        
    phaserPolesValueLabel.setText(juce::String(currentPhaserPoles) + " POLES", juce::dontSendNotification);
    
    if (parentSynthesizer != nullptr)
    {
        parentSynthesizer->audioProcessor.setPhaserPoles(currentPhaserPoles);
    }
}

void PhaserComponent::syncWithDSPState()
{
    if (parentSynthesizer == nullptr) return;
    
    phaserMixKnob.setValue(parentSynthesizer->audioProcessor.getPhaserMix(), juce::dontSendNotification);
    phaserRateKnob.setValue(parentSynthesizer->audioProcessor.getPhaserRate(), juce::dontSendNotification);
    phaserDepth1Knob.setValue(parentSynthesizer->audioProcessor.getPhaserDepth1(), juce::dontSendNotification);
    phaserDepth2Knob.setValue(parentSynthesizer->audioProcessor.getPhaserDepth2(), juce::dontSendNotification);
    phaserFrequencyKnob.setValue(parentSynthesizer->audioProcessor.getPhaserFrequency(), juce::dontSendNotification);
    phaserFeedbackKnob.setValue(parentSynthesizer->audioProcessor.getPhaserFeedback(), juce::dontSendNotification);
    phaserPhaseKnob.setValue(parentSynthesizer->audioProcessor.getPhaserPhase(), juce::dontSendNotification);
    phaserPowerButton.setToggleState(parentSynthesizer->audioProcessor.getPhaserEnabled(), juce::dontSendNotification);
    // phaserBpmButton.setToggleState(false, juce::dontSendNotification); // BPM sync not implemented in processor yet
    
    // Sync phaser poles
    currentPhaserPoles = parentSynthesizer->audioProcessor.getPhaserPoles();
    if (currentPhaserPoles >= 2 && currentPhaserPoles <= 8)
    {
        phaserPolesValueLabel.setText(juce::String(currentPhaserPoles) + " POLES", juce::dontSendNotification);
    }
}

void PhaserComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    phaserPowerButton.setLookAndFeel(digitalButtonLAF);
    phaserBpmButton.setLookAndFeel(digitalButtonLAF);
    
    phaserMixKnob.setLookAndFeel(digitalKnobLAF);
    phaserRateKnob.setLookAndFeel(digitalKnobLAF);
    phaserDepth1Knob.setLookAndFeel(digitalKnobLAF);
    phaserDepth2Knob.setLookAndFeel(digitalKnobLAF);
    phaserFrequencyKnob.setLookAndFeel(digitalKnobLAF);
    phaserFeedbackKnob.setLookAndFeel(digitalKnobLAF);
    phaserPhaseKnob.setLookAndFeel(digitalKnobLAF);
    
    phaserMixLabel.setLookAndFeel(digitalKnobLAF);
    phaserRateLabel.setLookAndFeel(digitalKnobLAF);
    phaserPolesLabel.setLookAndFeel(digitalKnobLAF);
    phaserPolesValueLabel.setLookAndFeel(ledNumberLAF);
    phaserDepth1Label.setLookAndFeel(digitalKnobLAF);
    phaserDepth2Label.setLookAndFeel(digitalKnobLAF);
    phaserFrequencyLabel.setLookAndFeel(digitalKnobLAF);
    phaserFeedbackLabel.setLookAndFeel(digitalKnobLAF);
    phaserPhaseLabel.setLookAndFeel(digitalKnobLAF);
}

void PhaserComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Row 1: Power and Mix controls
    phaserPowerButton.setBounds(69, 65, 80, 25);
    phaserMixKnob.setBounds(169, 60, 40, 40);
    phaserMixLabel.setBounds(169, 102, 40, 12);
    
    // Row 2: Rate, BPM Sync, and Poles
    phaserRateKnob.setBounds(83, 125, 50, 50);
    phaserRateLabel.setBounds(83, 180, 50, 15);
    phaserBpmButton.setBounds(153, 140, 70, 25);
    phaserPolesValueLabel.setBounds(98, 200, 100, 25);
    phaserPolesLabel.setBounds(98, 230, 100, 15);
    
    // Row 3: Depth 1 and Depth 2
    phaserDepth1Knob.setBounds(89, 260, 50, 50);
    phaserDepth1Label.setBounds(89, 315, 50, 15);
    phaserDepth2Knob.setBounds(159, 260, 50, 50);
    phaserDepth2Label.setBounds(159, 315, 50, 15);
    
    // Row 4: Frequency, Feedback, and Phase
    phaserFrequencyKnob.setBounds(54, 340, 50, 50);
    phaserFrequencyLabel.setBounds(54, 395, 50, 15);
    phaserFeedbackKnob.setBounds(124, 340, 50, 50);
    phaserFeedbackLabel.setBounds(124, 395, 50, 15);
    phaserPhaseKnob.setBounds(194, 340, 50, 50);
    phaserPhaseLabel.setBounds(194, 395, 50, 15);
}