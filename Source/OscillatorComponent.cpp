#include "OscillatorComponent.h"
#include "PluginProcessor.h"

OscillatorComponent::OscillatorComponent(SummonerXSerum2AudioProcessor& processor, int oscillatorNumber)
    : audioProcessor(processor), oscNumber(oscillatorNumber)
{
    // Initialize common controls for both oscillators
    initializeCommonControls();

    // Initialize specific controls based on oscillator number
    if (oscNumber == 1)
    {
        initializeOscillator1Controls();
    }
    else
    {
        initializeOscillator2Controls();
    }
}

OscillatorComponent::~OscillatorComponent()
{
}

void OscillatorComponent::initializeCommonControls()
{
    // Wave type buttons
    sineButton.setButtonText("SIN");
    sineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sineButton.setClickingTogglesState(true);
    sineButton.addListener(this);
    addAndMakeVisible(sineButton);

    sawButton.setButtonText("SAW");
    sawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sawButton.setClickingTogglesState(true);
    sawButton.addListener(this);
    addAndMakeVisible(sawButton);

    squareButton.setButtonText("SQR");
    squareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    squareButton.setClickingTogglesState(true);
    squareButton.addListener(this);
    addAndMakeVisible(squareButton);

    triangleButton.setButtonText("TRI");
    triangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    triangleButton.setClickingTogglesState(true);
    triangleButton.addListener(this);
    addAndMakeVisible(triangleButton);

    whiteNoiseButton.setButtonText("WHT");
    whiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    whiteNoiseButton.setClickingTogglesState(true);
    whiteNoiseButton.addListener(this);
    addAndMakeVisible(whiteNoiseButton);

    pinkNoiseButton.setButtonText("PNK");
    pinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    pinkNoiseButton.setClickingTogglesState(true);
    pinkNoiseButton.addListener(this);
    addAndMakeVisible(pinkNoiseButton);

    randomPhaseButton.setButtonText("RND PHASE");
    randomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    randomPhaseButton.setClickingTogglesState(true);
    randomPhaseButton.setToggleState(true, juce::dontSendNotification);
    randomPhaseButton.addListener(this);
    addAndMakeVisible(randomPhaseButton);

    // Volume controls
    volumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setJustificationType(juce::Justification::centred);
    volumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeLabel);

    volumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeKnob.setRange(0.0, 1.0, 0.01);
    volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeKnob.addListener(this);
    addAndMakeVisible(volumeKnob);

    detuneLabel.setText("DETUNE", juce::dontSendNotification);
    detuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    detuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    detuneLabel.setJustificationType(juce::Justification::centred);
    detuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(detuneLabel);

    detuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    detuneKnob.setRange(0.0, 1.0, 0.01);
    detuneKnob.setValue(0.0);
    detuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    detuneKnob.setLookAndFeel(&customKnobLookAndFeel);
    detuneKnob.addListener(this);
    addAndMakeVisible(detuneKnob);

    stereoWidthLabel.setText("STEREO", juce::dontSendNotification);
    stereoWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    stereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    stereoWidthLabel.setJustificationType(juce::Justification::centred);
    stereoWidthLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(stereoWidthLabel);

    stereoWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stereoWidthKnob.setRange(0.0, 1.0, 0.01);
    stereoWidthKnob.setValue(0.5);
    stereoWidthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    stereoWidthKnob.setLookAndFeel(&customKnobLookAndFeel);
    stereoWidthKnob.addListener(this);
    addAndMakeVisible(stereoWidthKnob);

    panLabel.setText("PAN", juce::dontSendNotification);
    panLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    panLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    panLabel.setJustificationType(juce::Justification::centred);
    panLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(panLabel);

    panKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    panKnob.setLookAndFeel(&customKnobLookAndFeel);
    panKnob.addListener(this);
    addAndMakeVisible(panKnob);

    // Phase controls
    phaseLabel.setText("PHASE", juce::dontSendNotification);
    phaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    phaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    phaseLabel.setJustificationType(juce::Justification::centred);
    phaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(phaseLabel);

    phaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseKnob.setRange(0.0, 360.0, 1.0);
    phaseKnob.setValue(0.0);
    phaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    phaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    phaseKnob.addListener(this);
    addAndMakeVisible(phaseKnob);

    // ADSR controls
    attackLabel.setText("ATTACK", juce::dontSendNotification);
    attackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(attackLabel);

    attackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackKnob.setRange(0.01, 2.0, 0.01);
    attackKnob.setValue(0.1);
    attackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackKnob.setLookAndFeel(&customKnobLookAndFeel);
    attackKnob.addListener(this);
    addAndMakeVisible(attackKnob);

    decayLabel.setText("DECAY", juce::dontSendNotification);
    decayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(decayLabel);

    decayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decayKnob.setRange(0.01, 2.0, 0.01);
    decayKnob.setValue(0.2);
    decayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    decayKnob.setLookAndFeel(&customKnobLookAndFeel);
    decayKnob.addListener(this);
    addAndMakeVisible(decayKnob);

    sustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    sustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centred);
    sustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(sustainLabel);

    sustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainKnob.setRange(0.0, 1.0, 0.01);
    sustainKnob.setValue(0.7);
    sustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    sustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    sustainKnob.addListener(this);
    addAndMakeVisible(sustainKnob);

    releaseLabel.setText("RELEASE", juce::dontSendNotification);
    releaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(releaseLabel);

    releaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseKnob.setRange(0.01, 3.0, 0.01);
    releaseKnob.setValue(0.3);
    releaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    releaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    releaseKnob.addListener(this);
    addAndMakeVisible(releaseKnob);

    // ADSR Envelope Visualizer
    addAndMakeVisible(adsrEnvelopeVisualizer);
}

