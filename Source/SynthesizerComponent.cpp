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
    // Custom look and feel handles all colors
    sineWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sineWaveButton.setClickingTogglesState(true);
    sineWaveButton.setToggleState(true, juce::dontSendNotification); // Start with sine selected
    sineWaveButton.addListener(this);
    addAndMakeVisible(sineWaveButton);
    
    sawWaveButton.setButtonText("SAW");
    // Custom look and feel handles all colors
    sawWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    sawWaveButton.setClickingTogglesState(true);
    sawWaveButton.addListener(this);
    addAndMakeVisible(sawWaveButton);
    
    squareWaveButton.setButtonText("SQR");
    // Custom look and feel handles all colors
    squareWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    squareWaveButton.setClickingTogglesState(true);
    squareWaveButton.addListener(this);
    addAndMakeVisible(squareWaveButton);
    
    triangleWaveButton.setButtonText("TRI");
    // Custom look and feel handles all colors
    triangleWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    triangleWaveButton.setClickingTogglesState(true);
    triangleWaveButton.addListener(this);
    addAndMakeVisible(triangleWaveButton);
    
    noiseWaveButton.setButtonText("WHT");
    // Custom look and feel handles all colors
    noiseWaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    noiseWaveButton.setClickingTogglesState(true);
    noiseWaveButton.addListener(this);
    addAndMakeVisible(noiseWaveButton);
    
    pinkNoiseButton.setButtonText("PNK");
    // Custom look and feel handles all colors
    pinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    pinkNoiseButton.setClickingTogglesState(true);
    pinkNoiseButton.addListener(this);
    addAndMakeVisible(pinkNoiseButton);
    
    // Random phase button
    randomPhaseButton.setButtonText("RND PHASE");
    // Custom look and feel handles all colors
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
    // Black background
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    // Add hollow spruce wood border around whole page
    auto pageBounds = getLocalBounds().toFloat();
    auto borderThickness = 20.0f;
    
    // Outer spruce wood border
    g.setColour(juce::Colour(0xff3d2914));
    g.fillRoundedRectangle(pageBounds, 8.0f);
    
    // Cut out inner area to make it hollow
    auto innerBounds = pageBounds.reduced(borderThickness);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(innerBounds, 4.0f);
    
    // Add spruce wood grain to border
    juce::Random borderRandom(42);
    for (int i = 0; i < 30; ++i)
    {
        // Top border grain
        auto y = pageBounds.getY() + borderRandom.nextFloat() * borderThickness;
        auto startX = pageBounds.getX();
        auto endX = pageBounds.getRight();
        g.setColour(juce::Colour(0xff1a0f08).withAlpha(0.2f + borderRandom.nextFloat() * 0.3f));
        g.drawLine(startX, y, endX, y, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Bottom border grain
        y = pageBounds.getBottom() - borderThickness + borderRandom.nextFloat() * borderThickness;
        g.drawLine(startX, y, endX, y, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Left border grain
        auto x = pageBounds.getX() + borderRandom.nextFloat() * borderThickness;
        auto startY = pageBounds.getY();
        auto endY = pageBounds.getBottom();
        g.drawLine(x, startY, x, endY, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Right border grain
        x = pageBounds.getRight() - borderThickness + borderRandom.nextFloat() * borderThickness;
        g.drawLine(x, startY, x, endY, 1.0f + borderRandom.nextFloat() * 1.0f);
    }
    
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
    auto masterHeight = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20; // all sections + spacing + extra
    
    auto masterOutlineBounds = juce::Rectangle<float>(masterLeft - 12, masterTop - 12, masterWidth + 24, masterHeight + 24);
    
    // Create 3D depth effect with multiple layers
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Main raised surface - metallic modular synth module look
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    
    // Dark metallic grey base
    g.setColour(juce::Colour(0xff404040));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff505050), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff303030), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add subtle metallic texture
    juce::Random metallicRandom(42);
    g.setColour(juce::Colour(0xff202020).withAlpha(0.3f));
    for (int i = 0; i < 200; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Add lighter metallic highlights
    metallicRandom.setSeed(123);
    g.setColour(juce::Colour(0xff606060).withAlpha(0.2f));
    for (int i = 0; i < 150; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
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
    
    // Add corner screws for modular synth module look
    auto screwRadius = 6.0f;
    auto screwInset = 12.0f;
    
    // Top-left screw
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff303030));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff606060));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff202020));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff303030));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff606060));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff202020));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff303030));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff606060));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff202020));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff303030));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff606060));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff202020));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Oscillator 1" label to bottom right of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getRight() - 120, raisedBounds.getBottom() - 35, 110, 25);
    
    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);
    
    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);
    
    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1, 
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);
    
    // Engraved 3D text effect
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 13.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("OSCILLATOR 1", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("OSCILLATOR 1", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("OSCILLATOR 1", textBounds, juce::Justification::centred, true);
    
    // Futuristic outline with glow effect on the raised surface
    
    // Add metal slots for knobs - match exact positions from resized()
    auto slotBounds = getLocalBounds();
    slotBounds.reduce(20, 20);
    
    // Skip to ADSR section (match resized() layout exactly)
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(40); // wave buttons
    slotBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    slotBounds.removeFromTop(60); // envelope visualization 
    slotBounds.removeFromTop(15); // increased spacing between envelope and knobs
    
    // ADSR knobs metal slots - match exact layout from resized()
    auto adsrRow = slotBounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(slotBounds.getWidth() / 3);
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // Same calculation as resized()
    
    // Attack knob slot - square metal
    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Decay knob slot - square metal
    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Sustain knob slot - square metal
    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Release knob slot - square metal
    auto releaseArea = adsrSection;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(20); // spacing
    
    // Volume section knobs metal slots - square metal
    auto volumeRow = slotBounds.removeFromTop(100);
    auto volumeSection = volumeRow.removeFromLeft(adsrKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Detune knob slot - square metal
    auto detuneSection = volumeRow.removeFromLeft(adsrKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Stereo Width knob slot - square metal
    auto stereoWidthSection = volumeRow.removeFromLeft(adsrKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Pan knob slot - square metal
    auto panSection = volumeRow.removeFromLeft(adsrKnobWidth);
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 40, panSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 39, panSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 37, panSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    // Skip to phase section
    slotBounds.removeFromTop(40); // spacing - match resized() method spacing
    slotBounds.removeFromTop(80); // tuning controls
    slotBounds.removeFromTop(20); // spacing
    
    // Phase knob metal slot - match exact resized() positioning
    auto phaseRow = slotBounds.removeFromTop(80);
    phaseRow.removeFromLeft(100); // random phase button
    phaseRow.removeFromLeft(15); // spacing
    auto phaseKnobArea = phaseRow.removeFromLeft(80);
    phaseKnobArea.removeFromTop(-5); // negative value to move background up even more
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 40, phaseKnobArea.getCentreY() - 46, 80, 94, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 39, phaseKnobArea.getCentreY() - 45, 78, 92, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 37, phaseKnobArea.getCentreY() - 43, 74, 88, 2.0f, 1.0f);
    
    
    // Draw futuristic section outlines for each row of controls
    auto sectionBounds = getLocalBounds();
    sectionBounds.reduce(20, 20);
    sectionBounds.removeFromTop(20);
    
    // Wave type buttons section outline
    auto waveSectionBounds = sectionBounds.removeFromTop(40);
    waveSectionBounds = waveSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3); // Same width as ADSR
    waveSectionBounds.reduce(-2, -2); // Expand the bounds slightly
    
    
    sectionBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    
    // Skip envelope visualization and spacing
    sectionBounds.removeFromTop(60 + 15); // envelope height + increased spacing
    
    // ADSR section outline
    auto adsrSectionBounds = sectionBounds.removeFromTop(100);
    adsrSectionBounds = adsrSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3);
    adsrSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Volume/Detune/Stereo/Pan section outline
    auto volumeSectionBounds = sectionBounds.removeFromTop(100);
    volumeSectionBounds = volumeSectionBounds.removeFromLeft(volumeSectionBounds.getWidth() / 3);
    volumeSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(40); // increased spacing even more to match octave controls pushed down further
    
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
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
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
    bounds.removeFromTop(20); // spacing
    
    // Use grouped layout methods
    layoutWaveTypeButtons(bounds);
    layoutADSREnvelope(bounds);
    layoutADSRKnobs(bounds);
    layoutVolumeKnobs(bounds);
    layoutOctaveControls(bounds);
    layoutPhaseControls(bounds);
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

// ============================================================================
// Grouped Layout and Background Methods
// ============================================================================

void SynthesizerComponent::layoutWaveTypeButtons(juce::Rectangle<int>& bounds)
{
    auto buttonHeight = 40;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonSection = buttonRow.removeFromLeft(bounds.getWidth() / 3); // Same width as ADSR
    
    // Store bounds for background drawing
    waveButtonsBounds = buttonSection;
    
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
}

void SynthesizerComponent::layoutADSREnvelope(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(7); // spacing
    
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection.removeFromLeft(bounds.getWidth() / 3);
    
    // Store bounds for background drawing
    adsrEnvelopeBounds = envelopeArea;
    
    envelopeComponent.setBounds(envelopeArea);
    
    bounds.removeFromTop(15); // spacing
}

void SynthesizerComponent::layoutADSRKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 3);
    
    // Store bounds for background drawing
    adsrKnobsBounds = adsrSection;
    
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
}

