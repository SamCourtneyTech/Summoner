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

    // Oscillator 1 Section Label
    oscillator1Label.setText("Oscillator 1", juce::dontSendNotification);
    oscillator1Label.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(oscillator1Label);

    // Oscillator 1 Level
    osc1LevelSlider.setSliderStyle(juce::Slider::Rotary);
    osc1LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc1LevelSlider);
    osc1LevelLabel.setText("Osc1 Level", juce::dontSendNotification);
    addAndMakeVisible(osc1LevelLabel);

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

    // Oscillator 2 Section Label
    oscillator2Label.setText("Oscillator 2", juce::dontSendNotification);
    oscillator2Label.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(oscillator2Label);

    // Oscillator 2 Level
    osc2LevelSlider.setSliderStyle(juce::Slider::Rotary);
    osc2LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc2LevelSlider);
    osc2LevelLabel.setText("Osc2 Level", juce::dontSendNotification);
    addAndMakeVisible(osc2LevelLabel);

    // Detune
    detuneSlider.setSliderStyle(juce::Slider::Rotary);
    detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(detuneSlider);
    detuneLabel.setText("Detune", juce::dontSendNotification);
    addAndMakeVisible(detuneLabel);

    // LFO Section
    lfoSectionLabel.setText("LFO (Filter Cutoff)", juce::dontSendNotification);
    lfoSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(lfoSectionLabel);

    lfoRateSlider.setSliderStyle(juce::Slider::Rotary);
    lfoRateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoRateSlider);
    lfoRateLabel.setText("Rate", juce::dontSendNotification);
    addAndMakeVisible(lfoRateLabel);

    lfoDepthSlider.setSliderStyle(juce::Slider::Rotary);
    lfoDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoDepthSlider);
    lfoDepthLabel.setText("Depth", juce::dontSendNotification);
    addAndMakeVisible(lfoDepthLabel);

    lfoWaveformCombo.addItem("Sine", 1);
    lfoWaveformCombo.addItem("Triangle", 2);
    lfoWaveformCombo.addItem("Saw", 3);
    lfoWaveformCombo.addItem("Square", 4);
    lfoWaveformCombo.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(lfoWaveformCombo);
    lfoWaveformLabel.setText("Waveform", juce::dontSendNotification);
    addAndMakeVisible(lfoWaveformLabel);

    // Delay Section
    delaySectionLabel.setText("Delay", juce::dontSendNotification);
    delaySectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(delaySectionLabel);

    delayTimeSlider.setSliderStyle(juce::Slider::Rotary);
    delayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(delayTimeSlider);
    delayTimeLabel.setText("Time", juce::dontSendNotification);
    addAndMakeVisible(delayTimeLabel);

    delayFeedbackSlider.setSliderStyle(juce::Slider::Rotary);
    delayFeedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(delayFeedbackSlider);
    delayFeedbackLabel.setText("Feedback", juce::dontSendNotification);
    addAndMakeVisible(delayFeedbackLabel);

    delayMixSlider.setSliderStyle(juce::Slider::Rotary);
    delayMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(delayMixSlider);
    delayMixLabel.setText("Mix", juce::dontSendNotification);
    addAndMakeVisible(delayMixLabel);

    // Reverb Section
    reverbSectionLabel.setText("Reverb", juce::dontSendNotification);
    reverbSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(reverbSectionLabel);

    reverbRoomSizeSlider.setSliderStyle(juce::Slider::Rotary);
    reverbRoomSizeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(reverbRoomSizeSlider);
    reverbRoomSizeLabel.setText("Room Size", juce::dontSendNotification);
    addAndMakeVisible(reverbRoomSizeLabel);

    reverbDampingSlider.setSliderStyle(juce::Slider::Rotary);
    reverbDampingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(reverbDampingSlider);
    reverbDampingLabel.setText("Damping", juce::dontSendNotification);
    addAndMakeVisible(reverbDampingLabel);

    reverbWetLevelSlider.setSliderStyle(juce::Slider::Rotary);
    reverbWetLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(reverbWetLevelSlider);
    reverbWetLevelLabel.setText("Wet Level", juce::dontSendNotification);
    addAndMakeVisible(reverbWetLevelLabel);

    reverbDryLevelSlider.setSliderStyle(juce::Slider::Rotary);
    reverbDryLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(reverbDryLevelSlider);
    reverbDryLevelLabel.setText("Dry Level", juce::dontSendNotification);
    addAndMakeVisible(reverbDryLevelLabel);

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
    osc1LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc1Level", osc1LevelSlider);
    osc2LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc2Level", osc2LevelSlider);

    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoRate", lfoRateSlider);
    lfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoDepth", lfoDepthSlider);
    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "lfoWaveform", lfoWaveformCombo);

    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayTime", delayTimeSlider);
    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayFeedback", delayFeedbackSlider);
    delayMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayMix", delayMixSlider);

    reverbRoomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbRoomSize", reverbRoomSizeSlider);
    reverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbDamping", reverbDampingSlider);
    reverbWetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbWetLevel", reverbWetLevelSlider);
    reverbDryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbDryLevel", reverbDryLevelSlider);

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
    auto bounds = getLocalBounds().toFloat().reduced(20); // Keep as float for removeFrom* operations
    float knobWidth = 100;
    float knobHeight = 100;
    float comboHeight = 30;
    float labelHeight = 20;
    float sectionLabelHeight = 30;
    float comboWidth = 120;

    // Split the bounds into six rows with some spacing between them
    auto topRow = bounds.removeFromTop(labelHeight + knobHeight);
    bounds.removeFromTop(20);
    auto oscRow = bounds.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    bounds.removeFromTop(20);
    auto lfoRow = bounds.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    bounds.removeFromTop(20);
    auto delayRow = bounds.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    bounds.removeFromTop(20);
    auto reverbRow = bounds.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    bounds.removeFromTop(20);
    auto bottomRow = bounds.removeFromTop(labelHeight + knobHeight);

    // Top Row: ADSR controls (Attack, Decay, Sustain, Release)
    auto attackBounds = topRow.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackBounds.removeFromTop(labelHeight).toNearestInt());
    attackSlider.setBounds(attackBounds.toNearestInt());

    auto decayBounds = topRow.removeFromLeft(knobWidth);
    decayLabel.setBounds(decayBounds.removeFromTop(labelHeight).toNearestInt());
    decaySlider.setBounds(decayBounds.toNearestInt());

    auto sustainBounds = topRow.removeFromLeft(knobWidth);
    sustainLabel.setBounds(sustainBounds.removeFromTop(labelHeight).toNearestInt());
    sustainSlider.setBounds(sustainBounds.toNearestInt());

    auto releaseBounds = topRow.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseBounds.removeFromTop(labelHeight).toNearestInt());
    releaseSlider.setBounds(releaseBounds.toNearestInt());

    // Oscillator Row: Oscillator controls (Waveform 1, Osc1 Level, Waveform 2, Osc2 Level, Detune)
    auto osc1Section = oscRow.removeFromLeft(knobWidth * 2 + 20);
    oscillator1Label.setBounds(osc1Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveformBounds = osc1Section.removeFromLeft(knobWidth);
    waveformLabel.setBounds(waveformBounds.removeFromTop(labelHeight).toNearestInt());
    waveformSlider.setBounds(waveformBounds.toNearestInt());
    auto osc1LevelBounds = osc1Section.removeFromLeft(knobWidth);
    osc1LevelLabel.setBounds(osc1LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc1LevelSlider.setBounds(osc1LevelBounds.toNearestInt());

    auto osc2Section = oscRow.removeFromLeft(knobWidth * 2 + 20);
    oscillator2Label.setBounds(osc2Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveform2Bounds = osc2Section.removeFromLeft(knobWidth);
    waveform2Label.setBounds(waveform2Bounds.removeFromTop(labelHeight).toNearestInt());
    waveform2Slider.setBounds(waveform2Bounds.toNearestInt());
    auto osc2LevelBounds = osc2Section.removeFromLeft(knobWidth);
    osc2LevelLabel.setBounds(osc2LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc2LevelSlider.setBounds(osc2LevelBounds.toNearestInt());

    auto detuneBounds = oscRow.removeFromLeft(knobWidth);
    detuneLabel.setBounds(detuneBounds.removeFromTop(labelHeight).toNearestInt());
    detuneSlider.setBounds(detuneBounds.toNearestInt());

    // LFO Row: LFO controls (Rate, Depth, Waveform)
    auto lfoSection = lfoRow.removeFromLeft(knobWidth * 3 + 40);
    lfoSectionLabel.setBounds(lfoSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto lfoRateBounds = lfoSection.removeFromLeft(knobWidth);
    lfoRateLabel.setBounds(lfoRateBounds.removeFromTop(labelHeight).toNearestInt());
    lfoRateSlider.setBounds(lfoRateBounds.toNearestInt());
    auto lfoDepthBounds = lfoSection.removeFromLeft(knobWidth);
    lfoDepthLabel.setBounds(lfoDepthBounds.removeFromTop(labelHeight).toNearestInt());
    lfoDepthSlider.setBounds(lfoDepthBounds.toNearestInt());
    auto lfoWaveformBounds = lfoSection.removeFromLeft(comboWidth);
    lfoWaveformLabel.setBounds(lfoWaveformBounds.removeFromTop(labelHeight).toNearestInt());
    lfoWaveformCombo.setBounds(lfoWaveformBounds.withHeight(comboHeight).withY(lfoWaveformLabel.getBottom()).toNearestInt());

    // Delay Row: Delay controls (Time, Feedback, Mix)
    auto delaySection = delayRow.removeFromLeft(knobWidth * 3 + 20);
    delaySectionLabel.setBounds(delaySection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto delayTimeBounds = delaySection.removeFromLeft(knobWidth);
    delayTimeLabel.setBounds(delayTimeBounds.removeFromTop(labelHeight).toNearestInt());
    delayTimeSlider.setBounds(delayTimeBounds.toNearestInt());
    auto delayFeedbackBounds = delaySection.removeFromLeft(knobWidth);
    delayFeedbackLabel.setBounds(delayFeedbackBounds.removeFromTop(labelHeight).toNearestInt());
    delayFeedbackSlider.setBounds(delayFeedbackBounds.toNearestInt());
    auto delayMixBounds = delaySection.removeFromLeft(knobWidth);
    delayMixLabel.setBounds(delayMixBounds.removeFromTop(labelHeight).toNearestInt());
    delayMixSlider.setBounds(delayMixBounds.toNearestInt());

    // Reverb Row: Reverb controls (Room Size, Damping, Wet Level, Dry Level)
    auto reverbSection = reverbRow.removeFromLeft(knobWidth * 4 + 20);
    reverbSectionLabel.setBounds(reverbSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto reverbRoomSizeBounds = reverbSection.removeFromLeft(knobWidth);
    reverbRoomSizeLabel.setBounds(reverbRoomSizeBounds.removeFromTop(labelHeight).toNearestInt());
    reverbRoomSizeSlider.setBounds(reverbRoomSizeBounds.toNearestInt());
    auto reverbDampingBounds = reverbSection.removeFromLeft(knobWidth);
    reverbDampingLabel.setBounds(reverbDampingBounds.removeFromTop(labelHeight).toNearestInt());
    reverbDampingSlider.setBounds(reverbDampingBounds.toNearestInt());
    auto reverbWetLevelBounds = reverbSection.removeFromLeft(knobWidth);
    reverbWetLevelLabel.setBounds(reverbWetLevelBounds.removeFromTop(labelHeight).toNearestInt());
    reverbWetLevelSlider.setBounds(reverbWetLevelBounds.toNearestInt());
    auto reverbDryLevelBounds = reverbSection.removeFromLeft(knobWidth);
    reverbDryLevelLabel.setBounds(reverbDryLevelBounds.removeFromTop(labelHeight).toNearestInt());
    reverbDryLevelSlider.setBounds(reverbDryLevelBounds.toNearestInt());

    // Bottom Row: Filter controls (Cutoff, Resonance, ADSR Mix, ADSR Depth, Filter Type)
    auto cutoffBounds = bottomRow.removeFromLeft(knobWidth);
    filterCutoffLabel.setBounds(cutoffBounds.removeFromTop(labelHeight).toNearestInt());
    filterCutoffSlider.setBounds(cutoffBounds.toNearestInt());

    auto resonanceBounds = bottomRow.removeFromLeft(knobWidth);
    filterResonanceLabel.setBounds(resonanceBounds.removeFromTop(labelHeight).toNearestInt());
    filterResonanceSlider.setBounds(resonanceBounds.toNearestInt());

    auto adsrMixBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRMixLabel.setBounds(adsrMixBounds.removeFromTop(labelHeight).toNearestInt());
    filterADSRMixSlider.setBounds(adsrMixBounds.toNearestInt());

    auto adsrDepthBounds = bottomRow.removeFromLeft(knobWidth);
    filterADSRDepthLabel.setBounds(adsrDepthBounds.removeFromTop(labelHeight).toNearestInt());
    filterADSRDepthSlider.setBounds(adsrDepthBounds.toNearestInt());

    auto filterTypeBounds = bottomRow.removeFromLeft(comboWidth);
    filterTypeLabel.setBounds(filterTypeBounds.removeFromTop(labelHeight).toNearestInt());
    filterTypeCombo.setBounds(filterTypeBounds.withHeight(comboHeight).withY(filterTypeLabel.getBottom()).toNearestInt());
}