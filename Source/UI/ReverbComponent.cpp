#include "ReverbComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

ReverbComponent::ReverbComponent()
{
    // Initialize reverb controls
    
    // Power button
    reverbPowerButton.setButtonText("REVERB ON");
    reverbPowerButton.setClickingTogglesState(true);
    reverbPowerButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(reverbPowerButton);
    
    // Mix knob
    reverbMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbMixKnob.setRange(0.0, 100.0, 1.0);
    reverbMixKnob.setValue(30.0);
    addAndMakeVisible(reverbMixKnob);
    
    reverbMixLabel.setText("MIX", juce::dontSendNotification);
    reverbMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbMixLabel);
    
    // Reverb Type (clickable label)
    reverbTypeLabel.setText("TYPE", juce::dontSendNotification);
    reverbTypeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbTypeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbTypeLabel);
    
    // Type value label (interactive)
    reverbTypeValueLabel.setText("PLATE", juce::dontSendNotification);
    reverbTypeValueLabel.setJustificationType(juce::Justification::centred);
    reverbTypeValueLabel.setInterceptsMouseClicks(true, true);
    reverbTypeValueLabel.setEditable(false, false, false);
    reverbTypeValueLabel.addMouseListener(this, false);
    addAndMakeVisible(reverbTypeValueLabel);
    
    // Low Cut knob
    reverbLowCutKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbLowCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbLowCutKnob.setRange(20.0, 1000.0, 1.0);
    reverbLowCutKnob.setValue(80.0);
    addAndMakeVisible(reverbLowCutKnob);
    
    reverbLowCutLabel.setText("LOW CUT", juce::dontSendNotification);
    reverbLowCutLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbLowCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLowCutLabel);
    
    // High Cut knob
    reverbHighCutKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbHighCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbHighCutKnob.setRange(1000.0, 20000.0, 1.0);
    reverbHighCutKnob.setValue(12000.0);
    addAndMakeVisible(reverbHighCutKnob);
    
    reverbHighCutLabel.setText("HIGH CUT", juce::dontSendNotification);
    reverbHighCutLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbHighCutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbHighCutLabel);
    
    // Size knob
    reverbSizeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbSizeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbSizeKnob.setRange(0.0, 100.0, 1.0);
    reverbSizeKnob.setValue(50.0);
    addAndMakeVisible(reverbSizeKnob);
    
    reverbSizeLabel.setText("SIZE", juce::dontSendNotification);
    reverbSizeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbSizeLabel);
    
    // Pre Delay knob
    reverbPreDelayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbPreDelayKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbPreDelayKnob.setRange(0.0, 200.0, 1.0);
    reverbPreDelayKnob.setValue(20.0);
    addAndMakeVisible(reverbPreDelayKnob);
    
    reverbPreDelayLabel.setText("PRE DELAY", juce::dontSendNotification);
    reverbPreDelayLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbPreDelayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbPreDelayLabel);
    
    // Damp knob
    reverbDampKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbDampKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbDampKnob.setRange(0.0, 100.0, 1.0);
    reverbDampKnob.setValue(50.0);
    addAndMakeVisible(reverbDampKnob);
    
    reverbDampLabel.setText("DAMP", juce::dontSendNotification);
    reverbDampLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbDampLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbDampLabel);
    
    // Width knob
    reverbWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbWidthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbWidthKnob.setRange(0.0, 100.0, 1.0);
    reverbWidthKnob.setValue(80.0);
    addAndMakeVisible(reverbWidthKnob);
    
    reverbWidthLabel.setText("WIDTH", juce::dontSendNotification);
    reverbWidthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbWidthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbWidthLabel);
    
    // Add listeners for reverb controls
    reverbPowerButton.addListener(this);
    reverbMixKnob.addListener(this);
    reverbLowCutKnob.addListener(this);
    reverbHighCutKnob.addListener(this);
    reverbSizeKnob.addListener(this);
    reverbPreDelayKnob.addListener(this);
    reverbDampKnob.addListener(this);
    reverbWidthKnob.addListener(this);
}

void ReverbComponent::paint(juce::Graphics& g)
{
    // Component background is handled by parent
}

void ReverbComponent::resized()
{
    auto bounds = getLocalBounds();
    layoutControls(bounds);
}

void ReverbComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parentSynthesizer == nullptr) return;
    
    if (slider == &reverbMixKnob)
    {
        parentSynthesizer->audioProcessor.setReverbMix(static_cast<float>(reverbMixKnob.getValue()));
    }
    else if (slider == &reverbLowCutKnob)
    {
        parentSynthesizer->audioProcessor.setReverbLowCut(static_cast<float>(reverbLowCutKnob.getValue()));
    }
    else if (slider == &reverbHighCutKnob)
    {
        parentSynthesizer->audioProcessor.setReverbHighCut(static_cast<float>(reverbHighCutKnob.getValue()));
    }
    else if (slider == &reverbSizeKnob)
    {
        parentSynthesizer->audioProcessor.setReverbSize(static_cast<float>(reverbSizeKnob.getValue()));
    }
    else if (slider == &reverbPreDelayKnob)
    {
        parentSynthesizer->audioProcessor.setReverbPreDelay(static_cast<float>(reverbPreDelayKnob.getValue()));
    }
    else if (slider == &reverbDampKnob)
    {
        parentSynthesizer->audioProcessor.setReverbDamping(static_cast<float>(reverbDampKnob.getValue()));
    }
    else if (slider == &reverbWidthKnob)
    {
        parentSynthesizer->audioProcessor.setReverbWidth(static_cast<float>(reverbWidthKnob.getValue()));
    }
}

