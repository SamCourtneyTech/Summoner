#include "SecondOscillatorComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

SecondOscillatorComponent::SecondOscillatorComponent(SynthesizerComponent& parent,
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
    initializeOsc2Controls();
}

SecondOscillatorComponent::~SecondOscillatorComponent()
{
    // Clean up look and feel references
    osc2SineButton.setLookAndFeel(nullptr);
    osc2SawButton.setLookAndFeel(nullptr);
    osc2SquareButton.setLookAndFeel(nullptr);
    osc2TriangleButton.setLookAndFeel(nullptr);
    osc2WhiteNoiseButton.setLookAndFeel(nullptr);
    osc2PinkNoiseButton.setLookAndFeel(nullptr);
    osc2RandomPhaseButton.setLookAndFeel(nullptr);

    osc2VolumeKnob.setLookAndFeel(nullptr);
    osc2DetuneKnob.setLookAndFeel(nullptr);
    osc2StereoKnob.setLookAndFeel(nullptr);
    osc2PanKnob.setLookAndFeel(nullptr);
    osc2PhaseKnob.setLookAndFeel(nullptr);

    osc2AttackKnob.setLookAndFeel(nullptr);
    osc2DecayKnob.setLookAndFeel(nullptr);
    osc2SustainKnob.setLookAndFeel(nullptr);
    osc2ReleaseKnob.setLookAndFeel(nullptr);

    osc2VolumeLabel.setLookAndFeel(nullptr);
    osc2DetuneLabel.setLookAndFeel(nullptr);
    osc2StereoLabel.setLookAndFeel(nullptr);
    osc2PanLabel.setLookAndFeel(nullptr);
    osc2PhaseLabel.setLookAndFeel(nullptr);

    osc2AttackLabel.setLookAndFeel(nullptr);
    osc2DecayLabel.setLookAndFeel(nullptr);
    osc2SustainLabel.setLookAndFeel(nullptr);
    osc2ReleaseLabel.setLookAndFeel(nullptr);

    osc2OctaveLabel.setLookAndFeel(nullptr);
    osc2OctaveValueLabel.setLookAndFeel(nullptr);
    osc2SemitoneLabel.setLookAndFeel(nullptr);
    osc2SemitoneValueLabel.setLookAndFeel(nullptr);
    osc2FineTuneLabel.setLookAndFeel(nullptr);
    osc2FineTuneValueLabel.setLookAndFeel(nullptr);
    osc2VoicesLabel.setLookAndFeel(nullptr);
    osc2VoicesValueLabel.setLookAndFeel(nullptr);

    osc2AdsrLinkButton.setLookAndFeel(nullptr);
}