void OscillatorComponent::initializeOscillator1Controls()
{
    // Set oscillator 1 specific values
    volumeKnob.setValue(0.75);
    panKnob.setRange(-50.0, 50.0, 1.0);
    panKnob.setValue(0.0);

    // Set default wave type to saw
    sawButton.setToggleState(true, juce::dontSendNotification);
    audioProcessor.setOsc1Type(1);

    // Pitch controls
    octaveLabel.setText("OCTAVE", juce::dontSendNotification);
    octaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    octaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveLabel.setJustificationType(juce::Justification::centred);
    octaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(octaveLabel);

    octaveValueLabel.setText("0", juce::dontSendNotification);
    octaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    octaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    octaveValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    octaveValueLabel.setJustificationType(juce::Justification::centred);
    octaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    octaveValueLabel.setInterceptsMouseClicks(true, false);
    octaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(octaveValueLabel);

    semitoneLabel.setText("SEMI", juce::dontSendNotification);
    semitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    semitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneLabel.setJustificationType(juce::Justification::centred);
    semitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(semitoneLabel);

    semitoneValueLabel.setText("0", juce::dontSendNotification);
    semitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    semitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    semitoneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    semitoneValueLabel.setJustificationType(juce::Justification::centred);
    semitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    semitoneValueLabel.setInterceptsMouseClicks(true, false);
    semitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(semitoneValueLabel);

    fineTuneLabel.setText("FINE", juce::dontSendNotification);
    fineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    fineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneLabel.setJustificationType(juce::Justification::centred);
    fineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(fineTuneLabel);

    fineTuneValueLabel.setText("0", juce::dontSendNotification);
    fineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    fineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    fineTuneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    fineTuneValueLabel.setJustificationType(juce::Justification::centred);
    fineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    fineTuneValueLabel.setInterceptsMouseClicks(true, false);
    fineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(fineTuneValueLabel);

    voiceCountLabel.setText("VOICES", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountLabel.setJustificationType(juce::Justification::centred);
    voiceCountLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(voiceCountLabel);

    voiceCountValueLabel.setText("1", juce::dontSendNotification);
    voiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    voiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    voiceCountValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    voiceCountValueLabel.setJustificationType(juce::Justification::centred);
    voiceCountValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    voiceCountValueLabel.setInterceptsMouseClicks(true, false);
    voiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(voiceCountValueLabel);
}

void OscillatorComponent::initializeOscillator2Controls()
{
    // Set oscillator 2 specific values
    volumeKnob.setValue(0.0);
    panKnob.setRange(-1.0, 1.0, 0.01);
    panKnob.setValue(0.0);

    // Set default wave type to saw
    sawButton.setToggleState(true, juce::dontSendNotification);
    audioProcessor.setOsc2Enabled(true);
    audioProcessor.setOsc2Type(1);

    // Title label
    titleLabel.setText("OSCILLATOR 2", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(titleLabel);

    // ADSR Link button
    adsrLinkButton.setButtonText("OSC1 ADSR");
    adsrLinkButton.setClickingTogglesState(true);
    adsrLinkButton.setToggleState(false, juce::dontSendNotification);
    adsrLinkButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    adsrLinkButton.addListener(this);
    addAndMakeVisible(adsrLinkButton);

    // Pitch controls
    octaveLabel.setText("OCTAVE", juce::dontSendNotification);
    octaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    octaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveLabel.setJustificationType(juce::Justification::centred);
    octaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(octaveLabel);

    octaveValueLabel.setText("0", juce::dontSendNotification);
    octaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    octaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveValueLabel.setJustificationType(juce::Justification::centred);
    octaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    octaveValueLabel.setInterceptsMouseClicks(true, false);
    octaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(octaveValueLabel);

    semitoneLabel.setText("SEMI", juce::dontSendNotification);
    semitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    semitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneLabel.setJustificationType(juce::Justification::centred);
    semitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(semitoneLabel);

    semitoneValueLabel.setText("0", juce::dontSendNotification);
    semitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    semitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneValueLabel.setJustificationType(juce::Justification::centred);
    semitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    semitoneValueLabel.setInterceptsMouseClicks(true, false);
    semitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(semitoneValueLabel);

    fineTuneLabel.setText("FINE", juce::dontSendNotification);
    fineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    fineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneLabel.setJustificationType(juce::Justification::centred);
    fineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(fineTuneLabel);

    fineTuneValueLabel.setText("0", juce::dontSendNotification);
    fineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    fineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneValueLabel.setJustificationType(juce::Justification::centred);
    fineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    fineTuneValueLabel.setInterceptsMouseClicks(true, false);
    fineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(fineTuneValueLabel);

    voiceCountLabel.setText("VOICES", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountLabel.setJustificationType(juce::Justification::centred);
    voiceCountLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(voiceCountLabel);

    voiceCountValueLabel.setText("1", juce::dontSendNotification);
    voiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    voiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountValueLabel.setJustificationType(juce::Justification::centred);
    voiceCountValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    voiceCountValueLabel.setInterceptsMouseClicks(true, false);
    voiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(voiceCountValueLabel);
}

void OscillatorComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    drawOscillatorBackground(g, bounds);
}

void OscillatorComponent::resized()
{
    auto bounds = getLocalBounds();

    if (oscNumber == 1)
    {
        // Layout for oscillator 1 - full layout with pitch controls
        layoutWaveTypeButtons(bounds);
        layoutADSREnvelope(bounds);
        layoutADSRKnobs(bounds);
        layoutVolumeKnobs(bounds);
        layoutOctaveControls(bounds);
        layoutPhaseControls(bounds);
    }
    else
    {
        // Layout for oscillator 2 - simplified layout
        layoutSecondOscillator(bounds);
    }
}

void OscillatorComponent::sliderValueChanged(juce::Slider* slider)
{
    if (oscNumber == 1)
    {
        // Oscillator 1 sliders
        if (slider == &attackKnob)
            audioProcessor.setOsc1Attack(slider->getValue());
        else if (slider == &decayKnob)
            audioProcessor.setOsc1Decay(slider->getValue());
        else if (slider == &sustainKnob)
            audioProcessor.setOsc1Sustain(slider->getValue());
        else if (slider == &releaseKnob)
            audioProcessor.setOsc1Release(slider->getValue());
        else if (slider == &volumeKnob)
            audioProcessor.setOsc1Volume(slider->getValue());
        else if (slider == &detuneKnob)
            audioProcessor.setOsc1Detune(slider->getValue());
        else if (slider == &stereoWidthKnob)
            audioProcessor.setOsc1StereoWidth(slider->getValue());
        else if (slider == &panKnob)
            audioProcessor.setOsc1Pan(slider->getValue());
        else if (slider == &phaseKnob)
            audioProcessor.setOsc1Phase(slider->getValue());
    }
    else
    {
        // Oscillator 2 sliders
        if (slider == &volumeKnob)
            audioProcessor.setOsc2Volume(slider->getValue());
        else if (slider == &detuneKnob)
            audioProcessor.setOsc2Detune(slider->getValue());
        else if (slider == &stereoWidthKnob)
            audioProcessor.setOsc2Stereo(slider->getValue());
        else if (slider == &panKnob)
            audioProcessor.setOsc2Pan(slider->getValue());
        else if (slider == &phaseKnob)
            audioProcessor.setOsc2Phase(slider->getValue());
        else if (slider == &attackKnob)
            audioProcessor.setOsc2Attack(slider->getValue());
        else if (slider == &decayKnob)
            audioProcessor.setOsc2Decay(slider->getValue());
        else if (slider == &sustainKnob)
            audioProcessor.setOsc2Sustain(slider->getValue());
        else if (slider == &releaseKnob)
            audioProcessor.setOsc2Release(slider->getValue());
    }

    updateEnvelopeDisplay();
}

void OscillatorComponent::buttonClicked(juce::Button* button)
{
    if (oscNumber == 1)
    {
        // Oscillator 1 wave type buttons
        if (button == &sineButton) {
            audioProcessor.setOsc1Type(0);
            updateWaveButtonStates(button, { &sawButton, &squareButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &sawButton) {
            audioProcessor.setOsc1Type(1);
            updateWaveButtonStates(button, { &sineButton, &squareButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &squareButton) {
            audioProcessor.setOsc1Type(2);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &triangleButton) {
            audioProcessor.setOsc1Type(3);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &whiteNoiseButton) {
            audioProcessor.setOsc1Type(4);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &triangleButton, &pinkNoiseButton });
        } else if (button == &pinkNoiseButton) {
            audioProcessor.setOsc1Type(5);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &triangleButton, &whiteNoiseButton });
        } else if (button == &randomPhaseButton) {
            audioProcessor.setOsc1RandomPhase(button->getToggleState());
        }
    }
    else
    {
        // Oscillator 2 wave type buttons
        if (button == &sineButton) {
            audioProcessor.setOsc2Type(0);
            updateWaveButtonStates(button, { &sawButton, &squareButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &sawButton) {
            audioProcessor.setOsc2Type(1);
            updateWaveButtonStates(button, { &sineButton, &squareButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &squareButton) {
            audioProcessor.setOsc2Type(2);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &triangleButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &triangleButton) {
            audioProcessor.setOsc2Type(3);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &whiteNoiseButton, &pinkNoiseButton });
        } else if (button == &whiteNoiseButton) {
            audioProcessor.setOsc2Type(4);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &triangleButton, &pinkNoiseButton });
        } else if (button == &pinkNoiseButton) {
            audioProcessor.setOsc2Type(5);
            updateWaveButtonStates(button, { &sineButton, &sawButton, &squareButton, &triangleButton, &whiteNoiseButton });
        } else if (button == &randomPhaseButton) {
            audioProcessor.setOsc2RandomPhase(button->getToggleState());
        } else if (button == &adsrLinkButton) {
            // TODO: Implement setOsc2AdsrLink in PluginProcessor
            // audioProcessor.setOsc2AdsrLink(button->getToggleState());
            updateAdsrLink();
        }
    }
}

void OscillatorComponent::updateEnvelopeDisplay()
{
    adsrEnvelopeVisualizer.updateEnvelope(
        attackKnob.getValue(),
        decayKnob.getValue(),
        sustainKnob.getValue(),
        releaseKnob.getValue()
    );
}

void OscillatorComponent::updateAllGuiControls()
{
    // This method would update all GUI controls from the processor state
    // Implementation depends on how the processor stores parameter values
}

void OscillatorComponent::mouseDown(const juce::MouseEvent& event)
{
    // Store initial mouse position for pitch control dragging
    auto mousePos = event.getScreenPosition();

    if (event.originalComponent == &octaveValueLabel)
    {
        isDraggingOctave = true;
        dragStartY = mousePos.y;
        dragStartOctave = octaveValue;
    }
    else if (event.originalComponent == &semitoneValueLabel)
    {
        isDraggingSemitone = true;
        dragStartY = mousePos.y;
        dragStartSemitone = semitoneValue;
    }
    else if (event.originalComponent == &fineTuneValueLabel)
    {
        isDraggingFineTune = true;
        dragStartY = mousePos.y;
        dragStartFineTune = fineTuneValue;
    }
    else if (event.originalComponent == &voiceCountValueLabel)
    {
        isDraggingVoiceCount = true;
        dragStartY = mousePos.y;
        dragStartVoiceCount = voiceCountValue;
    }
}

void OscillatorComponent::mouseDrag(const juce::MouseEvent& event)
{
    auto currentMousePosition = event.getScreenPosition();
    int deltaY = dragStartY - currentMousePosition.y;

    if (isDraggingOctave)
    {
        int newValue = dragStartOctave + (deltaY / 10);
        newValue = juce::jlimit(-2, 2, newValue);
        if (newValue != octaveValue)
        {
            octaveValue = newValue;
            if (oscNumber == 1)
                audioProcessor.setOsc1Octave(octaveValue);
            else
                audioProcessor.setOsc2Octave(octaveValue);
            updatePitchDisplay();
        }
    }
    else if (isDraggingSemitone)
    {
        int newValue = dragStartSemitone + (deltaY / 5);
        newValue = juce::jlimit(-12, 12, newValue);
        if (newValue != semitoneValue)
        {
            semitoneValue = newValue;
            if (oscNumber == 1)
                audioProcessor.setOsc1Semitone(semitoneValue);
            else
                audioProcessor.setOsc2Semitone(semitoneValue);
            updatePitchDisplay();
        }
    }
    else if (isDraggingFineTune)
    {
        int newValue = dragStartFineTune + (deltaY / 2);
        newValue = juce::jlimit(-100, 100, newValue);
        if (newValue != fineTuneValue)
        {
            fineTuneValue = newValue;
            if (oscNumber == 1)
                audioProcessor.setOsc1FineTune(fineTuneValue);
            else
                audioProcessor.setOsc2FineTune(fineTuneValue);
            updatePitchDisplay();
        }
    }
    else if (isDraggingVoiceCount)
    {
        int newValue = dragStartVoiceCount + (deltaY / 8);
        newValue = juce::jlimit(1, 16, newValue);
        if (newValue != voiceCountValue)
        {
            voiceCountValue = newValue;
            if (oscNumber == 1)
                audioProcessor.setOsc1VoiceCount(voiceCountValue);
            else
                audioProcessor.setOsc2VoiceCount(voiceCountValue);
            updatePitchDisplay();
        }
    }
}

void OscillatorComponent::mouseUp(const juce::MouseEvent& event)
{
    // Reset dragging state
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
}

void OscillatorComponent::updateWaveButtonStates(juce::Button* activeButton, std::vector<juce::Button*> inactiveButtons)
{
    activeButton->setToggleState(true, juce::dontSendNotification);
    for (auto* button : inactiveButtons)
    {
        button->setToggleState(false, juce::dontSendNotification);
    }
}

void OscillatorComponent::updateAdsrLink()
{
    if (oscNumber == 2 && adsrLinkButton.getToggleState())
    {
        // Store original values before linking
        originalAttack = static_cast<float>(attackKnob.getValue());
        originalDecay = static_cast<float>(decayKnob.getValue());
        originalSustain = static_cast<float>(sustainKnob.getValue());
        originalRelease = static_cast<float>(releaseKnob.getValue());

        // Link to oscillator 1 ADSR values - would need access to osc1 component
        // This is simplified for now
    }
    else if (oscNumber == 2)
    {
        // Restore original values
        attackKnob.setValue(originalAttack, juce::dontSendNotification);
        decayKnob.setValue(originalDecay, juce::dontSendNotification);
        sustainKnob.setValue(originalSustain, juce::dontSendNotification);
        releaseKnob.setValue(originalRelease, juce::dontSendNotification);
    }
    updateEnvelopeDisplay();
}

void OscillatorComponent::updatePitchControl(int& parameter, int delta, int step, int minVal, int maxVal)
{
    int newValue = parameter + (delta > 0 ? step : -step);
    parameter = juce::jlimit(minVal, maxVal, newValue);
}

void OscillatorComponent::updatePitchDisplay()
{
    octaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);
    semitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);
    fineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);
    voiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);
}

