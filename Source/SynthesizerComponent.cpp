#include "SynthesizerComponent.h"
#include "PluginProcessor.h"

SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    // Title
    titleLabel.setText("Internal Synthesizer", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Press Start 2P", 20.0f, juce::Font::plain));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    // Placeholder text
    placeholderLabel.setText("Sine wave oscillator - play MIDI keys!", juce::dontSendNotification);
    placeholderLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    placeholderLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    placeholderLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(placeholderLabel);
    
    // Volume control
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volumeLabel);
    
    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeSlider.setLookAndFeel(&customKnobLookAndFeel);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    
    // Detune control
    detuneLabel.setText("Detune", juce::dontSendNotification);
    detuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    detuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    detuneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(detuneLabel);

    detuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    detuneSlider.setRange(0.0, 1.0, 0.01);
    detuneSlider.setValue(0.0);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    detuneSlider.setLookAndFeel(&customKnobLookAndFeel);
    detuneSlider.addListener(this);
    addAndMakeVisible(detuneSlider);
    
    // Attack control
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel);
    
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setRange(0.01, 2.0, 0.01);
    attackSlider.setValue(0.1);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackSlider.setLookAndFeel(&customKnobLookAndFeel);
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);
    
    // Decay control
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel);
    
    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setRange(0.01, 2.0, 0.01);
    decaySlider.setValue(0.2);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    decaySlider.setLookAndFeel(&customKnobLookAndFeel);
    decaySlider.addListener(this);
    addAndMakeVisible(decaySlider);
    
    // Sustain control
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sustainLabel);
    
    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    sustainSlider.setLookAndFeel(&customKnobLookAndFeel);
    sustainSlider.addListener(this);
    addAndMakeVisible(sustainSlider);
    
    // Release control
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);
    
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setRange(0.01, 3.0, 0.01);
    releaseSlider.setValue(0.3);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    releaseSlider.setLookAndFeel(&customKnobLookAndFeel);
    releaseSlider.addListener(this);
    addAndMakeVisible(releaseSlider);
    
    // Pulse width slider (commented out for now)
    /*
    pulseWidthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pulseWidthSlider.setRange(0.1, 0.9, 0.01);
    pulseWidthSlider.setValue(0.5);
    pulseWidthLabel.setText("Pulse Width", juce::dontSendNotification);
    pulseWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pulseWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pulseWidthLabel.setJustificationType(juce::Justification::centred);
    pulseWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    pulseWidthSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    pulseWidthSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    pulseWidthSlider.setColour(juce::Slider::trackColourId, juce::Colours::grey);
    pulseWidthSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    pulseWidthSlider.addListener(this);
    addAndMakeVisible(pulseWidthSlider);
    addAndMakeVisible(pulseWidthLabel);
    */
    
    // Octave control
    octaveLabel.setText("Octave", juce::dontSendNotification);
    octaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    octaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(octaveLabel);
    
    octaveValueLabel.setText("0", juce::dontSendNotification);
    octaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    octaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    octaveValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    octaveValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    octaveValueLabel.setJustificationType(juce::Justification::centred);
    octaveValueLabel.setInterceptsMouseClicks(true, false);
    octaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(octaveValueLabel);
    
    // Semitone control
    semitoneLabel.setText("Semi", juce::dontSendNotification);
    semitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    semitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(semitoneLabel);
    
    semitoneValueLabel.setText("0", juce::dontSendNotification);
    semitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    semitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    semitoneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    semitoneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    semitoneValueLabel.setJustificationType(juce::Justification::centred);
    semitoneValueLabel.setInterceptsMouseClicks(true, false);
    semitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(semitoneValueLabel);
    
    // Fine tune control
    fineTuneLabel.setText("Fine", juce::dontSendNotification);
    fineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    fineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(fineTuneLabel);
    
    fineTuneValueLabel.setText("0", juce::dontSendNotification);
    fineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    fineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    fineTuneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    fineTuneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    fineTuneValueLabel.setJustificationType(juce::Justification::centred);
    fineTuneValueLabel.setInterceptsMouseClicks(true, false);
    fineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(fineTuneValueLabel);
    
    // Voice count control
    voiceCountLabel.setText("Voices", juce::dontSendNotification);
    voiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    voiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(voiceCountLabel);
    
    voiceCountValueLabel.setText("1", juce::dontSendNotification);
    voiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    voiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    voiceCountValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    voiceCountValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    voiceCountValueLabel.setJustificationType(juce::Justification::centred);
    voiceCountValueLabel.setInterceptsMouseClicks(true, false);
    voiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(voiceCountValueLabel);
    
    // Oscillator type buttons - using simple text for now
    sineWaveButton.setButtonText("SIN");
    sineWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    sineWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    sineWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    sineWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sineWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sineWaveButton.setClickingTogglesState(true);
    sineWaveButton.setToggleState(true, juce::dontSendNotification); // Start with sine selected
    sineWaveButton.addListener(this);
    addAndMakeVisible(sineWaveButton);
    
    sawWaveButton.setButtonText("SAW");
    sawWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    sawWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    sawWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    sawWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sawWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sawWaveButton.setClickingTogglesState(true);
    sawWaveButton.addListener(this);
    addAndMakeVisible(sawWaveButton);
    
    squareWaveButton.setButtonText("SQR");
    squareWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    squareWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    squareWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    squareWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    squareWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    squareWaveButton.setClickingTogglesState(true);
    squareWaveButton.addListener(this);
    addAndMakeVisible(squareWaveButton);
    
    triangleWaveButton.setButtonText("TRI");
    triangleWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    triangleWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    triangleWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    triangleWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    triangleWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    triangleWaveButton.setClickingTogglesState(true);
    triangleWaveButton.addListener(this);
    addAndMakeVisible(triangleWaveButton);
    
    noiseWaveButton.setButtonText("WHT");
    noiseWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    noiseWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    noiseWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    noiseWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    noiseWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    noiseWaveButton.setClickingTogglesState(true);
    noiseWaveButton.addListener(this);
    addAndMakeVisible(noiseWaveButton);
    
    pinkNoiseButton.setButtonText("PNK");
    pinkNoiseButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    pinkNoiseButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    pinkNoiseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    pinkNoiseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    pinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    pinkNoiseButton.setClickingTogglesState(true);
    pinkNoiseButton.addListener(this);
    addAndMakeVisible(pinkNoiseButton);
    
    // Random phase button
    randomPhaseButton.setButtonText("RND PHASE");
    randomPhaseButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    randomPhaseButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    randomPhaseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    randomPhaseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    randomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    randomPhaseButton.setClickingTogglesState(true);
    randomPhaseButton.setToggleState(true, juce::dontSendNotification); // Default to random phase ON
    randomPhaseButton.addListener(this);
    addAndMakeVisible(randomPhaseButton);
}

