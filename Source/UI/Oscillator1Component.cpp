#include "Oscillator1Component.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

Oscillator1Component::Oscillator1Component(SynthesizerComponent& parent,
                                           SummonerXSerum2AudioProcessor& processor,
                                           juce::LookAndFeel* customKnobLF,
                                           juce::LookAndFeel* waveButtonLF,
                                           juce::LookAndFeel* ledLabelLF,
                                           juce::LookAndFeel* ledNumberLF)
    : parentComponent(parent),
      audioProcessor(processor),
      customKnobLookAndFeel(customKnobLF),
      customWaveButtonLookAndFeel(waveButtonLF),
      ledLabelLookAndFeel(ledLabelLF),
      ledNumberLookAndFeel(ledNumberLF)
{
    setOpaque(false); // Allow parent's background to show through
    initializeOsc1Controls();
}

Oscillator1Component::~Oscillator1Component()
{
    // Clean up look and feel references
    waveTypeSineButton.setLookAndFeel(nullptr);
    waveTypeSawButton.setLookAndFeel(nullptr);
    waveTypeSquareButton.setLookAndFeel(nullptr);
    waveTypeTriangleButton.setLookAndFeel(nullptr);
    waveTypeWhiteNoiseButton.setLookAndFeel(nullptr);
    waveTypePinkNoiseButton.setLookAndFeel(nullptr);
    waveTypeRandomPhaseButton.setLookAndFeel(nullptr);

    phaseControlsPhaseKnob.setLookAndFeel(nullptr);
    phaseControlsPhaseLabel.setLookAndFeel(nullptr);

    adsrAttackKnob.setLookAndFeel(nullptr);
    adsrDecayKnob.setLookAndFeel(nullptr);
    adsrSustainKnob.setLookAndFeel(nullptr);
    adsrReleaseKnob.setLookAndFeel(nullptr);

    adsrAttackLabel.setLookAndFeel(nullptr);
    adsrDecayLabel.setLookAndFeel(nullptr);
    adsrSustainLabel.setLookAndFeel(nullptr);
    adsrReleaseLabel.setLookAndFeel(nullptr);

    pitchControlsOctaveLabel.setLookAndFeel(nullptr);
    pitchControlsOctaveValueLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneValueLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneValueLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(nullptr);
}

