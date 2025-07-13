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
    
    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    volumeControlsVolumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeControlsVolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsVolumeLabel.setJustificationType(juce::Justification::centred);
    volumeControlsVolumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsVolumeLabel);
    
    volumeControlsVolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsVolumeKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsVolumeKnob.setValue(0.5);
    volumeControlsVolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsVolumeKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsVolumeKnob.addListener(this);
    addAndMakeVisible(volumeControlsVolumeKnob);
    
    volumeControlsDetuneLabel.setText("DETUNE", juce::dontSendNotification);
    volumeControlsDetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsDetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsDetuneLabel.setJustificationType(juce::Justification::centred);
    volumeControlsDetuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsDetuneLabel);

    volumeControlsDetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsDetuneKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsDetuneKnob.setValue(0.0);
    volumeControlsDetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsDetuneKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsDetuneKnob.addListener(this);
    addAndMakeVisible(volumeControlsDetuneKnob);
    
    volumeControlsStereoWidthLabel.setText("STEREO", juce::dontSendNotification);
    volumeControlsStereoWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsStereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsStereoWidthLabel.setJustificationType(juce::Justification::centred);
    volumeControlsStereoWidthLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsStereoWidthLabel);

    volumeControlsStereoWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsStereoWidthKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsStereoWidthKnob.setValue(0.5);
    volumeControlsStereoWidthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsStereoWidthKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsStereoWidthKnob.addListener(this);
    addAndMakeVisible(volumeControlsStereoWidthKnob);
    
    volumeControlsPanLabel.setText("PAN", juce::dontSendNotification);
    volumeControlsPanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsPanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsPanLabel.setJustificationType(juce::Justification::centred);
    volumeControlsPanLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsPanLabel);

    volumeControlsPanKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsPanKnob.setRange(-50.0, 50.0, 1.0);
    volumeControlsPanKnob.setValue(0.0);
    volumeControlsPanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsPanKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsPanKnob.addListener(this);
    addAndMakeVisible(volumeControlsPanKnob);
    
    // PHASE CONTROLS GROUP - (STATIONARY)
    phaseControlsPhaseLabel.setText("PHASE", juce::dontSendNotification);
    phaseControlsPhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    phaseControlsPhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    phaseControlsPhaseLabel.setJustificationType(juce::Justification::centred);
    phaseControlsPhaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(phaseControlsPhaseLabel);

    phaseControlsPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseControlsPhaseKnob.setRange(0.0, 360.0, 1.0);
    phaseControlsPhaseKnob.setValue(0.0);
    phaseControlsPhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    phaseControlsPhaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    phaseControlsPhaseKnob.addListener(this);
    addAndMakeVisible(phaseControlsPhaseKnob);
    
    // ADSR KNOBS GROUP - Row 3 (MOVEABLE)
    adsrAttackLabel.setText("ATTACK", juce::dontSendNotification);
    adsrAttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrAttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrAttackLabel.setJustificationType(juce::Justification::centred);
    adsrAttackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrAttackLabel);
    
    adsrAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrAttackKnob.setRange(0.01, 2.0, 0.01);
    adsrAttackKnob.setValue(0.1);
    adsrAttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrAttackKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrAttackKnob.addListener(this);
    addAndMakeVisible(adsrAttackKnob);
    
    adsrDecayLabel.setText("DECAY", juce::dontSendNotification);
    adsrDecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrDecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrDecayLabel.setJustificationType(juce::Justification::centred);
    adsrDecayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrDecayLabel);
    
    adsrDecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrDecayKnob.setRange(0.01, 2.0, 0.01);
    adsrDecayKnob.setValue(0.2);
    adsrDecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrDecayKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrDecayKnob.addListener(this);
    addAndMakeVisible(adsrDecayKnob);
    
    adsrSustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    adsrSustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrSustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrSustainLabel.setJustificationType(juce::Justification::centred);
    adsrSustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrSustainLabel);
    
    adsrSustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrSustainKnob.setRange(0.0, 1.0, 0.01);
    adsrSustainKnob.setValue(0.7);
    adsrSustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrSustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrSustainKnob.addListener(this);
    addAndMakeVisible(adsrSustainKnob);
    
    adsrReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    adsrReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrReleaseLabel.setJustificationType(juce::Justification::centred);
    adsrReleaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrReleaseLabel);
    
    adsrReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrReleaseKnob.setRange(0.01, 3.0, 0.01);
    adsrReleaseKnob.setValue(0.3);
    adsrReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrReleaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrReleaseKnob.addListener(this);
    addAndMakeVisible(adsrReleaseKnob);
    
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
    
    // PITCH CONTROLS GROUP - Row 5 (MOVEABLE)
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
    
    // WAVE TYPE BUTTONS GROUP - Row 1 (MOVEABLE)
    waveTypeSineButton.setButtonText("SIN");
    // Custom look and feel handles all colors
    waveTypeSineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSineButton.setClickingTogglesState(true);
    waveTypeSineButton.setToggleState(true, juce::dontSendNotification); // Start with sine selected
    waveTypeSineButton.addListener(this);
    addAndMakeVisible(waveTypeSineButton);
    
    waveTypeSawButton.setButtonText("SAW");
    // Custom look and feel handles all colors
    waveTypeSawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSawButton.setClickingTogglesState(true);
    waveTypeSawButton.addListener(this);
    addAndMakeVisible(waveTypeSawButton);
    
    waveTypeSquareButton.setButtonText("SQR");
    // Custom look and feel handles all colors
    waveTypeSquareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSquareButton.setClickingTogglesState(true);
    waveTypeSquareButton.addListener(this);
    addAndMakeVisible(waveTypeSquareButton);
    
    waveTypeTriangleButton.setButtonText("TRI");
    // Custom look and feel handles all colors
    waveTypeTriangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeTriangleButton.setClickingTogglesState(true);
    waveTypeTriangleButton.addListener(this);
    addAndMakeVisible(waveTypeTriangleButton);
    
    waveTypeWhiteNoiseButton.setButtonText("WHT");
    // Custom look and feel handles all colors
    waveTypeWhiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeWhiteNoiseButton.setClickingTogglesState(true);
    waveTypeWhiteNoiseButton.addListener(this);
    addAndMakeVisible(waveTypeWhiteNoiseButton);
    
    waveTypePinkNoiseButton.setButtonText("PNK");
    // Custom look and feel handles all colors
    waveTypePinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypePinkNoiseButton.setClickingTogglesState(true);
    waveTypePinkNoiseButton.addListener(this);
    addAndMakeVisible(waveTypePinkNoiseButton);
    
    waveTypeRandomPhaseButton.setButtonText("RND PHASE");
    // Custom look and feel handles all colors
    waveTypeRandomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeRandomPhaseButton.setClickingTogglesState(true);
    waveTypeRandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Default to random phase ON
    waveTypeRandomPhaseButton.addListener(this);
    addAndMakeVisible(waveTypeRandomPhaseButton);
    
    // SECOND OSCILLATOR CONTROLS - Row 6 (MOVEABLE)
    osc2TitleLabel.setText("OSCILLATOR 2", juce::dontSendNotification);
    osc2TitleLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2TitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2TitleLabel.setJustificationType(juce::Justification::centred);
    osc2TitleLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2TitleLabel);
    
    osc2SineButton.setButtonText("SIN");
    osc2SineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SineButton.setClickingTogglesState(true);
    osc2SineButton.setToggleState(true, juce::dontSendNotification); // Start selected (sine is default)
    osc2SineButton.addListener(this);
    addAndMakeVisible(osc2SineButton);
    
    osc2VoicesLabel.setText("VOICES", juce::dontSendNotification);
    osc2VoicesLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VoicesLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2VoicesLabel);
    
    osc2VoicesValueLabel.setText("1", juce::dontSendNotification);
    osc2VoicesValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2VoicesValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesValueLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2VoicesValueLabel.setInterceptsMouseClicks(true, false);
    osc2VoicesValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2VoicesValueLabel);
    
    osc2PanLabel.setText("PAN", juce::dontSendNotification);
    osc2PanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PanLabel.setJustificationType(juce::Justification::centred);
    osc2PanLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2PanLabel);
    
    osc2PanKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    osc2PanKnob.setRange(-1.0, 1.0, 0.01);
    osc2PanKnob.setValue(0.0);
    osc2PanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2PanKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2PanKnob.addListener(this);
    addAndMakeVisible(osc2PanKnob);
    
    osc2OctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    osc2OctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2OctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2OctaveLabel);
    
    osc2OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc2OctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2OctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2OctaveValueLabel.setInterceptsMouseClicks(true, false);
    osc2OctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2OctaveValueLabel);
    
    osc2SemitoneLabel.setText("SEMI", juce::dontSendNotification);
    osc2SemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2SemitoneLabel);
    
    osc2SemitoneValueLabel.setText("0", juce::dontSendNotification);
    osc2SemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2SemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneValueLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2SemitoneValueLabel.setInterceptsMouseClicks(true, false);
    osc2SemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2SemitoneValueLabel);
    
    osc2FineTuneLabel.setText("FINE", juce::dontSendNotification);
    osc2FineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2FineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2FineTuneLabel);
    
    osc2FineTuneValueLabel.setText("0", juce::dontSendNotification);
    osc2FineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2FineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneValueLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2FineTuneValueLabel.setInterceptsMouseClicks(true, false);
    osc2FineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2FineTuneValueLabel);
    
    osc2RandomPhaseButton.setButtonText("RND PHASE");
    osc2RandomPhaseButton.setClickingTogglesState(true);
    osc2RandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    osc2RandomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2RandomPhaseButton.addListener(this);
    addAndMakeVisible(osc2RandomPhaseButton);
    
    osc2PhaseLabel.setText("PHASE", juce::dontSendNotification);
    osc2PhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PhaseLabel.setJustificationType(juce::Justification::centred);
    osc2PhaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2PhaseLabel);

    osc2PhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2PhaseKnob.setRange(0.0, 360.0, 1.0);
    osc2PhaseKnob.setValue(0.0);
    osc2PhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    osc2PhaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2PhaseKnob.addListener(this);
    addAndMakeVisible(osc2PhaseKnob);
    
    // Set oscillator 2 to sine wave and enable it by default
    audioProcessor.setOsc2Type(0); // 0 = sine
    audioProcessor.setOsc2Enabled(true);
    
    osc2SawButton.setButtonText("SAW");
    osc2SawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SawButton.setClickingTogglesState(true);
    osc2SawButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2SawButton.addListener(this);
    addAndMakeVisible(osc2SawButton);
    
    osc2SquareButton.setButtonText("SQR");
    osc2SquareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SquareButton.setClickingTogglesState(true);
    osc2SquareButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2SquareButton.addListener(this);
    addAndMakeVisible(osc2SquareButton);
    
    osc2TriangleButton.setButtonText("TRI");
    osc2TriangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2TriangleButton.setClickingTogglesState(true);
    osc2TriangleButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2TriangleButton.addListener(this);
    addAndMakeVisible(osc2TriangleButton);
    
    osc2WhiteNoiseButton.setButtonText("WHT");
    osc2WhiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2WhiteNoiseButton.setClickingTogglesState(true);
    osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2WhiteNoiseButton.addListener(this);
    addAndMakeVisible(osc2WhiteNoiseButton);
    
    osc2PinkNoiseButton.setButtonText("PNK");
    osc2PinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2PinkNoiseButton.setClickingTogglesState(true);
    osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2PinkNoiseButton.addListener(this);
    addAndMakeVisible(osc2PinkNoiseButton);
    
    osc2VolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2VolumeKnob.setRange(0.0, 0.237, 0.001); // Max reduced by 12.5 dB to match osc1 level
    osc2VolumeKnob.setValue(0.0); // Start at 0 volume
    osc2VolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2VolumeKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2VolumeKnob.addListener(this);
    addAndMakeVisible(osc2VolumeKnob);

    osc2DetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DetuneKnob.setRange(0.0, 1.0, 0.01);
    osc2DetuneKnob.setValue(0.0); // Start at 0 detune
    osc2DetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DetuneKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2DetuneKnob.addListener(this);
    addAndMakeVisible(osc2DetuneKnob);

    osc2StereoKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2StereoKnob.setRange(0.0, 1.0, 0.01);
    osc2StereoKnob.setValue(0.5); // Start at 50% stereo width
    osc2StereoKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2StereoKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2StereoKnob.addListener(this);
    addAndMakeVisible(osc2StereoKnob);
    
    osc2VolumeLabel.setText("VOLUME", juce::dontSendNotification);
    osc2VolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VolumeLabel.setJustificationType(juce::Justification::centred);
    osc2VolumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2VolumeLabel);

    osc2DetuneLabel.setText("DETUNE", juce::dontSendNotification);
    osc2DetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DetuneLabel.setJustificationType(juce::Justification::centred);
    osc2DetuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2DetuneLabel);

    osc2StereoLabel.setText("STEREO", juce::dontSendNotification);
    osc2StereoLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2StereoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2StereoLabel.setJustificationType(juce::Justification::centred);
    osc2StereoLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2StereoLabel);
    
    // Oscillator 2 ADSR controls
    osc2AttackLabel.setText("ATTACK", juce::dontSendNotification);
    osc2AttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2AttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2AttackLabel.setJustificationType(juce::Justification::centred);
    osc2AttackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2AttackLabel);
    
    osc2AttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2AttackKnob.setRange(0.01, 2.0, 0.01);
    osc2AttackKnob.setValue(0.1);
    osc2AttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2AttackKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2AttackKnob.addListener(this);
    addAndMakeVisible(osc2AttackKnob);
    
    osc2DecayLabel.setText("DECAY", juce::dontSendNotification);
    osc2DecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DecayLabel.setJustificationType(juce::Justification::centred);
    osc2DecayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2DecayLabel);
    
    osc2DecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DecayKnob.setRange(0.01, 2.0, 0.01);
    osc2DecayKnob.setValue(0.2);
    osc2DecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DecayKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2DecayKnob.addListener(this);
    addAndMakeVisible(osc2DecayKnob);
    
    osc2SustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    osc2SustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SustainLabel.setJustificationType(juce::Justification::centred);
    osc2SustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2SustainLabel);
    
    osc2SustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2SustainKnob.setRange(0.0, 1.0, 0.01);
    osc2SustainKnob.setValue(0.7);
    osc2SustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2SustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2SustainKnob.addListener(this);
    addAndMakeVisible(osc2SustainKnob);
    
    osc2ReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    osc2ReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2ReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2ReleaseLabel.setJustificationType(juce::Justification::centred);
    osc2ReleaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2ReleaseLabel);
    
    osc2ReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2ReleaseKnob.setRange(0.01, 3.0, 0.01);
    osc2ReleaseKnob.setValue(0.3);
    osc2ReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2ReleaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2ReleaseKnob.addListener(this);
    addAndMakeVisible(osc2ReleaseKnob);
    
    osc2AdsrLinkButton.setButtonText("LINK ADSR");
    osc2AdsrLinkButton.setClickingTogglesState(true);
    osc2AdsrLinkButton.setToggleState(false, juce::dontSendNotification); // Off by default
    osc2AdsrLinkButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2AdsrLinkButton.addListener(this);
    addAndMakeVisible(osc2AdsrLinkButton);
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    addAndMakeVisible(adsrEnvelopeVisualizer);
    
    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    addAndMakeVisible(osc2AdsrEnvelopeVisualizer);
    
    // Initialize envelope display with default values
    updateEnvelopeDisplay();
}

