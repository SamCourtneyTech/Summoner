#include "SynthesizerComponent.h"
#include "PluginProcessor.h"

ADSREnvelopeComponent::ADSREnvelopeComponent()
{
}

void ADSREnvelopeComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw metallic outline (like knobs) - outer dark frame
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    
    // Inner metallic frame
    auto metalBounds = bounds.toFloat().reduced(3.0f);
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(metalBounds, 6.0f);
    
    // Metallic highlight
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(metalBounds.reduced(1.0f), 5.0f, 1.0f);
    
    
    // Draw background inside the frames
    auto innerBounds = bounds.toFloat().reduced(8.0f);
    g.setColour(juce::Colour(0xff0f0f1f).withAlpha(0.7f));
    g.fillRoundedRectangle(innerBounds, 4.0f);
    
    // Calculate envelope curve (adjust for the additional frames)
    bounds.reduce(16, 16); // More reduction to account for larger metallic + white frames
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    if (width <= 0 || height <= 0) return;
    
    // Calculate time divisions
    auto totalTime = attackTime + decayTime + 0.5f + releaseTime; // 0.5s sustain for visualization
    auto attackWidth = (attackTime / totalTime) * width;
    auto decayWidth = (decayTime / totalTime) * width;
    auto sustainWidth = (0.5f / totalTime) * width;
    auto releaseWidth = (releaseTime / totalTime) * width;
    
    // Create envelope path
    juce::Path envelopePath;
    
    // Start at bottom left
    envelopePath.startNewSubPath(bounds.getX(), bounds.getBottom());
    
    // Attack phase - rise to top
    envelopePath.lineTo(bounds.getX() + attackWidth, bounds.getY());
    
    // Decay phase - fall to sustain level
    auto sustainY = bounds.getY() + height * (1.0f - sustainLevel);
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth, sustainY);
    
    // Sustain phase - hold level
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth, sustainY);
    
    // Release phase - fall to bottom
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth + releaseWidth, bounds.getBottom());
    
    // Draw the envelope curve
    g.setColour(juce::Colours::white);
    g.strokePath(envelopePath, juce::PathStrokeType(2.0f));
    
    // Draw fill under curve
    juce::Path fillPath = envelopePath;
    fillPath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth + releaseWidth, bounds.getBottom());
    fillPath.lineTo(bounds.getX(), bounds.getBottom());
    fillPath.closeSubPath();
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.fillPath(fillPath);
    
    // Draw grid lines
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    
    // Horizontal lines
    for (int i = 1; i < 4; ++i)
    {
        auto y = bounds.getY() + (height * i / 4.0f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
    }
    
    // Vertical phase dividers
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    auto x1 = bounds.getX() + attackWidth;
    auto x2 = bounds.getX() + attackWidth + decayWidth;
    auto x3 = bounds.getX() + attackWidth + decayWidth + sustainWidth;
    
    g.drawLine(x1, bounds.getY(), x1, bounds.getBottom(), 0.5f);
    g.drawLine(x2, bounds.getY(), x2, bounds.getBottom(), 0.5f);
    g.drawLine(x3, bounds.getY(), x3, bounds.getBottom(), 0.5f);
    
    // Draw phase labels
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font("Press Start 2P", 8.0f, juce::Font::plain));
    
    // Only draw labels if there's enough space
    if (attackWidth > 20)
        g.drawText("A", juce::Rectangle<int>(bounds.getX(), bounds.getY(), attackWidth, 12), juce::Justification::centred);
    if (decayWidth > 20)
        g.drawText("D", juce::Rectangle<int>(bounds.getX() + attackWidth, bounds.getY(), decayWidth, 12), juce::Justification::centred);
    if (sustainWidth > 20)
        g.drawText("S", juce::Rectangle<int>(bounds.getX() + attackWidth + decayWidth, bounds.getY(), sustainWidth, 12), juce::Justification::centred);
    if (releaseWidth > 20)
        g.drawText("R", juce::Rectangle<int>(bounds.getX() + attackWidth + decayWidth + sustainWidth, bounds.getY(), releaseWidth, 12), juce::Justification::centred);
}

void ADSREnvelopeComponent::resized()
{
    repaint();
}

void ADSREnvelopeComponent::updateEnvelope(float attack, float decay, float sustain, float release)
{
    attackTime = attack;
    decayTime = decay;
    sustainLevel = sustain;
    releaseTime = release;
    repaint();
}

SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    
    // Volume control
    volumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setJustificationType(juce::Justification::centred);
    volumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeLabel);
    
    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeSlider.setLookAndFeel(&customKnobLookAndFeel);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    
    // Detune control
    detuneLabel.setText("DETUNE", juce::dontSendNotification);
    detuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    detuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    detuneLabel.setJustificationType(juce::Justification::centred);
    detuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(detuneLabel);

    detuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    detuneSlider.setRange(0.0, 1.0, 0.01);
    detuneSlider.setValue(0.0);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    detuneSlider.setLookAndFeel(&customKnobLookAndFeel);
    detuneSlider.addListener(this);
    addAndMakeVisible(detuneSlider);
    
    // Stereo Width control
    stereoWidthLabel.setText("STEREO", juce::dontSendNotification);
    stereoWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    stereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    stereoWidthLabel.setJustificationType(juce::Justification::centred);
    stereoWidthLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(stereoWidthLabel);

    stereoWidthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    stereoWidthSlider.setRange(0.0, 1.0, 0.01);
    stereoWidthSlider.setValue(0.5);
    stereoWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    stereoWidthSlider.setLookAndFeel(&customKnobLookAndFeel);
    stereoWidthSlider.addListener(this);
    addAndMakeVisible(stereoWidthSlider);
    
    // Pan control
    panLabel.setText("PAN", juce::dontSendNotification);
    panLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    panLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    panLabel.setJustificationType(juce::Justification::centred);
    panLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(panLabel);

    panSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setRange(-50.0, 50.0, 1.0);
    panSlider.setValue(0.0);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    panSlider.setLookAndFeel(&customKnobLookAndFeel);
    panSlider.addListener(this);
    addAndMakeVisible(panSlider);
    
    // Phase control
    phaseLabel.setText("PHASE", juce::dontSendNotification);
    phaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    phaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    phaseLabel.setJustificationType(juce::Justification::centred);
    phaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(phaseLabel);

    phaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseSlider.setRange(0.0, 360.0, 1.0);
    phaseSlider.setValue(0.0);
    phaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    phaseSlider.setLookAndFeel(&customKnobLookAndFeel);
    phaseSlider.addListener(this);
    addAndMakeVisible(phaseSlider);
    
    // Attack control
    attackLabel.setText("ATTACK", juce::dontSendNotification);
    attackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(attackLabel);
    
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setRange(0.01, 2.0, 0.01);
    attackSlider.setValue(0.1);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackSlider.setLookAndFeel(&customKnobLookAndFeel);
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);
    
    // Decay control
    decayLabel.setText("DECAY", juce::dontSendNotification);
    decayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(decayLabel);
    
    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setRange(0.01, 2.0, 0.01);
    decaySlider.setValue(0.2);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    decaySlider.setLookAndFeel(&customKnobLookAndFeel);
    decaySlider.addListener(this);
    addAndMakeVisible(decaySlider);
    
    // Sustain control
    sustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    sustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centred);
    sustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(sustainLabel);
    
    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    sustainSlider.setLookAndFeel(&customKnobLookAndFeel);
    sustainSlider.addListener(this);
    addAndMakeVisible(sustainSlider);
    
    // Release control
    releaseLabel.setText("RELEASE", juce::dontSendNotification);
    releaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
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
    
    // Semitone control
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
    
    // Fine tune control
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
    
    // Voice count control
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
    
    // ADSR envelope visualization
    addAndMakeVisible(envelopeComponent);
    
    // Initialize envelope display with default values
    updateEnvelopeDisplay();
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
    
    // Reset LED label look and feel
    volumeLabel.setLookAndFeel(nullptr);
    detuneLabel.setLookAndFeel(nullptr);
    stereoWidthLabel.setLookAndFeel(nullptr);
    panLabel.setLookAndFeel(nullptr);
    phaseLabel.setLookAndFeel(nullptr);
    attackLabel.setLookAndFeel(nullptr);
    decayLabel.setLookAndFeel(nullptr);
    sustainLabel.setLookAndFeel(nullptr);
    releaseLabel.setLookAndFeel(nullptr);
    octaveLabel.setLookAndFeel(nullptr);
    semitoneLabel.setLookAndFeel(nullptr);
    fineTuneLabel.setLookAndFeel(nullptr);
    voiceCountLabel.setLookAndFeel(nullptr);
    
    // Reset LED number look and feel
    octaveValueLabel.setLookAndFeel(nullptr);
    semitoneValueLabel.setLookAndFeel(nullptr);
    fineTuneValueLabel.setLookAndFeel(nullptr);
    voiceCountValueLabel.setLookAndFeel(nullptr);
    // pulseWidthSlider.setLookAndFeel(nullptr); // commented out
}