void SynthesizerComponent::layoutVolumeKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    bounds.removeFromTop(20); // spacing
    auto volumeRow = bounds.removeFromTop(controlHeight);
    
    // Store bounds for background drawing
    volumeKnobsBounds = volumeRow.removeFromLeft(volumeRow.getWidth() / 3);
    
    auto adsrKnobWidth = (volumeKnobsBounds.getWidth() - 45) / 4;
    auto workingRow = volumeKnobsBounds;
    
    auto volumeSection = workingRow.removeFromLeft(adsrKnobWidth);
    volumeLabel.setBounds(volumeSection.removeFromTop(20));
    volumeSlider.setBounds(volumeSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto detuneSection = workingRow.removeFromLeft(adsrKnobWidth);
    detuneLabel.setBounds(detuneSection.removeFromTop(20));
    detuneSlider.setBounds(detuneSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto stereoWidthSection = workingRow.removeFromLeft(adsrKnobWidth);
    stereoWidthLabel.setBounds(stereoWidthSection.removeFromTop(20));
    stereoWidthSlider.setBounds(stereoWidthSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto panSection = workingRow;
    panLabel.setBounds(panSection.removeFromTop(20));
    panSlider.setBounds(panSection);
}

void SynthesizerComponent::layoutOctaveControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(40); // spacing
    auto bottomControlsRow = bounds.removeFromTop(80);
    
    // Store bounds for background drawing
    octaveControlsBounds = bottomControlsRow.removeFromLeft(320); // Fixed width for tuning controls
    
    auto workingRow = octaveControlsBounds;
    
    // Octave control (draggable label)
    auto octaveArea = workingRow.removeFromLeft(60);
    octaveLabel.setBounds(octaveArea.removeFromTop(20));
    octaveValueLabel.setBounds(octaveArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Semitone control (draggable label)
    auto semitoneArea = workingRow.removeFromLeft(60);
    semitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    semitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Fine tune control (draggable label)
    auto fineTuneArea = workingRow.removeFromLeft(60);
    fineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    fineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Voice count control (draggable label)
    auto voiceCountArea = workingRow.removeFromLeft(60);
    voiceCountLabel.setBounds(voiceCountArea.removeFromTop(20));
    voiceCountValueLabel.setBounds(voiceCountArea.removeFromTop(30));
}

void SynthesizerComponent::layoutPhaseControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(20); // spacing
    auto phaseRow = bounds.removeFromTop(80);
    
    // Store bounds for background drawing
    phaseControlsBounds = phaseRow.removeFromLeft(220); // Fixed width for phase controls
    
    auto workingRow = phaseControlsBounds;
    
    // Random phase button
    auto randomPhaseButtonWidth = 100;
    auto randomPhaseArea = workingRow.removeFromLeft(randomPhaseButtonWidth);
    randomPhaseButton.setBounds(randomPhaseArea.removeFromTop(40));
    
    workingRow.removeFromLeft(15); // spacing
    
    // Phase knob
    auto phaseKnobArea = workingRow.removeFromLeft(80);
    phaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    phaseSlider.setBounds(phaseKnobArea);
}

// ============================================================================
// Grouped Background Drawing Methods
// ============================================================================

void SynthesizerComponent::drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Wave type buttons don't have additional backgrounds beyond the button styling
    // This method is here for consistency and future expansion
}