void SecondOscillatorComponent::initializeOsc2Controls()
{
    // SECOND OSCILLATOR CONTROLS
    osc2SineButton.setButtonText("SIN");
    osc2SineButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2SineButton.setClickingTogglesState(true);
    osc2SineButton.setToggleState(false, juce::dontSendNotification);
    osc2SineButton.addListener(this);
    addAndMakeVisible(osc2SineButton);

    osc2VoicesLabel.setText("VOICES", juce::dontSendNotification);
    osc2VoicesLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VoicesLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2VoicesLabel);

    osc2VoicesValueLabel.setText("1", juce::dontSendNotification);
    osc2VoicesValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2VoicesValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesValueLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    osc2VoicesValueLabel.setInterceptsMouseClicks(true, false);
    osc2VoicesValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2VoicesValueLabel);

    osc2PanLabel.setText("PAN", juce::dontSendNotification);
    osc2PanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PanLabel.setJustificationType(juce::Justification::centred);
    osc2PanLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2PanLabel);

    osc2PanKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    osc2PanKnob.setRange(-1.0, 1.0, 0.01);
    osc2PanKnob.setValue(0.0);
    osc2PanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2PanKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2PanKnob.addListener(this);
    addAndMakeVisible(osc2PanKnob);

    osc2OctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    osc2OctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2OctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2OctaveLabel);

    osc2OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc2OctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2OctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    osc2OctaveValueLabel.setInterceptsMouseClicks(true, false);
    osc2OctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2OctaveValueLabel);

    osc2SemitoneLabel.setText("SEMI", juce::dontSendNotification);
    osc2SemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2SemitoneLabel);

    osc2SemitoneValueLabel.setText("0", juce::dontSendNotification);
    osc2SemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2SemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneValueLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    osc2SemitoneValueLabel.setInterceptsMouseClicks(true, false);
    osc2SemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2SemitoneValueLabel);

    osc2FineTuneLabel.setText("FINE", juce::dontSendNotification);
    osc2FineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2FineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2FineTuneLabel);

    osc2FineTuneValueLabel.setText("0", juce::dontSendNotification);
    osc2FineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2FineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneValueLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneValueLabel.setLookAndFeel(ledNumberLookAndFeel);
    osc2FineTuneValueLabel.setInterceptsMouseClicks(true, false);
    osc2FineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2FineTuneValueLabel);

    osc2RandomPhaseButton.setButtonText("RND PHASE");
    osc2RandomPhaseButton.setClickingTogglesState(true);
    osc2RandomPhaseButton.setToggleState(true, juce::dontSendNotification);
    osc2RandomPhaseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2RandomPhaseButton.addListener(this);
    addAndMakeVisible(osc2RandomPhaseButton);

    osc2PhaseLabel.setText("PHASE", juce::dontSendNotification);
    osc2PhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PhaseLabel.setJustificationType(juce::Justification::centred);
    osc2PhaseLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2PhaseLabel);

    osc2PhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2PhaseKnob.setRange(0.0, 360.0, 1.0);
    osc2PhaseKnob.setValue(0.0);
    osc2PhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    osc2PhaseKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2PhaseKnob.addListener(this);
    addAndMakeVisible(osc2PhaseKnob);

    // Set oscillator 2 to saw wave and enable it by default
    audioProcessor.setOsc2Enabled(true);
    audioProcessor.setOsc2Type(1); // 1 = saw wave

    osc2SawButton.setButtonText("SAW");
    osc2SawButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2SawButton.setClickingTogglesState(true);
    osc2SawButton.setToggleState(true, juce::dontSendNotification);
    osc2SawButton.addListener(this);
    addAndMakeVisible(osc2SawButton);

    osc2SquareButton.setButtonText("SQR");
    osc2SquareButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2SquareButton.setClickingTogglesState(true);
    osc2SquareButton.setToggleState(false, juce::dontSendNotification);
    osc2SquareButton.addListener(this);
    addAndMakeVisible(osc2SquareButton);

    osc2TriangleButton.setButtonText("TRI");
    osc2TriangleButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2TriangleButton.setClickingTogglesState(true);
    osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
    osc2TriangleButton.addListener(this);
    addAndMakeVisible(osc2TriangleButton);

    osc2WhiteNoiseButton.setButtonText("WHT");
    osc2WhiteNoiseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2WhiteNoiseButton.setClickingTogglesState(true);
    osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
    osc2WhiteNoiseButton.addListener(this);
    addAndMakeVisible(osc2WhiteNoiseButton);

    osc2PinkNoiseButton.setButtonText("PNK");
    osc2PinkNoiseButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2PinkNoiseButton.setClickingTogglesState(true);
    osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
    osc2PinkNoiseButton.addListener(this);
    addAndMakeVisible(osc2PinkNoiseButton);

    osc2VolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2VolumeKnob.setRange(0.0, 1.0, 0.01);
    osc2VolumeKnob.setValue(0.0);
    osc2VolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2VolumeKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2VolumeKnob.addListener(this);
    addAndMakeVisible(osc2VolumeKnob);

    osc2DetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DetuneKnob.setRange(0.0, 1.0, 0.01);
    osc2DetuneKnob.setValue(0.0);
    osc2DetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DetuneKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2DetuneKnob.addListener(this);
    addAndMakeVisible(osc2DetuneKnob);

    osc2StereoKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2StereoKnob.setRange(0.0, 1.0, 0.01);
    osc2StereoKnob.setValue(0.5);
    osc2StereoKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2StereoKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2StereoKnob.addListener(this);
    addAndMakeVisible(osc2StereoKnob);

    osc2VolumeLabel.setText("VOLUME", juce::dontSendNotification);
    osc2VolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VolumeLabel.setJustificationType(juce::Justification::centred);
    osc2VolumeLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2VolumeLabel);

    osc2DetuneLabel.setText("DETUNE", juce::dontSendNotification);
    osc2DetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DetuneLabel.setJustificationType(juce::Justification::centred);
    osc2DetuneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2DetuneLabel);

    osc2StereoLabel.setText("STEREO", juce::dontSendNotification);
    osc2StereoLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2StereoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2StereoLabel.setJustificationType(juce::Justification::centred);
    osc2StereoLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2StereoLabel);

    // Oscillator 2 ADSR controls
    osc2AttackLabel.setText("ATTACK", juce::dontSendNotification);
    osc2AttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2AttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2AttackLabel.setJustificationType(juce::Justification::centred);
    osc2AttackLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2AttackLabel);

    osc2AttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2AttackKnob.setRange(0.01, 2.0, 0.01);
    osc2AttackKnob.setValue(0.1);
    osc2AttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2AttackKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2AttackKnob.addListener(this);
    addAndMakeVisible(osc2AttackKnob);

    osc2DecayLabel.setText("DECAY", juce::dontSendNotification);
    osc2DecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DecayLabel.setJustificationType(juce::Justification::centred);
    osc2DecayLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2DecayLabel);

    osc2DecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DecayKnob.setRange(0.01, 2.0, 0.01);
    osc2DecayKnob.setValue(0.2);
    osc2DecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DecayKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2DecayKnob.addListener(this);
    addAndMakeVisible(osc2DecayKnob);

    osc2SustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    osc2SustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SustainLabel.setJustificationType(juce::Justification::centred);
    osc2SustainLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2SustainLabel);

    osc2SustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2SustainKnob.setRange(0.0, 1.0, 0.01);
    osc2SustainKnob.setValue(0.7);
    osc2SustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2SustainKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2SustainKnob.addListener(this);
    addAndMakeVisible(osc2SustainKnob);

    osc2ReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    osc2ReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2ReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2ReleaseLabel.setJustificationType(juce::Justification::centred);
    osc2ReleaseLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(osc2ReleaseLabel);

    osc2ReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2ReleaseKnob.setRange(0.01, 3.0, 0.01);
    osc2ReleaseKnob.setValue(0.3);
    osc2ReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2ReleaseKnob.setLookAndFeel(customKnobLookAndFeel);
    osc2ReleaseKnob.addListener(this);
    addAndMakeVisible(osc2ReleaseKnob);

    osc2AdsrLinkButton.setButtonText("OSC1 ADSR");
    osc2AdsrLinkButton.setClickingTogglesState(true);
    osc2AdsrLinkButton.setToggleState(false, juce::dontSendNotification);
    osc2AdsrLinkButton.setLookAndFeel(customWaveButtonLookAndFeel);
    osc2AdsrLinkButton.addListener(this);
    addAndMakeVisible(osc2AdsrLinkButton);

    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    addAndMakeVisible(osc2AdsrEnvelopeVisualizer);
}

