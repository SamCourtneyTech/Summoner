#include "SynthComponent.h"

SynthComponent::SynthComponent(juce::AudioProcessorValueTreeState& p) : params(p) {
    // Existing constructor code (unchanged until LFO section)
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

    osc1OctaveSlider.setSliderStyle(juce::Slider::Rotary);
    osc1OctaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc1OctaveSlider.setRange(-4.0, 4.0, 0.01);
    addAndMakeVisible(osc1OctaveSlider);
    osc1OctaveSlider.onValueChange = [this] {
        int octave = static_cast<int>(std::round(osc1OctaveSlider.getValue()));
        osc1OctaveLabel.setText("Osc1 Oct: " + juce::String(octave >= 0 ? "+" + juce::String(octave) : juce::String(octave)), juce::dontSendNotification);
        };
    osc1OctaveLabel.setText("Osc1 Oct: 0", juce::dontSendNotification);
    addAndMakeVisible(osc1OctaveLabel);

    osc1UnisonVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    osc1UnisonVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc1UnisonVoicesSlider.setRange(1, 8, 1);
    addAndMakeVisible(osc1UnisonVoicesSlider);
    osc1UnisonVoicesLabel.setText("Osc1 Unison Voices", juce::dontSendNotification);
    addAndMakeVisible(osc1UnisonVoicesLabel);

    osc1UnisonDetuneSlider.setSliderStyle(juce::Slider::Rotary);
    osc1UnisonDetuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc1UnisonDetuneSlider);
    osc1UnisonDetuneLabel.setText("Osc1 Unison Detune", juce::dontSendNotification);
    addAndMakeVisible(osc1UnisonDetuneLabel);

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

    osc2OctaveSlider.setSliderStyle(juce::Slider::Rotary);
    osc2OctaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc2OctaveSlider.setRange(-4.0, 4.0, 0.01);
    addAndMakeVisible(osc2OctaveSlider);
    osc2OctaveSlider.onValueChange = [this] {
        int octave = static_cast<int>(std::round(osc2OctaveSlider.getValue()));
        osc2OctaveLabel.setText("Osc2 Oct: " + juce::String(octave >= 0 ? "+" + juce::String(octave) : juce::String(octave)), juce::dontSendNotification);
        };
    osc2OctaveLabel.setText("Osc2 Oct: 0", juce::dontSendNotification);
    addAndMakeVisible(osc2OctaveLabel);

    osc2UnisonVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    osc2UnisonVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc2UnisonVoicesSlider.setRange(1, 8, 1);
    addAndMakeVisible(osc2UnisonVoicesSlider);
    osc2UnisonVoicesLabel.setText("Osc2 Unison Voices", juce::dontSendNotification);
    addAndMakeVisible(osc2UnisonVoicesLabel);

    osc2UnisonDetuneSlider.setSliderStyle(juce::Slider::Rotary);
    osc2UnisonDetuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc2UnisonDetuneSlider);
    osc2UnisonDetuneLabel.setText("Osc2 Unison Detune", juce::dontSendNotification);
    addAndMakeVisible(osc2UnisonDetuneLabel);

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

    osc3OctaveSlider.setSliderStyle(juce::Slider::Rotary);
    osc3OctaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc3OctaveSlider.setRange(-4.0, 4.0, 0.01);
    addAndMakeVisible(osc3OctaveSlider);
    osc3OctaveSlider.onValueChange = [this] {
        int octave = static_cast<int>(std::round(osc3OctaveSlider.getValue()));
        osc3OctaveLabel.setText("Osc3 Oct: " + juce::String(octave >= 0 ? "+" + juce::String(octave) : juce::String(octave)), juce::dontSendNotification);
        };
    osc3OctaveLabel.setText("Osc3 Oct: 0", juce::dontSendNotification);
    addAndMakeVisible(osc3OctaveLabel);

    osc3UnisonVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    osc3UnisonVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc3UnisonVoicesSlider.setRange(1, 8, 1);
    addAndMakeVisible(osc3UnisonVoicesSlider);
    osc3UnisonVoicesLabel.setText("Osc3 Unison Voices", juce::dontSendNotification);
    addAndMakeVisible(osc3UnisonVoicesLabel);

    osc3UnisonDetuneSlider.setSliderStyle(juce::Slider::Rotary);
    osc3UnisonDetuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(osc3UnisonDetuneSlider);
    osc3UnisonDetuneLabel.setText("Osc3 Unison Detune", juce::dontSendNotification);
    addAndMakeVisible(osc3UnisonDetuneLabel);

    noiseWaveformSlider.setSliderStyle(juce::Slider::Rotary);
    noiseWaveformSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    noiseWaveformSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(noiseWaveformSlider);
    noiseWaveformSlider.onValueChange = [this] {
        float value = noiseWaveformSlider.getValue();
        noiseWaveformLabel.setText(value <= 0.5f ? "White" : "Pink", juce::dontSendNotification);
        };
    noiseWaveformLabel.setText("White", juce::dontSendNotification);
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
        else if (value <= 0.33f) subWaveformLabel.setText("Rounded", juce::dontSendNotification);
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

    subOctaveSlider.setSliderStyle(juce::Slider::Rotary);
    subOctaveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    subOctaveSlider.setRange(-4.0, 4.0, 0.01);
    addAndMakeVisible(subOctaveSlider);
    subOctaveSlider.onValueChange = [this] {
        int octave = static_cast<int>(std::round(subOctaveSlider.getValue()));
        subOctaveLabel.setText("Sub Oct: " + juce::String(octave >= 0 ? "+" + juce::String(octave) : juce::String(octave)), juce::dontSendNotification);
        };
    subOctaveLabel.setText("Sub Oct: 0", juce::dontSendNotification);
    addAndMakeVisible(subOctaveLabel);

    detuneSlider.setSliderStyle(juce::Slider::Rotary);
    detuneSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(detuneSlider);
    detuneLabel.setText("Detune", juce::dontSendNotification);
    addAndMakeVisible(detuneLabel);

    numVoicesSlider.setSliderStyle(juce::Slider::Rotary);
    numVoicesSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    numVoicesSlider.setRange(1, 16, 1);
    addAndMakeVisible(numVoicesSlider);
    polyphonyLabel.setText("Polyphony", juce::dontSendNotification);
    addAndMakeVisible(polyphonyLabel);

    lfoRateSlider.setSliderStyle(juce::Slider::Rotary);
    lfoRateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoRateSlider);
    lfoRateLabel.setText("LFO Rate", juce::dontSendNotification);
    addAndMakeVisible(lfoRateLabel);

    lfoWaveformCombo.addItem("Sine", 1);
    lfoWaveformCombo.addItem("Triangle", 2);
    lfoWaveformCombo.addItem("Saw", 3);
    lfoWaveformCombo.addItem("Square", 4);
    addAndMakeVisible(lfoWaveformCombo);
    lfoWaveformLabel.setText("LFO Waveform", juce::dontSendNotification);
    addAndMakeVisible(lfoWaveformLabel);

    lfoSectionLabel.setText("LFO", juce::dontSendNotification);
    lfoSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(lfoSectionLabel);

    // New modulation matrix components
    modMatrixSectionLabel.setText("Modulation Matrix", juce::dontSendNotification);
    modMatrixSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(modMatrixSectionLabel);

    lfoToOsc1FreqSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc1FreqSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc1FreqSlider);
    lfoToOsc1FreqLabel.setText("LFO->Osc1 Freq", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc1FreqLabel);

    lfoToOsc2FreqSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc2FreqSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc2FreqSlider);
    lfoToOsc2FreqLabel.setText("LFO->Osc2 Freq", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc2FreqLabel);

    lfoToOsc3FreqSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc3FreqSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc3FreqSlider);
    lfoToOsc3FreqLabel.setText("LFO->Osc3 Freq", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc3FreqLabel);

    lfoToOsc1LevelSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc1LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc1LevelSlider);
    lfoToOsc1LevelLabel.setText("LFO->Osc1 Level", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc1LevelLabel);

    lfoToOsc2LevelSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc2LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc2LevelSlider);
    lfoToOsc2LevelLabel.setText("LFO->Osc2 Level", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc2LevelLabel);

    lfoToOsc3LevelSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToOsc3LevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToOsc3LevelSlider);
    lfoToOsc3LevelLabel.setText("LFO->Osc3 Level", juce::dontSendNotification);
    addAndMakeVisible(lfoToOsc3LevelLabel);

    lfoToFilterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    lfoToFilterCutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(lfoToFilterCutoffSlider);
    lfoToFilterCutoffLabel.setText("LFO->Filter Cutoff", juce::dontSendNotification);
    addAndMakeVisible(lfoToFilterCutoffLabel);

    // Rest of existing constructor (unchanged)
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

    distortionSectionLabel.setText("Distortion", juce::dontSendNotification);
    distortionSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(distortionSectionLabel);

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

    delaySectionLabel.setText("Delay", juce::dontSendNotification);
    delaySectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(delaySectionLabel);

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

    chorusSectionLabel.setText("Chorus", juce::dontSendNotification);
    chorusSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(chorusSectionLabel);

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

    phaserSectionLabel.setText("Phaser", juce::dontSendNotification);
    phaserSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(phaserSectionLabel);

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

    flangerSectionLabel.setText("Flanger", juce::dontSendNotification);
    flangerSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(flangerSectionLabel);

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
    reverbWetLevelLabel.setText("Wet", juce::dontSendNotification);
    addAndMakeVisible(reverbWetLevelLabel);

    reverbDryLevelSlider.setSliderStyle(juce::Slider::Rotary);
    reverbDryLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(reverbDryLevelSlider);
    reverbDryLevelLabel.setText("Dry", juce::dontSendNotification);
    addAndMakeVisible(reverbDryLevelLabel);

    reverbSectionLabel.setText("Reverb", juce::dontSendNotification);
    reverbSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(reverbSectionLabel);

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

    filterTypeCombo.addItem("Low Pass", 1);
    filterTypeCombo.addItem("High Pass", 2);
    filterTypeCombo.addItem("Band Pass", 3);
    filterTypeCombo.addItem("Notch", 4);
    addAndMakeVisible(filterTypeCombo);
    filterTypeLabel.setText("Type", juce::dontSendNotification);
    addAndMakeVisible(filterTypeLabel);

    filterSectionLabel.setText("Filter", juce::dontSendNotification);
    filterSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(filterSectionLabel);

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

    compressorSectionLabel.setText("Compressor", juce::dontSendNotification);
    compressorSectionLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(compressorSectionLabel);

    // Existing attachments (moved to initAttachments)
}

