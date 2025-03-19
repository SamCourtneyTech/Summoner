#include "SynthComponent.h"

SynthComponent::SynthComponent(juce::AudioProcessorValueTreeState& p) : params(p) {
    // Attack
    attackSlider.setSliderStyle(juce::Slider::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(attackSlider);
    attackLabel.setText("Attack", juce::dontSendNotification);
    addAndMakeVisible(attackLabel);

    // Decay
    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(decaySlider);
    decayLabel.setText("Decay", juce::dontSendNotification);
    addAndMakeVisible(decayLabel);

    // Sustain
    sustainSlider.setSliderStyle(juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(sustainSlider);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    addAndMakeVisible(sustainLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(releaseSlider);
    releaseLabel.setText("Release", juce::dontSendNotification);
    addAndMakeVisible(releaseLabel);

    // Waveform
    waveformSlider.setSliderStyle(juce::Slider::Rotary);
    waveformSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveformSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(waveformSlider);
    waveformSlider.onValueChange = [this] {
        float value = waveformSlider.getValue();
        if (value <= 0.16f)
            waveformLabel.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.32f)
            waveformLabel.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.48f)
            waveformLabel.setText("Square", juce::dontSendNotification);
        else if (value <= 0.64f)
            waveformLabel.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.80f)
            waveformLabel.setText("Pulse 25%", juce::dontSendNotification);
        else if (value <= 0.90f)
            waveformLabel.setText("White Noise", juce::dontSendNotification);
        else
            waveformLabel.setText("Pink Noise", juce::dontSendNotification);
        };

    waveformLabel.setText("Sine", juce::dontSendNotification);
    addAndMakeVisible(waveformLabel);

    // Filter Cutoff
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    addAndMakeVisible(filterCutoffLabel);

    // Filter Resonance
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    addAndMakeVisible(filterResonanceLabel);

    // Filter ADSR Mix
    filterADSRMixSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterADSRMixSlider);
    filterADSRMixLabel.setText("ADSR Mix", juce::dontSendNotification);
    addAndMakeVisible(filterADSRMixLabel);

    // Filter Type
    filterTypeCombo.addItem("Low Pass", 1);
    filterTypeCombo.addItem("High Pass", 2);
    filterTypeCombo.addItem("Band Pass", 3);
    filterTypeCombo.addItem("Notch", 4);
    addAndMakeVisible(filterTypeCombo);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    addAndMakeVisible(filterTypeLabel);
}

SynthComponent::~SynthComponent() {}

void SynthComponent::initAttachments() {
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "attack", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "decay", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "sustain", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "release", releaseSlider);
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "waveform", waveformSlider);

    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterCutoff", filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterResonance", filterResonanceSlider);
    filterADSRMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRMix", filterADSRMixSlider);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "filterType", filterTypeCombo);
}

void SynthComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void SynthComponent::resized() {
    auto bounds = getLocalBounds().reduced(10);
    int knobWidth = 100;
    int knobHeight = 100;

    attackSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    attackLabel.setBounds(attackSlider.getBounds().removeFromBottom(20));
    decaySlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    decayLabel.setBounds(decaySlider.getBounds().removeFromBottom(20));
    sustainSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    sustainLabel.setBounds(sustainSlider.getBounds().removeFromBottom(20));
    releaseSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    releaseLabel.setBounds(releaseSlider.getBounds().removeFromBottom(20));
    waveformSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    waveformLabel.setBounds(waveformSlider.getBounds().removeFromBottom(20));

    filterCutoffSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    filterCutoffLabel.setBounds(filterCutoffSlider.getBounds().removeFromBottom(20));
    filterResonanceSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    filterResonanceLabel.setBounds(filterResonanceSlider.getBounds().removeFromBottom(20));
    filterADSRMixSlider.setBounds(bounds.removeFromLeft(knobWidth).withHeight(knobHeight));
    filterADSRMixLabel.setBounds(filterADSRMixSlider.getBounds().removeFromBottom(20));
    filterTypeCombo.setBounds(bounds.removeFromLeft(knobWidth).withHeight(30));
    filterTypeLabel.setBounds(filterTypeCombo.getBounds().removeFromBottom(20));
}