void SecondOscillatorComponent::paint(juce::Graphics& g)
{
    // Background drawing is handled by parent component
}

void SecondOscillatorComponent::resized()
{
    auto bounds = getLocalBounds();

    // Layout similar to oscillator 1 - organized in rows
    auto workingArea = bounds;

    // Title label (emblem)
    auto titleHeight = 20;
    auto titleArea = workingArea.removeFromTop(titleHeight);
    osc2TitleLabel.setBounds(titleArea);

    workingArea.removeFromTop(15);

    // Wave type buttons row
    auto buttonHeight = 40;
    auto waveButtonsRow = workingArea.removeFromTop(buttonHeight);
    auto buttonWidth = 55;
    auto screwInset = 15;
    auto buttonStartX = 17;

    auto availableWidth = waveButtonsRow.getWidth() - buttonStartX - screwInset - buttonWidth + 8;
    auto totalButtonWidth = 6 * buttonWidth;
    auto totalSpacingWidth = availableWidth - totalButtonWidth + buttonWidth;
    auto buttonSpacing = totalSpacingWidth / 5;

    auto buttonArea = waveButtonsRow.withX(waveButtonsRow.getX() + buttonStartX);

    osc2SineButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);

    osc2SawButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);

    osc2SquareButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);

    osc2TriangleButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);

    osc2WhiteNoiseButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);

    osc2PinkNoiseButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);

    workingArea.removeFromTop(5);

    // ADSR envelope visualizer row
    auto envelopeHeight = 60;
    auto osc2EnvelopeArea = workingArea.removeFromTop(envelopeHeight);
    auto originalBounds = parentComponent.getLocalBounds();
    originalBounds.reduce(20, 20);
    auto envelopeWidth = originalBounds.getWidth() / 3;
    auto centeredEnvelopeArea = osc2EnvelopeArea.withSizeKeepingCentre(envelopeWidth, envelopeHeight);
    osc2AdsrEnvelopeVisualizer.setBounds(centeredEnvelopeArea.translated(2, 1));

    workingArea.removeFromTop(5);

    // Apply offset to all elements below ADSR visualizer
    auto elementsOffset = juce::Point<int>(-6, 12);
    workingArea = workingArea.translated(elementsOffset.x, elementsOffset.y);

    // ADSR knobs row
    auto knobHeight = 80;
    auto knobLabelHeight = 20;
    auto adsrKnobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);
    auto knobWidth = 80;
    auto knobSpacing = 15;

    auto totalKnobWidth = 4 * knobWidth + 3 * knobSpacing + 12;
    auto knobStartX = (adsrKnobsRow.getWidth() - totalKnobWidth) / 2 + 6;
    auto knobArea = adsrKnobsRow.withX(adsrKnobsRow.getX() + knobStartX).withWidth(totalKnobWidth);
    auto stretchedSpacing = knobSpacing + (12.0f / 3.0f);

    // Attack knob
    auto attackArea = knobArea.removeFromLeft(knobWidth);
    osc2AttackLabel.setBounds(attackArea.removeFromTop(knobLabelHeight));
    osc2AttackKnob.setBounds(attackArea);
    knobArea.removeFromLeft(stretchedSpacing);

    // Decay knob
    auto decayArea = knobArea.removeFromLeft(knobWidth);
    osc2DecayLabel.setBounds(decayArea.removeFromTop(knobLabelHeight));
    osc2DecayKnob.setBounds(decayArea);
    knobArea.removeFromLeft(stretchedSpacing);

    // Sustain knob
    auto sustainArea = knobArea.removeFromLeft(knobWidth);
    osc2SustainLabel.setBounds(sustainArea.removeFromTop(knobLabelHeight));
    osc2SustainKnob.setBounds(sustainArea);
    knobArea.removeFromLeft(stretchedSpacing);

    // Release knob
    auto releaseArea = knobArea.removeFromLeft(knobWidth);
    osc2ReleaseLabel.setBounds(releaseArea.removeFromTop(knobLabelHeight));
    osc2ReleaseKnob.setBounds(releaseArea);

    // Position ADSR Link button - needs access to parent's volumeKnobsBounds
    // This will be positioned by parent since it's relative to osc1 position

    // First additional row - knobs (volume, detune, stereo, pan)
    workingArea.removeFromTop(20);
    auto knobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);

    auto totalKnobsWidth = 4 * knobWidth + 3 * knobSpacing + 12;
    auto knobsStartX = (knobsRow.getWidth() - totalKnobsWidth) / 2 + 6;
    auto knobsArea = knobsRow.withX(knobsRow.getX() + knobsStartX).withWidth(totalKnobsWidth);
    auto stretchedKnobSpacing = knobSpacing + (12.0f / 3.0f);

    // Volume knob
    auto volumeArea = knobsArea.removeFromLeft(knobWidth);
    osc2VolumeLabel.setBounds(volumeArea.removeFromTop(knobLabelHeight));
    osc2VolumeKnob.setBounds(volumeArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);

    // Detune knob
    auto detuneArea = knobsArea.removeFromLeft(knobWidth);
    osc2DetuneLabel.setBounds(detuneArea.removeFromTop(knobLabelHeight));
    osc2DetuneKnob.setBounds(detuneArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);

    // Stereo knob
    auto stereoArea = knobsArea.removeFromLeft(knobWidth);
    osc2StereoLabel.setBounds(stereoArea.removeFromTop(knobLabelHeight));
    osc2StereoKnob.setBounds(stereoArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);

    // Pan knob
    auto panArea = knobsArea.removeFromLeft(knobWidth);
    osc2PanLabel.setBounds(panArea.removeFromTop(knobLabelHeight));
    osc2PanKnob.setBounds(panArea);

    workingArea.removeFromTop(33);

    // Second additional row - pitch controls (octave, semitone, fine tune, voices)
    auto pitchControlsHeight = 50;
    auto pitchControlsRow = workingArea.removeFromTop(pitchControlsHeight);

    // Store bounds for background drawing
    osc2PitchControlsBounds = pitchControlsRow;

    auto pitchControlWidth = 60;
    auto pitchControlSpacing = 10;
    auto totalPitchControlsWidth = 4 * pitchControlWidth + 3 * pitchControlSpacing;
    auto pitchControlsStartX = (pitchControlsRow.getWidth() - totalPitchControlsWidth) / 2 - 36;
    auto pitchControlsArea = pitchControlsRow.withX(pitchControlsRow.getX() + pitchControlsStartX).withWidth(totalPitchControlsWidth);

    // Octave control
    auto octaveArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2OctaveLabel.setBounds(octaveArea.removeFromTop(20));
    osc2OctaveValueLabel.setBounds(octaveArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);

    // Semitone control
    auto semitoneArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2SemitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    osc2SemitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);

    // Fine tune control
    auto fineTuneArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2FineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    osc2FineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);

    // Voices control
    auto voicesArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2VoicesLabel.setBounds(voicesArea.removeFromTop(20));
    osc2VoicesValueLabel.setBounds(voicesArea.removeFromTop(30));

    workingArea.removeFromTop(5);

    // Random phase button and phase knob row
    auto phaseControlsRow = workingArea.removeFromTop(100);

    // Store bounds for background drawing
    osc2PhaseControlsBounds = phaseControlsRow;

    auto randomPhaseButtonWidth = 100;
    auto phaseControlsWidth = randomPhaseButtonWidth + knobSpacing + knobWidth;
    auto phaseControlsStartX = (phaseControlsRow.getWidth() - phaseControlsWidth) / 2 - 70;
    auto phaseControlsArea = phaseControlsRow.withX(phaseControlsRow.getX() + phaseControlsStartX).withWidth(phaseControlsWidth);

    // Random phase button (left side)
    auto randomPhaseButtonArea = phaseControlsArea.removeFromLeft(randomPhaseButtonWidth);
    auto randomPhaseButtonHeight = 40;
    auto buttonVerticalOffset = (phaseControlsRow.getHeight() - randomPhaseButtonHeight) / 2 - 3;
    randomPhaseButtonArea = randomPhaseButtonArea.withY(randomPhaseButtonArea.getY() + buttonVerticalOffset).withHeight(randomPhaseButtonHeight);
    randomPhaseButtonArea = randomPhaseButtonArea.translated(-2, 0);
    osc2RandomPhaseButton.setBounds(randomPhaseButtonArea);

    phaseControlsArea.removeFromLeft(knobSpacing);

    // Phase knob (right side)
    auto phaseKnobArea = phaseControlsArea.removeFromLeft(knobWidth);
    auto phaseLabelArea = phaseKnobArea.removeFromTop(knobLabelHeight);
    osc2PhaseLabel.setBounds(phaseLabelArea.translated(0, 31)); // Move label down 31 pixels to match original

    // Match oscillator 1 phase knob size: 80 width × 60 height, move down 33px
    auto matchingKnobArea = juce::Rectangle<int>(phaseKnobArea.getX(), phaseKnobArea.getY() + 33, 80, 60);
    osc2PhaseKnob.setBounds(matchingKnobArea);
}

void SecondOscillatorComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &osc2VolumeKnob)
    {
        audioProcessor.setOsc2Volume(slider->getValue());
    }
    else if (slider == &osc2DetuneKnob)
    {
        audioProcessor.setOsc2Detune(slider->getValue());
    }
    else if (slider == &osc2StereoKnob)
    {
        audioProcessor.setOsc2Stereo(slider->getValue());
    }
    else if (slider == &osc2PanKnob)
    {
        audioProcessor.setOsc2Pan(slider->getValue());
    }
    else if (slider == &osc2PhaseKnob)
    {
        audioProcessor.setOsc2Phase(slider->getValue());
    }
    else if (slider == &osc2AttackKnob)
    {
        audioProcessor.setOsc2Attack(slider->getValue());
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2DecayKnob)
    {
        audioProcessor.setOsc2Decay(slider->getValue());
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2SustainKnob)
    {
        audioProcessor.setOsc2Sustain(slider->getValue());
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2ReleaseKnob)
    {
        audioProcessor.setOsc2Release(slider->getValue());
        updateEnvelopeDisplay();
    }
}

void SecondOscillatorComponent::handleWaveButtonClick(juce::Button* button)
{
    // Deselect all buttons first
    osc2SineButton.setToggleState(false, juce::dontSendNotification);
    osc2SawButton.setToggleState(false, juce::dontSendNotification);
    osc2SquareButton.setToggleState(false, juce::dontSendNotification);
    osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
    osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
    osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);

    // Select the clicked button
    button->setToggleState(true, juce::dontSendNotification);

    // Update processor
    if (button == &osc2SineButton)
        audioProcessor.setOsc2Type(0);
    else if (button == &osc2SawButton)
        audioProcessor.setOsc2Type(1);
    else if (button == &osc2SquareButton)
        audioProcessor.setOsc2Type(2);
    else if (button == &osc2TriangleButton)
        audioProcessor.setOsc2Type(3);
    else if (button == &osc2WhiteNoiseButton)
        audioProcessor.setOsc2Type(4);
    else if (button == &osc2PinkNoiseButton)
        audioProcessor.setOsc2Type(5);
}