void OscillatorComponent::layoutWaveTypeButtons(juce::Rectangle<int>& bounds)
{
    auto waveTypeBounds = bounds.removeFromTop(40);
    waveTypeBounds = waveTypeBounds.removeFromLeft(bounds.getWidth() / 3);

    int buttonWidth = 40;
    int buttonSpacing = 7;

    sineButton.setBounds(waveTypeBounds.getX(), waveTypeBounds.getY(), buttonWidth, 30);
    sawButton.setBounds(sineButton.getRight() + buttonSpacing, waveTypeBounds.getY(), buttonWidth, 30);
    squareButton.setBounds(sawButton.getRight() + buttonSpacing, waveTypeBounds.getY(), buttonWidth, 30);
    triangleButton.setBounds(squareButton.getRight() + buttonSpacing, waveTypeBounds.getY(), buttonWidth, 30);
    whiteNoiseButton.setBounds(triangleButton.getRight() + buttonSpacing, waveTypeBounds.getY(), buttonWidth, 30);
    pinkNoiseButton.setBounds(whiteNoiseButton.getRight() + buttonSpacing, waveTypeBounds.getY(), buttonWidth, 30);
}

void OscillatorComponent::layoutADSREnvelope(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(7);
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection.removeFromLeft(bounds.getWidth() / 3);

    adsrEnvelopeVisualizer.setBounds(envelopeArea);
    bounds.removeFromTop(15);
}

