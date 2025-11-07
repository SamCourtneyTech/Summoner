#include "PitchControlsComponent.h"
#include "../PluginProcessor.h"
#include "../LookAndFeel.h"

PitchControlsComponent::PitchControlsComponent(SummonerXSerum2AudioProcessor& processor,
                                               LEDLabelLookAndFeel& labelLookAndFeel,
                                               LEDNumberLookAndFeel& numberLookAndFeel)
    : audioProcessor(processor),
      ledLabelLookAndFeel(labelLookAndFeel),
      ledNumberLookAndFeel(numberLookAndFeel)
{
    // PITCH CONTROLS - Octave
    pitchControlsOctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    pitchControlsOctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsOctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsOctaveLabel);

    pitchControlsOctaveValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsOctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsOctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsOctaveValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsOctaveValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsOctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsOctaveValueLabel);

    // PITCH CONTROLS - Semitone
    pitchControlsSemitoneLabel.setText("SEMI", juce::dontSendNotification);
    pitchControlsSemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsSemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsSemitoneLabel);

    pitchControlsSemitoneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsSemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsSemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsSemitoneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsSemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsSemitoneValueLabel);

    // PITCH CONTROLS - Fine Tune
    pitchControlsFineTuneLabel.setText("FINE", juce::dontSendNotification);
    pitchControlsFineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsFineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsFineTuneLabel);

    pitchControlsFineTuneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsFineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsFineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsFineTuneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsFineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsFineTuneValueLabel);

    // PITCH CONTROLS - Voice Count
    pitchControlsVoiceCountLabel.setText("VOICES", juce::dontSendNotification);
    pitchControlsVoiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsVoiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsVoiceCountLabel);

    pitchControlsVoiceCountValueLabel.setText("1", juce::dontSendNotification);
    pitchControlsVoiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsVoiceCountValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsVoiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsVoiceCountValueLabel);
}

PitchControlsComponent::~PitchControlsComponent()
{
    // Reset look and feel for all labels
    pitchControlsOctaveLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountLabel.setLookAndFeel(nullptr);

    pitchControlsOctaveValueLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneValueLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneValueLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(nullptr);
}

void PitchControlsComponent::paint(juce::Graphics& g)
{
    // Background drawing handled by parent SynthesizerComponent
}

void PitchControlsComponent::resized()
{
    auto bounds = getLocalBounds();

    // Octave control (draggable label)
    auto octaveArea = bounds.removeFromLeft(60);
    pitchControlsOctaveLabel.setBounds(octaveArea.removeFromTop(20));
    pitchControlsOctaveValueLabel.setBounds(octaveArea.removeFromTop(30));

    bounds.removeFromLeft(10); // spacing

    // Semitone control (draggable label)
    auto semitoneArea = bounds.removeFromLeft(60);
    pitchControlsSemitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    pitchControlsSemitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));

    bounds.removeFromLeft(10); // spacing

    // Fine tune control (draggable label)
    auto fineTuneArea = bounds.removeFromLeft(60);
    pitchControlsFineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    pitchControlsFineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));

    bounds.removeFromLeft(10); // spacing

    // Voice count control (draggable label)
    auto voiceCountArea = bounds.removeFromLeft(60);
    pitchControlsVoiceCountLabel.setBounds(voiceCountArea.removeFromTop(20));
    pitchControlsVoiceCountValueLabel.setBounds(voiceCountArea.removeFromTop(30));
}

void PitchControlsComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &pitchControlsOctaveValueLabel)
    {
        isDraggingOctave = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOctave = octaveValue;
    }
    else if (event.eventComponent == &pitchControlsSemitoneValueLabel)
    {
        isDraggingSemitone = true;
        dragStartY = event.getScreenPosition().y;
        dragStartSemitone = semitoneValue;
    }
    else if (event.eventComponent == &pitchControlsFineTuneValueLabel)
    {
        isDraggingFineTune = true;
        dragStartY = event.getScreenPosition().y;
        dragStartFineTune = fineTuneValue;
    }
    else if (event.eventComponent == &pitchControlsVoiceCountValueLabel)
    {
        isDraggingVoiceCount = true;
        dragStartY = event.getScreenPosition().y;
        dragStartVoiceCount = voiceCountValue;
    }
}

void PitchControlsComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDraggingOctave)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newOctave = dragStartOctave + (deltaY / 10); // 10 pixels per octave

        // Clamp to valid range
        newOctave = juce::jlimit(-4, 4, newOctave);

        if (newOctave != octaveValue)
        {
            octaveValue = newOctave;
            pitchControlsOctaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);
            audioProcessor.setOsc1Octave(octaveValue);
        }
    }
    else if (isDraggingSemitone)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newSemitone = dragStartSemitone + (deltaY / 5); // 5 pixels per semitone

        // Clamp to valid range (-12 to +12 semitones)
        newSemitone = juce::jlimit(-12, 12, newSemitone);

        if (newSemitone != semitoneValue)
        {
            semitoneValue = newSemitone;
            pitchControlsSemitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);
            audioProcessor.setOsc1Semitone(semitoneValue);
        }
    }
    else if (isDraggingFineTune)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newFineTune = dragStartFineTune + (deltaY / 2); // 2 pixels per cent

        // Clamp to valid range (-100 to +100 cents)
        newFineTune = juce::jlimit(-100, 100, newFineTune);

        if (newFineTune != fineTuneValue)
        {
            fineTuneValue = newFineTune;
            pitchControlsFineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);
            audioProcessor.setOsc1FineTune(fineTuneValue);
        }
    }
    else if (isDraggingVoiceCount)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newVoiceCount = dragStartVoiceCount + (deltaY / 8); // 8 pixels per voice

        // Clamp to valid range (1 to 16 voices)
        newVoiceCount = juce::jlimit(1, 16, newVoiceCount);

        if (newVoiceCount != voiceCountValue)
        {
            voiceCountValue = newVoiceCount;
            pitchControlsVoiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);
            audioProcessor.setOsc1VoiceCount(voiceCountValue);
        }
    }
}

void PitchControlsComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
}

void PitchControlsComponent::updateAllGuiControls()
{
    // Update pitch control values from processor
    octaveValue = audioProcessor.getOsc1Octave();
    pitchControlsOctaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);

    semitoneValue = audioProcessor.getOsc1Semitone();
    pitchControlsSemitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);

    fineTuneValue = audioProcessor.getOsc1FineTune();
    pitchControlsFineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);

    voiceCountValue = audioProcessor.getOsc1VoiceCount();
    pitchControlsVoiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);
}
