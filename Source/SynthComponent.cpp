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

    // Filter ADSR Depth
    filterADSRDepthSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(filterADSRDepthSlider);
    filterADSRDepthLabel.setText("ADSR Depth", juce::dontSendNotification);
    addAndMakeVisible(filterADSRDepthLabel);

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

    filterADSRDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRDepth", filterADSRDepthSlider);
}

void SynthComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void SynthComponent::resized() {
    auto bounds = getLocalBounds().reduced(10); // Add some padding
    int knobWidth = 100;
    int knobHeight = 100;
    int comboHeight = 30; // Height for the dropdown
    int labelHeight = 20; // Height for labels
    int comboWidth = 120; // Slightly wider for readability

    // Split the bounds into two rows with some spacing between them
    auto topRow = bounds.removeFromTop(labelHeight + knobHeight);
    bounds.removeFromTop(10); // Add 10 pixels of spacing between rows
    auto bottomRow = bounds.removeFromTop(labelHeight + knobHeight);

    // Top Row: Oscillator/ADSR controls (Attack, Decay, Sustain, Release, Waveform)
    // Attack
    auto attackBounds = topRow.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackBounds.removeFromTop(labelHeight));
    attackSlider.setBounds(attackBounds);

    // Decay
    auto decayBounds = topRow.removeFromLeft(knobWidth);
    decayLabel.setBounds(decayBounds.removeFromTop(labelHeight));
    decaySlider.setBounds(decayBounds);

    // Sustain
    auto sustainBounds = topRow.removeFromLeft(knobWidth);
    sustainLabel.setBounds(sustainBounds.removeFromTop(labelHeight));
    sustainSlider.setBounds(sustainBounds);

    // Release
    auto releaseBounds = topRow.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseBounds.removeFromTop(labelHeight));
    releaseSlider.setBounds(releaseBounds);

    // Waveform
    auto waveformBounds = topRow.removeFromLeft(knobWidth);
    waveformLabel.setBounds(waveformBounds.removeFromTop(labelHeight));
    waveformSlider.setBounds(waveformBounds);

    // Bottom Row: Filter controls (Cutoff, Resonance, ADSR Mix, ADSR Depth, Filter Type)
    // Filter Cutoff
    auto cutoffBounds = bottomRow.removeFromLeft(knobWidth);
    filterCutoffLabel.setBounds(cutoffBounds.removeFromTop(labelHeight));
    filterCutoffSlider.setBounds(cutoffBounds);

    // Filter Resonance
    auto resonanceBounds = bottomRow.removeFromLeft(knobWidth);
    filterResonanceLabel.setBounds(resonanceBounds.removeFromTop(labelHeight));
    filterResonanceSlider.setBounds(resonanceBounds);

    // Filter ADSR Mix
    auto adsrMixBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRMixLabel.setBounds(adsrMixBounds.removeFromTop(labelHeight));
    filterADSRMixSlider.setBounds(adsrMixBounds);

    // Filter ADSR Depth
    auto adsrDepthBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRDepthLabel.setBounds(adsrDepthBounds.removeFromTop(labelHeight));
    filterADSRDepthSlider.setBounds(adsrDepthBounds);

    // Filter Type
    auto filterTypeBounds = bottomRow.removeFromLeft(comboWidth);
    filterTypeLabel.setBounds(filterTypeBounds.removeFromTop(labelHeight));
    // Align the dropdown vertically with the sliders by adjusting its height and position
    filterTypeCombo.setBounds(filterTypeBounds.withHeight(knobHeight).withY(filterTypeLabel.getBottom()));
}