void OscillatorComponent::layoutADSRKnobs(juce::Rectangle<int>& bounds)
{
    auto adsrRow = bounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 3);

    int knobWidth = (adsrSection.getWidth() - 45) / 4;
    int labelHeight = 15;
    int knobSize = 80;

    auto createKnobLayout = [&](juce::Slider& knob, juce::Label& label, juce::Rectangle<int>& section) {
        auto area = section.removeFromLeft(knobWidth);
        label.setBounds(area.getX(), area.getY(), area.getWidth(), labelHeight);
        knob.setBounds(area.getCentreX() - knobSize / 2, area.getY() + labelHeight, knobSize, knobSize);
    };

    createKnobLayout(attackKnob, attackLabel, adsrSection);
    adsrSection.removeFromLeft(15);
    createKnobLayout(decayKnob, decayLabel, adsrSection);
    adsrSection.removeFromLeft(15);
    createKnobLayout(sustainKnob, sustainLabel, adsrSection);
    adsrSection.removeFromLeft(15);
    createKnobLayout(releaseKnob, releaseLabel, adsrSection);
}

void OscillatorComponent::layoutVolumeKnobs(juce::Rectangle<int>& bounds)
{
    auto volumeRow = bounds.removeFromTop(100);
    auto volumeControlsSection = volumeRow.removeFromLeft(bounds.getWidth() / 3);

    int knobWidth = (volumeControlsSection.getWidth() - 45) / 4;
    int labelHeight = 15;
    int knobSize = 80;

    auto layoutKnob = [&](juce::Slider& knob, juce::Label& label, juce::Rectangle<int>& section) {
        auto area = section.removeFromLeft(knobWidth);
        label.setBounds(area.getX(), area.getY(), area.getWidth(), labelHeight);
        knob.setBounds(area.getCentreX() - knobSize / 2, area.getY() + labelHeight, knobSize, knobSize);
    };

    layoutKnob(volumeKnob, volumeLabel, volumeControlsSection);
    volumeControlsSection.removeFromLeft(15);
    layoutKnob(detuneKnob, detuneLabel, volumeControlsSection);
    volumeControlsSection.removeFromLeft(15);
    layoutKnob(stereoWidthKnob, stereoWidthLabel, volumeControlsSection);
    volumeControlsSection.removeFromLeft(15);
    layoutKnob(panKnob, panLabel, volumeControlsSection);
}

