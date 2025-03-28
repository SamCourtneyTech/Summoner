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

    // Existing components (unchanged)
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, waveformSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, waveformLabel;
    juce::Slider waveform2Slider;
    juce::Label waveform2Label;
    juce::Slider waveform3Slider;
    juce::Label waveform3Label;
    juce::Slider noiseWaveformSlider;
    juce::Label noiseWaveformLabel;
    juce::Slider subWaveformSlider;
    juce::Label subWaveformLabel;
    juce::Label oscillator1Label, oscillator2Label, oscillator3Label, noiseOscillatorLabel, subOscillatorLabel;
    juce::Slider detuneSlider, osc1LevelSlider, osc2LevelSlider, osc3LevelSlider, noiseLevelSlider, subLevelSlider;
    juce::Label detuneLabel, osc1LevelLabel, osc2LevelLabel, osc3LevelLabel, noiseLevelLabel, subLevelLabel;
    juce::Slider osc1OctaveSlider, osc2OctaveSlider, osc3OctaveSlider, subOctaveSlider;
    juce::Label osc1OctaveLabel, osc2OctaveLabel, osc3OctaveLabel, subOctaveLabel;
    juce::Slider numVoicesSlider;
    juce::Label polyphonyLabel;
    juce::Slider osc1UnisonVoicesSlider, osc1UnisonDetuneSlider;
    juce::Label osc1UnisonVoicesLabel, osc1UnisonDetuneLabel;
    juce::Slider osc2UnisonVoicesSlider, osc2UnisonDetuneSlider;
    juce::Label osc2UnisonVoicesLabel, osc2UnisonDetuneLabel;
    juce::Slider osc3UnisonVoicesSlider, osc3UnisonDetuneSlider;
    juce::Label osc3UnisonVoicesLabel, osc3UnisonDetuneLabel;
    juce::Slider lfoRateSlider;
    juce::ComboBox lfoWaveformCombo;
    juce::Label lfoRateLabel, lfoWaveformLabel, lfoSectionLabel;
    juce::Slider distortionDriveSlider, distortionToneSlider, distortionMixSlider;
    juce::Label distortionDriveLabel, distortionToneLabel, distortionMixLabel, distortionSectionLabel;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;
    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel, delaySectionLabel;
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider, chorusDelaySlider;
    juce::Label chorusRateLabel, chorusDepthLabel, chorusMixLabel, chorusDelayLabel, chorusSectionLabel;
    juce::Slider phaserRateSlider, phaserDepthSlider, phaserMixSlider;
    juce::Label phaserRateLabel, phaserDepthLabel, phaserMixLabel, phaserSectionLabel;
    juce::Slider flangerRateSlider, flangerDepthSlider, flangerMixSlider, flangerDelaySlider;
    juce::Label flangerRateLabel, flangerDepthLabel, flangerMixLabel, flangerDelayLabel, flangerSectionLabel;
    juce::Slider reverbRoomSizeSlider, reverbDampingSlider, reverbWetLevelSlider, reverbDryLevelSlider;
    juce::Label reverbRoomSizeLabel, reverbDampingLabel, reverbWetLevelLabel, reverbDryLevelLabel, reverbSectionLabel;
    juce::Slider filterCutoffSlider, filterResonanceSlider;
    juce::ComboBox filterTypeCombo;
    juce::Label filterCutoffLabel, filterResonanceLabel, filterTypeLabel;
    juce::Label filterSectionLabel;
    juce::Label adsrSectionLabel;
    juce::Slider compressorThresholdSlider, compressorRatioSlider, compressorAttackSlider, compressorReleaseSlider, compressorMakeupGainSlider;
    juce::Label compressorThresholdLabel, compressorRatioLabel, compressorAttackLabel, compressorReleaseLabel, compressorMakeupGainLabel, compressorSectionLabel;

    // New modulation matrix components
    juce::Label modMatrixSectionLabel;
    juce::Slider lfoToOsc1FreqSlider, lfoToOsc2FreqSlider, lfoToOsc3FreqSlider;
    juce::Slider lfoToOsc1LevelSlider, lfoToOsc2LevelSlider, lfoToOsc3LevelSlider;
    juce::Slider lfoToFilterCutoffSlider;
    juce::Label lfoToOsc1FreqLabel, lfoToOsc2FreqLabel, lfoToOsc3FreqLabel;
    juce::Label lfoToOsc1LevelLabel, lfoToOsc2LevelLabel, lfoToOsc3LevelLabel;
    juce::Label lfoToFilterCutoffLabel;

    // Existing attachments (unchanged)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveform2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveform3Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseWaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subWaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1OctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2OctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3OctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subOctaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> numVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1UnisonVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1UnisonDetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2UnisonVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2UnisonDetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3UnisonVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3UnisonDetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionToneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phaserMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbRoomSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbWetLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDryLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorRatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compressorMakeupGainAttachment;

    // New modulation matrix attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc1FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc2FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc3FreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc1LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc2LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToOsc3LevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoToFilterCutoffAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthComponent)
};