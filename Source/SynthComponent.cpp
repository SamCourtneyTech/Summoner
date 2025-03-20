#include "SynthComponent.h"

SynthComponent::SynthComponent(juce::AudioProcessorValueTreeState& p) : params(p) {
    attackSlider.setSliderStyle(juce::Slider::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(attackSlider);
    attackLabel.setText("Attack", juce::dontSendNotification);
    addAndMakeVisible(attackLabel);

    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(decaySlider);
    decayLabel.setText("Decay", juce::dontSendNotification);
    addAndMakeVisible(decayLabel);

    sustainSlider.setSliderStyle(juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(sustainSlider);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    addAndMakeVisible(sustainLabel);

    releaseSlider.setSliderStyle(juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(releaseSlider);
    releaseLabel.setText("Release", juce::dontSendNotification);
    addAndMakeVisible(releaseLabel);

    adsrSectionLabel.setText("Envelope", juce::dontSendNotification);
    adsrSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(adsrSectionLabel);

    waveformSlider.setSliderStyle(juce::Slider::Rotary);
    waveformSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveformSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(waveformSlider);
    waveformSlider.onValueChange = [this] {
        float value = waveformSlider.getValue();
        if (value <= 0.16f) waveformLabel.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.33f) waveformLabel.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.50f) waveformLabel.setText("Square", juce::dontSendNotification);
        else if (value <= 0.66f) waveformLabel.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.83f) waveformLabel.setText("Pulse 25%", juce::dontSendNotification);
        else waveformLabel.setText("Pulse 15%", juce::dontSendNotification);
        };
    waveformLabel.setText("Saw", juce::dontSendNotification);
    addAndMakeVisible(waveformLabel);

    oscillator1Label.setText("Oscillator 1", juce::dontSendNotification);
    oscillator1Label.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(oscillator1Label);

    osc1LevelSlider.setSliderStyle(juce::Slider::Rotary);
    osc1LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc1LevelSlider);
    osc1LevelLabel.setText("Osc1 Level", juce::dontSendNotification);
    addAndMakeVisible(osc1LevelLabel);

    waveform2Slider.setSliderStyle(juce::Slider::Rotary);
    waveform2Slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveform2Slider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(waveform2Slider);
    waveform2Slider.onValueChange = [this] {
        float value = waveform2Slider.getValue();
        if (value <= 0.16f) waveform2Label.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.33f) waveform2Label.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.50f) waveform2Label.setText("Square", juce::dontSendNotification);
        else if (value <= 0.66f) waveform2Label.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.83f) waveform2Label.setText("Pulse 25%", juce::dontSendNotification);
        else waveform2Label.setText("Pulse 15%", juce::dontSendNotification);
        };
    waveform2Label.setText("Saw", juce::dontSendNotification);
    addAndMakeVisible(waveform2Label);

    oscillator2Label.setText("Oscillator 2", juce::dontSendNotification);
    oscillator2Label.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(oscillator2Label);

    osc2LevelSlider.setSliderStyle(juce::Slider::Rotary);
    osc2LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc2LevelSlider);
    osc2LevelLabel.setText("Osc2 Level", juce::dontSendNotification);
    addAndMakeVisible(osc2LevelLabel);

    waveform3Slider.setSliderStyle(juce::Slider::Rotary);
    waveform3Slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveform3Slider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(waveform3Slider);
    waveform3Slider.onValueChange = [this] {
        float value = waveform3Slider.getValue();
        if (value <= 0.16f) waveform3Label.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.33f) waveform3Label.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.50f) waveform3Label.setText("Square", juce::dontSendNotification);
        else if (value <= 0.66f) waveform3Label.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.83f) waveform3Label.setText("Pulse 25%", juce::dontSendNotification);
        else waveform3Label.setText("Pulse 15%", juce::dontSendNotification);
        };
    waveform3Label.setText("Saw", juce::dontSendNotification);
    addAndMakeVisible(waveform3Label);

    oscillator3Label.setText("Oscillator 3", juce::dontSendNotification);
    oscillator3Label.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(oscillator3Label);

    osc3LevelSlider.setSliderStyle(juce::Slider::Rotary);
    osc3LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc3LevelSlider);
    osc3LevelLabel.setText("Osc3 Level", juce::dontSendNotification);
    addAndMakeVisible(osc3LevelLabel);

    noiseWaveformSlider.setSliderStyle(juce::Slider::Rotary);
    noiseWaveformSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    noiseWaveformSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(noiseWaveformSlider);
    noiseWaveformSlider.onValueChange = [this] {
        float value = noiseWaveformSlider.getValue();
        noiseWaveformLabel.setText(value <= 0.5f ? "White Noise" : "Pink Noise", juce::dontSendNotification);
        };
    noiseWaveformLabel.setText("White Noise", juce::dontSendNotification);
    addAndMakeVisible(noiseWaveformLabel);

    noiseOscillatorLabel.setText("Noise Oscillator", juce::dontSendNotification);
    noiseOscillatorLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(noiseOscillatorLabel);

    noiseLevelSlider.setSliderStyle(juce::Slider::Rotary);
    noiseLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(noiseLevelSlider);
    noiseLevelLabel.setText("Noise Level", juce::dontSendNotification);
    addAndMakeVisible(noiseLevelLabel);

    subWaveformSlider.setSliderStyle(juce::Slider::Rotary);
    subWaveformSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subWaveformSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(subWaveformSlider);
    subWaveformSlider.onValueChange = [this] {
        float value = subWaveformSlider.getValue();
        if (value <= 0.16f) subWaveformLabel.setText("Sine", juce::dontSendNotification);
        else if (value <= 0.33f) subWaveformLabel.setText("Rounded Sine", juce::dontSendNotification);
        else if (value <= 0.50f) subWaveformLabel.setText("Triangle", juce::dontSendNotification);
        else if (value <= 0.66f) subWaveformLabel.setText("Saw", juce::dontSendNotification);
        else if (value <= 0.83f) subWaveformLabel.setText("Square", juce::dontSendNotification);
        else subWaveformLabel.setText("Pulse 33%", juce::dontSendNotification);
        };
    subWaveformLabel.setText("Sine", juce::dontSendNotification);
    addAndMakeVisible(subWaveformLabel);

    subOscillatorLabel.setText("Sub Oscillator", juce::dontSendNotification);
    subOscillatorLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(subOscillatorLabel);

    subLevelSlider.setSliderStyle(juce::Slider::Rotary);
    subLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(subLevelSlider);
    subLevelLabel.setText("Sub Level", juce::dontSendNotification);
    addAndMakeVisible(subLevelLabel);

    detuneSlider.setSliderStyle(juce::Slider::Rotary);
    detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(detuneSlider);
    detuneLabel.setText("Detune", juce::dontSendNotification);
    addAndMakeVisible(detuneLabel);

    numVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    numVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(numVoicesSlider);
    polyphonyLabel.setText("Polyphony", juce::dontSendNotification);
    addAndMakeVisible(polyphonyLabel);

    unisonVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    unisonVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(unisonVoicesSlider);
    unisonVoicesLabel.setText("Unison Voices", juce::dontSendNotification);
    addAndMakeVisible(unisonVoicesLabel);

    unisonDetuneSlider.setSliderStyle(juce::Slider::Rotary);
    unisonDetuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(unisonDetuneSlider);
    unisonDetuneLabel.setText("Unison Detune", juce::dontSendNotification);
    addAndMakeVisible(unisonDetuneLabel);

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

    filterSectionLabel.setText("Filter", juce::dontSendNotification);
    filterSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(filterSectionLabel);

    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    addAndMakeVisible(filterCutoffLabel);

    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    addAndMakeVisible(filterResonanceLabel);

    filterADSRMixSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterADSRMixSlider);
    filterADSRMixLabel.setText("ADSR Mix", juce::dontSendNotification);
    addAndMakeVisible(filterADSRMixLabel);

    filterADSRDepthSlider.setSliderStyle(juce::Slider::Rotary);
    filterADSRDepthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(filterADSRDepthSlider);
    filterADSRDepthLabel.setText("ADSR Depth", juce::dontSendNotification);
    addAndMakeVisible(filterADSRDepthLabel);

    filterTypeCombo.addItem("Low Pass", 1);
    filterTypeCombo.addItem("High Pass", 2);
    filterTypeCombo.addItem("Band Pass", 3);
    filterTypeCombo.addItem("Notch", 4);
    filterTypeCombo.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(filterTypeCombo);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    addAndMakeVisible(filterTypeLabel);

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
    waveform3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "waveform3", waveform3Slider);
    noiseWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "noiseWaveform", noiseWaveformSlider);
    subWaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "subWaveform", subWaveformSlider);
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "detune", detuneSlider);
    osc1LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc1Level", osc1LevelSlider);
    osc2LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc2Level", osc2LevelSlider);
    osc3LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc3Level", osc3LevelSlider);
    noiseLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "noiseLevel", noiseLevelSlider);
    subLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "subLevel", subLevelSlider);
    numVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "numVoices", numVoicesSlider);
    unisonVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "unisonVoices", unisonVoicesSlider);
    unisonDetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "unisonDetune", unisonDetuneSlider);
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
    float knobWidth = 80;
    float knobHeight = 80;
    float comboHeight = 30;
    float labelHeight = 20;
    float sectionLabelHeight = 30;
    float comboWidth = 100;

    // Split the window into three columns
    auto leftColumn = bounds.removeFromLeft(bounds.getWidth() / 3 - 10);
    auto middleColumn = bounds.removeFromLeft(bounds.getWidth() / 2 - 10);
    auto rightColumn = bounds;

    // Left Column: ADSR, Oscillator 1, Oscillator 2, Oscillator 3, Noise Oscillator, Sub Oscillator, Detune, Polyphony, Unison Voices, Unison Detune, LFO
    auto adsrRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto osc1Row = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto osc2Row = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto osc3Row = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto noiseOscRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto subOscRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto detuneRow = leftColumn.removeFromTop(labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto polyphonyRow = leftColumn.removeFromTop(labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto unisonVoicesRow = leftColumn.removeFromTop(labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto unisonDetuneRow = leftColumn.removeFromTop(labelHeight + knobHeight);
    leftColumn.removeFromTop(10);
    auto lfoRow = leftColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Middle Column: Distortion, Filter, Compressor
    auto distortionRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    middleColumn.removeFromTop(10);
    auto filterRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    middleColumn.removeFromTop(10);
    auto compressorRow = middleColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Right Column: Delay, Chorus, Phaser, Flanger, Reverb
    auto delayRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(10);
    auto chorusRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(10);
    auto phaserRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(10);
    auto flangerRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);
    rightColumn.removeFromTop(10);
    auto reverbRow = rightColumn.removeFromTop(sectionLabelHeight + labelHeight + knobHeight);

    // Left Column Layout
    // ADSR Row
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

    // Oscillator 1 Row
    auto osc1Section = osc1Row.removeFromLeft(knobWidth * 2 + 10);
    oscillator1Label.setBounds(osc1Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveformBounds = osc1Section.removeFromLeft(knobWidth);
    waveformLabel.setBounds(waveformBounds.removeFromTop(labelHeight).toNearestInt());
    waveformSlider.setBounds(waveformBounds.toNearestInt());
    auto osc1LevelBounds = osc1Section.removeFromLeft(knobWidth);
    osc1LevelLabel.setBounds(osc1LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc1LevelSlider.setBounds(osc1LevelBounds.toNearestInt());

    // Oscillator 2 Row
    auto osc2Section = osc2Row.removeFromLeft(knobWidth * 2 + 10);
    oscillator2Label.setBounds(osc2Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveform2Bounds = osc2Section.removeFromLeft(knobWidth);
    waveform2Label.setBounds(waveform2Bounds.removeFromTop(labelHeight).toNearestInt());
    waveform2Slider.setBounds(waveform2Bounds.toNearestInt());
    auto osc2LevelBounds = osc2Section.removeFromLeft(knobWidth);
    osc2LevelLabel.setBounds(osc2LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc2LevelSlider.setBounds(osc2LevelBounds.toNearestInt());

    // Oscillator 3 Row
    auto osc3Section = osc3Row.removeFromLeft(knobWidth * 2 + 10);
    oscillator3Label.setBounds(osc3Section.removeFromTop(sectionLabelHeight).toNearestInt());
    auto waveform3Bounds = osc3Section.removeFromLeft(knobWidth);
    waveform3Label.setBounds(waveform3Bounds.removeFromTop(labelHeight).toNearestInt());
    waveform3Slider.setBounds(waveform3Bounds.toNearestInt());
    auto osc3LevelBounds = osc3Section.removeFromLeft(knobWidth);
    osc3LevelLabel.setBounds(osc3LevelBounds.removeFromTop(labelHeight).toNearestInt());
    osc3LevelSlider.setBounds(osc3LevelBounds.toNearestInt());

    // Noise Oscillator Row
    auto noiseOscSection = noiseOscRow.removeFromLeft(knobWidth * 2 + 10);
    noiseOscillatorLabel.setBounds(noiseOscSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto noiseWaveformBounds = noiseOscSection.removeFromLeft(knobWidth);
    noiseWaveformLabel.setBounds(noiseWaveformBounds.removeFromTop(labelHeight).toNearestInt());
    noiseWaveformSlider.setBounds(noiseWaveformBounds.toNearestInt());
    auto noiseLevelBounds = noiseOscSection.removeFromLeft(knobWidth);
    noiseLevelLabel.setBounds(noiseLevelBounds.removeFromTop(labelHeight).toNearestInt());
    noiseLevelSlider.setBounds(noiseLevelBounds.toNearestInt());

    // Sub Oscillator Row
    auto subOscSection = subOscRow.removeFromLeft(knobWidth * 2 + 10);
    subOscillatorLabel.setBounds(subOscSection.removeFromTop(sectionLabelHeight).toNearestInt());
    auto subWaveformBounds = subOscSection.removeFromLeft(knobWidth);
    subWaveformLabel.setBounds(subWaveformBounds.removeFromTop(labelHeight).toNearestInt());
    subWaveformSlider.setBounds(subWaveformBounds.toNearestInt());
    auto subLevelBounds = subOscSection.removeFromLeft(knobWidth);
    subLevelLabel.setBounds(subLevelBounds.removeFromTop(labelHeight).toNearestInt());
    subLevelSlider.setBounds(subLevelBounds.toNearestInt());

    // Detune Row
    auto detuneBounds = detuneRow.removeFromLeft(knobWidth);
    detuneLabel.setBounds(detuneBounds.removeFromTop(labelHeight).toNearestInt());
    detuneSlider.setBounds(detuneBounds.toNearestInt());

    // Polyphony Row
    auto polyphonyBounds = polyphonyRow.removeFromLeft(knobWidth);
    polyphonyLabel.setBounds(polyphonyBounds.removeFromTop(labelHeight).toNearestInt());
    numVoicesSlider.setBounds(polyphonyBounds.toNearestInt());

    // Unison Voices Row
    auto unisonVoicesBounds = unisonVoicesRow.removeFromLeft(knobWidth);
    unisonVoicesLabel.setBounds(unisonVoicesBounds.removeFromTop(labelHeight).toNearestInt());
    unisonVoicesSlider.setBounds(unisonVoicesBounds.toNearestInt());

    // Unison Detune Row
    auto unisonDetuneBounds = unisonDetuneRow.removeFromLeft(knobWidth);
    unisonDetuneLabel.setBounds(unisonDetuneBounds.removeFromTop(labelHeight).toNearestInt());
    unisonDetuneSlider.setBounds(unisonDetuneBounds.toNearestInt());

    // LFO Row
    auto lfoSection = lfoRow.removeFromLeft(knobWidth * 3 + 20);
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
    // Distortion Row
    auto distortionSection = distortionRow.removeFromLeft(knobWidth * 3 + 10);
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

    // Filter Row
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

    // Compressor Row
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
    // Delay Row
    auto delaySection = delayRow.removeFromLeft(knobWidth * 3 + 10);
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

    // Chorus Row
    auto chorusSection = chorusRow.removeFromLeft(knobWidth * 4 + 10);
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

    // Phaser Row
    auto phaserSection = phaserRow.removeFromLeft(knobWidth * 3 + 10);
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

    // Flanger Row
    auto flangerSection = flangerRow.removeFromLeft(knobWidth * 4 + 10);
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

    // Reverb Row
    auto reverbSection = reverbRow.removeFromLeft(knobWidth * 4 + 10);
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