void OscillatorComponent::layoutOctaveControls(juce::Rectangle<int>& bounds)
{
    auto tuningRow = bounds.removeFromTop(80);
    auto tuningSection = tuningRow.removeFromLeft(320);

    int controlWidth = 60;
    int spacing = 20;
    int labelHeight = 15;
    int valueLabelHeight = 30;

    auto layoutControl = [&](juce::Label& label, juce::Label& valueLabel, juce::Rectangle<int>& section) {
        auto area = section.removeFromLeft(controlWidth);
        label.setBounds(area.getX(), area.getY(), area.getWidth(), labelHeight);
        valueLabel.setBounds(area.getX(), area.getY() + labelHeight + 5, area.getWidth(), valueLabelHeight);
    };

    layoutControl(octaveLabel, octaveValueLabel, tuningSection);
    tuningSection.removeFromLeft(spacing);
    layoutControl(semitoneLabel, semitoneValueLabel, tuningSection);
    tuningSection.removeFromLeft(spacing);
    layoutControl(fineTuneLabel, fineTuneValueLabel, tuningSection);
    tuningSection.removeFromLeft(spacing);
    layoutControl(voiceCountLabel, voiceCountValueLabel, tuningSection);
}

void OscillatorComponent::layoutPhaseControls(juce::Rectangle<int>& bounds)
{
    auto phaseRow = bounds.removeFromTop(80);
    auto phaseSection = phaseRow.removeFromLeft(220);

    int buttonWidth = 100;
    int knobSize = 80;
    int labelHeight = 15;

    randomPhaseButton.setBounds(phaseSection.getX(), phaseSection.getCentreY() - 15, buttonWidth, 30);

    auto knobArea = phaseSection;
    knobArea.removeFromLeft(buttonWidth + 15);
    phaseLabel.setBounds(knobArea.getX(), knobArea.getY(), knobSize, labelHeight);
    phaseKnob.setBounds(knobArea.getX(), knobArea.getY() + labelHeight, knobSize, knobSize);
}

