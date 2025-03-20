#include "PluginEditor.h"

SummonerAudioProcessorEditor::SummonerAudioProcessorEditor(SummonerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    chatBar(p),
    synthComponent(p.parameters)
{
    setName("SummonerAudioProcessorEditor");
    setSize(1625, 900);

    // Set up the SynthComponent inside the Viewport
    synthViewport.setViewedComponent(&synthComponent, false);
    synthViewport.setScrollBarsShown(true, false); // Show vertical scrollbar, hide horizontal

    // Set the size of SynthComponent (width matches the editor, height is large for scrolling)
    const int viewportWidth = 1625 - 20; // Account for some padding
    synthComponent.setSize(viewportWidth - synthViewport.getScrollBarThickness(), 2000);

    // Add the Viewport to the editor
    addAndMakeVisible(synthViewport);

    // Add tabs
    tabs.addTab("ChatGPT", juce::Colours::transparentBlack, &chatBar, false);
    tabs.addTab("Synth", juce::Colours::transparentBlack, &synthViewport, false);
    addAndMakeVisible(tabs);

    // Initialize attachments after construction
    synthComponent.initAttachments();
}

SummonerAudioProcessorEditor::~SummonerAudioProcessorEditor() {}

void SummonerAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("Summoner", getLocalBounds(), juce::Justification::centredTop, 1);
}

void SummonerAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    tabs.setBounds(bounds);

    // Set the Viewport bounds to match the content area of the Synth tab
    // We use the tab's content area, accounting for the tab bar height
    auto tabBarHeight = tabs.getTabBarDepth();
    auto contentBounds = bounds.withTrimmedTop(tabBarHeight).reduced(10); // Add some padding
    synthViewport.setBounds(contentBounds);
}