void Oscillator1Component::initializeOsc1Controls()
{
    // WAVE TYPE BUTTONS GROUP
    waveTypeSineButton.setButtonText("SIN");
    waveTypeSineButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeSineButton.setClickingTogglesState(true);
    waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
    waveTypeSineButton.addListener(this);
    addAndMakeVisible(waveTypeSineButton);

    waveTypeSawButton.setButtonText("SAW");
    waveTypeSawButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeSawButton.setClickingTogglesState(true);
    waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Start with saw selected
    waveTypeSawButton.addListener(this);
    addAndMakeVisible(waveTypeSawButton);

    // Set oscillator 1 to saw wave by default
    audioProcessor.setOsc1Type(1); // 1 = saw wave

    waveTypeSquareButton.setButtonText("SQR");
    waveTypeSquareButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeSquareButton.setClickingTogglesState(true);
    waveTypeSquareButton.addListener(this);
    addAndMakeVisible(waveTypeSquareButton);

    waveTypeTriangleButton.setButtonText("TRI");
    waveTypeTriangleButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeTriangleButton.setClickingTogglesState(true);
    waveTypeTriangleButton.addListener(this);
    addAndMakeVisible(waveTypeTriangleButton);

    waveTypeWhiteNoiseButton.setButtonText("WHT");
    waveTypeWhiteNoiseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeWhiteNoiseButton.setClickingTogglesState(true);
    waveTypeWhiteNoiseButton.addListener(this);
    addAndMakeVisible(waveTypeWhiteNoiseButton);

    waveTypePinkNoiseButton.setButtonText("PNK");
    waveTypePinkNoiseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypePinkNoiseButton.setClickingTogglesState(true);
    waveTypePinkNoiseButton.addListener(this);
    addAndMakeVisible(waveTypePinkNoiseButton);

    waveTypeRandomPhaseButton.setButtonText("RND PHASE");
    waveTypeRandomPhaseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    waveTypeRandomPhaseButton.setClickingTogglesState(true);
    waveTypeRandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Default to random phase ON
    waveTypeRandomPhaseButton.addListener(this);
    addAndMakeVisible(waveTypeRandomPhaseButton);

    // PITCH CONTROLS GROUP
    pitchControlsOctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    pitchControlsOctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsOctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsOctaveLabel);

    pitchControlsOctaveValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsOctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsOctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsOctaveValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    pitchControlsOctaveValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsOctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsOctaveValueLabel);

    pitchControlsSemitoneLabel.setText("SEMI", juce::dontSendNotification);
    pitchControlsSemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsSemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsSemitoneLabel);

    pitchControlsSemitoneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsSemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsSemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    pitchControlsSemitoneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsSemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsSemitoneValueLabel);

    pitchControlsFineTuneLabel.setText("FINE", juce::dontSendNotification);
    pitchControlsFineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsFineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsFineTuneLabel);

    pitchControlsFineTuneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsFineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsFineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    pitchControlsFineTuneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsFineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsFineTuneValueLabel);

    pitchControlsVoiceCountLabel.setText("VOICES", juce::dontSendNotification);
    pitchControlsVoiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsVoiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsVoiceCountLabel);

    pitchControlsVoiceCountValueLabel.setText("1", juce::dontSendNotification);
    pitchControlsVoiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    pitchControlsVoiceCountValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsVoiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsVoiceCountValueLabel);

    // PHASE CONTROLS GROUP
    phaseControlsPhaseLabel.setText("PHASE", juce::dontSendNotification);
    phaseControlsPhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    phaseControlsPhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    phaseControlsPhaseLabel.setJustificationType(juce::Justification::centred);
    phaseControlsPhaseLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(phaseControlsPhaseLabel);

    phaseControlsPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseControlsPhaseKnob.setRange(0.0, 360.0, 1.0);
    phaseControlsPhaseKnob.setValue(0.0);
    phaseControlsPhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    phaseControlsPhaseKnob.setLookAndFeel(customKnobLookAndFeel);
    phaseControlsPhaseKnob.addListener(this);
    addAndMakeVisible(phaseControlsPhaseKnob);

    // ADSR KNOBS GROUP
    adsrAttackLabel.setText("ATTACK", juce::dontSendNotification);
    adsrAttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrAttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrAttackLabel.setJustificationType(juce::Justification::centred);
    adsrAttackLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(adsrAttackLabel);

    adsrAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrAttackKnob.setRange(0.01, 2.0, 0.01);
    adsrAttackKnob.setValue(0.1);
    adsrAttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrAttackKnob.setLookAndFeel(customKnobLookAndFeel);
    adsrAttackKnob.addListener(this);
    addAndMakeVisible(adsrAttackKnob);

    adsrDecayLabel.setText("DECAY", juce::dontSendNotification);
    adsrDecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrDecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrDecayLabel.setJustificationType(juce::Justification::centred);
    adsrDecayLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(adsrDecayLabel);

    adsrDecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrDecayKnob.setRange(0.01, 2.0, 0.01);
    adsrDecayKnob.setValue(0.2);
    adsrDecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrDecayKnob.setLookAndFeel(customKnobLookAndFeel);
    adsrDecayKnob.addListener(this);
    addAndMakeVisible(adsrDecayKnob);

    adsrSustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    adsrSustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrSustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrSustainLabel.setJustificationType(juce::Justification::centred);
    adsrSustainLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(adsrSustainLabel);

    adsrSustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrSustainKnob.setRange(0.0, 1.0, 0.01);
    adsrSustainKnob.setValue(0.7);
    adsrSustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrSustainKnob.setLookAndFeel(customKnobLookAndFeel);
    adsrSustainKnob.addListener(this);
    addAndMakeVisible(adsrSustainKnob);

    adsrReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    adsrReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrReleaseLabel.setJustificationType(juce::Justification::centred);
    adsrReleaseLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(adsrReleaseLabel);

    adsrReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrReleaseKnob.setRange(0.01, 3.0, 0.01);
    adsrReleaseKnob.setValue(0.3);
    adsrReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrReleaseKnob.setLookAndFeel(customKnobLookAndFeel);
    adsrReleaseKnob.addListener(this);
    addAndMakeVisible(adsrReleaseKnob);

    // ADSR ENVELOPE VISUALIZER
    addAndMakeVisible(adsrEnvelopeVisualizer);
}

void Oscillator1Component::paint(juce::Graphics& g)
{
    // Parent component handles background drawing
}