void OscillatorComponent::layoutSecondOscillator(juce::Rectangle<int>& bounds)
{
    if (oscNumber != 2) return;

    auto osc2Row = bounds.removeFromTop(100);
    auto osc2Section = osc2Row.withSizeKeepingCentre(200, 100);

    int buttonWidth = 40;
    int buttonSpacing = 7;
    int knobSize = 60;
    int labelHeight = 15;

    // Title
    titleLabel.setBounds(osc2Section.getX(), osc2Section.getY() - 20, osc2Section.getWidth(), 15);

    sineButton.setBounds(osc2Section.getX(), osc2Section.getY(), buttonWidth, 30);
    sawButton.setBounds(sineButton.getRight() + buttonSpacing, osc2Section.getY(), buttonWidth, 30);
    squareButton.setBounds(sawButton.getRight() + buttonSpacing, osc2Section.getY(), buttonWidth, 30);
    triangleButton.setBounds(squareButton.getRight() + buttonSpacing, osc2Section.getY(), buttonWidth, 30);

    auto bottomRow = osc2Section;
    bottomRow.removeFromTop(40);

    volumeLabel.setBounds(bottomRow.getX(), bottomRow.getY(), knobSize, labelHeight);
    volumeKnob.setBounds(bottomRow.getX(), bottomRow.getY() + labelHeight, knobSize, knobSize);

    auto detuneArea = bottomRow;
    detuneArea.removeFromLeft(knobSize + 10);
    detuneLabel.setBounds(detuneArea.getX(), detuneArea.getY(), knobSize, labelHeight);
    detuneKnob.setBounds(detuneArea.getX(), detuneArea.getY() + labelHeight, knobSize, knobSize);

    auto stereoArea = detuneArea;
    stereoArea.removeFromLeft(knobSize + 10);
    stereoWidthLabel.setBounds(stereoArea.getX(), stereoArea.getY(), knobSize, labelHeight);
    stereoWidthKnob.setBounds(stereoArea.getX(), stereoArea.getY() + labelHeight, knobSize, knobSize);

    // ADSR Link button
    adsrLinkButton.setBounds(osc2Section.getX(), osc2Section.getBottom() + 10, 100, 30);
}

void OscillatorComponent::drawOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto masterOutlineBounds = bounds.toFloat();

    // Shadow
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);

    // Main background
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);

    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);

    // Metallic gradient
    juce::ColourGradient metallicGradient(
        juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
        juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);

    // Title label
    juce::String title = (oscNumber == 1) ? "OSCILLATOR 1" : "OSCILLATOR 2";
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);

    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);

    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff606060));
    g.drawText(title, labelBounds.toNearestInt(), juce::Justification::centred, true);
}

void OscillatorComponent::drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for wave type buttons
}

void OscillatorComponent::drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for ADSR envelope visualizer
}

void OscillatorComponent::drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for ADSR knobs
}

void OscillatorComponent::drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for volume knobs
}

void OscillatorComponent::drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for octave controls
}

void OscillatorComponent::drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Background for phase controls
}