void SynthesizerComponent::paint(juce::Graphics& g)
{
    // Dark background with blue gradient
    juce::ColourGradient gradient(juce::Colour(0xff0a0a0a), 0.0f, 0.0f,
                                 juce::Colour(0xff1a1a2e), 0.0f, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Controls section background with gritty analog texture
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20);
    
    bounds.removeFromTop(20);
    
    // Calculate controls section bounds (everything except the outer margins)
    auto controlsBounds = bounds;
    
    // Draw wood texture background
    // Base wood color - warm brown
    g.setColour(juce::Colour(0xff8b4513));
    g.fillRoundedRectangle(controlsBounds.toFloat(), 8.0f);
    
    // Add wood grain gradient for depth
    juce::ColourGradient woodGradient(juce::Colour(0xffa0522d), 0.0f, controlsBounds.getY(),
                                     juce::Colour(0xff654321), 0.0f, controlsBounds.getBottom(), false);
    g.setGradientFill(woodGradient);
    g.fillRoundedRectangle(controlsBounds.toFloat(), 8.0f);
    
    // Create wood grain lines (horizontal)
    juce::Random random(42); // Fixed seed for consistent grain
    for (int i = 0; i < 50; ++i)
    {
        auto y = controlsBounds.getY() + random.nextFloat() * controlsBounds.getHeight();
        auto startX = controlsBounds.getX();
        auto endX = controlsBounds.getRight();
        auto grainOpacity = 0.1f + random.nextFloat() * 0.3f;
        
        // Vary the grain color
        if (random.nextBool())
            g.setColour(juce::Colour(0xff5d4037).withAlpha(grainOpacity));
        else
            g.setColour(juce::Colour(0xffbcaaa4).withAlpha(grainOpacity));
        
        // Draw wavy grain lines
        juce::Path grainPath;
        grainPath.startNewSubPath(startX, y);
        
        for (float x = startX; x < endX; x += 20.0f)
        {
            auto waveY = y + sin((x - startX) * 0.02f) * (2.0f + random.nextFloat() * 3.0f);
            grainPath.lineTo(x, waveY);
        }
        grainPath.lineTo(endX, y);
        
        g.strokePath(grainPath, juce::PathStrokeType(0.5f + random.nextFloat() * 1.5f));
    }
    
    // Add subtle wood texture noise
    random.setSeed(123);
    g.setColour(juce::Colour(0xff4a2c2a).withAlpha(0.04f));
    for (int i = 0; i < 1000; ++i)
    {
        auto x = controlsBounds.getX() + random.nextFloat() * controlsBounds.getWidth();
        auto y = controlsBounds.getY() + random.nextFloat() * controlsBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Add lighter wood highlights
    random.setSeed(456);
    g.setColour(juce::Colour(0xffd2b48c).withAlpha(0.06f));
    for (int i = 0; i < 800; ++i)
    {
        auto x = controlsBounds.getX() + random.nextFloat() * controlsBounds.getWidth();
        auto y = controlsBounds.getY() + random.nextFloat() * controlsBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    
    // Draw subtle border
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawRoundedRectangle(controlsBounds.toFloat(), 8.0f, 1.0f);
    
    // Add inner highlight for that analog hardware look
    g.setColour(juce::Colour(0xff5a5a5a).withAlpha(0.3f));
    g.drawRoundedRectangle(controlsBounds.toFloat().reduced(1.0f), 7.0f, 1.0f);
    
    // Add futuristic outline around the actual controls bounds
    // Calculate the tight bounds that encompass all controls
    auto masterBounds = getLocalBounds();
    masterBounds.reduce(20, 20);
    masterBounds.removeFromTop(20);
    
    // Start from wave buttons and go to the bottom of phase section
    auto masterTop = masterBounds.getY();
    auto masterLeft = masterBounds.getX();
    
    // Width should encompass the tuning section (which is the widest at 320px)
    auto masterWidth = 320 + 63; // tuning width + a bit more padding
    
    // Height should go from wave buttons to bottom of phase section
    auto masterHeight = 40 + 20 + 60 + 10 + 100 + 20 + 100 + 20 + 80 + 20 + 80 + 20; // all sections + spacing + extra
    
    auto masterOutlineBounds = juce::Rectangle<float>(masterLeft - 12, masterTop - 12, masterWidth + 24, masterHeight + 24);
    
    // Create 3D depth effect with multiple layers
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Main raised surface - wood texture to show it's raised
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    
    // Base spruce wood color for raised surface - darker and more realistic
    g.setColour(juce::Colour(0xff3d2914));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add detailed spruce wood grain gradient
    juce::ColourGradient spruceGradient(juce::Colour(0xff4a3319), raisedBounds.getX(), raisedBounds.getY(),
                                       juce::Colour(0xff2a1b0f), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(spruceGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Create detailed spruce wood grain patterns
    juce::Random spruceRandom(42);
    
    // Major grain lines - pronounced spruce characteristics
    for (int i = 0; i < 40; ++i)
    {
        auto y = raisedBounds.getY() + spruceRandom.nextFloat() * raisedBounds.getHeight();
        auto startX = raisedBounds.getX();
        auto endX = raisedBounds.getRight();
        auto grainOpacity = 0.15f + spruceRandom.nextFloat() * 0.4f;
        
        // Darker grain lines for spruce
        if (spruceRandom.nextBool())
            g.setColour(juce::Colour(0xff1a0f08).withAlpha(grainOpacity));
        else
            g.setColour(juce::Colour(0xff2d1b10).withAlpha(grainOpacity));
        
        // More pronounced wavy grain for spruce character
        juce::Path grainPath;
        grainPath.startNewSubPath(startX, y);
        
        for (float x = startX; x < endX; x += 15.0f)
        {
            auto waveY = y + sin((x - startX) * 0.015f) * (3.0f + spruceRandom.nextFloat() * 5.0f);
            grainPath.lineTo(x, waveY);
        }
        grainPath.lineTo(endX, y);
        
        g.strokePath(grainPath, juce::PathStrokeType(0.8f + spruceRandom.nextFloat() * 2.0f));
    }
    
    // Add annual rings for spruce realism
    spruceRandom.setSeed(999);
    for (int i = 0; i < 8; ++i)
    {
        auto centerX = raisedBounds.getCentreX() + spruceRandom.nextFloat() * 100 - 50;
        auto centerY = raisedBounds.getCentreY() + spruceRandom.nextFloat() * 100 - 50;
        auto radius = 30 + spruceRandom.nextFloat() * 80;
        
        g.setColour(juce::Colour(0xff1a0f08).withAlpha(0.1f + spruceRandom.nextFloat() * 0.15f));
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2, 
                     0.5f + spruceRandom.nextFloat() * 1.0f);
    }
    
    // Add fine spruce texture
    spruceRandom.setSeed(789);
    g.setColour(juce::Colour(0xff0f0906).withAlpha(0.08f));
    for (int i = 0; i < 800; ++i)
    {
        auto x = raisedBounds.getX() + spruceRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + spruceRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f + spruceRandom.nextFloat() * 1.0f, 1.0f);
    }
    
    // Add subtle spruce highlights
    spruceRandom.setSeed(101112);
    g.setColour(juce::Colour(0xff6b4423).withAlpha(0.04f));
    for (int i = 0; i < 300; ++i)
    {
        auto x = raisedBounds.getX() + spruceRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + spruceRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff707070).withAlpha(0.8f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight  
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Futuristic outline with glow effect on the raised surface
    
    // Add metal slots for knobs - match exact positions from resized()
    auto slotBounds = getLocalBounds();
    slotBounds.reduce(20, 20);
    
    // Skip to ADSR section (match resized() layout exactly)
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(40); // wave buttons
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(60); // envelope visualization 
    slotBounds.removeFromTop(10); // spacing
    
    // ADSR knobs metal slots - match exact layout from resized()
    auto adsrRow = slotBounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(slotBounds.getWidth() / 3);
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // Same calculation as resized()
    
    // Attack knob slot - square metal
    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 35, attackArea.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 34, attackArea.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 32, attackArea.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Decay knob slot - square metal
    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 35, decayArea.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 34, decayArea.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 32, decayArea.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Sustain knob slot - square metal
    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 35, sustainArea.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 34, sustainArea.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 32, sustainArea.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Release knob slot - square metal
    auto releaseArea = adsrSection;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 35, releaseArea.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 34, releaseArea.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 32, releaseArea.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(20); // spacing
    
    // Volume section knobs metal slots - square metal
    auto volumeRow = slotBounds.removeFromTop(100);
    auto volumeSection = volumeRow.removeFromLeft(adsrKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 35, volumeSection.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 34, volumeSection.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 32, volumeSection.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Detune knob slot - square metal
    auto detuneSection = volumeRow.removeFromLeft(adsrKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 35, detuneSection.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 34, detuneSection.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 32, detuneSection.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Stereo Width knob slot - square metal
    auto stereoWidthSection = volumeRow.removeFromLeft(adsrKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 35, stereoWidthSection.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 34, stereoWidthSection.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 32, stereoWidthSection.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Pan knob slot - square metal
    auto panSection = volumeRow.removeFromLeft(adsrKnobWidth);
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 35, panSection.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 34, panSection.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 32, panSection.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    // Skip to phase section
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(80); // tuning controls
    slotBounds.removeFromTop(20); // spacing
    
    // Phase knob metal slot - square metal
    auto phaseRow = slotBounds.removeFromTop(80);
    phaseRow.removeFromLeft(100); // random phase button
    phaseRow.removeFromLeft(15); // spacing
    auto phaseKnobArea = phaseRow.removeFromLeft(80);
    phaseKnobArea.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 35, phaseKnobArea.getCentreY() - 55, 70, 110, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 34, phaseKnobArea.getCentreY() - 54, 68, 108, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 32, phaseKnobArea.getCentreY() - 52, 64, 104, 2.0f, 1.0f);
    
    
    // Draw futuristic section outlines for each row of controls
    auto sectionBounds = getLocalBounds();
    sectionBounds.reduce(20, 20);
    sectionBounds.removeFromTop(20);
    
    // Wave type buttons section outline
    auto waveSectionBounds = sectionBounds.removeFromTop(40);
    waveSectionBounds = waveSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3); // Same width as ADSR
    waveSectionBounds.reduce(-2, -2); // Expand the bounds slightly
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Skip envelope visualization and spacing
    sectionBounds.removeFromTop(60 + 10); // envelope height + spacing
    
    // ADSR section outline
    auto adsrSectionBounds = sectionBounds.removeFromTop(100);
    adsrSectionBounds = adsrSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3);
    adsrSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Volume/Detune/Stereo/Pan section outline
    auto volumeSectionBounds = sectionBounds.removeFromTop(100);
    volumeSectionBounds = volumeSectionBounds.removeFromLeft(volumeSectionBounds.getWidth() / 3);
    volumeSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Octave/Semitone/Fine/Voices section outline
    auto tuningSectionBounds = sectionBounds.removeFromTop(80);
    tuningSectionBounds = tuningSectionBounds.removeFromLeft(320); // Fixed width for tuning controls
    tuningSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    // Individual background boxes for tuning controls - center them
    auto tuningControlsRow = tuningSectionBounds.reduced(3, 3);
    tuningControlsRow.removeFromLeft(2); // Shift right slightly by reducing bounds
    tuningControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Phase section outline
    auto phaseSectionBounds = sectionBounds.removeFromTop(80);
    phaseSectionBounds = phaseSectionBounds.removeFromLeft(220); // Fixed width for phase controls
    phaseSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    // Main window border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20);
    
    
    // Controls in a 2x3 grid (2 linear sliders, 4 ADSR knobs)
    auto controlHeight = 100;
    auto controlWidth = bounds.getWidth() / 2 - 10;
    auto knobWidth = bounds.getWidth() / 4 - 15;
    
    bounds.removeFromTop(20); // spacing
    
    // Oscillator type buttons (spread across same width as ADSR section)
    auto buttonHeight = 40;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonSection = buttonRow.removeFromLeft(bounds.getWidth() / 3); // Same width as ADSR
    
    // Calculate button width and spacing to fill the section
    auto totalButtons = 6;
    auto buttonSpacing = 10;
    auto totalSpacing = (totalButtons - 1) * buttonSpacing;
    auto buttonWidth = (buttonSection.getWidth() - totalSpacing) / totalButtons;
    
    sineWaveButton.setBounds(buttonSection.getX(), buttonRow.getY(), buttonWidth, buttonHeight);
    sawWaveButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing), buttonRow.getY(), buttonWidth, buttonHeight);
    squareWaveButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 2, buttonRow.getY(), buttonWidth, buttonHeight);
    triangleWaveButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 3, buttonRow.getY(), buttonWidth, buttonHeight);
    noiseWaveButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 4, buttonRow.getY(), buttonWidth, buttonHeight);
    pinkNoiseButton.setBounds(buttonSection.getX() + (buttonWidth + buttonSpacing) * 5, buttonRow.getY(), buttonWidth, buttonHeight);
    
    
    bounds.removeFromTop(20); // spacing between rows
    
    // ADSR envelope visualization above knobs
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection.removeFromLeft(bounds.getWidth() / 3);
    envelopeComponent.setBounds(envelopeArea);
    
    bounds.removeFromTop(10); // spacing between envelope and knobs
    
    // Bottom row - ADSR knobs (left third of screen)
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 3);
    
    // Calculate smaller knob width for 4 knobs in 1/3 of screen width
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
    
    // Volume, Detune, Stereo, Pan knob section - constrained to about 1/3 screen width
    bounds.removeFromTop(20); // spacing
    auto volumeRow = bounds.removeFromTop(controlHeight);
    auto volumeSection = volumeRow.removeFromLeft(adsrKnobWidth);
    
    volumeLabel.setBounds(volumeSection.removeFromTop(20));
    volumeSlider.setBounds(volumeSection);
    
    volumeRow.removeFromLeft(15); // spacing
    
    auto detuneSection = volumeRow.removeFromLeft(adsrKnobWidth);
    detuneLabel.setBounds(detuneSection.removeFromTop(20));
    detuneSlider.setBounds(detuneSection);
    
    volumeRow.removeFromLeft(15); // spacing
    
    auto stereoWidthSection = volumeRow.removeFromLeft(adsrKnobWidth);
    stereoWidthLabel.setBounds(stereoWidthSection.removeFromTop(20));
    stereoWidthSlider.setBounds(stereoWidthSection);
    
    volumeRow.removeFromLeft(15); // spacing
    
    auto panSection = volumeRow.removeFromLeft(adsrKnobWidth);
    panLabel.setBounds(panSection.removeFromTop(20));
    panSlider.setBounds(panSection);
    
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
    
    // Phase controls - new row below everything
    bounds.removeFromTop(20); // spacing
    auto phaseRow = bounds.removeFromTop(80);
    
    // Random phase button
    auto randomPhaseButtonWidth = 100;
    auto randomPhaseArea = phaseRow.removeFromLeft(randomPhaseButtonWidth);
    randomPhaseButton.setBounds(randomPhaseArea.removeFromTop(40));
    
    phaseRow.removeFromLeft(15); // spacing
    
    // Phase knob
    auto phaseKnobArea = phaseRow.removeFromLeft(80);
    phaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    phaseSlider.setBounds(phaseKnobArea);
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
    else if (slider == &stereoWidthSlider)
    {
        audioProcessor.setSynthStereoWidth(static_cast<float>(stereoWidthSlider.getValue()));
    }
    else if (slider == &panSlider)
    {
        audioProcessor.setSynthPan(static_cast<float>(panSlider.getValue()));
    }
    else if (slider == &phaseSlider)
    {
        audioProcessor.setSynthPhase(static_cast<float>(phaseSlider.getValue()));
    }
    else if (slider == &attackSlider)
    {
        audioProcessor.setSynthAttack(static_cast<float>(attackSlider.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &decaySlider)
    {
        audioProcessor.setSynthDecay(static_cast<float>(decaySlider.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &sustainSlider)
    {
        audioProcessor.setSynthSustain(static_cast<float>(sustainSlider.getValue()));
        updateEnvelopeDisplay();
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setSynthRelease(static_cast<float>(releaseSlider.getValue()));
        updateEnvelopeDisplay();
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

void SynthesizerComponent::updateEnvelopeDisplay()
{
    envelopeComponent.updateEnvelope(
        static_cast<float>(attackSlider.getValue()),
        static_cast<float>(decaySlider.getValue()),
        static_cast<float>(sustainSlider.getValue()),
        static_cast<float>(releaseSlider.getValue())
    );
}
