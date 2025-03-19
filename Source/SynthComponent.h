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
    juce::Slider detuneSlider, osc1LevelSlider, osc2LevelSlider; // Replaced oscMixSlider with osc1LevelSlider and osc2LevelSlider
    juce::Label detuneLabel, osc1LevelLabel, osc2LevelLabel; // Replaced oscMixLabel with osc1LevelLabel and osc2LevelLabel

    // Filter controls
    juce::Slider filterCutoffSlider, filterResonanceSlider, filterADSRMixSlider, filterADSRDepthSlider;
    juce::ComboBox filterTypeCombo;
    juce::Label filterCutoffLabel, filterResonanceLabel, filterADSRMixLabel, filterADSRDepthLabel, filterTypeLabel;

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
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1LevelAttachment; // New
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2LevelAttachment; // New

    // Filter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterADSRMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterADSRDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthComponent)
};