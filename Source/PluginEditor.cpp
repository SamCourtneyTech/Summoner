#include "PluginEditor.h"

SummonerAudioProcessorEditor::SummonerAudioProcessorEditor(SummonerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    chatBar(p),
    synthComponent(p.parameters)
{
    setName("SummonerAudioProcessorEditor");
    setSize(1625, 900);
    tabs.addTab("ChatGPT", juce::Colours::transparentBlack, &chatBar, false);
    tabs.addTab("Synth", juce::Colours::transparentBlack, &synthComponent, false);
    addAndMakeVisible(tabs);

    // Initialize attachments after construction
    synthComponent.initAttachments();
}

SummonerAudioProcessorEditor::~SummonerAudioProcessorEditor() {}

void SummonerAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("Summoner", getLocalBounds(), juce::Justification::centred, 1);
}

void SummonerAudioProcessorEditor::resized() {
    tabs.setBounds(getLocalBounds());
}