SynthesizerComponent::~SynthesizerComponent()
{
    // Reset custom look and feel to avoid dangling pointers
    attackSlider.setLookAndFeel(nullptr);
    decaySlider.setLookAndFeel(nullptr);
    sustainSlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
    sineWaveButton.setLookAndFeel(nullptr);
    sawWaveButton.setLookAndFeel(nullptr);
    squareWaveButton.setLookAndFeel(nullptr);
    triangleWaveButton.setLookAndFeel(nullptr);
    noiseWaveButton.setLookAndFeel(nullptr);
    pinkNoiseButton.setLookAndFeel(nullptr);
    randomPhaseButton.setLookAndFeel(nullptr);
    // pulseWidthSlider.setLookAndFeel(nullptr); // commented out
}

void SynthesizerComponent::paint(juce::Graphics& g)
{
    // Dark background with blue gradient
    juce::ColourGradient gradient(juce::Colour(0xff0a0a0a), 0.0f, 0.0f,
                                 juce::Colour(0xff1a1a2e), 0.0f, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20);
    
    // Title at the top
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(10);
    
    // Placeholder text
    placeholderLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(30);
    
    // Controls in a 2x3 grid (2 linear sliders, 4 ADSR knobs)
    auto controlHeight = 100;
    auto controlWidth = bounds.getWidth() / 2 - 10;
    auto knobWidth = bounds.getWidth() / 4 - 15;
    
    bounds.removeFromTop(20); // spacing
    
    // Oscillator type buttons (left aligned)
    auto buttonHeight = 40;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonWidth = 40;
    auto buttonSpacing = 15;
    
    // Left align the 7 buttons
    auto startX = 0;
    
    sineWaveButton.setBounds(startX, buttonRow.getY(), buttonWidth, buttonHeight);
    sawWaveButton.setBounds(startX + (buttonWidth + buttonSpacing), buttonRow.getY(), buttonWidth, buttonHeight);
    squareWaveButton.setBounds(startX + (buttonWidth + buttonSpacing) * 2, buttonRow.getY(), buttonWidth, buttonHeight);
    triangleWaveButton.setBounds(startX + (buttonWidth + buttonSpacing) * 3, buttonRow.getY(), buttonWidth, buttonHeight);
    noiseWaveButton.setBounds(startX + (buttonWidth + buttonSpacing) * 4, buttonRow.getY(), buttonWidth, buttonHeight);
    pinkNoiseButton.setBounds(startX + (buttonWidth + buttonSpacing) * 5, buttonRow.getY(), buttonWidth, buttonHeight);
    
    // Random phase button (wider to fit text)
    auto phaseButtonWidth = 80;
    randomPhaseButton.setBounds(startX + (buttonWidth + buttonSpacing) * 6 + 10, buttonRow.getY(), phaseButtonWidth, buttonHeight);
    
    bounds.removeFromTop(20); // spacing between rows
    
    // Bottom row - ADSR knobs (left half of screen)
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 2);
    
    // Calculate smaller knob width for 4 knobs in 1/2 of screen width
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // 45 = 3 spacings of 15px each
    
    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    attackLabel.setBounds(attackArea.removeFromTop(20));
    attackSlider.setBounds(attackArea);
    
    adsrSection.removeFromLeft(15); // spacing
    
    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    decayLabel.setBounds(decayArea.removeFromTop(20));
    decaySlider.setBounds(decayArea);
    
    adsrSection.removeFromLeft(15); // spacing
    
    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    sustainLabel.setBounds(sustainArea.removeFromTop(20));
    sustainSlider.setBounds(sustainArea);
    
    adsrSection.removeFromLeft(15); // spacing
    
    auto releaseArea = adsrSection;
    releaseLabel.setBounds(releaseArea.removeFromTop(20));
    releaseSlider.setBounds(releaseArea);
    
    // Volume and Detune knob section - add another row below ADSR
    bounds.removeFromTop(20); // spacing
    auto volumeRow = bounds.removeFromTop(controlHeight);
    auto volumeSection = volumeRow.removeFromLeft(adsrKnobWidth);
    
    volumeLabel.setBounds(volumeSection.removeFromTop(20));
    volumeSlider.setBounds(volumeSection);
    
    volumeRow.removeFromLeft(15); // spacing
    
    auto detuneSection = volumeRow.removeFromLeft(adsrKnobWidth);
    detuneLabel.setBounds(detuneSection.removeFromTop(20));
    detuneSlider.setBounds(detuneSection);
    
    // Bottom controls - Pulse width and Octave
    bounds.removeFromTop(20); // spacing
    auto bottomControlsRow = bounds.removeFromTop(80);
    
    // Pulse width control (commented out for now)
    /*
    auto pulseWidthArea = bottomControlsRow.removeFromLeft(200);
    pulseWidthLabel.setBounds(pulseWidthArea.removeFromTop(20));
    pulseWidthSlider.setBounds(pulseWidthArea);
    
    bottomControlsRow.removeFromLeft(20); // spacing
    */
    
    // Octave control (draggable label)
    auto octaveArea = bottomControlsRow.removeFromLeft(60);
    octaveLabel.setBounds(octaveArea.removeFromTop(20));
    octaveValueLabel.setBounds(octaveArea.removeFromTop(30));
    
    bottomControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control (draggable label)
    auto semitoneArea = bottomControlsRow.removeFromLeft(60);
    semitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    semitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));
    
    bottomControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control (draggable label)
    auto fineTuneArea = bottomControlsRow.removeFromLeft(60);
    fineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    fineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));
    
    bottomControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control (draggable label)
    auto voiceCountArea = bottomControlsRow.removeFromLeft(60);
    voiceCountLabel.setBounds(voiceCountArea.removeFromTop(20));
    voiceCountValueLabel.setBounds(voiceCountArea.removeFromTop(30));
}

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioProcessor.setSynthVolume(static_cast<float>(volumeSlider.getValue()));
    }
    else if (slider == &detuneSlider)
    {
        audioProcessor.setSynthDetune(static_cast<float>(detuneSlider.getValue()));
    }
    else if (slider == &attackSlider)
    {
        audioProcessor.setSynthAttack(static_cast<float>(attackSlider.getValue()));
    }
    else if (slider == &decaySlider)
    {
        audioProcessor.setSynthDecay(static_cast<float>(decaySlider.getValue()));
    }
    else if (slider == &sustainSlider)
    {
        audioProcessor.setSynthSustain(static_cast<float>(sustainSlider.getValue()));
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setSynthRelease(static_cast<float>(releaseSlider.getValue()));
    }
    /*
    else if (slider == &pulseWidthSlider)
    {
        audioProcessor.setPulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    }
    */
}

