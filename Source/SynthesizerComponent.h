#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;

class SynthesizerComponent : public juce::Component, private juce::Slider::Listener
{
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider filterSlider;
    juce::Label filterLabel;
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};