SynthComponent::~SynthComponent() {}

void SynthComponent::initAttachments() {
    // Existing attachments
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
    osc1OctaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc1Octave", osc1OctaveSlider);
    osc2OctaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc2Octave", osc2OctaveSlider);
    osc3OctaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc3Octave", osc3OctaveSlider);
    subOctaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "subOctave", subOctaveSlider);
    numVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "numVoices", numVoicesSlider);
    osc1UnisonVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc1UnisonVoices", osc1UnisonVoicesSlider);
    osc1UnisonDetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc1UnisonDetune", osc1UnisonDetuneSlider);
    osc2UnisonVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc2UnisonVoices", osc2UnisonVoicesSlider);
    osc2UnisonDetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc2UnisonDetune", osc2UnisonDetuneSlider);
    osc3UnisonVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc3UnisonVoices", osc3UnisonVoicesSlider);
    osc3UnisonDetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "osc3UnisonDetune", osc3UnisonDetuneSlider);
    lfoRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoRate", lfoRateSlider);
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
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "filterType", filterTypeCombo);
    compressorThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorThreshold", compressorThresholdSlider);
    compressorRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorRatio", compressorRatioSlider);
    compressorAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorAttack", compressorAttackSlider);
    compressorReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorRelease", compressorReleaseSlider);
    compressorMakeupGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "compressorMakeupGain", compressorMakeupGainSlider);

    // New modulation matrix attachments
    lfoToOsc1FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc1Freq", lfoToOsc1FreqSlider);
    lfoToOsc2FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc2Freq", lfoToOsc2FreqSlider);
    lfoToOsc3FreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc3Freq", lfoToOsc3FreqSlider);
    lfoToOsc1LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc1Level", lfoToOsc1LevelSlider);
    lfoToOsc2LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc2Level", lfoToOsc2LevelSlider);
    lfoToOsc3LevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToOsc3Level", lfoToOsc3LevelSlider);
    lfoToFilterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params, "lfoToFilterCutoff", lfoToFilterCutoffSlider);
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
    float padding = 10;

    // Split into two columns: main controls on left, mod matrix on right
    auto leftColumn = bounds.withWidth(bounds.getWidth() * 0.7f);
    auto rightColumn = bounds.withLeft(leftColumn.getRight() + padding).withWidth(bounds.getWidth() * 0.25f);
    float currentYLeft = leftColumn.getY();
    float currentYRight = rightColumn.getY();

    // Helper lambda to place section label
    auto placeSectionLabel = [&](juce::Label& label, juce::Rectangle<float>& column, float& currentY) {
        label.setBounds(column.getX(), currentY, column.getWidth(), sectionLabelHeight);
        currentY += sectionLabelHeight + padding;
        };

    // Helper lambda to place a horizontal row of knobs
    auto placeKnobRow = [&](std::vector<std::pair<juce::Label*, juce::Slider*>> knobs, juce::Rectangle<float>& column, float& currentY) {
        auto rowBounds = juce::Rectangle<float>(column.getX(), currentY, column.getWidth(), labelHeight + knobHeight);
        float currentX = rowBounds.getX();
        for (auto& [label, slider] : knobs) {
            auto knobBounds = juce::Rectangle<float>(currentX, currentY, knobWidth, labelHeight + knobHeight);
            label->setBounds(knobBounds.removeFromTop(labelHeight).toNearestInt());
            slider->setBounds(knobBounds.toNearestInt());
            currentX += knobWidth + padding;
        }
        currentY += labelHeight + knobHeight + padding;
        };

    // Helper lambda to place a horizontal row with a combo box
    auto placeComboRow = [&](std::vector<std::pair<juce::Label*, juce::Slider*>> knobs, juce::Label& comboLabel, juce::ComboBox& combo, juce::Rectangle<float>& column, float& currentY) {
        auto rowBounds = juce::Rectangle<float>(column.getX(), currentY, column.getWidth(), labelHeight + std::max(knobHeight, comboHeight));
        float currentX = rowBounds.getX();
        for (auto& [label, slider] : knobs) {
            auto knobBounds = juce::Rectangle<float>(currentX, currentY, knobWidth, labelHeight + knobHeight);
            label->setBounds(knobBounds.removeFromTop(labelHeight).toNearestInt());
            slider->setBounds(knobBounds.toNearestInt());
            currentX += knobWidth + padding;
        }
        auto comboBounds = juce::Rectangle<float>(currentX, currentY, comboWidth, labelHeight + comboHeight);
        comboLabel.setBounds(comboBounds.removeFromTop(labelHeight).toNearestInt());
        combo.setBounds(comboBounds.withHeight(comboHeight).toNearestInt());
        currentY += labelHeight + std::max(knobHeight, comboHeight) + padding;
        };

    // Left Column: Main Controls
    placeSectionLabel(adsrSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&attackLabel, &attackSlider},
        {&decayLabel, &decaySlider},
        {&sustainLabel, &sustainSlider},
        {&releaseLabel, &releaseSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(oscillator1Label, leftColumn, currentYLeft);
    placeKnobRow({
        {&waveformLabel, &waveformSlider},
        {&osc1LevelLabel, &osc1LevelSlider},
        {&osc1OctaveLabel, &osc1OctaveSlider},
        {&osc1UnisonVoicesLabel, &osc1UnisonVoicesSlider},
        {&osc1UnisonDetuneLabel, &osc1UnisonDetuneSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(oscillator2Label, leftColumn, currentYLeft);
    placeKnobRow({
        {&waveform2Label, &waveform2Slider},
        {&osc2LevelLabel, &osc2LevelSlider},
        {&osc2OctaveLabel, &osc2OctaveSlider},
        {&osc2UnisonVoicesLabel, &osc2UnisonVoicesSlider},
        {&osc2UnisonDetuneLabel, &osc2UnisonDetuneSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(oscillator3Label, leftColumn, currentYLeft);
    placeKnobRow({
        {&waveform3Label, &waveform3Slider},
        {&osc3LevelLabel, &osc3LevelSlider},
        {&osc3OctaveLabel, &osc3OctaveSlider},
        {&osc3UnisonVoicesLabel, &osc3UnisonVoicesSlider},
        {&osc3UnisonDetuneLabel, &osc3UnisonDetuneSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(noiseOscillatorLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&noiseWaveformLabel, &noiseWaveformSlider},
        {&noiseLevelLabel, &noiseLevelSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(subOscillatorLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&subWaveformLabel, &subWaveformSlider},
        {&subLevelLabel, &subLevelSlider},
        {&subOctaveLabel, &subOctaveSlider}
        }, leftColumn, currentYLeft);

    placeKnobRow({
        {&detuneLabel, &detuneSlider},
        {&polyphonyLabel, &numVoicesSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(lfoSectionLabel, leftColumn, currentYLeft);
    placeComboRow({ {&lfoRateLabel, &lfoRateSlider} }, lfoWaveformLabel, lfoWaveformCombo, leftColumn, currentYLeft);

    placeSectionLabel(distortionSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&distortionDriveLabel, &distortionDriveSlider},
        {&distortionToneLabel, &distortionToneSlider},
        {&distortionMixLabel, &distortionMixSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(filterSectionLabel, leftColumn, currentYLeft);
    placeComboRow({
        {&filterCutoffLabel, &filterCutoffSlider},
        {&filterResonanceLabel, &filterResonanceSlider}
        }, filterTypeLabel, filterTypeCombo, leftColumn, currentYLeft);

    placeSectionLabel(compressorSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&compressorThresholdLabel, &compressorThresholdSlider},
        {&compressorRatioLabel, &compressorRatioSlider},
        {&compressorAttackLabel, &compressorAttackSlider},
        {&compressorReleaseLabel, &compressorReleaseSlider},
        {&compressorMakeupGainLabel, &compressorMakeupGainSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(delaySectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&delayTimeLabel, &delayTimeSlider},
        {&delayFeedbackLabel, &delayFeedbackSlider},
        {&delayMixLabel, &delayMixSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(chorusSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&chorusRateLabel, &chorusRateSlider},
        {&chorusDepthLabel, &chorusDepthSlider},
        {&chorusMixLabel, &chorusMixSlider},
        {&chorusDelayLabel, &chorusDelaySlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(phaserSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&phaserRateLabel, &phaserRateSlider},
        {&phaserDepthLabel, &phaserDepthSlider},
        {&phaserMixLabel, &phaserMixSlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(flangerSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&flangerRateLabel, &flangerRateSlider},
        {&flangerDepthLabel, &flangerDepthSlider},
        {&flangerMixLabel, &flangerMixSlider},
        {&flangerDelayLabel, &flangerDelaySlider}
        }, leftColumn, currentYLeft);

    placeSectionLabel(reverbSectionLabel, leftColumn, currentYLeft);
    placeKnobRow({
        {&reverbRoomSizeLabel, &reverbRoomSizeSlider},
        {&reverbDampingLabel, &reverbDampingSlider},
        {&reverbWetLevelLabel, &reverbWetLevelSlider},
        {&reverbDryLevelLabel, &reverbDryLevelSlider}
        }, leftColumn, currentYLeft);

    // Right Column: Modulation Matrix
    placeSectionLabel(modMatrixSectionLabel, rightColumn, currentYRight);
    placeKnobRow({
        {&lfoToOsc1FreqLabel, &lfoToOsc1FreqSlider},
        {&lfoToOsc2FreqLabel, &lfoToOsc2FreqSlider},
        {&lfoToOsc3FreqLabel, &lfoToOsc3FreqSlider}
        }, rightColumn, currentYRight);

    placeKnobRow({
        {&lfoToOsc1LevelLabel, &lfoToOsc1LevelSlider},
        {&lfoToOsc2LevelLabel, &lfoToOsc2LevelSlider},
        {&lfoToOsc3LevelLabel, &lfoToOsc3LevelSlider}
        }, rightColumn, currentYRight);

    placeKnobRow({
        {&lfoToFilterCutoffLabel, &lfoToFilterCutoffSlider}
        }, rightColumn, currentYRight);
}