void ReverbComponent::buttonClicked(juce::Button* button)
{
    if (parentSynthesizer == nullptr) return;
    
    if (button == &reverbPowerButton)
    {
        parentSynthesizer->audioProcessor.setReverbEnabled(reverbPowerButton.getToggleState());
    }
}

void ReverbComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check if mouse click is on the reverb type value label
    if (event.eventComponent == &reverbTypeValueLabel)
    {
        cycleReverbType();
    }
}

void ReverbComponent::cycleReverbType()
{
    // Cycle from 1 to 5, then back to 1
    currentReverbType++;
    if (currentReverbType > 5)
        currentReverbType = 1;
        
    reverbTypeValueLabel.setText(reverbTypes[currentReverbType - 1], juce::dontSendNotification);
    
    if (parentSynthesizer != nullptr)
    {
        parentSynthesizer->audioProcessor.setReverbType(currentReverbType);
    }
}

void ReverbComponent::syncWithDSPState()
{
    if (parentSynthesizer == nullptr) return;
    
    reverbMixKnob.setValue(parentSynthesizer->audioProcessor.getReverbMix(), juce::dontSendNotification);
    reverbLowCutKnob.setValue(parentSynthesizer->audioProcessor.getReverbLowCut(), juce::dontSendNotification);
    reverbHighCutKnob.setValue(parentSynthesizer->audioProcessor.getReverbHighCut(), juce::dontSendNotification);
    reverbSizeKnob.setValue(parentSynthesizer->audioProcessor.getReverbSize(), juce::dontSendNotification);
    reverbPreDelayKnob.setValue(parentSynthesizer->audioProcessor.getReverbPreDelay(), juce::dontSendNotification);
    reverbDampKnob.setValue(parentSynthesizer->audioProcessor.getReverbDamping(), juce::dontSendNotification);
    reverbWidthKnob.setValue(parentSynthesizer->audioProcessor.getReverbWidth(), juce::dontSendNotification);
    reverbPowerButton.setToggleState(parentSynthesizer->audioProcessor.getReverbEnabled(), juce::dontSendNotification);
    
    // Sync reverb type
    currentReverbType = parentSynthesizer->audioProcessor.getReverbType();
    if (currentReverbType >= 1 && currentReverbType <= 5)
    {
        reverbTypeValueLabel.setText(reverbTypes[currentReverbType - 1], juce::dontSendNotification);
    }
}

void ReverbComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    reverbPowerButton.setLookAndFeel(digitalButtonLAF);
    reverbMixKnob.setLookAndFeel(digitalKnobLAF);
    reverbLowCutKnob.setLookAndFeel(digitalKnobLAF);
    reverbHighCutKnob.setLookAndFeel(digitalKnobLAF);
    reverbSizeKnob.setLookAndFeel(digitalKnobLAF);
    reverbPreDelayKnob.setLookAndFeel(digitalKnobLAF);
    reverbDampKnob.setLookAndFeel(digitalKnobLAF);
    reverbWidthKnob.setLookAndFeel(digitalKnobLAF);
    
    reverbMixLabel.setLookAndFeel(digitalKnobLAF);
    reverbTypeLabel.setLookAndFeel(digitalKnobLAF);
    reverbTypeValueLabel.setLookAndFeel(ledNumberLAF);
    reverbLowCutLabel.setLookAndFeel(digitalKnobLAF);
    reverbHighCutLabel.setLookAndFeel(digitalKnobLAF);
    reverbSizeLabel.setLookAndFeel(digitalKnobLAF);
    reverbPreDelayLabel.setLookAndFeel(digitalKnobLAF);
    reverbDampLabel.setLookAndFeel(digitalKnobLAF);
    reverbWidthLabel.setLookAndFeel(digitalKnobLAF);
}

void ReverbComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Row 1: Power and Mix controls
    reverbPowerButton.setBounds(68, 65, 80, 25);
    reverbMixKnob.setBounds(168, 60, 40, 40);
    reverbMixLabel.setBounds(168, 102, 40, 12);
    
    // Row 2: Reverb Type (clickable label)
    reverbTypeLabel.setBounds(49, 125, 200, 15);
    reverbTypeValueLabel.setBounds(49, 145, 200, 20);
    
    // Row 3: Low Cut and High Cut
    reverbLowCutKnob.setBounds(89, 185, 50, 50);
    reverbLowCutLabel.setBounds(89, 240, 50, 15);
    reverbHighCutKnob.setBounds(159, 185, 50, 50);
    reverbHighCutLabel.setBounds(159, 240, 50, 15);
    
    // Row 4: Size and Pre Delay
    reverbSizeKnob.setBounds(89, 265, 50, 50);
    reverbSizeLabel.setBounds(89, 320, 50, 15);
    reverbPreDelayKnob.setBounds(159, 265, 50, 50);
    reverbPreDelayLabel.setBounds(159, 320, 50, 15);
    
    // Row 5: Damp and Width
    reverbDampKnob.setBounds(89, 345, 50, 50);
    reverbDampLabel.setBounds(89, 400, 50, 15);
    reverbWidthKnob.setBounds(159, 345, 50, 50);
    reverbWidthLabel.setBounds(159, 400, 50, 15);
}