void SecondOscillatorComponent::buttonClicked(juce::Button* button)
{
    if (button == &osc2SineButton || button == &osc2SawButton ||
        button == &osc2SquareButton || button == &osc2TriangleButton ||
        button == &osc2WhiteNoiseButton || button == &osc2PinkNoiseButton)
    {
        handleWaveButtonClick(button);
    }
    else if (button == &osc2RandomPhaseButton)
    {
        audioProcessor.setOsc2RandomPhase(button->getToggleState());
    }
    else if (button == &osc2AdsrLinkButton)
    {
        if (button->getToggleState())
        {
            // Store original values when linking is enabled
            osc2OriginalAttack = osc2AttackKnob.getValue();
            osc2OriginalDecay = osc2DecayKnob.getValue();
            osc2OriginalSustain = osc2SustainKnob.getValue();
            osc2OriginalRelease = osc2ReleaseKnob.getValue();
        }
        else
        {
            // Restore original values when linking is disabled
            osc2AttackKnob.setValue(osc2OriginalAttack, juce::sendNotification);
            osc2DecayKnob.setValue(osc2OriginalDecay, juce::sendNotification);
            osc2SustainKnob.setValue(osc2OriginalSustain, juce::sendNotification);
            osc2ReleaseKnob.setValue(osc2OriginalRelease, juce::sendNotification);
        }
    }
}

