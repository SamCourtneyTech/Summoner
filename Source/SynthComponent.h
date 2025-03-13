#pragma once
#include <JuceHeader.h>

class SynthComponent : public juce::Component {
public:
    SynthComponent(juce::AudioProcessorValueTreeState& params);
    ~SynthComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void initAttachments(); // New method to initialize attachments

private:
    juce::AudioProcessorValueTreeState& params; // Store reference to params
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, waveformSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveformAttachment;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, waveformLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthComponent)
};