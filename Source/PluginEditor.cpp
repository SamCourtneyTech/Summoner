#include "PluginEditor.h"

SummonerAudioProcessorEditor::SummonerAudioProcessorEditor(SummonerAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    chatBar(p),
    synthComponent(p.parameters),
    octaveOffset(0)
{
    setName("SummonerAudioProcessorEditor");
    setSize(1625, 900);

    // Enable key listening
    addKeyListener(this);
    setWantsKeyboardFocus(true);

    // Set up the SynthComponent inside the Viewport
    synthViewport.setViewedComponent(&synthComponent, false);
    synthViewport.setScrollBarsShown(true, false);

    // Set the size of SynthComponent, increasing height to 3000 for more scrolling space
    const int viewportWidth = 1625 - 20;
    synthComponent.setSize(viewportWidth - synthViewport.getScrollBarThickness(), 3000);

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

    // Handle octave shifting
    if (keyCode == 'Z') {
        octaveOffset = juce::jmax(octaveOffset - 1, -5); // Lower octave, limit to 5 octaves down
        return true;
    }
    else if (keyCode == 'X') {
        octaveOffset = juce::jmin(octaveOffset + 1, 5); // Raise octave, limit to 5 octaves up
        return true;
    }

    // Map keys to MIDI notes starting from C4 (MIDI note 60) with octave offset
    switch (keyCode) {
    case 'A': midiNote = 60 + (octaveOffset * 12); break; // C
    case 'W': midiNote = 61 + (octaveOffset * 12); break; // C#
    case 'S': midiNote = 62 + (octaveOffset * 12); break; // D
    case 'E': midiNote = 63 + (octaveOffset * 12); break; // D#
    case 'D': midiNote = 64 + (octaveOffset * 12); break; // E
    case 'F': midiNote = 65 + (octaveOffset * 12); break; // F
    case 'T': midiNote = 66 + (octaveOffset * 12); break; // F#
    case 'G': midiNote = 67 + (octaveOffset * 12); break; // G
    case 'Y': midiNote = 68 + (octaveOffset * 12); break; // G#
    case 'H': midiNote = 69 + (octaveOffset * 12); break; // A
    case 'U': midiNote = 70 + (octaveOffset * 12); break; // A#
    case 'J': midiNote = 71 + (octaveOffset * 12); break; // B
    case 'K': midiNote = 72 + (octaveOffset * 12); break; // C (next octave)
    default: return false;
    }

    // Ensure MIDI note is within valid range (0 to 127)
    midiNote = juce::jlimit(0, 127, midiNote);

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