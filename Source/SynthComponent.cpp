#include "SynthComponent.h"

SynthComponent::SynthComponent(juce::AudioProcessorValueTreeState& p) : params(p) {
    // Attack
    attackSlider.setSliderStyle(juce::Slider::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(attackSlider);
    attackLabel.setText("Attack", juce::dontSendNotification);
    addAndMakeVisible(attackLabel);

    // Decay
    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(decaySlider);
    decayLabel.setText("Decay", juce::dontSendNotification);
    addAndMakeVisible(decayLabel);

    // Sustain
    sustainSlider.setSliderStyle(juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(sustainSlider);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    addAndMakeVisible(sustainLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(releaseSlider);
    releaseLabel.setText("Release", juce::dontSendNotification);
    addAndMakeVisible(releaseLabel);

    // Waveform (Oscillator 1)
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
    waveformLabel.setText("Saw", juce::dontSendNotification);
    addAndMakeVisible(waveformLabel);

    // Waveform (Oscillator 2)
    waveform2Slider.setSliderStyle(juce::Slider::Rotary);
    waveform2Slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveform2Slider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(waveform2Slider);
    waveform2Slider.onValueChange = [this] {
        float value = waveform2Slider.getValue();
        if (value <= 0.16f)
            waveform2Label.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.32f)
            waveform2Label.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.48f)
            waveform2Label.setText("Square", juce::dontSendNotification);
        else if (value <= 0.64f)
            waveform2Label.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.80f)
            waveform2Label.setText("Pulse 25%", juce::dontSendNotification);
        else if (value <= 0.90f)
            waveform2Label.setText("White Noise", juce::dontSendNotification);
        else
            waveform2Label.setText("Pink Noise", juce::dontSendNotification);
        };
    waveform2Label.setText("Saw", juce::dontSendNotification);
    addAndMakeVisible(waveform2Label);

    // Detune
    detuneSlider.setSliderStyle(juce::Slider::Rotary);
    detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(detuneSlider);
    detuneLabel.setText("Detune", juce::dontSendNotification);
    addAndMakeVisible(detuneLabel);

    // Oscillator Mix
    oscMixSlider.setSliderStyle(juce::Slider::Rotary);
    oscMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(oscMixSlider);
    oscMixLabel.setText("Osc Mix", juce::dontSendNotification);
    addAndMakeVisible(oscMixLabel);

    // Filter Cutoff
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    addAndMakeVisible(filterCutoffLabel);

    // Filter Resonance
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    addAndMakeVisible(filterResonanceLabel);

    // Filter ADSR Mix
    filterADSRMixSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterADSRMixSlider);
    filterADSRMixLabel.setText("ADSR Mix", juce::dontSendNotification);
    addAndMakeVisible(filterADSRMixLabel);

    // Filter ADSR Depth
    filterADSRDepthSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterADSRDepthSlider);
    filterADSRDepthLabel.setText("ADSR Depth", juce::dontSendNotification);
    addAndMakeVisible(filterADSRDepthLabel);

    // Filter Type Dropdown
    filterTypeCombo.addItem("Low Pass", 1);
    filterTypeCombo.addItem("High Pass", 2);
    filterTypeCombo.addItem("Band Pass", 3);
    filterTypeCombo.addItem("Notch", 4);
    filterTypeCombo.setSelectedId(1, juce::dontSendNotification);
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
    waveform2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "waveform2", waveform2Slider);
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "detune", detuneSlider);
    oscMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "oscMix", oscMixSlider);

    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterCutoff", filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterResonance", filterResonanceSlider);
    filterADSRMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRMix", filterADSRMixSlider);
    filterADSRDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRDepth", filterADSRDepthSlider);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "filterType", filterTypeCombo);
}

void SynthComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void SynthComponent::resized() {
    auto bounds = getLocalBounds().reduced(10);
    int knobWidth = 100;
    int knobHeight = 100;
    int comboHeight = 30;
    int labelHeight = 20;
    int comboWidth = 120;

    // Split the bounds into three rows to accommodate the new controls
    auto topRow = bounds.removeFromTop(labelHeight + knobHeight);
    bounds.removeFromTop(10);
    auto middleRow = bounds.removeFromTop(labelHeight + knobHeight);
    bounds.removeFromTop(10);
    auto bottomRow = bounds.removeFromTop(labelHeight + knobHeight);

    // Top Row: Oscillator/ADSR controls (Attack, Decay, Sustain, Release)
    auto attackBounds = topRow.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackBounds.removeFromTop(labelHeight));
    attackSlider.setBounds(attackBounds);

    auto decayBounds = topRow.removeFromLeft(knobWidth);
    decayLabel.setBounds(decayBounds.removeFromTop(labelHeight));
    decaySlider.setBounds(decayBounds);

    auto sustainBounds = topRow.removeFromLeft(knobWidth);
    sustainLabel.setBounds(sustainBounds.removeFromTop(labelHeight));
    sustainSlider.setBounds(sustainBounds);

    auto releaseBounds = topRow.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseBounds.removeFromTop(labelHeight));
    releaseSlider.setBounds(releaseBounds);

    // Middle Row: Oscillator controls (Waveform 1, Waveform 2, Detune, Osc Mix)
    auto waveformBounds = middleRow.removeFromLeft(knobWidth);
    waveformLabel.setBounds(waveformBounds.removeFromTop(labelHeight));
    waveformSlider.setBounds(waveformBounds);

    auto waveform2Bounds = middleRow.removeFromLeft(knobWidth);
    waveform2Label.setBounds(waveform2Bounds.removeFromTop(labelHeight));
    waveform2Slider.setBounds(waveform2Bounds);

    auto detuneBounds = middleRow.removeFromLeft(knobWidth);
    detuneLabel.setBounds(detuneBounds.removeFromTop(labelHeight));
    detuneSlider.setBounds(detuneBounds);

    auto oscMixBounds = middleRow.removeFromLeft(knobWidth);
    oscMixLabel.setBounds(oscMixBounds.removeFromTop(labelHeight));
    oscMixSlider.setBounds(oscMixBounds);

    // Bottom Row: Filter controls (Cutoff, Resonance, ADSR Mix, ADSR Depth, Filter Type)
    auto cutoffBounds = bottomRow.removeFromLeft(knobWidth);
    filterCutoffLabel.setBounds(cutoffBounds.removeFromTop(labelHeight));
    filterCutoffSlider.setBounds(cutoffBounds);

    auto resonanceBounds = bottomRow.removeFromLeft(knobWidth);
    filterResonanceLabel.setBounds(resonanceBounds.removeFromTop(labelHeight));
    filterResonanceSlider.setBounds(resonanceBounds);

    auto adsrMixBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRMixLabel.setBounds(adsrMixBounds.removeFromTop(labelHeight));
    filterADSRMixSlider.setBounds(adsrMixBounds);

    auto adsrDepthBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRDepthLabel.setBounds(adsrDepthBounds.removeFromTop(labelHeight));
    filterADSRDepthSlider.setBounds(adsrDepthBounds);

    auto filterTypeBounds = bottomRow.removeFromLeft(comboWidth);
    filterTypeLabel.setBounds(filterTypeBounds.removeFromTop(labelHeight));
    filterTypeCombo.setBounds(filterTypeBounds.withHeight(knobHeight).withY(filterTypeLabel.getBottom()));
}