void Oscillator1Component::resized()
{
    auto bounds = getLocalBounds();

    // Layout wave type buttons (Row 1)
    auto buttonHeight = 40;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonSection = buttonRow;

    // Store bounds for background drawing
    waveButtonsBounds = buttonSection;

    // Calculate button width and spacing to fill the section
    auto totalButtons = 6;
    auto buttonSpacing = 10;
    auto totalSpacing = (totalButtons - 1) * buttonSpacing;
    auto buttonWidth = (buttonSection.getWidth() - totalSpacing) / totalButtons;

    waveTypeSineButton.setBounds(buttonSection.getX(), buttonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSawButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing), buttonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSquareButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 2, buttonRow.getY(), buttonWidth, buttonHeight);
    waveTypeTriangleButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 3, buttonRow.getY(), buttonWidth, buttonHeight);
    waveTypeWhiteNoiseButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 4, buttonRow.getY(), buttonWidth, buttonHeight);
    waveTypePinkNoiseButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 5, buttonRow.getY(), buttonWidth, buttonHeight);

    bounds.removeFromTop(7); // spacing

    // Layout ADSR envelope visualizer (Row 2)
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection;

    // Store bounds for background drawing
    adsrEnvelopeBounds = envelopeArea;

    adsrEnvelopeVisualizer.setBounds(envelopeArea);

    bounds.removeFromTop(15); // spacing

    // Layout ADSR knobs (Row 3)
    auto controlHeight = 100;
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow;

    // Store bounds for background drawing
    adsrKnobsBounds = adsrSection;

    // Calculate smaller knob width for 4 knobs
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // 45 = 3 spacings of 15px each

    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    adsrAttackLabel.setBounds(attackArea.removeFromTop(20));
    adsrAttackKnob.setBounds(attackArea);

    adsrSection.removeFromLeft(15); // spacing

    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    adsrDecayLabel.setBounds(decayArea.removeFromTop(20));
    adsrDecayKnob.setBounds(decayArea);

    adsrSection.removeFromLeft(15); // spacing

    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    adsrSustainLabel.setBounds(sustainArea.removeFromTop(20));
    adsrSustainKnob.setBounds(sustainArea);

    adsrSection.removeFromLeft(15); // spacing

    auto releaseArea = adsrSection;
    adsrReleaseLabel.setBounds(releaseArea.removeFromTop(20));
    adsrReleaseKnob.setBounds(releaseArea);

    bounds.removeFromTop(40); // spacing

    // Layout pitch controls (Row 5 - Octave, Semitone, Fine Tune, Voice Count)
    auto bottomControlsRow = bounds.removeFromTop(80);

    auto pitchControlsSection = bottomControlsRow.removeFromLeft(320); // Fixed width for tuning controls

    // Store bounds for background drawing
    pitchControlsBounds = pitchControlsSection;

    auto workingRow = pitchControlsSection;

    // Octave control (draggable label)
    auto octaveArea = workingRow.removeFromLeft(60);
    pitchControlsOctaveLabel.setBounds(octaveArea.removeFromTop(20));
    pitchControlsOctaveValueLabel.setBounds(octaveArea.removeFromTop(30));

    workingRow.removeFromLeft(10); // spacing

    // Semitone control (draggable label)
    auto semitoneArea = workingRow.removeFromLeft(60);
    pitchControlsSemitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    pitchControlsSemitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));

    workingRow.removeFromLeft(10); // spacing

    // Fine tune control (draggable label)
    auto fineTuneArea = workingRow.removeFromLeft(60);
    pitchControlsFineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    pitchControlsFineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));

    workingRow.removeFromLeft(10); // spacing

    // Voice count control (draggable label)
    auto voiceCountArea = workingRow.removeFromLeft(60);
    pitchControlsVoiceCountLabel.setBounds(voiceCountArea.removeFromTop(20));
    pitchControlsVoiceCountValueLabel.setBounds(voiceCountArea.removeFromTop(30));

    // Layout phase controls (below pitch controls)
    bottomControlsRow.removeFromLeft(10); // spacing between pitch and phase

    auto phaseRow = bottomControlsRow.removeFromTop(80);

    // Store bounds for background drawing
    phaseControlsBounds = phaseRow.removeFromLeft(220); // Fixed width for phase controls

    auto phaseWorkingRow = phaseControlsBounds;

    // Random phase button
    auto waveTypeRandomPhaseButtonWidth = 100;
    auto randomPhaseArea = phaseWorkingRow.removeFromLeft(waveTypeRandomPhaseButtonWidth);
    waveTypeRandomPhaseButton.setBounds(randomPhaseArea.removeFromTop(40));

    phaseWorkingRow.removeFromLeft(15); // spacing

    // Phase knob
    auto phaseKnobArea = phaseWorkingRow.removeFromLeft(80);
    phaseControlsPhaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    phaseControlsPhaseKnob.setBounds(phaseKnobArea);
}

void Oscillator1Component::mouseDown(const juce::MouseEvent& event)
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

void Oscillator1Component::mouseDrag(const juce::MouseEvent& event)
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

void Oscillator1Component::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
}

void Oscillator1Component::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setOsc1Phase(static_cast<float>(phaseControlsPhaseKnob.getValue()));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(adsrAttackKnob.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(adsrDecayKnob.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(adsrReleaseKnob.getValue()));
        updateEnvelopeDisplay();
    }
}

