#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SynthComponent.h"

class SummonerAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    SummonerAudioProcessorEditor(SummonerAudioProcessor&);
    ~SummonerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SummonerAudioProcessor& audioProcessor;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
    ChatBarComponent chatBar;
    SynthComponent synthComponent;
    juce::Viewport synthViewport; // Add a viewport for the SynthComponent

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerAudioProcessorEditor)
};