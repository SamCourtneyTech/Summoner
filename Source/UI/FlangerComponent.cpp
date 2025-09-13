#include "FlangerComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

FlangerComponent::FlangerComponent()
{
    // Initialize flanger controls
    
    // Power button
    flangerPowerButton.setButtonText("FLANGER ON");
    flangerPowerButton.setClickingTogglesState(true);
    flangerPowerButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(flangerPowerButton);
    
    // Mix knob
    flangerMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerMixKnob.setRange(0.0, 100.0, 1.0);
    flangerMixKnob.setValue(50.0);
    addAndMakeVisible(flangerMixKnob);
    
    flangerMixLabel.setText("MIX", juce::dontSendNotification);
    flangerMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(flangerMixLabel);
    
    // Rate knob
    flangerRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerRateKnob.setRange(0.1, 10.0, 0.1);
    flangerRateKnob.setValue(1.0);
    addAndMakeVisible(flangerRateKnob);
    
    flangerRateLabel.setText("RATE", juce::dontSendNotification);
    flangerRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerRateLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(flangerRateLabel);
    
    // BPM Sync button
    flangerBpmButton.setButtonText("BPM SYNC");
    flangerBpmButton.setClickingTogglesState(true);
    flangerBpmButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(flangerBpmButton);
    
    // Depth knob
    flangerDepthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerDepthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerDepthKnob.setRange(0.0, 100.0, 1.0);
    flangerDepthKnob.setValue(50.0);
    addAndMakeVisible(flangerDepthKnob);
    
    flangerDepthLabel.setText("DEPTH", juce::dontSendNotification);
    flangerDepthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerDepthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(flangerDepthLabel);
    
    // Feedback knob
    flangerFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerFeedbackKnob.setRange(0.0, 100.0, 1.0);
    flangerFeedbackKnob.setValue(25.0);
    addAndMakeVisible(flangerFeedbackKnob);
    
    flangerFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    flangerFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerFeedbackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(flangerFeedbackLabel);
    
    // Phase knob
    flangerPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerPhaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerPhaseKnob.setRange(0.0, 360.0, 1.0);
    flangerPhaseKnob.setValue(0.0);
    addAndMakeVisible(flangerPhaseKnob);
    
    flangerPhaseLabel.setText("PHASE", juce::dontSendNotification);
    flangerPhaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerPhaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(flangerPhaseLabel);
    
    // Add listeners for flanger controls
    flangerPowerButton.addListener(this);
    flangerMixKnob.addListener(this);
    flangerRateKnob.addListener(this);
    flangerBpmButton.addListener(this);
    flangerDepthKnob.addListener(this);
    flangerFeedbackKnob.addListener(this);
    flangerPhaseKnob.addListener(this);
}

void FlangerComponent::paint(juce::Graphics& g)
{
    // Component background is handled by parent
}

void FlangerComponent::resized()
{
    auto bounds = getLocalBounds();
    layoutControls(bounds);
}

void FlangerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parentSynthesizer == nullptr) return;
    
    if (slider == &flangerMixKnob)
    {
        parentSynthesizer->audioProcessor.setFlangerMix(static_cast<float>(flangerMixKnob.getValue()));
    }
    else if (slider == &flangerRateKnob)
    {
        parentSynthesizer->audioProcessor.setFlangerRate(static_cast<float>(flangerRateKnob.getValue()));
    }
    else if (slider == &flangerDepthKnob)
    {
        parentSynthesizer->audioProcessor.setFlangerDepth(static_cast<float>(flangerDepthKnob.getValue()));
    }
    else if (slider == &flangerFeedbackKnob)
    {
        parentSynthesizer->audioProcessor.setFlangerFeedback(static_cast<float>(flangerFeedbackKnob.getValue()));
    }
    else if (slider == &flangerPhaseKnob)
    {
        parentSynthesizer->audioProcessor.setFlangerPhase(static_cast<float>(flangerPhaseKnob.getValue()));
    }
}

void FlangerComponent::buttonClicked(juce::Button* button)
{
    if (parentSynthesizer == nullptr) return;
    
    if (button == &flangerPowerButton)
    {
        parentSynthesizer->audioProcessor.setFlangerEnabled(flangerPowerButton.getToggleState());
    }
    else if (button == &flangerBpmButton)
    {
        // TODO: Implement BPM sync for flanger if needed
        // For now, just toggle the button state
    }
}

void FlangerComponent::syncWithDSPState()
{
    if (parentSynthesizer == nullptr) return;
    
    flangerMixKnob.setValue(parentSynthesizer->audioProcessor.getFlangerMix(), juce::dontSendNotification);
    flangerRateKnob.setValue(parentSynthesizer->audioProcessor.getFlangerRate(), juce::dontSendNotification);
    flangerDepthKnob.setValue(parentSynthesizer->audioProcessor.getFlangerDepth(), juce::dontSendNotification);
    flangerFeedbackKnob.setValue(parentSynthesizer->audioProcessor.getFlangerFeedback(), juce::dontSendNotification);
    flangerPhaseKnob.setValue(parentSynthesizer->audioProcessor.getFlangerPhase(), juce::dontSendNotification);
    flangerPowerButton.setToggleState(parentSynthesizer->audioProcessor.getFlangerEnabled(), juce::dontSendNotification);
    // flangerBpmButton.setToggleState(false, juce::dontSendNotification); // BPM sync not implemented in processor yet
}

void FlangerComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    flangerPowerButton.setLookAndFeel(digitalButtonLAF);
    flangerBpmButton.setLookAndFeel(digitalButtonLAF);
    
    flangerMixKnob.setLookAndFeel(digitalKnobLAF);
    flangerRateKnob.setLookAndFeel(digitalKnobLAF);
    flangerDepthKnob.setLookAndFeel(digitalKnobLAF);
    flangerFeedbackKnob.setLookAndFeel(digitalKnobLAF);
    flangerPhaseKnob.setLookAndFeel(digitalKnobLAF);
    
    flangerMixLabel.setLookAndFeel(digitalKnobLAF);
    flangerRateLabel.setLookAndFeel(digitalKnobLAF);
    flangerDepthLabel.setLookAndFeel(digitalKnobLAF);
    flangerFeedbackLabel.setLookAndFeel(digitalKnobLAF);
    flangerPhaseLabel.setLookAndFeel(digitalKnobLAF);
}

void FlangerComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Row 1: Power and Mix controls
    flangerPowerButton.setBounds(68, 130, 80, 25);
    flangerMixKnob.setBounds(168, 125, 40, 40);
    flangerMixLabel.setBounds(168, 167, 40, 12);
    
    // Row 2: Rate, BPM Sync, and Depth
    flangerRateKnob.setBounds(86, 190, 50, 50);
    flangerRateLabel.setBounds(86, 245, 50, 15);
    flangerBpmButton.setBounds(146, 205, 70, 25);
    flangerDepthKnob.setBounds(195, 270, 50, 50);
    flangerDepthLabel.setBounds(195, 325, 50, 15);
    
    // Row 3: Feedback and Phase
    flangerFeedbackKnob.setBounds(51, 270, 50, 50);
    flangerFeedbackLabel.setBounds(51, 325, 50, 15);
    flangerPhaseKnob.setBounds(125, 270, 50, 50);
    flangerPhaseLabel.setBounds(125, 325, 50, 15);
}