void SecondOscillatorComponent::mouseDown(const juce::MouseEvent& event)
{
    auto* source = event.originalComponent;

    if (source == &osc2OctaveValueLabel)
    {
        isDraggingOsc2Octave = true;
        dragStartY = event.getScreenY();
        dragStartOsc2Octave = osc2OctaveValue;
    }
    else if (source == &osc2SemitoneValueLabel)
    {
        isDraggingOsc2Semitone = true;
        dragStartY = event.getScreenY();
        dragStartOsc2Semitone = osc2SemitoneValue;
    }
    else if (source == &osc2FineTuneValueLabel)
    {
        isDraggingOsc2FineTune = true;
        dragStartY = event.getScreenY();
        dragStartOsc2FineTune = osc2FineTuneValue;
    }
    else if (source == &osc2VoicesValueLabel)
    {
        isDraggingOsc2VoiceCount = true;
        dragStartY = event.getScreenY();
        dragStartOsc2VoiceCount = osc2VoiceCountValue;
    }
}

void SecondOscillatorComponent::mouseDrag(const juce::MouseEvent& event)
{
    auto* source = event.originalComponent;

    if (isDraggingOsc2Octave && source == &osc2OctaveValueLabel)
    {
        int deltaY = dragStartY - event.getScreenY();
        int change = deltaY / 10;
        osc2OctaveValue = juce::jlimit(-4, 4, dragStartOsc2Octave + change);
        osc2OctaveValueLabel.setText(juce::String(osc2OctaveValue), juce::dontSendNotification);
        audioProcessor.setOsc2Octave(osc2OctaveValue);
    }
    else if (isDraggingOsc2Semitone && source == &osc2SemitoneValueLabel)
    {
        int deltaY = dragStartY - event.getScreenY();
        int change = deltaY / 10;
        osc2SemitoneValue = juce::jlimit(-12, 12, dragStartOsc2Semitone + change);
        osc2SemitoneValueLabel.setText(juce::String(osc2SemitoneValue), juce::dontSendNotification);
        audioProcessor.setOsc2Semitone(osc2SemitoneValue);
    }
    else if (isDraggingOsc2FineTune && source == &osc2FineTuneValueLabel)
    {
        int deltaY = dragStartY - event.getScreenY();
        int change = deltaY / 5;
        osc2FineTuneValue = juce::jlimit(-100, 100, dragStartOsc2FineTune + change);
        osc2FineTuneValueLabel.setText(juce::String(osc2FineTuneValue), juce::dontSendNotification);
        audioProcessor.setOsc2FineTune(osc2FineTuneValue);
    }
    else if (isDraggingOsc2VoiceCount && source == &osc2VoicesValueLabel)
    {
        int deltaY = dragStartY - event.getScreenY();
        int change = deltaY / 10;
        osc2VoiceCountValue = juce::jlimit(1, 8, dragStartOsc2VoiceCount + change);
        osc2VoicesValueLabel.setText(juce::String(osc2VoiceCountValue), juce::dontSendNotification);
        audioProcessor.setOsc2VoiceCount(osc2VoiceCountValue);
    }
}

void SecondOscillatorComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOsc2Octave = false;
    isDraggingOsc2Semitone = false;
    isDraggingOsc2FineTune = false;
    isDraggingOsc2VoiceCount = false;
}

void SecondOscillatorComponent::updateEnvelopeDisplay()
{
    osc2AdsrEnvelopeVisualizer.updateEnvelope(
        osc2AttackKnob.getValue(),
        osc2DecayKnob.getValue(),
        osc2SustainKnob.getValue(),
        osc2ReleaseKnob.getValue()
    );
    osc2AdsrEnvelopeVisualizer.repaint();
}

void SecondOscillatorComponent::updateAllGuiControls()
{
    osc2OctaveValueLabel.setText(juce::String(osc2OctaveValue), juce::dontSendNotification);
    osc2SemitoneValueLabel.setText(juce::String(osc2SemitoneValue), juce::dontSendNotification);
    osc2FineTuneValueLabel.setText(juce::String(osc2FineTuneValue), juce::dontSendNotification);
    osc2VoicesValueLabel.setText(juce::String(osc2VoiceCountValue), juce::dontSendNotification);
    updateEnvelopeDisplay();
}
