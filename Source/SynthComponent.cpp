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

    // ADSR Section Label
    adsrSectionLabel.setText("Envelope", juce::dontSendNotification);
    adsrSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(adsrSectionLabel);

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

    // Number of Voices
    numVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    numVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(numVoicesSlider);
    numVoicesLabel.setText("Voices", juce::dontSendNotification);
    addAndMakeVisible(numVoicesLabel);

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

    // Distortion Section
    distortionSectionLabel.setText("Distortion", juce::dontSendNotification);
    distortionSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(distortionSectionLabel);

    distortionDriveSlider.setSliderStyle(juce::Slider::Rotary);
    distortionDriveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(distortionDriveSlider);
    distortionDriveLabel.setText("Drive", juce::dontSendNotification);
    addAndMakeVisible(distortionDriveLabel);

    distortionToneSlider.setSliderStyle(juce::Slider::Rotary);
    distortionToneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(distortionToneSlider);
    distortionToneLabel.setText("Tone", juce::dontSendNotification);
    addAndMakeVisible(distortionToneLabel);

    distortionMixSlider.setSliderStyle(juce::Slider::Rotary);
    distortionMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(distortionMixSlider);
    distortionMixLabel.setText("Mix", juce::dontSendNotification);
    addAndMakeVisible(distortionMixLabel);

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

    // Chorus Section
    chorusSectionLabel.setText("Chorus", juce::dontSendNotification);
    chorusSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(chorusSectionLabel);

    chorusRateSlider.setSliderStyle(juce::Slider::Rotary);
    chorusRateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(chorusRateSlider);
    chorusRateLabel.setText("Rate", juce::dontSendNotification);
    addAndMakeVisible(chorusRateLabel);

    chorusDepthSlider.setSliderStyle(juce::Slider::Rotary);
    chorusDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(chorusDepthSlider);
    chorusDepthLabel.setText("Depth", juce::dontSendNotification);
    addAndMakeVisible(chorusDepthLabel);

    chorusMixSlider.setSliderStyle(juce::Slider::Rotary);
    chorusMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(chorusMixSlider);
    chorusMixLabel.setText("Mix", juce::dontSendNotification);
    addAndMakeVisible(chorusMixLabel);

    chorusDelaySlider.setSliderStyle(juce::Slider::Rotary);
    chorusDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(chorusDelaySlider);
    chorusDelayLabel.setText("Delay", juce::dontSendNotification);
    addAndMakeVisible(chorusDelayLabel);

    // Phaser Section
    phaserSectionLabel.setText("Phaser", juce::dontSendNotification);
    phaserSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(phaserSectionLabel);

    phaserRateSlider.setSliderStyle(juce::Slider::Rotary);
    phaserRateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(phaserRateSlider);
    phaserRateLabel.setText("Rate", juce::dontSendNotification);
    addAndMakeVisible(phaserRateLabel);

    phaserDepthSlider.setSliderStyle(juce::Slider::Rotary);
    phaserDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(phaserDepthSlider);
    phaserDepthLabel.setText("Depth", juce::dontSendNotification);
    addAndMakeVisible(phaserDepthLabel);

    phaserMixSlider.setSliderStyle(juce::Slider::Rotary);
    phaserMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(phaserMixSlider);
    phaserMixLabel.setText("Mix", juce::dontSendNotification);
    addAndMakeVisible(phaserMixLabel);

    // Flanger Section
    flangerSectionLabel.setText("Flanger", juce::dontSendNotification);
    flangerSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(flangerSectionLabel);

    flangerRateSlider.setSliderStyle(juce::Slider::Rotary);
    flangerRateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(flangerRateSlider);
    flangerRateLabel.setText("Rate", juce::dontSendNotification);
    addAndMakeVisible(flangerRateLabel);

    flangerDepthSlider.setSliderStyle(juce::Slider::Rotary);
    flangerDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(flangerDepthSlider);
    flangerDepthLabel.setText("Depth", juce::dontSendNotification);
    addAndMakeVisible(flangerDepthLabel);

    flangerMixSlider.setSliderStyle(juce::Slider::Rotary);
    flangerMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(flangerMixSlider);
    flangerMixLabel.setText("Mix", juce::dontSendNotification);
    addAndMakeVisible(flangerMixLabel);

    flangerDelaySlider.setSliderStyle(juce::Slider::Rotary);
    flangerDelaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(flangerDelaySlider);
    flangerDelayLabel.setText("Delay", juce::dontSendNotification);
    addAndMakeVisible(flangerDelayLabel);

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

    // Filter Section Label
    filterSectionLabel.setText("Filter", juce::dontSendNotification);
    filterSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(filterSectionLabel);

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

    // Compressor Section
    compressorSectionLabel.setText("Compressor", juce::dontSendNotification);
    compressorSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(compressorSectionLabel);

    compressorThresholdSlider.setSliderStyle(juce::Slider::Rotary);
    compressorThresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(compressorThresholdSlider);
    compressorThresholdLabel.setText("Threshold", juce::dontSendNotification);
    addAndMakeVisible(compressorThresholdLabel);

    compressorRatioSlider.setSliderStyle(juce::Slider::Rotary);
    compressorRatioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(compressorRatioSlider);
    compressorRatioLabel.setText("Ratio", juce::dontSendNotification);
    addAndMakeVisible(compressorRatioLabel);

    compressorAttackSlider.setSliderStyle(juce::Slider::Rotary);
    compressorAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(compressorAttackSlider);
    compressorAttackLabel.setText("Attack", juce::dontSendNotification);
    addAndMakeVisible(compressorAttackLabel);

    compressorReleaseSlider.setSliderStyle(juce::Slider::Rotary);
    compressorReleaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(compressorReleaseSlider);
    compressorReleaseLabel.setText("Release", juce::dontSendNotification);
    addAndMakeVisible(compressorReleaseLabel);

    compressorMakeupGainSlider.setSliderStyle(juce::Slider::Rotary);
    compressorMakeupGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(compressorMakeupGainSlider);
    compressorMakeupGainLabel.setText("Makeup Gain", juce::dontSendNotification);
    addAndMakeVisible(compressorMakeupGainLabel);
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

    // New attachment for number of voices
    numVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "numVoices", numVoicesSlider);

    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoRate", lfoRateSlider);
    lfoDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoDepth", lfoDepthSlider);
    lfoWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "lfoWaveform", lfoWaveformCombo);

    distortionDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "distortionDrive", distortionDriveSlider);
    distortionToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "distortionTone", distortionToneSlider);
    distortionMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "distortionMix", distortionMixSlider);

    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayTime", delayTimeSlider);
    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayFeedback", delayFeedbackSlider);
    delayMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "delayMix", delayMixSlider);

    chorusRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "chorusRate", chorusRateSlider);
    chorusDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "chorusDepth", chorusDepthSlider);
    chorusMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "chorusMix", chorusMixSlider);
    chorusDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "chorusDelay", chorusDelaySlider);

    phaserRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "phaserRate", phaserRateSlider);
    phaserDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "phaserDepth", phaserDepthSlider);
    phaserMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "phaserMix", phaserMixSlider);

    flangerRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "flangerRate", flangerRateSlider);
    flangerDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "flangerDepth", flangerDepthSlider);
    flangerMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "flangerMix", flangerMixSlider);
    flangerDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "flangerDelay", flangerDelaySlider);

    reverbRoomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbRoomSize", reverbRoomSizeSlider);
    reverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbDamping", reverbDampingSlider);
    reverbWetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbWetLevel", reverbWetLevelSlider);
    reverbDryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "reverbDryLevel", reverbDryLevelSlider);

    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterCutoff", filterCutoffSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterResonance", filterResonanceSlider);
    filterADSRMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRMix", filterADSRMixSlider);
    filterADSRDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "filterADSRDepth", filterADSRDepthSlider);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "filterType", filterTypeCombo);

    compressorThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorThreshold", compressorThresholdSlider);
    compressorRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorRatio", compressorRatioSlider);
    compressorAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorAttack", compressorAttackSlider);
    compressorReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorRelease", compressorReleaseSlider);
    compressorMakeupGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorMakeupGain", compressorMakeupGainSlider);
}


void SynthComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void SynthComponent::resized() {
    auto bounds = getLocalBounds().toFloat().reduced(20);
    float knobWidth = 100;
    float knobHeight = 100;
    float comboHeight = 30;
    float labelHeight = 20;
    float sectionLabelHeight = 30;
    float comboWidth = 120;

    // Split the window into three columns
    auto leftColumn = bounds.removeFromLeft(bounds.getWidth() / 3 - 20); // Left column with padding
    auto middleColumn = bounds.removeFromLeft(bounds.getWidth() / 2 - 20); // Middle column with padding
    auto rightColumn = bounds; // Right column

    // Left Column: ADSR, Oscillator 1, Oscillator 2, Detune, Num Voices, LFO
    auto adsrRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(20);
    auto osc1Row = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(20);
    auto osc2Row = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(20);
    auto detuneRow = leftColumn.removeFromTop(labelHeight + knobHeight);
    leftColumn.removeFromTop(20);
    auto numVoicesRow = leftColumn.removeFromTop(labelHeight + knobHeight); // Added row for num voices
    leftColumn.removeFromTop(20);
    auto lfoRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Middle Column: Distortion, Filter, Compressor
    auto distortionRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    middleColumn.removeFromTop(20);
    auto filterRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    middleColumn.removeFromTop(20);
    auto compressorRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Right Column: Delay, Chorus, Phaser, Flanger, Reverb
    auto delayRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(20);
    auto chorusRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(20);
    auto phaserRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(20);
    auto flangerRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(20);
    auto reverbRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Left Column Layout
    // ADSR Row: Envelope label, Attack, Decay, Sustain, Release
    auto adsrSection = adsrRow;
    adsrSectionLabel.setBounds(adsrSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto attackBounds = adsrSection.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackBounds.removeFromTop(labelHeight).toNearestInt());
    attackSlider.setBounds(attackBounds.toNearestInt());

    auto decayBounds = adsrSection.removeFromLeft(knobWidth);
    decayLabel.setBounds(decayBounds.removeFromTop(labelHeight).toNearestInt());
    decaySlider.setBounds(decayBounds.toNearestInt());

    auto sustainBounds = adsrSection.removeFromLeft(knobWidth);
    sustainLabel.setBounds(sustainBounds.removeFromTop(labelHeight).toNearestInt());
    sustainSlider.setBounds(sustainBounds.toNearestInt());

    auto releaseBounds = adsrSection.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseBounds.removeFromTop(labelHeight).toNearestInt());
    releaseSlider.setBounds(releaseBounds.toNearestInt());

    // Oscillator 1 Row: Waveform 1, Osc1 Level
    auto osc1Section = osc1Row.removeFromLeft(knobWidth * 2 + 20);
    oscillator1Label.setBounds(osc1Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveformBounds = osc1Section.removeFromLeft(knobWidth);
    waveformLabel.setBounds(waveformBounds.removeFromTop(labelHeight).toNearestInt());
    waveformSlider.setBounds(waveformBounds.toNearestInt());
    auto osc1LevelBounds = osc1Section.removeFromLeft(knobWidth);
    osc1LevelLabel.setBounds(osc1LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc1LevelSlider.setBounds(osc1LevelBounds.toNearestInt());

    // Oscillator 2 Row: Waveform 2, Osc2 Level
    auto osc2Section = osc2Row.removeFromLeft(knobWidth * 2 + 20);
    oscillator2Label.setBounds(osc2Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveform2Bounds = osc2Section.removeFromLeft(knobWidth);
    waveform2Label.setBounds(waveform2Bounds.removeFromTop(labelHeight).toNearestInt());
    waveform2Slider.setBounds(waveform2Bounds.toNearestInt());
    auto osc2LevelBounds = osc2Section.removeFromLeft(knobWidth);
    osc2LevelLabel.setBounds(osc2LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc2LevelSlider.setBounds(osc2LevelBounds.toNearestInt());

    // Detune Row: Detune
    auto detuneBounds = detuneRow.removeFromLeft(knobWidth);
    detuneLabel.setBounds(detuneBounds.removeFromTop(labelHeight).toNearestInt());
    detuneSlider.setBounds(detuneBounds.toNearestInt());

    // Num Voices Row: Number of Voices
    auto numVoicesBounds = numVoicesRow.removeFromLeft(knobWidth);
    numVoicesLabel.setBounds(numVoicesBounds.removeFromTop(labelHeight).toNearestInt());
    numVoicesSlider.setBounds(numVoicesBounds.toNearestInt());

    // LFO Row: Rate, Depth, Waveform
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

    // Middle Column Layout
    // Distortion Row: Drive, Tone, Mix
    auto distortionSection = distortionRow.removeFromLeft(knobWidth * 3 + 20);
    distortionSectionLabel.setBounds(distortionSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto distortionDriveBounds = distortionSection.removeFromLeft(knobWidth);
    distortionDriveLabel.setBounds(distortionDriveBounds.removeFromTop(labelHeight).toNearestInt());
    distortionDriveSlider.setBounds(distortionDriveBounds.toNearestInt());
    auto distortionToneBounds = distortionSection.removeFromLeft(knobWidth);
    distortionToneLabel.setBounds(distortionToneBounds.removeFromTop(labelHeight).toNearestInt());
    distortionToneSlider.setBounds(distortionToneBounds.toNearestInt());
    auto distortionMixBounds = distortionSection.removeFromLeft(knobWidth);
    distortionMixLabel.setBounds(distortionMixBounds.removeFromTop(labelHeight).toNearestInt());
    distortionMixSlider.setBounds(distortionMixBounds.toNearestInt());

    // Filter Row: Filter label, Cutoff, Resonance, ADSR Mix, ADSR Depth, Filter Type
    auto filterSection = filterRow;
    filterSectionLabel.setBounds(filterSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto cutoffBounds = filterSection.removeFromLeft(knobWidth);
    filterCutoffLabel.setBounds(cutoffBounds.removeFromTop(labelHeight).toNearestInt());
    filterCutoffSlider.setBounds(cutoffBounds.toNearestInt());

    auto resonanceBounds = filterSection.removeFromLeft(knobWidth);
    filterResonanceLabel.setBounds(resonanceBounds.removeFromTop(labelHeight).toNearestInt());
    filterResonanceSlider.setBounds(resonanceBounds.toNearestInt());

    auto adsrMixBounds = filterSection.removeFromLeft(knobWidth);
    filterADSRMixLabel.setBounds(adsrMixBounds.removeFromTop(labelHeight).toNearestInt());
    filterADSRMixSlider.setBounds(adsrMixBounds.toNearestInt());

    auto adsrDepthBounds = filterSection.removeFromLeft(knobWidth);
    filterADSRDepthLabel.setBounds(adsrDepthBounds.removeFromTop(labelHeight).toNearestInt());
    filterADSRDepthSlider.setBounds(adsrDepthBounds.toNearestInt());

    auto filterTypeBounds = filterSection.removeFromLeft(comboWidth);
    filterTypeLabel.setBounds(filterTypeBounds.removeFromTop(labelHeight).toNearestInt());
    filterTypeCombo.setBounds(filterTypeBounds.withHeight(comboHeight).withY(filterTypeLabel.getBottom()).toNearestInt());

    // Compressor Row: Threshold, Ratio, Attack, Release, Makeup Gain
    auto compressorSection = compressorRow;
    compressorSectionLabel.setBounds(compressorSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto compressorThresholdBounds = compressorSection.removeFromLeft(knobWidth);
    compressorThresholdLabel.setBounds(compressorThresholdBounds.removeFromTop(labelHeight).toNearestInt());
    compressorThresholdSlider.setBounds(compressorThresholdBounds.toNearestInt());
    auto compressorRatioBounds = compressorSection.removeFromLeft(knobWidth);
    compressorRatioLabel.setBounds(compressorRatioBounds.removeFromTop(labelHeight).toNearestInt());
    compressorRatioSlider.setBounds(compressorRatioBounds.toNearestInt());
    auto compressorAttackBounds = compressorSection.removeFromLeft(knobWidth);
    compressorAttackLabel.setBounds(compressorAttackBounds.removeFromTop(labelHeight).toNearestInt());
    compressorAttackSlider.setBounds(compressorAttackBounds.toNearestInt());
    auto compressorReleaseBounds = compressorSection.removeFromLeft(knobWidth);
    compressorReleaseLabel.setBounds(compressorReleaseBounds.removeFromTop(labelHeight).toNearestInt());
    compressorReleaseSlider.setBounds(compressorReleaseBounds.toNearestInt());
    auto compressorMakeupGainBounds = compressorSection.removeFromLeft(knobWidth);
    compressorMakeupGainLabel.setBounds(compressorMakeupGainBounds.removeFromTop(labelHeight).toNearestInt());
    compressorMakeupGainSlider.setBounds(compressorMakeupGainBounds.toNearestInt());

    // Right Column Layout
    // Delay Row: Time, Feedback, Mix
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

    // Chorus Row: Rate, Depth, Mix, Delay
    auto chorusSection = chorusRow.removeFromLeft(knobWidth * 4 + 20);
    chorusSectionLabel.setBounds(chorusSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto chorusRateBounds = chorusSection.removeFromLeft(knobWidth);
    chorusRateLabel.setBounds(chorusRateBounds.removeFromTop(labelHeight).toNearestInt());
    chorusRateSlider.setBounds(chorusRateBounds.toNearestInt());
    auto chorusDepthBounds = chorusSection.removeFromLeft(knobWidth);
    chorusDepthLabel.setBounds(chorusDepthBounds.removeFromTop(labelHeight).toNearestInt());
    chorusDepthSlider.setBounds(chorusDepthBounds.toNearestInt());
    auto chorusMixBounds = chorusSection.removeFromLeft(knobWidth);
    chorusMixLabel.setBounds(chorusMixBounds.removeFromTop(labelHeight).toNearestInt());
    chorusMixSlider.setBounds(chorusMixBounds.toNearestInt());
    auto chorusDelayBounds = chorusSection.removeFromLeft(knobWidth);
    chorusDelayLabel.setBounds(chorusDelayBounds.removeFromTop(labelHeight).toNearestInt());
    chorusDelaySlider.setBounds(chorusDelayBounds.toNearestInt());

    // Phaser Row: Rate, Depth, Mix
    auto phaserSection = phaserRow.removeFromLeft(knobWidth * 3 + 20);
    phaserSectionLabel.setBounds(phaserSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto phaserRateBounds = phaserSection.removeFromLeft(knobWidth);
    phaserRateLabel.setBounds(phaserRateBounds.removeFromTop(labelHeight).toNearestInt());
    phaserRateSlider.setBounds(phaserRateBounds.toNearestInt());
    auto phaserDepthBounds = phaserSection.removeFromLeft(knobWidth);
    phaserDepthLabel.setBounds(phaserDepthBounds.removeFromTop(labelHeight).toNearestInt());
    phaserDepthSlider.setBounds(phaserDepthBounds.toNearestInt());
    auto phaserMixBounds = phaserSection.removeFromLeft(knobWidth);
    phaserMixLabel.setBounds(phaserMixBounds.removeFromTop(labelHeight).toNearestInt());
    phaserMixSlider.setBounds(phaserMixBounds.toNearestInt());

    // Flanger Row: Rate, Depth, Mix, Delay
    auto flangerSection = flangerRow.removeFromLeft(knobWidth * 4 + 20);
    flangerSectionLabel.setBounds(flangerSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto flangerRateBounds = flangerSection.removeFromLeft(knobWidth);
    flangerRateLabel.setBounds(flangerRateBounds.removeFromTop(labelHeight).toNearestInt());
    flangerRateSlider.setBounds(flangerRateBounds.toNearestInt());
    auto flangerDepthBounds = flangerSection.removeFromLeft(knobWidth);
    flangerDepthLabel.setBounds(flangerDepthBounds.removeFromTop(labelHeight).toNearestInt());
    flangerDepthSlider.setBounds(flangerDepthBounds.toNearestInt());
    auto flangerMixBounds = flangerSection.removeFromLeft(knobWidth);
    flangerMixLabel.setBounds(flangerMixBounds.removeFromTop(labelHeight).toNearestInt());
    flangerMixSlider.setBounds(flangerMixBounds.toNearestInt());
    auto flangerDelayBounds = flangerSection.removeFromLeft(knobWidth);
    flangerDelayLabel.setBounds(flangerDelayBounds.removeFromTop(labelHeight).toNearestInt());
    flangerDelaySlider.setBounds(flangerDelayBounds.toNearestInt());

    // Reverb Row: Room Size, Damping, Wet Level, Dry Level
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
}