void SynthesizerComponent::drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // ADSR envelope visualization doesn't have additional background
    // This method is here for consistency and future expansion
}

void SynthesizerComponent::drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Calculate knob positions to match layout exactly
    auto adsrKnobWidth = (bounds.getWidth() - 45) / 4; // Same calculation as layout
    auto workingBounds = bounds;
    
    // Attack knob slot
    auto attackArea = workingBounds.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Decay knob slot
    auto decayArea = workingBounds.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Sustain knob slot
    auto sustainArea = workingBounds.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Release knob slot
    auto releaseArea = workingBounds;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void SynthesizerComponent::drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Calculate knob positions to match layout exactly
    auto adsrKnobWidth = (bounds.getWidth() - 45) / 4; // Same calculation as layout
    auto workingBounds = bounds;
    
    // Volume knob slot
    auto volumeSection = workingBounds.removeFromLeft(adsrKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Detune knob slot
    auto detuneSection = workingBounds.removeFromLeft(adsrKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Stereo width knob slot
    auto stereoWidthSection = workingBounds.removeFromLeft(adsrKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Pan knob slot
    auto panSection = workingBounds;
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 40, panSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 39, panSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 37, panSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void SynthesizerComponent::drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Individual background boxes for tuning controls
    auto tuningControlsRow = bounds.reduced(3, 3);
    tuningControlsRow.removeFromLeft(2); // Shift right slightly
    tuningControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
}

void SynthesizerComponent::drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Phase knob background - matching the phase knob position exactly
    auto workingBounds = bounds;
    
    // Skip random phase button area
    workingBounds.removeFromLeft(100); // random phase button
    workingBounds.removeFromLeft(15); // spacing
    
    // Phase knob background
    auto phaseKnobArea = workingBounds.removeFromLeft(80);
    phaseKnobArea.removeFromTop(-5); // negative value to move background up to match current positioning
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 40, phaseKnobArea.getCentreY() - 46, 80, 94, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 39, phaseKnobArea.getCentreY() - 45, 78, 92, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 37, phaseKnobArea.getCentreY() - 43, 74, 88, 2.0f, 1.0f);
}
