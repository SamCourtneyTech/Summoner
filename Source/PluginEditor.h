#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SynthComponent.h"

class SummonerAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::KeyListener {
public:
    SummonerAudioProcessorEditor(SummonerAudioProcessor&);
    ~SummonerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) override;

private:
    SummonerAudioProcessor& audioProcessor;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
    ChatBarComponent chatBar;
    SynthComponent synthComponent;
    juce::Viewport synthViewport;

    // Map for tracking active notes
    std::map<int, int> activeNotes; // MIDI note number -> key code

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerAudioProcessorEditor)
};