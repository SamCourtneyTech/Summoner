#include "PluginEditor.h"

SummonerAudioProcessorEditor::SummonerAudioProcessorEditor(SummonerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    chatBar(p),
    synthComponent(p.parameters)
{
    setName("SummonerAudioProcessorEditor");
    setSize(1625, 900);

    // Enable key listening
    addKeyListener(this);
    setWantsKeyboardFocus(true);

    // Set up the SynthComponent inside the Viewport
    synthViewport.setViewedComponent(&synthComponent, false);
    synthViewport.setScrollBarsShown(true, false);

    // Set the size of SynthComponent
    const int viewportWidth = 1625 - 20;
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

SummonerAudioProcessorEditor::~SummonerAudioProcessorEditor() {
    removeKeyListener(this);
}

void SummonerAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f));
    g.drawFittedText("Summoner", getLocalBounds(), juce::Justification::centredTop, 1);
}

void SummonerAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    tabs.setBounds(bounds);

    // Set the Viewport bounds
    auto tabBarHeight = tabs.getTabBarDepth();
    auto contentBounds = bounds.withTrimmedTop(tabBarHeight).reduced(10);
    synthViewport.setBounds(contentBounds);
}

bool SummonerAudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) {
    int keyCode = key.getKeyCode();
    int midiNote = -1;

    // Map keys to MIDI notes starting from C4 (MIDI note 60)
    switch (keyCode) {
    case 'A': midiNote = 60; break; // C4
    case 'W': midiNote = 61; break; // C#4
    case 'S': midiNote = 62; break; // D4
    case 'E': midiNote = 63; break; // D#4
    case 'D': midiNote = 64; break; // E4
    case 'F': midiNote = 65; break; // F4
    case 'T': midiNote = 66; break; // F#4
    case 'G': midiNote = 67; break; // G4
    case 'Y': midiNote = 68; break; // G#4
    case 'H': midiNote = 69; break; // A4
    case 'U': midiNote = 70; break; // A#4
    case 'J': midiNote = 71; break; // B4
    case 'K': midiNote = 72; break; // C5
    default: return false;
    }

    if (midiNote != -1 && activeNotes.find(midiNote) == activeNotes.end()) {
        // Send note on
        juce::MidiBuffer midiBuffer;
        auto message = juce::MidiMessage::noteOn(1, midiNote, 0.8f);
        midiBuffer.addEvent(message, 0);
        audioProcessor.processBlock(juce::AudioBuffer<float>(2, 1), midiBuffer);
        activeNotes[midiNote] = keyCode;
    }

    return true;
}

bool SummonerAudioProcessorEditor::keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) {
    if (!isKeyDown) {
        for (auto it = activeNotes.begin(); it != activeNotes.end();) {
            int midiNote = it->first;
            int keyCode = it->second;
            if (!juce::KeyPress::isKeyCurrentlyDown(keyCode)) {
                // Send note off
                juce::MidiBuffer midiBuffer;
                auto message = juce::MidiMessage::noteOff(1, midiNote);
                midiBuffer.addEvent(message, 0);
                audioProcessor.processBlock(juce::AudioBuffer<float>(2, 1), midiBuffer);
                it = activeNotes.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    return true;
}