void SynthesizerComponent::buttonClicked(juce::Button* button)
{
    if (button == &sineWaveButton)
    {
        if (sineWaveButton.getToggleState())
        {
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            squareWaveButton.setToggleState(false, juce::dontSendNotification);
            triangleWaveButton.setToggleState(false, juce::dontSendNotification);
            noiseWaveButton.setToggleState(false, juce::dontSendNotification);
            pinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(0); // 0 = sine wave
        }
        else
        {
            sineWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &sawWaveButton)
    {
        if (sawWaveButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            squareWaveButton.setToggleState(false, juce::dontSendNotification);
            triangleWaveButton.setToggleState(false, juce::dontSendNotification);
            noiseWaveButton.setToggleState(false, juce::dontSendNotification);
            pinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(1); // 1 = saw wave
        }
        else
        {
            sawWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &squareWaveButton)
    {
        if (squareWaveButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            triangleWaveButton.setToggleState(false, juce::dontSendNotification);
            noiseWaveButton.setToggleState(false, juce::dontSendNotification);
            pinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(2); // 2 = square wave
        }
        else
        {
            squareWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &triangleWaveButton)
    {
        if (triangleWaveButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            squareWaveButton.setToggleState(false, juce::dontSendNotification);
            noiseWaveButton.setToggleState(false, juce::dontSendNotification);
            pinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(3); // 3 = triangle wave
        }
        else
        {
            triangleWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &noiseWaveButton)
    {
        if (noiseWaveButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            squareWaveButton.setToggleState(false, juce::dontSendNotification);
            triangleWaveButton.setToggleState(false, juce::dontSendNotification);
            pinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(4); // 4 = white noise
        }
        else
        {
            noiseWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &pinkNoiseButton)
    {
        if (pinkNoiseButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            squareWaveButton.setToggleState(false, juce::dontSendNotification);
            triangleWaveButton.setToggleState(false, juce::dontSendNotification);
            noiseWaveButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(5); // 5 = pink noise
        }
        else
        {
            pinkNoiseButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &randomPhaseButton)
    {
        // Random phase button is independent - doesn't affect other buttons
        audioProcessor.setRandomPhase(randomPhaseButton.getToggleState());
    }
}

void SynthesizerComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &octaveValueLabel)
    {
        isDraggingOctave = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOctave = octaveValue;
    }
    else if (event.eventComponent == &semitoneValueLabel)
    {
        isDraggingSemitone = true;
        dragStartY = event.getScreenPosition().y;
        dragStartSemitone = semitoneValue;
    }
    else if (event.eventComponent == &fineTuneValueLabel)
    {
        isDraggingFineTune = true;
        dragStartY = event.getScreenPosition().y;
        dragStartFineTune = fineTuneValue;
    }
    else if (event.eventComponent == &voiceCountValueLabel)
    {
        isDraggingVoiceCount = true;
        dragStartY = event.getScreenPosition().y;
        dragStartVoiceCount = voiceCountValue;
    }
}

void SynthesizerComponent::mouseDrag(const juce::MouseEvent& event)
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
            octaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);
            audioProcessor.setOctave(octaveValue);
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
            semitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);
            audioProcessor.setSemitone(semitoneValue);
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
            fineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);
            audioProcessor.setFineTune(fineTuneValue);
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
            voiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);
            audioProcessor.setVoiceCount(voiceCountValue);
        }
    }
}

void SynthesizerComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
}
