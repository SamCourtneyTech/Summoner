#pragma once
#include <JuceHeader.h>

class SynthComponent : public juce::Component {
public:
    SynthComponent(juce::AudioProcessorValueTreeState& params);
    ~SynthComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void initAttachments();

private:
    juce::AudioProcessorValueTreeState& params;

    // Existing sliders and labels
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, waveformSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, waveformLabel;

    // Second oscillator
    juce::Slider waveform2Slider;
    juce::Label waveform2Label;

    // Section labels for oscillators
    juce::Label oscillator1Label, oscillator2Label;

    // Detune and level controls
    juce::Slider detuneSlider, osc1LevelSlider, osc2LevelSlider;
    juce::Label detuneLabel, osc1LevelLabel, osc2LevelLabel;

    // LFO controls
    juce::Slider lfoRateSlider, lfoDepthSlider;
    juce::ComboBox lfoWaveformCombo;
    juce::Label lfoRateLabel, lfoDepthLabel, lfoWaveformLabel, lfoSectionLabel;

    // Distortion controls
    juce::Slider distortionDriveSlider, distortionToneSlider, distortionMixSlider;
    juce::Label distortionDriveLabel, distortionToneLabel, distortionMixLabel, distortionSectionLabel;

    // Delay controls
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;
    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel, delaySectionLabel;

    // Chorus controls
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider, chorusDelaySlider;
    juce::Label chorusRateLabel, chorusDepthLabel, chorusMixLabel, chorusDelayLabel, chorusSectionLabel;

    // Phaser controls
    juce::Slider phaserRateSlider, phaserDepthSlider, phaserMixSlider;
    juce::Label phaserRateLabel, phaserDepthLabel, phaserMixLabel, phaserSectionLabel;

    // Flanger controls
    juce::Slider flangerRateSlider, flangerDepthSlider, flangerMixSlider, flangerDelaySlider;
    juce::Label flangerRateLabel, flangerDepthLabel, flangerMixLabel, flangerDelayLabel, flangerSectionLabel;

    // Reverb controls
    juce::Slider reverbRoomSizeSlider, reverbDampingSlider, reverbWetLevelSlider, reverbDryLevelSlider;
    juce::Label reverbRoomSizeLabel, reverbDampingLabel, reverbWetLevelLabel, reverbDryLevelLabel, reverbSectionLabel;

    // Filter controls
    juce::Slider filterCutoffSlider, filterResonanceSlider, filterADSRMixSlider, filterADSRDepthSlider;
    juce::ComboBox filterTypeCombo;
    juce::Label filterCutoffLabel, filterResonanceLabel, filterADSRMixLabel, filterADSRDepthLabel, filterTypeLabel;
    juce::Label filterSectionLabel;

    // ADSR section label
    juce::Label adsrSectionLabel;

    // Compressor controls
    juce::Slider compressorThresholdSlider, compressorRatioSlider, compressorAttackSlider, compressorReleaseSlider, compressorMakeupGainSlider;
    juce::Label compressorThresholdLabel, compressorRatioLabel, compressorAttackLabel, compressorReleaseLabel, compressorMakeupGainLabel, compressorSectionLabel;

    // New controls for number of voices
    juce::Slider numVoicesSlider;
    juce::Label numVoicesLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> numVoicesAttachment;

    // Existing attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment;

    // Second oscillator attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveform2Attachment;

    // Attachments for detune and levels
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2LevelAttachment;

    // LFO attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;

    // Distortion attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionToneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionMixAttachment;

    // Delay attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayMixAttachment;

    // Chorus attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDelayAttachment;

    // Phaser attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserMixAttachment;

    // Flanger attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDelayAttachment;

    // Reverb attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbRoomSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbWetLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDryLevelAttachment;

    // Filter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterADSRMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterADSRDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;

    // Compressor attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorRatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorMakeupGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthComponent)
};