void Oscillator1Component::buttonClicked(juce::Button* button)
{
    if (button == &waveTypeSineButton)
    {
        handleWaveButtonClick(button);
        if (waveTypeSineButton.getToggleState())
            audioProcessor.setOsc1Type(0); // 0 = sine wave
    }
    else if (button == &waveTypeSawButton)
    {
        handleWaveButtonClick(button);
        if (waveTypeSawButton.getToggleState())
            audioProcessor.setOsc1Type(1); // 1 = saw wave
    }
    else if (button == &waveTypeSquareButton)
    {
        handleWaveButtonClick(button);
        if (waveTypeSquareButton.getToggleState())
            audioProcessor.setOsc1Type(2); // 2 = square wave
    }
    else if (button == &waveTypeTriangleButton)
    {
        handleWaveButtonClick(button);
        if (waveTypeTriangleButton.getToggleState())
            audioProcessor.setOsc1Type(3); // 3 = triangle wave
    }
    else if (button == &waveTypeWhiteNoiseButton)
    {
        handleWaveButtonClick(button);
        if (waveTypeWhiteNoiseButton.getToggleState())
            audioProcessor.setOsc1Type(4); // 4 = white noise
    }
    else if (button == &waveTypePinkNoiseButton)
    {
        handleWaveButtonClick(button);
        if (waveTypePinkNoiseButton.getToggleState())
            audioProcessor.setOsc1Type(5); // 5 = pink noise
    }
    else if (button == &waveTypeRandomPhaseButton)
    {
        // Random phase button is independent - doesn't affect other buttons
        audioProcessor.setOsc1RandomPhase(waveTypeRandomPhaseButton.getToggleState());
    }
}

void Oscillator1Component::handleWaveButtonClick(juce::Button* clickedButton)
{
    // Ensure only one wave type is selected at a time
    if (clickedButton->getToggleState())
    {
        // Deselect all other wave type buttons
        if (clickedButton != &waveTypeSineButton)
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
        if (clickedButton != &waveTypeSawButton)
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
        if (clickedButton != &waveTypeSquareButton)
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
        if (clickedButton != &waveTypeTriangleButton)
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
        if (clickedButton != &waveTypeWhiteNoiseButton)
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
        if (clickedButton != &waveTypePinkNoiseButton)
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
    }
    else
    {
        // Keep at least one button selected
        clickedButton->setToggleState(true, juce::dontSendNotification);
    }
}

void Oscillator1Component::updateEnvelopeDisplay()
{
    adsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(adsrAttackKnob.getValue()),
        static_cast<float>(adsrDecayKnob.getValue()),
        static_cast<float>(adsrSustainKnob.getValue()),
        static_cast<float>(adsrReleaseKnob.getValue())
    );
    adsrEnvelopeVisualizer.repaint();
}

void Oscillator1Component::updateAllGuiControls()
{
    // Update all GUI controls to match the audio processor state
    // This would be called when loading presets, etc.

    // Update wave type buttons based on current type
    int currentType = audioProcessor.getOsc1Type();
    waveTypeSineButton.setToggleState(currentType == 0, juce::dontSendNotification);
    waveTypeSawButton.setToggleState(currentType == 1, juce::dontSendNotification);
    waveTypeSquareButton.setToggleState(currentType == 2, juce::dontSendNotification);
    waveTypeTriangleButton.setToggleState(currentType == 3, juce::dontSendNotification);
    waveTypeWhiteNoiseButton.setToggleState(currentType == 4, juce::dontSendNotification);
    waveTypePinkNoiseButton.setToggleState(currentType == 5, juce::dontSendNotification);

    // Update random phase button
    waveTypeRandomPhaseButton.setToggleState(audioProcessor.getOsc1RandomPhase(), juce::dontSendNotification);

    // Update pitch controls
    octaveValue = audioProcessor.getOsc1Octave();
    pitchControlsOctaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);

    semitoneValue = audioProcessor.getOsc1Semitone();
    pitchControlsSemitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);

    fineTuneValue = audioProcessor.getOsc1FineTune();
    pitchControlsFineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);

    voiceCountValue = audioProcessor.getOsc1VoiceCount();
    pitchControlsVoiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);

    // Update phase knob
    phaseControlsPhaseKnob.setValue(audioProcessor.getOsc1Phase(), juce::dontSendNotification);

    // Update ADSR knobs
    adsrAttackKnob.setValue(audioProcessor.getOsc1Attack(), juce::dontSendNotification);
    adsrDecayKnob.setValue(audioProcessor.getOsc1Decay(), juce::dontSendNotification);
    adsrSustainKnob.setValue(audioProcessor.getOsc1Sustain(), juce::dontSendNotification);
    adsrReleaseKnob.setValue(audioProcessor.getOsc1Release(), juce::dontSendNotification);

    // Update envelope visualizer
    updateEnvelopeDisplay();
}