SynthesizerComponent::~SynthesizerComponent()
{
    // Reset custom look and feel to avoid dangling pointers
    adsrAttackKnob.setLookAndFeel(nullptr);
    adsrDecayKnob.setLookAndFeel(nullptr);
    adsrSustainKnob.setLookAndFeel(nullptr);
    adsrReleaseKnob.setLookAndFeel(nullptr);
    waveTypeSineButton.setLookAndFeel(nullptr);
    waveTypeSawButton.setLookAndFeel(nullptr);
    waveTypeSquareButton.setLookAndFeel(nullptr);
    waveTypeTriangleButton.setLookAndFeel(nullptr);
    waveTypeWhiteNoiseButton.setLookAndFeel(nullptr);
    waveTypePinkNoiseButton.setLookAndFeel(nullptr);
    waveTypeRandomPhaseButton.setLookAndFeel(nullptr);
    
    // Reset LED label look and feel
    volumeControlsVolumeLabel.setLookAndFeel(nullptr);
    volumeControlsDetuneLabel.setLookAndFeel(nullptr);
    volumeControlsStereoWidthLabel.setLookAndFeel(nullptr);
    volumeControlsPanLabel.setLookAndFeel(nullptr);
    phaseControlsPhaseLabel.setLookAndFeel(nullptr);
    adsrAttackLabel.setLookAndFeel(nullptr);
    adsrDecayLabel.setLookAndFeel(nullptr);
    adsrSustainLabel.setLookAndFeel(nullptr);
    adsrReleaseLabel.setLookAndFeel(nullptr);
    pitchControlsOctaveLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountLabel.setLookAndFeel(nullptr);
    
    // Reset LED number look and feel
    pitchControlsOctaveValueLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneValueLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneValueLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(nullptr);
    
    // Reset oscillator 2 controls look and feel
    osc2SineButton.setLookAndFeel(nullptr);
    osc2SawButton.setLookAndFeel(nullptr);
    osc2SquareButton.setLookAndFeel(nullptr);
    osc2TriangleButton.setLookAndFeel(nullptr);
    osc2WhiteNoiseButton.setLookAndFeel(nullptr);
    osc2PinkNoiseButton.setLookAndFeel(nullptr);
    osc2VolumeKnob.setLookAndFeel(nullptr);
    osc2VolumeLabel.setLookAndFeel(nullptr);
    // osc2VoicesKnob uses default look and feel (no custom styling needed)
    osc2VoicesLabel.setLookAndFeel(nullptr);
    osc2PanKnob.setLookAndFeel(nullptr);
    osc2PanLabel.setLookAndFeel(nullptr);
    // osc2OctaveKnob uses default look and feel (no custom styling needed)
    osc2OctaveLabel.setLookAndFeel(nullptr);
    // osc2SemitoneKnob uses default look and feel (no custom styling needed)
    osc2SemitoneLabel.setLookAndFeel(nullptr);
    // osc2FineTuneKnob uses default look and feel (no custom styling needed)
    osc2FineTuneLabel.setLookAndFeel(nullptr);
    osc2RandomPhaseButton.setLookAndFeel(nullptr);
    osc2PhaseKnob.setLookAndFeel(nullptr);
    osc2PhaseLabel.setLookAndFeel(nullptr);
    osc2TitleLabel.setLookAndFeel(nullptr);
    osc2AttackKnob.setLookAndFeel(nullptr);
    osc2AttackLabel.setLookAndFeel(nullptr);
    osc2DecayKnob.setLookAndFeel(nullptr);
    osc2DecayLabel.setLookAndFeel(nullptr);
    osc2SustainKnob.setLookAndFeel(nullptr);
    osc2SustainLabel.setLookAndFeel(nullptr);
    osc2ReleaseKnob.setLookAndFeel(nullptr);
    osc2ReleaseLabel.setLookAndFeel(nullptr);
    osc2AdsrLinkButton.setLookAndFeel(nullptr);
    osc2DetuneKnob.setLookAndFeel(nullptr);
    osc2DetuneLabel.setLookAndFeel(nullptr);
    osc2StereoKnob.setLookAndFeel(nullptr);
    osc2StereoLabel.setLookAndFeel(nullptr);
    
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
    
    // Apply oscillator offset for easy group movement (defaults to 0,0 for no change)
    auto masterOutlineBounds = juce::Rectangle<float>(masterLeft - 12 + oscillatorBackgroundOffsetX, masterTop - 12 + oscillatorBackgroundOffsetY, masterWidth + 24, masterHeight + 24);
    
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
    
    // Much darker metallic grey base
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
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
    auto screwInset = 15.0f; // Increased inset to ensure screws are visible
    
    // Top-left screw
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Oscillator 1" label to top center of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);
    
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
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
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
    // Apply oscillator offset to match the UI components movement
    slotBounds = slotBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    
    // Skip to ADSR section (match resized() layout exactly)
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(40); // wave buttons
    slotBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    slotBounds.removeFromTop(60); // envelope visualization 
    slotBounds.removeFromTop(15); // increased spacing between envelope and knobs
    
    // ADSR knobs metal slots - match exact layout from resized()
    auto adsrRow = slotBounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(slotBounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) - same as layout
    adsrSection = adsrSection.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
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
    auto volumeControlsSection = volumeRow.removeFromLeft(slotBounds.getWidth() / 3); // Full section for all 4 knobs
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) - same as layout
    volumeControlsSection = volumeControlsSection.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    auto volumeKnobWidth = (volumeControlsSection.getWidth() - 45) / 4; // Width for each knob
    auto volumeSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Detune knob slot - square metal
    auto detuneSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Stereo Width knob slot - square metal
    auto stereoWidthSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Pan knob slot - square metal
    auto panSection = volumeControlsSection;
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
    // Apply oscillator offset to match the UI components movement
    sectionBounds = sectionBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
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
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) section outline
    adsrSectionBounds = adsrSectionBounds.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
    adsrSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Volume/Detune/Stereo/Pan section outline
    auto volumeSectionBounds = sectionBounds.removeFromTop(100);
    volumeSectionBounds = volumeSectionBounds.removeFromLeft(volumeSectionBounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) section outline
    volumeSectionBounds = volumeSectionBounds.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    volumeSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(40); // increased spacing even more to match octave controls pushed down further
    
    // Octave/Semitone/Fine/Voices section outline
    auto tuningSectionBounds = sectionBounds.removeFromTop(80);
    tuningSectionBounds = tuningSectionBounds.removeFromLeft(320); // Fixed width for tuning controls
    
    // Apply group offset for MOVEABLE Pitch Controls Group (Row 5) section outline
    tuningSectionBounds = tuningSectionBounds.translated(
        static_cast<int>(pitchControlsGroupOffsetX), 
        static_cast<int>(pitchControlsGroupOffsetY)
    );
    
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
    
    // Second oscillator section
    sectionBounds.removeFromTop(20); // spacing
    auto secondOscSectionBounds = sectionBounds.removeFromTop(100);
    // Center the oscillator 2 section to match layout
    auto sectionWidth = 200; // Same width as in layout function
    secondOscSectionBounds = secondOscSectionBounds.withSizeKeepingCentre(sectionWidth, 100);
    secondOscSectionBounds = secondOscSectionBounds.translated(
        static_cast<int>(secondOscillatorGroupOffsetX), 
        static_cast<int>(secondOscillatorGroupOffsetY)
    );
    drawSecondOscillatorBackground(g, secondOscillatorBounds);
    drawOsc2PitchControlsBackground(g, osc2PitchControlsBounds);
    // drawOsc2PhaseControlsBackground(g, osc2PhaseControlsBounds); // Removed - was covering the knob
    
    // Precision alignment grid overlay - symmetrical and aligned to boundaries
    auto gridBounds = getLocalBounds().toFloat();
    auto centerX = gridBounds.getWidth() / 2.0f;
    auto centerY = gridBounds.getHeight() / 2.0f;
    
    g.setColour(juce::Colours::cyan.withAlpha(0.3f));
    
    // Vertical grid lines - symmetrical from center
    for (int i = 1; i * 20 < centerX; ++i)
    {
        float offset = i * 20.0f;
        // Right side
        g.drawLine(centerX + offset, 0, centerX + offset, gridBounds.getHeight(), 0.5f);
        // Left side
        g.drawLine(centerX - offset, 0, centerX - offset, gridBounds.getHeight(), 0.5f);
    }
    
    // Horizontal grid lines - symmetrical from center
    for (int i = 1; i * 20 < centerY; ++i)
    {
        float offset = i * 20.0f;
        // Bottom half
        g.drawLine(0, centerY + offset, gridBounds.getWidth(), centerY + offset, 0.5f);
        // Top half
        g.drawLine(0, centerY - offset, gridBounds.getWidth(), centerY - offset, 0.5f);
    }
    
    // Center lines
    g.setColour(juce::Colours::cyan.withAlpha(0.6f));
    g.drawLine(centerX, 0, centerX, gridBounds.getHeight(), 1.0f); // Vertical center
    g.drawLine(0, centerY, gridBounds.getWidth(), centerY, 1.0f);   // Horizontal center
    
    // Major grid lines every 100 pixels - also symmetrical
    g.setColour(juce::Colours::cyan.withAlpha(0.5f));
    for (int i = 1; i * 100 < centerX; ++i)
    {
        float offset = i * 100.0f;
        g.drawLine(centerX + offset, 0, centerX + offset, gridBounds.getHeight(), 1.0f);
        g.drawLine(centerX - offset, 0, centerX - offset, gridBounds.getHeight(), 1.0f);
    }
    for (int i = 1; i * 100 < centerY; ++i)
    {
        float offset = i * 100.0f;
        g.drawLine(0, centerY + offset, gridBounds.getWidth(), centerY + offset, 1.0f);
        g.drawLine(0, centerY - offset, gridBounds.getWidth(), centerY - offset, 1.0f);
    }

    // Main window border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20);
    bounds.removeFromTop(20); // spacing
    
    // Apply oscillator offset to move all UI components together
    bounds = bounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    
    // Use grouped layout methods
    layoutWaveTypeButtons(bounds);
    layoutADSREnvelope(bounds);
    layoutADSRKnobs(bounds);
    layoutVolumeKnobs(bounds);
    layoutOctaveControls(bounds);
    layoutPhaseControls(bounds);
    layoutSecondOscillator(bounds);
}

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeControlsVolumeKnob)
    {
        audioProcessor.setOsc1Volume(static_cast<float>(volumeControlsVolumeKnob.getValue()));
    }
    else if (slider == &volumeControlsDetuneKnob)
    {
        audioProcessor.setSynthDetune(static_cast<float>(volumeControlsDetuneKnob.getValue()));
    }
    else if (slider == &volumeControlsStereoWidthKnob)
    {
        audioProcessor.setSynthStereoWidth(static_cast<float>(volumeControlsStereoWidthKnob.getValue()));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setSynthPan(static_cast<float>(volumeControlsPanKnob.getValue()));
    }
    else if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setSynthPhase(static_cast<float>(phaseControlsPhaseKnob.getValue()));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setSynthAttack(static_cast<float>(adsrAttackKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2AttackKnob.setValue(adsrAttackKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Attack(static_cast<float>(adsrAttackKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setSynthDecay(static_cast<float>(adsrDecayKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2DecayKnob.setValue(adsrDecayKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Decay(static_cast<float>(adsrDecayKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setSynthSustain(static_cast<float>(adsrSustainKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2SustainKnob.setValue(adsrSustainKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setSynthRelease(static_cast<float>(adsrReleaseKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2ReleaseKnob.setValue(adsrReleaseKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Release(static_cast<float>(adsrReleaseKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2VolumeKnob)
    {
        audioProcessor.setOsc2Volume(static_cast<float>(osc2VolumeKnob.getValue()));
    }
    else if (slider == &osc2DetuneKnob)
    {
        audioProcessor.setOsc2Detune(static_cast<float>(osc2DetuneKnob.getValue()));
    }
    else if (slider == &osc2StereoKnob)
    {
        audioProcessor.setOsc2Stereo(static_cast<float>(osc2StereoKnob.getValue()));
    }
    else if (slider == &osc2AttackKnob)
    {
        audioProcessor.setOsc2Attack(static_cast<float>(osc2AttackKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrAttackKnob.setValue(osc2AttackKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setSynthAttack(static_cast<float>(osc2AttackKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2DecayKnob)
    {
        audioProcessor.setOsc2Decay(static_cast<float>(osc2DecayKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrDecayKnob.setValue(osc2DecayKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setSynthDecay(static_cast<float>(osc2DecayKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2SustainKnob)
    {
        audioProcessor.setOsc2Sustain(static_cast<float>(osc2SustainKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrSustainKnob.setValue(osc2SustainKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setSynthSustain(static_cast<float>(osc2SustainKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2ReleaseKnob)
    {
        audioProcessor.setOsc2Release(static_cast<float>(osc2ReleaseKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrReleaseKnob.setValue(osc2ReleaseKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setSynthRelease(static_cast<float>(osc2ReleaseKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    // osc2VoicesKnob removed - now handled by mouse events
    else if (slider == &osc2PanKnob)
    {
        audioProcessor.setOsc2Pan(static_cast<float>(osc2PanKnob.getValue()));
    }
    // osc2OctaveKnob removed - now handled by mouse events
    // osc2SemitoneKnob removed - now handled by mouse events
    // osc2FineTuneKnob removed - now handled by mouse events
    else if (slider == &osc2PhaseKnob)
    {
        audioProcessor.setOsc2Phase(static_cast<float>(osc2PhaseKnob.getValue()));
    }
    /*
    else if (slider == &pulseWidthSlider)
    {
        audioProcessor.setOsc1PulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    }
    */
}

void SynthesizerComponent::buttonClicked(juce::Button* button)
{
    if (button == &waveTypeSineButton)
    {
        if (waveTypeSineButton.getToggleState())
        {
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(0); // 0 = sine wave
        }
        else
        {
            waveTypeSineButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeSawButton)
    {
        if (waveTypeSawButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(1); // 1 = saw wave
        }
        else
        {
            waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeSquareButton)
    {
        if (waveTypeSquareButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(2); // 2 = square wave
        }
        else
        {
            waveTypeSquareButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeTriangleButton)
    {
        if (waveTypeTriangleButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(3); // 3 = triangle wave
        }
        else
        {
            waveTypeTriangleButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeWhiteNoiseButton)
    {
        if (waveTypeWhiteNoiseButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(4); // 4 = white noise
        }
        else
        {
            waveTypeWhiteNoiseButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypePinkNoiseButton)
    {
        if (waveTypePinkNoiseButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(5); // 5 = pink noise
        }
        else
        {
            waveTypePinkNoiseButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeRandomPhaseButton)
    {
        // Random phase button is independent - doesn't affect other buttons
        audioProcessor.setOsc1RandomPhase(waveTypeRandomPhaseButton.getToggleState());
    }
    else if (button == &osc2SineButton)
    {
        if (osc2SineButton.getToggleState())
        {
            // Select sine wave for oscillator 2
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(0); // 0 = sine
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting sine, keep at least one selected
            if (!osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SineButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2SawButton)
    {
        if (osc2SawButton.getToggleState())
        {
            // Select saw wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(1); // 1 = saw
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting saw, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SawButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2SquareButton)
    {
        if (osc2SquareButton.getToggleState())
        {
            // Select square wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(2); // 2 = square
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting square, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SquareButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2TriangleButton)
    {
        if (osc2TriangleButton.getToggleState())
        {
            // Select triangle wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(3); // 3 = triangle
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting triangle, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2TriangleButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2WhiteNoiseButton)
    {
        if (osc2WhiteNoiseButton.getToggleState())
        {
            // Select white noise for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(4); // 4 = white noise
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting white noise, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2WhiteNoiseButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2PinkNoiseButton)
    {
        if (osc2PinkNoiseButton.getToggleState())
        {
            // Select pink noise for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(5); // 5 = pink noise
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting pink noise, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState())
            {
                osc2PinkNoiseButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2RandomPhaseButton)
    {
        audioProcessor.setOsc2RandomPhase(osc2RandomPhaseButton.getToggleState());
    }
    else if (button == &osc2AdsrLinkButton)
    {
        if (osc2AdsrLinkButton.getToggleState())
        {
            // Store oscillator 2's current ADSR values before linking
            osc2OriginalAttack = static_cast<float>(osc2AttackKnob.getValue());
            osc2OriginalDecay = static_cast<float>(osc2DecayKnob.getValue());
            osc2OriginalSustain = static_cast<float>(osc2SustainKnob.getValue());
            osc2OriginalRelease = static_cast<float>(osc2ReleaseKnob.getValue());
            
            // Link oscillator 2 ADSR to oscillator 1 - copy current values
            osc2AttackKnob.setValue(adsrAttackKnob.getValue(), juce::dontSendNotification);
            osc2DecayKnob.setValue(adsrDecayKnob.getValue(), juce::dontSendNotification);
            osc2SustainKnob.setValue(adsrSustainKnob.getValue(), juce::dontSendNotification);
            osc2ReleaseKnob.setValue(adsrReleaseKnob.getValue(), juce::dontSendNotification);
            
            // Update audio processor with linked values
            audioProcessor.setOsc2Attack(static_cast<float>(adsrAttackKnob.getValue()));
            audioProcessor.setOsc2Decay(static_cast<float>(adsrDecayKnob.getValue()));
            audioProcessor.setOsc2Sustain(static_cast<float>(adsrSustainKnob.getValue()));
            audioProcessor.setOsc2Release(static_cast<float>(adsrReleaseKnob.getValue()));
            
            // Update the envelope display
            updateEnvelopeDisplay();
        }
        else
        {
            // Restore oscillator 2's original ADSR values when unlinking
            osc2AttackKnob.setValue(osc2OriginalAttack, juce::dontSendNotification);
            osc2DecayKnob.setValue(osc2OriginalDecay, juce::dontSendNotification);
            osc2SustainKnob.setValue(osc2OriginalSustain, juce::dontSendNotification);
            osc2ReleaseKnob.setValue(osc2OriginalRelease, juce::dontSendNotification);
            
            // Update audio processor with restored values
            audioProcessor.setOsc2Attack(osc2OriginalAttack);
            audioProcessor.setOsc2Decay(osc2OriginalDecay);
            audioProcessor.setOsc2Sustain(osc2OriginalSustain);
            audioProcessor.setOsc2Release(osc2OriginalRelease);
            
            // Update the envelope display
            updateEnvelopeDisplay();
        }
    }
}

void SynthesizerComponent::mouseDown(const juce::MouseEvent& event)
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
    // Oscillator 2 pitch controls
    else if (event.eventComponent == &osc2OctaveValueLabel)
    {
        isDraggingOsc2Octave = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2Octave = osc2OctaveValue;
    }
    else if (event.eventComponent == &osc2SemitoneValueLabel)
    {
        isDraggingOsc2Semitone = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2Semitone = osc2SemitoneValue;
    }
    else if (event.eventComponent == &osc2FineTuneValueLabel)
    {
        isDraggingOsc2FineTune = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2FineTune = osc2FineTuneValue;
    }
    else if (event.eventComponent == &osc2VoicesValueLabel)
    {
        isDraggingOsc2VoiceCount = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2VoiceCount = osc2VoiceCountValue;
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
    // Oscillator 2 pitch controls
    else if (isDraggingOsc2Octave)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newOctave = dragStartOsc2Octave + (deltaY / 10); // 10 pixels per octave
        
        // Clamp to valid range
        newOctave = juce::jlimit(-4, 4, newOctave);
        
        if (newOctave != osc2OctaveValue)
        {
            osc2OctaveValue = newOctave;
            osc2OctaveValueLabel.setText(juce::String(osc2OctaveValue), juce::dontSendNotification);
            audioProcessor.setOsc2Octave(osc2OctaveValue);
        }
    }
    else if (isDraggingOsc2Semitone)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newSemitone = dragStartOsc2Semitone + (deltaY / 5); // 5 pixels per semitone
        
        // Clamp to valid range (-12 to +12 semitones)
        newSemitone = juce::jlimit(-12, 12, newSemitone);
        
        if (newSemitone != osc2SemitoneValue)
        {
            osc2SemitoneValue = newSemitone;
            osc2SemitoneValueLabel.setText(juce::String(osc2SemitoneValue), juce::dontSendNotification);
            audioProcessor.setOsc2Semitone(osc2SemitoneValue);
        }
    }
    else if (isDraggingOsc2FineTune)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newFineTune = dragStartOsc2FineTune + (deltaY / 2); // 2 pixels per cent
        
        // Clamp to valid range (-100 to +100 cents)
        newFineTune = juce::jlimit(-100, 100, newFineTune);
        
        if (newFineTune != osc2FineTuneValue)
        {
            osc2FineTuneValue = newFineTune;
            osc2FineTuneValueLabel.setText(juce::String(osc2FineTuneValue), juce::dontSendNotification);
            audioProcessor.setOsc2FineTune(osc2FineTuneValue);
        }
    }
    else if (isDraggingOsc2VoiceCount)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newVoiceCount = dragStartOsc2VoiceCount + (deltaY / 8); // 8 pixels per voice
        
        // Clamp to valid range (1 to 16 voices)
        newVoiceCount = juce::jlimit(1, 16, newVoiceCount);
        
        if (newVoiceCount != osc2VoiceCountValue)
        {
            osc2VoiceCountValue = newVoiceCount;
            osc2VoicesValueLabel.setText(juce::String(osc2VoiceCountValue), juce::dontSendNotification);
            audioProcessor.setOsc2VoiceCount(osc2VoiceCountValue);
        }
    }
}

void SynthesizerComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
    // Oscillator 2 drag states
    isDraggingOsc2Octave = false;
    isDraggingOsc2Semitone = false;
    isDraggingOsc2FineTune = false;
    isDraggingOsc2VoiceCount = false;
}

void SynthesizerComponent::updateEnvelopeDisplay()
{
    adsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(adsrAttackKnob.getValue()),
        static_cast<float>(adsrDecayKnob.getValue()),
        static_cast<float>(adsrSustainKnob.getValue()),
        static_cast<float>(adsrReleaseKnob.getValue())
    );
    
    osc2AdsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(osc2AttackKnob.getValue()),
        static_cast<float>(osc2DecayKnob.getValue()),
        static_cast<float>(osc2SustainKnob.getValue()),
        static_cast<float>(osc2ReleaseKnob.getValue())
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
    
    // Apply group offset for MOVEABLE Wave Type Buttons Group (Row 1)
    auto offsetButtonSection = buttonSection.translated(
        static_cast<int>(waveTypeButtonsGroupOffsetX), 
        static_cast<int>(waveTypeButtonsGroupOffsetY)
    );
    auto offsetButtonRow = buttonRow.translated(
        static_cast<int>(waveTypeButtonsGroupOffsetX), 
        static_cast<int>(waveTypeButtonsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    waveButtonsBounds = offsetButtonSection;
    
    // Calculate button width and spacing to fill the section
    auto totalButtons = 6;
    auto buttonSpacing = 10;
    auto totalSpacing = (totalButtons - 1) * buttonSpacing;
    auto buttonWidth = (offsetButtonSection.getWidth() - totalSpacing) / totalButtons;
    
    waveTypeSineButton.setBounds(offsetButtonSection.getX(), offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSawButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing), offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSquareButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 2, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeTriangleButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 3, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeWhiteNoiseButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 4, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypePinkNoiseButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 5, offsetButtonRow.getY(), buttonWidth, buttonHeight);
}

void SynthesizerComponent::layoutADSREnvelope(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(7); // spacing
    
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection.removeFromLeft(bounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Envelope Visualizer Group (Row 2)
    auto offsetEnvelopeArea = envelopeArea.translated(
        static_cast<int>(adsrEnvelopeGroupOffsetX), 
        static_cast<int>(adsrEnvelopeGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    adsrEnvelopeBounds = offsetEnvelopeArea;
    
    adsrEnvelopeVisualizer.setBounds(offsetEnvelopeArea);
    
    bounds.removeFromTop(15); // spacing
}

void SynthesizerComponent::layoutADSRKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3)
    auto offsetAdsrSection = adsrSection.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    adsrKnobsBounds = offsetAdsrSection;
    
    // Calculate smaller knob width for 4 knobs in 1/3 of screen width
    auto adsrKnobWidth = (offsetAdsrSection.getWidth() - 45) / 4; // 45 = 3 spacings of 15px each
    
    auto attackArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrAttackLabel.setBounds(attackArea.removeFromTop(20));
    adsrAttackKnob.setBounds(attackArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto decayArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrDecayLabel.setBounds(decayArea.removeFromTop(20));
    adsrDecayKnob.setBounds(decayArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto sustainArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrSustainLabel.setBounds(sustainArea.removeFromTop(20));
    adsrSustainKnob.setBounds(sustainArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto releaseArea = offsetAdsrSection;
    adsrReleaseLabel.setBounds(releaseArea.removeFromTop(20));
    adsrReleaseKnob.setBounds(releaseArea);
}

void SynthesizerComponent::layoutVolumeKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    bounds.removeFromTop(20); // spacing
    auto volumeRow = bounds.removeFromTop(controlHeight);
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4)
    auto volumeSection = volumeRow.removeFromLeft(volumeRow.getWidth() / 3);
    auto offsetVolumeSection = volumeSection.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    volumeKnobsBounds = offsetVolumeSection;
    
    auto adsrKnobWidth = (offsetVolumeSection.getWidth() - 45) / 4;
    auto workingRow = offsetVolumeSection;
    
    auto volumeKnobArea = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsVolumeLabel.setBounds(volumeKnobArea.removeFromTop(20));
    volumeControlsVolumeKnob.setBounds(volumeKnobArea);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto detuneSection = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsDetuneLabel.setBounds(detuneSection.removeFromTop(20));
    volumeControlsDetuneKnob.setBounds(detuneSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto stereoWidthSection = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsStereoWidthLabel.setBounds(stereoWidthSection.removeFromTop(20));
    volumeControlsStereoWidthKnob.setBounds(stereoWidthSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto panSection = workingRow;
    volumeControlsPanLabel.setBounds(panSection.removeFromTop(20));
    volumeControlsPanKnob.setBounds(panSection);
}

void SynthesizerComponent::layoutOctaveControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(40); // spacing
    auto bottomControlsRow = bounds.removeFromTop(80);
    
    auto pitchControlsSection = bottomControlsRow.removeFromLeft(320); // Fixed width for tuning controls
    
    // Apply group offset for MOVEABLE Pitch Controls Group (Row 5)
    auto offsetPitchControlsSection = pitchControlsSection.translated(
        static_cast<int>(pitchControlsGroupOffsetX), 
        static_cast<int>(pitchControlsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    octaveControlsBounds = offsetPitchControlsSection;
    
    auto workingRow = offsetPitchControlsSection;
    
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
}

void SynthesizerComponent::layoutPhaseControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(20); // spacing
    auto phaseRow = bounds.removeFromTop(80);
    
    // Store bounds for background drawing
    phaseControlsBounds = phaseRow.removeFromLeft(220); // Fixed width for phase controls
    
    auto workingRow = phaseControlsBounds;
    
    // Random phase button
    auto waveTypeRandomPhaseButtonWidth = 100;
    auto randomPhaseArea = workingRow.removeFromLeft(waveTypeRandomPhaseButtonWidth);
    waveTypeRandomPhaseButton.setBounds(randomPhaseArea.removeFromTop(40));
    
    workingRow.removeFromLeft(15); // spacing
    
    // Phase knob
    auto phaseKnobArea = workingRow.removeFromLeft(80);
    phaseControlsPhaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    phaseControlsPhaseKnob.setBounds(phaseKnobArea);
}

void SynthesizerComponent::layoutSecondOscillator(juce::Rectangle<int>& bounds)
{
    // Move to top-right position - use a specific position instead of removing from bounds
    auto totalWidth = getWidth();
    auto totalHeight = getHeight();
    
    // Position oscillator 2 in top-right corner - match oscillator 1 size exactly
    auto osc2Width = 320 + 63 + 24; // Same calculation as oscillator 1: tuning width + padding + border = 407
    auto osc2Height = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20 + 24; // Same calculation as oscillator 1: all sections + spacing + border = 606
    auto osc2X = totalWidth - osc2Width - 20; // 20px margin to touch wood border
    auto osc2Y = 20; // 20px margin from top edge
    
    auto osc2Section = juce::Rectangle<int>(osc2X, osc2Y, osc2Width, osc2Height);
    
    // Apply group offset for MOVEABLE Second Oscillator Group (Row 6)
    auto offsetOsc2Section = osc2Section.translated(
        static_cast<int>(secondOscillatorGroupOffsetX), 
        static_cast<int>(secondOscillatorGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    secondOscillatorBounds = offsetOsc2Section;
    
    // Layout similar to oscillator 1 - organized in rows
    auto workingArea = offsetOsc2Section;
    
    // Title label (emblem)
    auto titleHeight = 20;
    auto titleArea = workingArea.removeFromTop(titleHeight);
    osc2TitleLabel.setBounds(titleArea);
    
    workingArea.removeFromTop(15); // reduced spacing to push buttons up slightly
    
    // Wave type buttons row - stretch to align pink noise button with right screw
    auto buttonHeight = 40; // Match oscillator 1 height
    auto waveButtonsRow = workingArea.removeFromTop(buttonHeight);
    auto buttonWidth = 55; // Back to previous setting
    auto screwInset = 15; // Screw position from edge (from background drawing code)
    auto buttonStartX = 17; // Move buttons slightly to the right
    
    // Calculate spacing to stretch row so pink noise button aligns with right screw (extend further)
    auto availableWidth = waveButtonsRow.getWidth() - buttonStartX - screwInset - buttonWidth + 8; // Add 8px stretch for more extension
    auto totalButtonWidth = 6 * buttonWidth; // Total width of all buttons
    auto totalSpacingWidth = availableWidth - totalButtonWidth + buttonWidth; // Available space for 5 gaps
    auto buttonSpacing = totalSpacingWidth / 5; // Spacing between buttons
    
    auto buttonArea = waveButtonsRow.withX(waveButtonsRow.getX() + buttonStartX);
    
    // Set button bounds with explicit height like oscillator 1
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
    
    workingArea.removeFromTop(5); // reduced spacing between rows
    
    // ADSR envelope visualizer row - match oscillator 1 proportions exactly
    auto envelopeHeight = 60; // Match oscillator 1 height
    auto osc2EnvelopeArea = workingArea.removeFromTop(envelopeHeight);
    // Use the same width calculation as oscillator 1's original bounds
    auto originalBounds = getLocalBounds();
    originalBounds.reduce(20, 20);
    auto envelopeWidth = originalBounds.getWidth() / 3; // Match oscillator 1 exact calculation
    auto centeredEnvelopeArea = osc2EnvelopeArea.withSizeKeepingCentre(envelopeWidth, envelopeHeight);
    osc2AdsrEnvelopeVisualizer.setBounds(centeredEnvelopeArea.translated(2, 1));
    
    workingArea.removeFromTop(5); // reduced spacing between envelope and knobs
    
    // Apply offset to all elements below ADSR visualizer
    auto elementsOffset = juce::Point<int>(-6, 12); // Move left 6px, down 12px
    workingArea = workingArea.translated(elementsOffset.x, elementsOffset.y);
    
    // ADSR knobs row - match oscillator 1 dimensions
    auto knobHeight = 80; // Match oscillator 1 knob height
    auto knobLabelHeight = 20; // Match oscillator 1 label height
    auto adsrKnobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);
    auto knobWidth = 80;
    auto knobSpacing = 15; // Match oscillator 1 spacing
    
    // Center the ADSR knobs horizontally
    auto totalKnobWidth = 4 * knobWidth + 3 * knobSpacing;
    auto knobStartX = (adsrKnobsRow.getWidth() - totalKnobWidth) / 2;
    auto knobArea = adsrKnobsRow.withX(adsrKnobsRow.getX() + knobStartX).withWidth(totalKnobWidth);
    
    // Attack knob
    auto attackArea = knobArea.removeFromLeft(knobWidth);
    osc2AttackLabel.setBounds(attackArea.removeFromTop(knobLabelHeight));
    osc2AttackKnob.setBounds(attackArea);
    knobArea.removeFromLeft(knobSpacing);
    
    // Decay knob
    auto decayArea = knobArea.removeFromLeft(knobWidth);
    osc2DecayLabel.setBounds(decayArea.removeFromTop(knobLabelHeight));
    osc2DecayKnob.setBounds(decayArea);
    knobArea.removeFromLeft(knobSpacing);
    
    // Sustain knob
    auto sustainArea = knobArea.removeFromLeft(knobWidth);
    osc2SustainLabel.setBounds(sustainArea.removeFromTop(knobLabelHeight));
    osc2SustainKnob.setBounds(sustainArea);
    knobArea.removeFromLeft(knobSpacing);
    
    // Release knob
    auto releaseArea = knobArea.removeFromLeft(knobWidth);
    osc2ReleaseLabel.setBounds(releaseArea.removeFromTop(knobLabelHeight));
    osc2ReleaseKnob.setBounds(releaseArea);
    
    // Position ADSR Link button outside the main row layout - centered on screen
    auto linkButtonHeight = 25;
    auto linkButtonWidth = 100;
    auto screenCenterX = totalWidth / 2;
    auto linkButtonArea = juce::Rectangle<int>(
        screenCenterX - linkButtonWidth / 2, // centered horizontally on screen
        offsetOsc2Section.getY() + 30, // 30px from top edge (below title)
        linkButtonWidth,
        linkButtonHeight
    );
    osc2AdsrLinkButton.setBounds(linkButtonArea);
    
    // No spacing needed since link button is now outside the row flow
    
    // First additional row - knobs (volume, detune, stereo, pan) - move up closer
    workingArea.removeFromTop(5); // smaller spacing to pull rows up
    auto knobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);
    
    // Center the knobs horizontally (4 knobs)
    auto totalKnobsWidth = 4 * knobWidth + 3 * knobSpacing;
    auto knobsStartX = (knobsRow.getWidth() - totalKnobsWidth) / 2;
    auto knobsArea = knobsRow.withX(knobsRow.getX() + knobsStartX).withWidth(totalKnobsWidth);
    
    // Volume knob
    auto volumeArea = knobsArea.removeFromLeft(knobWidth);
    osc2VolumeLabel.setBounds(volumeArea.removeFromTop(knobLabelHeight));
    osc2VolumeKnob.setBounds(volumeArea);
    knobsArea.removeFromLeft(knobSpacing);
    
    // Detune knob
    auto detuneArea = knobsArea.removeFromLeft(knobWidth);
    osc2DetuneLabel.setBounds(detuneArea.removeFromTop(knobLabelHeight));
    osc2DetuneKnob.setBounds(detuneArea);
    knobsArea.removeFromLeft(knobSpacing);
    
    // Stereo knob
    auto stereoArea = knobsArea.removeFromLeft(knobWidth);
    osc2StereoLabel.setBounds(stereoArea.removeFromTop(knobLabelHeight));
    osc2StereoKnob.setBounds(stereoArea);
    knobsArea.removeFromLeft(knobSpacing);
    
    // Pan knob
    auto panArea = knobsArea.removeFromLeft(knobWidth);
    osc2PanLabel.setBounds(panArea.removeFromTop(knobLabelHeight));
    osc2PanKnob.setBounds(panArea);
    
    workingArea.removeFromTop(5); // reduced spacing between knobs and sliders
    
    // Second additional row - pitch controls (octave, semitone, fine tune, voices) - label style like oscillator 1
    auto pitchControlsHeight = 50; // 20 for label + 30 for value
    auto pitchControlsRow = workingArea.removeFromTop(pitchControlsHeight);
    
    // Store bounds for background drawing
    osc2PitchControlsBounds = pitchControlsRow;
    
    // Center the pitch controls horizontally (4 controls)
    auto pitchControlWidth = 60; // Match oscillator 1
    auto pitchControlSpacing = 10; // Match oscillator 1
    auto totalPitchControlsWidth = 4 * pitchControlWidth + 3 * pitchControlSpacing;
    auto pitchControlsStartX = (pitchControlsRow.getWidth() - totalPitchControlsWidth) / 2;
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
    
    workingArea.removeFromTop(5); // reduced spacing between sliders and random phase button
    
    // Random phase button and phase knob row - increase height for better knob visibility
    auto phaseControlsRow = workingArea.removeFromTop(100);
    
    // Position random phase button and phase knob side by side
    auto randomPhaseButtonWidth = 100; // Match oscillator 1 width
    auto phaseControlsWidth = randomPhaseButtonWidth + knobSpacing + knobWidth;
    auto phaseControlsStartX = (phaseControlsRow.getWidth() - phaseControlsWidth) / 2;
    auto phaseControlsArea = phaseControlsRow.withX(phaseControlsRow.getX() + phaseControlsStartX).withWidth(phaseControlsWidth);
    
    // Random phase button (left side)
    auto randomPhaseButtonArea = phaseControlsArea.removeFromLeft(randomPhaseButtonWidth);
    // Center the button vertically in the row
    auto randomPhaseButtonHeight = 40; // Match oscillator 1 height
    auto buttonVerticalOffset = (phaseControlsRow.getHeight() - randomPhaseButtonHeight) / 2;
    randomPhaseButtonArea = randomPhaseButtonArea.withY(randomPhaseButtonArea.getY() + buttonVerticalOffset).withHeight(randomPhaseButtonHeight);
    osc2RandomPhaseButton.setBounds(randomPhaseButtonArea);
    
    phaseControlsArea.removeFromLeft(knobSpacing);
    
    // Phase knob (right side)
    auto phaseKnobArea = phaseControlsArea.removeFromLeft(knobWidth);
    
    // Store bounds for background drawing - use the full area before any modifications
    osc2PhaseControlsBounds = phaseKnobArea;
    
    osc2PhaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    
    // Match oscillator 1 phase knob size: 80 width × 60 height
    auto matchingKnobArea = juce::Rectangle<int>(phaseKnobArea.getX(), phaseKnobArea.getY(), 80, 60);
    osc2PhaseKnob.setBounds(matchingKnobArea);
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

void SynthesizerComponent::drawOsc2PitchControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Individual background boxes for oscillator 2 pitch controls
    auto pitchControlsRow = bounds.reduced(3, 3);
    pitchControlsRow.removeFromLeft(2); // Shift right slightly
    pitchControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = pitchControlsRow.removeFromLeft(60);
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

void SynthesizerComponent::drawOsc2PhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Phase knob background for oscillator 2 - matching oscillator 1 style
    // Use the bounds directly with proper centering
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    
    // Adjust background size to fit within the 100px total height (20 label + 80 knob)
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(centreX - 40, centreY - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(centreX - 39, centreY - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(centreX - 37, centreY - 47, 74, 94, 2.0f, 1.0f);
}

void SynthesizerComponent::drawSecondOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same as oscillator 1
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Main raised surface - metallic modular synth module look
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    
    // Much darker metallic grey base
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
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
    auto screwInset = 15.0f;
    auto topScrewInset = screwInset - 2.0f; // Move top screws up 3 pixels higher
    
    // Top-left screw (moved up slightly and right by 2px)
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset + 2.0f, raisedBounds.getY() + topScrewInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw (moved up slightly and right by 2px from original)
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2 + 2.0f, raisedBounds.getY() + topScrewInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Oscillator 2" label to top center of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);
    
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
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("OSCILLATOR 2", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for raised effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("OSCILLATOR 2", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("OSCILLATOR 2", textBounds, juce::Justification::centred, true);
    
    // Add metal slots for knobs - match oscillator 2 layout
    // Get oscillator 2 bounds from secondOscillatorBounds
    auto slotBounds = secondOscillatorBounds;
    
    // Skip title area
    slotBounds.removeFromTop(20); // title height
    slotBounds.removeFromTop(10); // spacing
    
    // Wave buttons section
    slotBounds.removeFromTop(40); // wave buttons height
    slotBounds.removeFromTop(10); // spacing
    
    // ADSR envelope visualization
    slotBounds.removeFromTop(60); // envelope height
    slotBounds.removeFromTop(10); // spacing
    
    // ADSR knobs metal slots - 4 knobs in a row
    auto adsrRow = slotBounds.removeFromTop(80);
    auto adsrKnobWidth = (adsrRow.getWidth() - 45) / 4; // Same calculation as oscillator 1
    
    // Attack knob slot
    auto attackArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(15); // spacing
    
    // Decay knob slot
    auto decayArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(15); // spacing
    
    // Sustain knob slot
    auto sustainArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(15); // spacing
    
    // Release knob slot
    auto releaseArea = adsrRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Volume controls metal slots - 4 knobs in a row
    auto volumeRow = slotBounds.removeFromTop(80);
    auto volumeKnobWidth = (volumeRow.getWidth() - 45) / 4;
    
    // Volume knob slot
    auto volumeArea = volumeRow.removeFromLeft(volumeKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeArea.getCentreX() - 40, volumeArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeArea.getCentreX() - 39, volumeArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeArea.getCentreX() - 37, volumeArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Detune knob slot
    auto detuneArea = volumeRow.removeFromLeft(volumeKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneArea.getCentreX() - 40, detuneArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneArea.getCentreX() - 39, detuneArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneArea.getCentreX() - 37, detuneArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Stereo knob slot
    auto stereoArea = volumeRow.removeFromLeft(volumeKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoArea.getCentreX() - 40, stereoArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 39, stereoArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 37, stereoArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Pan knob slot
    auto panArea = volumeRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panArea.getCentreX() - 40, panArea.getCentreY() - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panArea.getCentreX() - 39, panArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panArea.getCentreX() - 37, panArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Pitch controls metal slots - 4 controls in a row
    auto pitchRow = slotBounds.removeFromTop(60);
    auto pitchControlWidth = pitchRow.getWidth() / 4;
    
    // Octave control slot
    auto octaveArea = pitchRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveArea.getCentreX() - 35, octaveArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 34, octaveArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 32, octaveArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    // Semitone control slot
    auto semitoneArea = pitchRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneArea.getCentreX() - 35, semitoneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 34, semitoneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 32, semitoneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    // Fine tune control slot
    auto fineTuneArea = pitchRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneArea.getCentreX() - 35, fineTuneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 34, fineTuneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 32, fineTuneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    // Voice count control slot
    auto voiceArea = pitchRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceArea.getCentreX() - 35, voiceArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 34, voiceArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 32, voiceArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Phase control slot - single knob
    auto phaseRow = slotBounds.removeFromTop(60);
    auto phaseArea = phaseRow.removeFromRight(80); // Position on right side
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseArea.getCentreX() - 40, phaseArea.getCentreY() - 40, 80, 80, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseArea.getCentreX() - 39, phaseArea.getCentreY() - 39, 78, 78, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseArea.getCentreX() - 37, phaseArea.getCentreY() - 37, 74, 74, 2.0f, 1.0f);
}

