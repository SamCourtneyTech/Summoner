#include "SynthesizerComponent.h"
#include "PluginProcessor.h"
#include <set>


SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor),
      effectsModule(juce::TabbedButtonBar::TabsAtTop),
      secondOscillator(*this, processor, &customKnobLookAndFeel, &customWaveButtonLookAndFeel, &ledLabelLookAndFeel, &ledNumberLookAndFeel),
      macroControls(*this, processor, &simpleKnobLookAndFeel, &engravedLabelLookAndFeel)
{
    addAndMakeVisible(secondOscillator);
    addAndMakeVisible(macroControls);

    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    volumeControlsVolumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeControlsVolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsVolumeLabel.setJustificationType(juce::Justification::centred);
    volumeControlsVolumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsVolumeLabel);
    
    volumeControlsVolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsVolumeKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsVolumeKnob.setValue(0.75); // Start oscillator 1 at 75% volume
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
    waveTypeSineButton.setToggleState(false, juce::dontSendNotification); // Sine not selected by default
    waveTypeSineButton.addListener(this);
    addAndMakeVisible(waveTypeSineButton);
    
    waveTypeSawButton.setButtonText("SAW");
    // Custom look and feel handles all colors
    waveTypeSawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSawButton.setClickingTogglesState(true);
    waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Start with saw selected
    waveTypeSawButton.addListener(this);
    addAndMakeVisible(waveTypeSawButton);
    
    // Set oscillator 1 to saw wave by default
    audioProcessor.setOsc1Type(1); // 1 = saw wave
    
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

    // FILTER CONTROLS - now handled by FilterControlComponent
    filterModule.setParentSynthesizer(this);
    addAndMakeVisible(filterModule);
    filterModule.setLookAndFeels(&digitalFilterKnobLookAndFeel, &engravedLabelLookAndFeel, &customWaveButtonLookAndFeel);
    filterModule.syncWithDSPState();
    
    // MACRO CONTROLS - 4 placeholder knobs
    // Macro controls now handled by MacroControlsComponent
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    addAndMakeVisible(adsrEnvelopeVisualizer);

    // LFO MODULE
    lfoModule.setLookAndFeel(&customKnobLookAndFeel, &customWaveButtonLookAndFeel, &ledLabelLookAndFeel);
    addAndMakeVisible(lfoModule);
    
    // EFFECTS MODULE - Digital Screen Style
    effectsModule.setTabBarDepth(29); // Back to single row height
    effectsModule.setCurrentTabIndex(4); // Set to EQ tab for testing
    
    // Enable multiple rows for tabs - force tabs to wrap by setting a much smaller minimum scale
    effectsModule.getTabbedButtonBar().setMinimumTabScaleFactor(0.3f); // Allow much smaller tabs to force wrapping
    
    // Set digital screen colors
    effectsModule.setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xff000008)); // Very dark blue-black
    effectsModule.setColour(juce::TabbedComponent::outlineColourId, juce::Colour(0xff00ff00).withAlpha(0.8f)); // Bright green outline
    
    // Create placeholder components for each effect with digital styling
    auto chorusTab = new juce::Component();
    // Set up DistortionComponent
    distortionModule.setParentSynthesizer(this);
    distortionModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    
    // Delay module setup
    delayModule.setParentSynthesizer(this);
    delayModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    
    // Chorus module setup
    chorusModule.setParentSynthesizer(this);
    chorusModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel);
    
    // Compressor module setup
    compressorModule.setParentSynthesizer(this);
    compressorModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    
    // Reverb module setup
    reverbModule.setParentSynthesizer(this);
    reverbModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    auto equalizerTab = new juce::Component();
    equalizerTab->setVisible(true);
    
    // Initialize parametric EQ component
    parametricEQ.setParentSynthesizer(this);
    parametricEQ.syncWithDSPState(); // Sync with initial DSP state
    chorusModule.syncWithDSPState(); // Sync chorus module with initial DSP state
    delayModule.syncWithDSPState(); // Sync delay module with initial DSP state
    compressorModule.syncWithDSPState(); // Sync compressor module with initial DSP state
    flangerModule.syncWithDSPState(); // Sync flanger module with initial DSP state
    phaserModule.syncWithDSPState(); // Sync phaser module with initial DSP state
    reverbModule.syncWithDSPState(); // Sync reverb module with initial DSP state
    parametricEQ.setVisible(true);
    equalizerTab->addAndMakeVisible(parametricEQ);
    
    // Initialize EQ on/off button - positioned above the EQ graph
    eqOnOffButton.setButtonText("EQ ON");
    eqOnOffButton.setClickingTogglesState(true);
    eqOnOffButton.setToggleState(false, juce::dontSendNotification); // EQ disabled by default
    eqOnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eqOnOffButton.addListener(this);
    eqOnOffButton.setVisible(true);
    eqOnOffButton.setBounds(120, 50, 60, 25); // Centered above EQ graph, moved higher
    equalizerTab->addAndMakeVisible(eqOnOffButton);
    
    // Set initial bounds with space for button above (will be properly laid out in resized())
    parametricEQ.setBounds(0, 95, 300, 200);
    
    // Initialize flanger module
    flangerModule.setParentSynthesizer(this);
    flangerModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    
    // Initialize phaser module
    phaserModule.setParentSynthesizer(this);
    phaserModule.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel, &greenLEDNumberLookAndFeel);
    
    // REVERB EFFECT CONTROLS - now handled by ReverbComponent
    
    // Digital screen background color for tabs
    juce::Colour digitalBg = juce::Colour(0xff001100); // Dark green tint
    
    effectsModule.addTab("CHORUS", digitalBg, &chorusModule, true);
    effectsModule.addTab("COMP", digitalBg, &compressorModule, true);
    effectsModule.addTab("DELAY", digitalBg, &delayModule, true);
    effectsModule.addTab("DIST", digitalBg, &distortionModule, true);
    effectsModule.addTab("EQ", digitalBg, equalizerTab, true);
    effectsModule.addTab("FLANGE", digitalBg, &flangerModule, true);
    effectsModule.addTab("PHASER", digitalBg, &phaserModule, true);
    effectsModule.addTab("REVERB", digitalBg, &reverbModule, true);
    
    // Apply digital screen look and feel
    effectsModule.setLookAndFeel(&digitalScreenLookAndFeel);
    
    // CHORUS EFFECT CONTROLS - now handled by ChorusComponent
    
    
    
    // EQ EFFECT CONTROLS
    // Band 1 (Left) filter type buttons
    eq1ShelfButton.setButtonText("SHELF"); // Low shelf - for later implementation
    eq1ShelfButton.setClickingTogglesState(true);
    eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq1ShelfButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1ShelfButton.addListener(this);
    eq1ShelfButton.setVisible(true);
    eq1ShelfButton.setBounds(17, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1ShelfButton);
    
    eq1PeakButton.setButtonText("PEAK");
    eq1PeakButton.setClickingTogglesState(true);
    eq1PeakButton.setToggleState(true, juce::dontSendNotification);
    eq1PeakButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1PeakButton.addListener(this);
    eq1PeakButton.setVisible(true);
    eq1PeakButton.setBounds(62, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1PeakButton);
    
    eq1PassButton.setButtonText("HIPASS");
    eq1PassButton.setClickingTogglesState(true);
    eq1PassButton.setToggleState(false, juce::dontSendNotification);
    eq1PassButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1PassButton.addListener(this);
    eq1PassButton.setVisible(true);
    eq1PassButton.setBounds(107, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1PassButton);
    
    // Band 1 on/off button
    eq1OnOffButton.setButtonText("BAND 1");
    eq1OnOffButton.setClickingTogglesState(true);
    eq1OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq1OnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1OnOffButton.addListener(this);
    eq1OnOffButton.setVisible(true);
    eq1OnOffButton.setBounds(17, 370, 50, 22); // Below filter type buttons
    equalizerTab->addAndMakeVisible(eq1OnOffButton);
    
    // Band 1 knobs
    eq1FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1FreqKnob.setValue(400.0);
    eq1FreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1FreqKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1FreqKnob);
    
    eq1FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq1FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1FreqLabel.setJustificationType(juce::Justification::centred);
    eq1FreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1FreqLabel);
    
    eq1QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1QKnob.setRange(0.1, 10.0, 0.1);
    eq1QKnob.setValue(1.0);
    eq1QKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1QKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1QKnob);
    
    eq1QLabel.setText("Q", juce::dontSendNotification);
    eq1QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1QLabel.setJustificationType(juce::Justification::centred);
    eq1QLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1QLabel);
    
    eq1GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1GainKnob.setRange(-15.0, 15.0, 0.1);
    eq1GainKnob.setValue(0.0);
    eq1GainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1GainKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1GainKnob);
    
    eq1GainLabel.setText("GAIN", juce::dontSendNotification);
    eq1GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1GainLabel.setJustificationType(juce::Justification::centred);
    eq1GainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1GainLabel);
    
    // Band 2 (Right) filter type buttons
    eq2ShelfButton.setButtonText("SHELF"); // High shelf - for later implementation
    eq2ShelfButton.setClickingTogglesState(true);
    eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq2ShelfButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2ShelfButton.addListener(this);
    eq2ShelfButton.setVisible(true);
    eq2ShelfButton.setBounds(152, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2ShelfButton);
    
    eq2PeakButton.setButtonText("PEAK");
    eq2PeakButton.setClickingTogglesState(true);
    eq2PeakButton.setToggleState(true, juce::dontSendNotification);
    eq2PeakButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2PeakButton.addListener(this);
    eq2PeakButton.setVisible(true);
    eq2PeakButton.setBounds(197, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2PeakButton);
    
    eq2PassButton.setButtonText("LOPASS");
    eq2PassButton.setClickingTogglesState(true);
    eq2PassButton.setToggleState(false, juce::dontSendNotification);
    eq2PassButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2PassButton.addListener(this);
    eq2PassButton.setVisible(true);
    eq2PassButton.setBounds(242, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2PassButton);
    
    // Band 2 on/off button
    eq2OnOffButton.setButtonText("BAND 2");
    eq2OnOffButton.setClickingTogglesState(true);
    eq2OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq2OnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2OnOffButton.addListener(this);
    eq2OnOffButton.setVisible(true);
    eq2OnOffButton.setBounds(232, 370, 50, 22); // Below filter type buttons
    equalizerTab->addAndMakeVisible(eq2OnOffButton);
    
    // Band 2 knobs
    eq2FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2FreqKnob.setValue(4000.0);
    eq2FreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2FreqKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2FreqKnob);
    
    eq2FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq2FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2FreqLabel.setJustificationType(juce::Justification::centred);
    eq2FreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2FreqLabel);
    
    eq2QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2QKnob.setRange(0.1, 10.0, 0.1);
    eq2QKnob.setValue(1.0);
    eq2QKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2QKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2QKnob);
    
    eq2QLabel.setText("Q", juce::dontSendNotification);
    eq2QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2QLabel.setJustificationType(juce::Justification::centred);
    eq2QLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2QLabel);
    
    eq2GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2GainKnob.setRange(-15.0, 15.0, 0.1);
    eq2GainKnob.setValue(0.0);
    eq2GainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2GainKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2GainKnob);
    
    eq2GainLabel.setText("GAIN", juce::dontSendNotification);
    eq2GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2GainLabel.setJustificationType(juce::Justification::centred);
    eq2GainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2GainLabel);
    
    // NEW EQ CONTROLS ROW - Additional frequency and Q knobs
    // Band 1 new frequency knob
    eq1NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1NewFreqKnob.setValue(400.0);
    eq1NewFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewFreqKnob.addListener(this);
    eq1NewFreqKnob.setVisible(true);
    eq1NewFreqKnob.setBounds(8, 400, 35, 35); // New row below existing controls
    equalizerTab->addAndMakeVisible(eq1NewFreqKnob);
    
    eq1NewFreqLabel.setText("FREQ1", juce::dontSendNotification);
    eq1NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq1NewFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewFreqLabel.setVisible(true);
    eq1NewFreqLabel.setBounds(8, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewFreqLabel);
    
    // Band 1 new Q knob
    eq1NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewQKnob.setRange(0.1, 30.0, 0.1);
    eq1NewQKnob.setValue(1.0);
    eq1NewQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewQKnob.addListener(this);
    eq1NewQKnob.setVisible(true);
    eq1NewQKnob.setBounds(53, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq1NewQKnob);
    
    eq1NewQLabel.setText("Q1", juce::dontSendNotification);
    eq1NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewQLabel.setJustificationType(juce::Justification::centred);
    eq1NewQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewQLabel.setVisible(true);
    eq1NewQLabel.setBounds(53, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewQLabel);
    
    // Band 1 new gain knob
    eq1NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq1NewGainKnob.setValue(0.0);
    eq1NewGainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewGainKnob.addListener(this);
    eq1NewGainKnob.setVisible(true);
    eq1NewGainKnob.setBounds(98, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq1NewGainKnob);
    
    eq1NewGainLabel.setText("GAIN1", juce::dontSendNotification);
    eq1NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewGainLabel.setJustificationType(juce::Justification::centred);
    eq1NewGainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewGainLabel.setVisible(true);
    eq1NewGainLabel.setBounds(98, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewGainLabel);
    
    // Band 2 new frequency knob
    eq2NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2NewFreqKnob.setValue(4000.0);
    eq2NewFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewFreqKnob.addListener(this);
    eq2NewFreqKnob.setVisible(true);
    eq2NewFreqKnob.setBounds(163, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewFreqKnob);
    
    eq2NewFreqLabel.setText("FREQ2", juce::dontSendNotification);
    eq2NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq2NewFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewFreqLabel.setVisible(true);
    eq2NewFreqLabel.setBounds(163, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewFreqLabel);
    
    // Band 2 new Q knob
    eq2NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewQKnob.setRange(0.1, 30.0, 0.1);
    eq2NewQKnob.setValue(1.0);
    eq2NewQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewQKnob.addListener(this);
    eq2NewQKnob.setVisible(true);
    eq2NewQKnob.setBounds(208, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewQKnob);
    
    eq2NewQLabel.setText("Q2", juce::dontSendNotification);
    eq2NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewQLabel.setJustificationType(juce::Justification::centred);
    eq2NewQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewQLabel.setVisible(true);
    eq2NewQLabel.setBounds(208, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewQLabel);
    
    // Band 2 new gain knob
    eq2NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq2NewGainKnob.setValue(0.0);
    eq2NewGainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewGainKnob.addListener(this);
    eq2NewGainKnob.setVisible(true);
    eq2NewGainKnob.setBounds(253, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewGainKnob);
    
    eq2NewGainLabel.setText("GAIN2", juce::dontSendNotification);
    eq2NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewGainLabel.setJustificationType(juce::Justification::centred);
    eq2NewGainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewGainLabel.setVisible(true);
    eq2NewGainLabel.setBounds(253, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewGainLabel);
    
    // EQ Point labels - positioned above buttons (bracket lines will be painted separately)
    eq1PointLabel.setText("BAND 1", juce::dontSendNotification);
    eq1PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq1PointLabel.setJustificationType(juce::Justification::centred);
    eq1PointLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
    eq1PointLabel.setBounds(45, 314, 70, 15);
    equalizerTab->addAndMakeVisible(eq1PointLabel);
    
    eq2PointLabel.setText("BAND 2", juce::dontSendNotification);
    eq2PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq2PointLabel.setJustificationType(juce::Justification::centred);
    eq2PointLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    eq2PointLabel.setBounds(183, 314, 70, 15);
    equalizerTab->addAndMakeVisible(eq2PointLabel);
    
    // EFFECTS PRESET CONTROLS - Placeholder functionality
    effectsPresetPrevButton.setButtonText("<");
    effectsPresetPrevButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetPrevButton);
    
    effectsPresetNextButton.setButtonText(">");
    effectsPresetNextButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetNextButton);
    
    effectsPresetNameLabel.setText("DEFAULT", juce::dontSendNotification);
    effectsPresetNameLabel.setJustificationType(juce::Justification::centred);
    effectsPresetNameLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    effectsPresetNameLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    addAndMakeVisible(effectsPresetNameLabel);
    
    effectsPresetSaveButton.setButtonText("SAVE");
    effectsPresetSaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetSaveButton);
    
    effectsPresetLoadButton.setButtonText("LOAD");
    effectsPresetLoadButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetLoadButton);
    
    effectsPresetInitButton.setButtonText("INIT");
    effectsPresetInitButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetInitButton);
    
    // Set up preset button callbacks
    effectsPresetSaveButton.onClick = [this]() {
        showSavePresetDialog();
    };
    
    effectsPresetLoadButton.onClick = [this]() {
        showLoadPresetDialog();
    };
    
    effectsPresetInitButton.onClick = [this]() {
        showInitPresetDialog();
    };
    
    effectsPresetPrevButton.onClick = [this]() {
        audioProcessor.previousPreset();
        updatePresetDisplay();
    };
    
    effectsPresetNextButton.onClick = [this]() {
        audioProcessor.nextPreset();
        updatePresetDisplay();
    };
    
    // Add border component behind effects module
    addAndMakeVisible(effectsBorder);
    addAndMakeVisible(effectsModule);
    
    // Initialize envelope display with default values
    updateEnvelopeDisplay();
    
    // Initialize preset display
    updatePresetDisplay();
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
    // Oscillator 2 cleanup now handled by SecondOscillatorComponent
    // Macro controls cleanup now handled by MacroControlsComponent

    // Reset effects preset controls look and feel
    effectsPresetPrevButton.setLookAndFeel(nullptr);
    effectsPresetNextButton.setLookAndFeel(nullptr);
    effectsPresetSaveButton.setLookAndFeel(nullptr);
    effectsPresetLoadButton.setLookAndFeel(nullptr);
    effectsPresetInitButton.setLookAndFeel(nullptr);
    
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
    // drawOsc2PitchControlsBackground(g, osc2PitchControlsBounds); // REMOVED - individual pitch control backgrounds
    // drawOsc2PhaseControlsBackground(g, osc2PhaseControlsBounds); // Removed - was covering the knob
    
    // Filter section background
    drawFilterSectionBackground(g, filterSectionBounds);
    
    // Macro knobs background
    drawMacroKnobsBackground(g, macroKnobsBounds);
    
    // Main window border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::paintOverChildren(juce::Graphics& g)
{
    // Precision alignment grid overlay - symmetrical and aligned to boundaries
    // ENABLED FOR ALIGNMENT - DRAWN OVER ALL CHILD COMPONENTS
    auto gridBounds = getLocalBounds().toFloat();
    auto centerX = gridBounds.getWidth() / 2.0f;
    auto centerY = gridBounds.getHeight() / 2.0f;
    
    // Main synth page grid temporarily commented out
    /*
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
    */
    
    // Draw white bracket lines for EQ bands (only if we're in the effects module tab)
    if (effectsModule.getCurrentTabIndex() == 4) // EQ tab index
    {
        g.setColour(juce::Colours::lime); // Using named color lime
        
        // Get the effects module bounds to calculate relative positions
        auto effectsBounds = effectsModule.getBounds();
        
        // Band 1 bracket lines (upside-down brackets around left group with symmetric gap for label)
        // Top horizontal line - left part (equal length segments with gap for "BAND 1" label)
        g.drawLine(effectsBounds.getX() + 31, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 61, effectsBounds.getY() + 350, 1.0f);
        // Top horizontal line - right part (same length as left part)
        g.drawLine(effectsBounds.getX() + 101, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 131, effectsBounds.getY() + 350, 1.0f);
        // Left vertical line
        g.drawLine(effectsBounds.getX() + 31, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 31, effectsBounds.getY() + 365, 1.0f);
        // Right vertical line  
        g.drawLine(effectsBounds.getX() + 131, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 131, effectsBounds.getY() + 365, 1.0f);
        
        // Band 2 bracket lines (upside-down brackets around right group with symmetric gap for label)
        // Top horizontal line - left part (equal length segments with gap for "BAND 2" label)
        g.drawLine(effectsBounds.getX() + 169, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 199, effectsBounds.getY() + 350, 1.0f);
        // Top horizontal line - right part (same length as left part)
        g.drawLine(effectsBounds.getX() + 239, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 269, effectsBounds.getY() + 350, 1.0f);
        // Left vertical line
        g.drawLine(effectsBounds.getX() + 169, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 169, effectsBounds.getY() + 365, 1.0f);
        // Right vertical line
        g.drawLine(effectsBounds.getX() + 269, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 269, effectsBounds.getY() + 365, 1.0f);
    }
    
    // Draw macro indicators on top of everything
    macroControls.drawMacroIndicators(g);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Original oscillator bounds - unchanged!
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
    layoutLFOModule(bounds);
    layoutEffectsModule(bounds);
    layoutSecondOscillator(bounds);
    
    // MACRO KNOBS - Center bottom positioning (2 rows of 4 columns)
    auto totalBounds = getLocalBounds();
    auto macroKnobSize = 45; // 10% smaller than 50
    auto macroLabelHeight = 15;
    auto macroKnobSpacing = 18; // Slightly smaller spacing too
    auto macroRowSpacing = 10;
    auto totalMacroWidth = (4 * macroKnobSize) + (3 * macroKnobSpacing);
    auto totalMacroHeight = (2 * (macroKnobSize + macroLabelHeight)) + macroRowSpacing;
    
    // Position at center bottom
    auto macroStartX = (totalBounds.getWidth() - totalMacroWidth) / 2;
    auto macroY = totalBounds.getHeight() - totalMacroHeight - 12; // 12px margin from bottom (moved down 3px)
    
    // Store bounds for background drawing (with reduced padding for less width) - keep background at original position
    auto backgroundY = totalBounds.getHeight() - totalMacroHeight - 20; // Original background position
    macroKnobsBounds = juce::Rectangle<int>(macroStartX - 40, backgroundY - 13, totalMacroWidth + 80, totalMacroHeight + 11);

    // Position MacroControlsComponent
    macroControls.setBounds(macroStartX, macroY, totalMacroWidth, totalMacroHeight);
}

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeControlsVolumeKnob)
    {
        audioProcessor.setOsc1Volume(static_cast<float>(volumeControlsVolumeKnob.getValue()));
    }
    else if (slider == &volumeControlsDetuneKnob)
    {
        audioProcessor.setOsc1Detune(static_cast<float>(volumeControlsDetuneKnob.getValue()));
    }
    else if (slider == &volumeControlsStereoWidthKnob)
    {
        audioProcessor.setOsc1StereoWidth(static_cast<float>(volumeControlsStereoWidthKnob.getValue()));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setOsc1Pan(static_cast<float>(volumeControlsPanKnob.getValue()));
    }
    else if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setOsc1Phase(static_cast<float>(phaseControlsPhaseKnob.getValue()));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(adsrAttackKnob.getValue()));

        // If ADSR is linked, update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            secondOscillator.getAttackKnob().setValue(adsrAttackKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Attack(static_cast<float>(adsrAttackKnob.getValue()));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(adsrDecayKnob.getValue()));

        // If ADSR is linked, update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            secondOscillator.getDecayKnob().setValue(adsrDecayKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Decay(static_cast<float>(adsrDecayKnob.getValue()));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(adsrSustainKnob.getValue()));

        // If ADSR is linked, update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            secondOscillator.getSustainKnob().setValue(adsrSustainKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(adsrReleaseKnob.getValue()));

        // If ADSR is linked, update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            secondOscillator.getReleaseKnob().setValue(adsrReleaseKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Release(static_cast<float>(adsrReleaseKnob.getValue()));
        }

        updateEnvelopeDisplay();
    }
    // Oscillator 2 slider handling now in SecondOscillatorComponent
    // Filter controls now handled by FilterControlComponent
    // Chorus effect sliders now handled by ChorusComponent
    // Flanger effect sliders - now handled by FlangerComponent
    // Phaser effect sliders - now handled by PhaserComponent
    // Compressor sliders now handled by CompressorComponent
    // Distortion sliders now handled by DistortionComponent
    
    
    /*
    else if (slider == &pulseWidthSlider)
    {
        audioProcessor.setOsc1PulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    }
    */
    
    // REVERB CONTROLS - now handled by ReverbComponent
    // EQ CONTROLS
    else if (slider == &eq1FreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1FreqKnob.getValue()));
        parametricEQ.getBand(0).frequency = static_cast<float>(eq1FreqKnob.getValue());
        eq1NewFreqKnob.setValue(eq1FreqKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq1QKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1QKnob.getValue()));
        parametricEQ.getBand(0).q = static_cast<float>(eq1QKnob.getValue());
        eq1NewQKnob.setValue(eq1QKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.repaint();
    }
    else if (slider == &eq1GainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1GainKnob.getValue()));
        parametricEQ.getBand(0).gain = static_cast<float>(eq1GainKnob.getValue());
        eq1NewGainKnob.setValue(eq1GainKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2FreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2FreqKnob.getValue()));
        parametricEQ.getBand(1).frequency = static_cast<float>(eq2FreqKnob.getValue());
        eq2NewFreqKnob.setValue(eq2FreqKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2QKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2QKnob.getValue()));
        parametricEQ.getBand(1).q = static_cast<float>(eq2QKnob.getValue());
        eq2NewQKnob.setValue(eq2QKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2GainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2GainKnob.getValue()));
        parametricEQ.getBand(1).gain = static_cast<float>(eq2GainKnob.getValue());
        eq2NewGainKnob.setValue(eq2GainKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    // NEW EQ KNOBS - Additional controls that auto-sync with graphic
    else if (slider == &eq1NewFreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1NewFreqKnob.getValue()));
        parametricEQ.getBand(0).frequency = static_cast<float>(eq1NewFreqKnob.getValue());
        eq1FreqKnob.setValue(eq1NewFreqKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq1NewQKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1NewQKnob.getValue()));
        parametricEQ.getBand(0).q = static_cast<float>(eq1NewQKnob.getValue());
        eq1QKnob.setValue(eq1NewQKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewFreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2NewFreqKnob.getValue()));
        parametricEQ.getBand(1).frequency = static_cast<float>(eq2NewFreqKnob.getValue());
        eq2FreqKnob.setValue(eq2NewFreqKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewQKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2NewQKnob.getValue()));
        parametricEQ.getBand(1).q = static_cast<float>(eq2NewQKnob.getValue());
        eq2QKnob.setValue(eq2NewQKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq1NewGainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1NewGainKnob.getValue()));
        parametricEQ.getBand(0).gain = static_cast<float>(eq1NewGainKnob.getValue());
        eq1GainKnob.setValue(eq1NewGainKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewGainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2NewGainKnob.getValue()));
        parametricEQ.getBand(1).gain = static_cast<float>(eq2NewGainKnob.getValue());
        eq2GainKnob.setValue(eq2NewGainKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    // Macro knobs - update all linked parameters
    // Macro controls now handled by MacroControlsComponent
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
    // Oscillator 2 button handling now in SecondOscillatorComponent
    // Filter controls now handled by FilterControlComponent
    // EQ On/Off button
    else if (button == &eqOnOffButton)
    {
        if (eqOnOffButton.getToggleState())
        {
            eqOnOffButton.setButtonText("EQ ON");
            audioProcessor.setEQEnabled(true);
            parametricEQ.setVisible(true);
            parametricEQ.syncWithDSPState(); // Sync when enabled
        }
        else
        {
            eqOnOffButton.setButtonText("EQ OFF");
            audioProcessor.setEQEnabled(false);
            parametricEQ.setVisible(true); // Keep visible but show it's disabled
        }
        parametricEQ.repaint();
    }
    // EQ Band 1 buttons
    else if (button == &eq1PeakButton)
    {
        if (eq1PeakButton.getToggleState())
        {
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ1Type(0); // 0 = Peak
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1PeakButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq1ShelfButton)
    {
        if (eq1ShelfButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ1Type(1); // 1 = Low Shelf
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1ShelfButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq1PassButton)
    {
        if (eq1PassButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ1Type(2); // 2 = High Pass
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1PassButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    // EQ Band 2 buttons
    else if (button == &eq2PeakButton)
    {
        if (eq2PeakButton.getToggleState())
        {
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ2Type(0); // 0 = Peak
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2PeakButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq2ShelfButton)
    {
        if (eq2ShelfButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ2Type(1); // 1 = High Shelf
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2ShelfButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq2PassButton)
    {
        if (eq2PassButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ2Type(2); // 2 = Low Pass
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2PassButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    // EQ Band on/off buttons
    else if (button == &eq1OnOffButton)
    {
        audioProcessor.setEQ1Enabled(eq1OnOffButton.getToggleState());
        parametricEQ.syncWithDSPState();
    }
    else if (button == &eq2OnOffButton)
    {
        audioProcessor.setEQ2Enabled(eq2OnOffButton.getToggleState());
        parametricEQ.syncWithDSPState();
    }
    // Chorus effect power button - handled by ChorusComponent
    // Flanger effect buttons - now handled by FlangerComponent
    // Phaser effect buttons - now handled by PhaserComponent
    // Compressor buttons now handled by CompressorComponent
    
    // REVERB CONTROLS - now handled by ReverbComponent
}

bool SynthesizerComponent::hitTest(int x, int y)
{
    // Check if this click should be handled by arc interaction
    auto mapping = findMacroMappingAtPosition(juce::Point<int>(x, y));
    if (mapping != nullptr)
    {
        // Arc handle found, this component should handle the event
        return true;
    }
    
    // For everything else, use default hit testing (sliders will handle their own restricted areas)
    return juce::Component::hitTest(x, y);
}

void SynthesizerComponent::mouseDown(const juce::MouseEvent& event)
{
    // FIRST: Check for arc interaction before any other mouse handling
    // This ensures arc handles get priority over other components
    juce::Logger::writeToLog("Mouse down at position: " + juce::String(event.getPosition().x) + ", " + juce::String(event.getPosition().y));
    auto mapping = findMacroMappingAtPosition(event.getPosition());
    if (mapping != nullptr)
    {
        juce::Logger::writeToLog("Found mapping for macro " + juce::String(mapping->macroIndex));
        isDraggingArc = true;
        draggedMapping = mapping;
        dragStartPoint = event.getPosition();
        
        // Determine which endpoint we're dragging by checking handle positions
        auto bounds = mapping->targetSlider->getBounds();
        if (mapping->targetSlider->getParentComponent() != this)
        {
            bounds = getLocalArea(mapping->targetSlider->getParentComponent(), bounds);
        }
        
        auto arcCenter = bounds.getCentre().toFloat();
        float radius = bounds.getWidth() * 0.4f;
        float handleRadius = 5.0f;
        
        float minAngle = juce::jmap(static_cast<float>(mapping->userMinRange), 
                                  static_cast<float>(mapping->targetSlider->getMinimum()),
                                  static_cast<float>(mapping->targetSlider->getMaximum()),
                                  2.35619f, 0.785398f);
        float maxAngle = juce::jmap(static_cast<float>(mapping->userMaxRange), 
                                  static_cast<float>(mapping->targetSlider->getMinimum()),
                                  static_cast<float>(mapping->targetSlider->getMaximum()),
                                  2.35619f, 0.785398f);
        
        float minX = arcCenter.x + radius * std::cos(minAngle - juce::MathConstants<float>::halfPi);
        float minY = arcCenter.y + radius * std::sin(minAngle - juce::MathConstants<float>::halfPi);
        float maxX = arcCenter.x + radius * std::cos(maxAngle - juce::MathConstants<float>::halfPi);
        float maxY = arcCenter.y + radius * std::sin(maxAngle - juce::MathConstants<float>::halfPi);
        
        float distToMin = event.getPosition().toFloat().getDistanceFrom(juce::Point<float>(minX, minY));
        float distToMax = event.getPosition().toFloat().getDistanceFrom(juce::Point<float>(maxX, maxY));
        
        draggingMinRange = (distToMin < distToMax);
        juce::Logger::writeToLog("Dragging " + juce::String(draggingMinRange ? "min" : "max") + " range handle");
        repaint();
        return; // Don't process other mouse events when dragging arc
    }
    
    // SECOND: Handle other component-specific mouse events only if no arc was clicked
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
    // Oscillator 2 pitch controls now handled by SecondOscillatorComponent
    // Reverb type cycling now handled by ReverbComponent
    // Phaser poles value label clicking now handled by PhaserComponent
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
    // Oscillator 2 pitch controls dragging now handled by SecondOscillatorComponent
    // Reverb type dragging now handled by ReverbComponent
    // Phaser poles dragging now handled by PhaserComponent
    
    // Handle arc dragging
    if (isDraggingArc && draggedMapping != nullptr)
    {
        auto bounds = draggedMapping->targetSlider->getBounds();
        if (draggedMapping->targetSlider->getParentComponent() != this)
        {
            bounds = getLocalArea(draggedMapping->targetSlider->getParentComponent(), bounds);
        }
        
        auto arcCenter = bounds.getCentre().toFloat();
        auto mousePos = event.getPosition().toFloat();
        auto centerToMouse = mousePos - arcCenter;
        
        // Calculate angle from center to mouse position
        float mouseAngle = std::atan2(centerToMouse.y, centerToMouse.x);
        
        // Normalize angle to 0-2π range
        if (mouseAngle < 0) mouseAngle += 2.0f * juce::MathConstants<float>::pi;
        
        // Convert angle to slider value range (adjust for knob's angle mapping)
        // Knobs typically go from 2.35619f (bottom-left) to 0.785398f (bottom-right)
        float normalizedAngle;
        if (mouseAngle > juce::MathConstants<float>::pi)
        {
            normalizedAngle = juce::jmap(mouseAngle, 2.35619f, 2.0f * juce::MathConstants<float>::pi, 0.0f, 0.5f);
        }
        else
        {
            normalizedAngle = juce::jmap(mouseAngle, 0.0f, 0.785398f, 0.5f, 1.0f);
        }
        
        normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
        
        // Convert to slider's value range
        double newValue = juce::jmap(static_cast<double>(normalizedAngle), 
                                   0.0, 1.0,
                                   draggedMapping->targetSlider->getMinimum(),
                                   draggedMapping->targetSlider->getMaximum());
        
        // Update the appropriate range
        if (draggingMinRange)
        {
            draggedMapping->userMinRange = newValue;
            // Ensure min doesn't exceed max
            if (draggedMapping->userMinRange > draggedMapping->userMaxRange)
                draggedMapping->userMinRange = draggedMapping->userMaxRange;
        }
        else
        {
            draggedMapping->userMaxRange = newValue;
            // Ensure max doesn't go below min
            if (draggedMapping->userMaxRange < draggedMapping->userMinRange)
                draggedMapping->userMaxRange = draggedMapping->userMinRange;
        }
        
        repaint();
    }
}

void SynthesizerComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
    // Oscillator 2 drag states now handled by SecondOscillatorComponent
    isDraggingReverbType = false;
    // isDraggingPhaserPoles handled by PhaserComponent
    
    // Cleanup arc dragging
    if (isDraggingArc)
    {
        isDraggingArc = false;
        draggedMapping = nullptr;
        repaint();
    }
}

void SynthesizerComponent::updateEnvelopeDisplay()
{
    adsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(adsrAttackKnob.getValue()),
        static_cast<float>(adsrDecayKnob.getValue()),
        static_cast<float>(adsrSustainKnob.getValue()),
        static_cast<float>(adsrReleaseKnob.getValue())
    );

    // Oscillator 2 envelope display now handled by SecondOscillatorComponent
    secondOscillator.updateEnvelopeDisplay();
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

void SynthesizerComponent::layoutLFOModule(juce::Rectangle<int>& bounds)
{
    // Position LFO module in bottom left corner - height reduced by 2 pixels from bottom
    auto totalBounds = getLocalBounds();
    auto lfoArea = juce::Rectangle<int>(20, totalBounds.getHeight() - 162, 409, 142); // Width: 409, Height: 144-2=142, Y: -162
    lfoModule.setBounds(lfoArea);
}

void SynthesizerComponent::layoutEffectsModule(juce::Rectangle<int>& bounds)
{
    // Position effects module with reduced width and increased height
    auto totalBounds = getLocalBounds();
    int effectsWidth = 300;  // Much narrower than oscillators
    int effectsHeight = 530; // Added 50px more height for additional content space
    int borderPadding = 6; // Extra space for border
    
    int centerX = (totalBounds.getWidth() - effectsWidth - borderPadding * 2) / 2;
    int centerY = (totalBounds.getHeight() - effectsHeight - borderPadding * 2) / 2 - 50; // Move up 30px total for preset controls
    
    // Preset controls area above the effects module
    auto presetControlsHeight = 35;
    auto presetY = centerY - presetControlsHeight - 10; // 10px gap above effects module
    
    // Preset controls layout
    auto buttonWidth = 40;
    auto buttonHeight = 25;
    auto labelWidth = 120;
    auto spacing = 10;
    
    // Calculate positions for preset controls (centered above effects module)
    auto totalPresetWidth = (4 * buttonWidth) + labelWidth + (4 * spacing);
    auto presetStartX = centerX + borderPadding + (effectsWidth - totalPresetWidth) / 2;
    
    // Previous button
    effectsPresetPrevButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Preset name label
    effectsPresetNameLabel.setBounds(presetStartX, presetY, labelWidth, presetControlsHeight);
    presetStartX += labelWidth + spacing;
    
    // Next button
    effectsPresetNextButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Save button - moved up 10 pixels total
    effectsPresetSaveButton.setBounds(presetStartX, presetY - 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Load button - moved up 10 pixels total
    effectsPresetLoadButton.setBounds(presetStartX, presetY - 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Init button - moved down 14 pixels and left 75 pixels
    effectsPresetInitButton.setBounds(presetStartX - 75, presetY + 19, buttonWidth, buttonHeight);
    
    // Position border component (larger to encompass effects module)
    auto borderArea = juce::Rectangle<int>(centerX, centerY, effectsWidth + borderPadding * 2, effectsHeight + borderPadding * 2);
    effectsBorder.setBounds(borderArea);
    
    // Position effects module inside the border
    auto effectsArea = juce::Rectangle<int>(centerX + borderPadding, centerY + borderPadding, effectsWidth, effectsHeight);
    effectsModule.setBounds(effectsArea);
    
    // Layout effects controls within the tab content area
    auto tabContentArea = effectsArea.reduced(8, 35); // Account for tab bar and borders
    // ChorusComponent handles its own layout
    // CompressorComponent handles its own layout
    // DelayComponent handles its own layout
    // DistortionComponent handles its own layout
}





void SynthesizerComponent::layoutSecondOscillator(juce::Rectangle<int>& bounds)
{
    // Position oscillator 2 in top-right corner - match oscillator 1 size exactly
    auto totalWidth = getWidth();
    auto osc2Width = 320 + 63 + 24; // Same calculation as oscillator 1
    auto osc2Height = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20 + 24;
    auto osc2X = totalWidth - osc2Width - 20;
    auto osc2Y = 20;

    auto osc2Section = juce::Rectangle<int>(osc2X, osc2Y, osc2Width, osc2Height);

    // Apply group offset
    auto offsetOsc2Section = osc2Section.translated(
        static_cast<int>(secondOscillatorGroupOffsetX),
        static_cast<int>(secondOscillatorGroupOffsetY)
    );

    // Store bounds for background drawing
    secondOscillatorBounds = offsetOsc2Section;

    // Position the SecondOscillatorComponent
    secondOscillator.setBounds(offsetOsc2Section);

    // Position ADSR Link button (relative to oscillator 1's volumeKnobsBounds)
    auto linkButtonHeight = 25;
    auto linkButtonWidth = 100;
    auto linkButtonArea = juce::Rectangle<int>(
        volumeKnobsBounds.getRight() - linkButtonWidth - 40 + 760,
        volumeKnobsBounds.getBottom() - linkButtonHeight - 5 + 160,
        linkButtonWidth,
        linkButtonHeight
    );
    secondOscillator.getAdsrLinkButton().setBounds(linkButtonArea);

    // Filter section positioning - under oscillator 2 in landscape format
    auto filterSectionX = offsetOsc2Section.getX();
    auto filterSectionY = offsetOsc2Section.getBottom() - 5;
    auto filterSectionWidth = offsetOsc2Section.getWidth();
    auto filterSectionHeight = 141;

    // Store bounds for background drawing
    filterSectionBounds = juce::Rectangle<int>(filterSectionX, filterSectionY, filterSectionWidth, filterSectionHeight);
    
    // Filter section - handled by FilterControlComponent
    filterModule.setBounds(filterSectionBounds);
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
    slotBounds.removeFromTop(20); // increased spacing from 10 to 20 (moves ADSR row down 10 pixels)
    
    // ADSR knobs metal slots - 4 knobs in a row - matching oscillator 1 style with minimal spacing
    auto adsrRow = slotBounds.removeFromTop(80);
    auto adsrKnobWidth = (adsrRow.getWidth() - 3) / 4; // Minimal spacing: reduced from 9 to 3 (3x1 instead of 3x3)
    
    // Attack knob slot - elongated metal (matching oscillator 1) - extended down 2px and moved down 2px
    auto attackArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40 + 5, attackArea.getCentreY() - 60 + 7 - 2.5f + 2, 80, 117, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39 + 5, attackArea.getCentreY() - 59 + 7 - 2.5f + 2, 78, 115, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37 + 5, attackArea.getCentreY() - 57 + 7 - 2.5f + 2, 74, 111, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Decay knob slot - elongated metal (matching oscillator 1) - moved down 5px and right 3px, shortened by 2px
    auto decayArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40 + 3, decayArea.getCentreY() - 60 + 5 + 1, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39 + 3, decayArea.getCentreY() - 59 + 5 + 1, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37 + 3, decayArea.getCentreY() - 57 + 5 + 1, 74, 112, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Sustain knob slot - elongated metal (matching oscillator 1) - height reduced by 2px, moved down 5px, moved left 1px
    auto sustainArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40 - 1, sustainArea.getCentreY() - 60 + 1 + 5, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39 - 1, sustainArea.getCentreY() - 59 + 1 + 5, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37 - 1, sustainArea.getCentreY() - 57 + 1 + 5, 74, 112, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Release knob slot - elongated metal (matching oscillator 1) - height reduced by 2px, moved down 5px, moved left 5px
    auto releaseArea = adsrRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40 - 5, releaseArea.getCentreY() - 60 + 1 + 5, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39 - 5, releaseArea.getCentreY() - 59 + 1 + 5, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37 - 5, releaseArea.getCentreY() - 57 + 1 + 5, 74, 112, 2.0f, 1.0f);
    
    // Duplicate ADSR knob backgrounds for stereo knob row (121px lower)
    
    // Duplicate Attack knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40 + 5, attackArea.getCentreY() - 60 + 7 - 2.5f + 2 + 121, 80, 117, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39 + 5, attackArea.getCentreY() - 59 + 7 - 2.5f + 2 + 121, 78, 115, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37 + 5, attackArea.getCentreY() - 57 + 7 - 2.5f + 2 + 121, 74, 111, 2.0f, 1.0f);
    
    // Duplicate Decay knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40 + 3, decayArea.getCentreY() - 60 + 5 + 1 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39 + 3, decayArea.getCentreY() - 59 + 5 + 1 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37 + 3, decayArea.getCentreY() - 57 + 5 + 1 + 121, 74, 112, 2.0f, 1.0f);
    
    // Duplicate Sustain knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40 - 1, sustainArea.getCentreY() - 60 + 1 + 5 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39 - 1, sustainArea.getCentreY() - 59 + 1 + 5 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37 - 1, sustainArea.getCentreY() - 57 + 1 + 5 + 121, 74, 112, 2.0f, 1.0f);
    
    // Duplicate Release knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40 - 5, releaseArea.getCentreY() - 60 + 1 + 5 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39 - 5, releaseArea.getCentreY() - 59 + 1 + 5 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37 - 5, releaseArea.getCentreY() - 57 + 1 + 5 + 121, 74, 112, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Volume controls metal slots - only stereo knob background enabled
    auto volumeRow = slotBounds.removeFromTop(80);
    auto volumeKnobWidth = (volumeRow.getWidth() - 45) / 4;
    
    // Volume knob slot - BACKGROUND REMOVED
    auto volumeArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(volumeArea.getCentreX() - 40, volumeArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(volumeArea.getCentreX() - 39, volumeArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(volumeArea.getCentreX() - 37, volumeArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Detune knob slot - BACKGROUND REMOVED
    auto detuneArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(detuneArea.getCentreX() - 40, detuneArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(detuneArea.getCentreX() - 39, detuneArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(detuneArea.getCentreX() - 37, detuneArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Stereo knob slot - only duplicate background enabled, shifted down 100 pixels
    auto stereoArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // Original stereo background removed - only keeping the duplicate
    
    // Osc2 Phase knob background - custom positioned for phase control (shifted down 244 pixels and left 70 pixels from stereo position)
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoArea.getCentreX() - 40 - 70, stereoArea.getCentreY() - 50 + 244, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 39 - 70, stereoArea.getCentreY() - 49 + 244, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 37 - 70, stereoArea.getCentreY() - 47 + 244, 74, 94, 2.0f, 1.0f);
    
    // volumeRow.removeFromLeft(15); // spacing
    
    // Pan knob slot - BACKGROUND REMOVED
    // auto panArea = volumeRow;
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(panArea.getCentreX() - 40, panArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(panArea.getCentreX() - 39, panArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(panArea.getCentreX() - 37, panArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Pitch controls metal slots - 3 pitch controls + 1 voices control - matching oscillator 1 style, squeezed very close together
    auto pitchRow = slotBounds.removeFromTop(60);
    auto totalWidth = 4 * 70; // 4 backgrounds at 70px width + no gaps = completely touching
    auto pitchControlWidth = 70; // Fixed width per control
    auto gapWidth = 0; // No gap between backgrounds - completely touching
    auto startX = (pitchRow.getWidth() - totalWidth) / 2 - 50 + 20 - 5 - 5 - 3; // Center the squeezed row, then move left 50px, then right 20px, then left 5px, then left 5px more, then left 3px
    auto squeezedRow = pitchRow.withX(pitchRow.getX() + startX).withWidth(totalWidth).translated(0, 30 + 40 - 10 + 5 - 1 - 1 - 1); // Move down 30px, then down 40px more, then up 10px, then down 5px, then up 1px, then up 1px more, then up 1px again
    
    // Octave control slot - matching oscillator 1 style
    auto octaveArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveArea.getCentreX() - 35, octaveArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 34, octaveArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 32, octaveArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Semitone control slot - matching oscillator 1 style
    auto semitoneArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneArea.getCentreX() - 35, semitoneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 34, semitoneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 32, semitoneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Fine tune control slot - matching oscillator 1 style
    auto fineTuneArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneArea.getCentreX() - 35, fineTuneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 34, fineTuneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 32, fineTuneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Voice count control slot - matching oscillator 1 style
    auto voiceArea = squeezedRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceArea.getCentreX() - 35, voiceArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 34, voiceArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 32, voiceArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Phase control slot - single knob - BACKGROUND REMOVED
    auto phaseRow = slotBounds.removeFromTop(60);
    // auto phaseArea = phaseRow.removeFromRight(80); // Position on right side
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(phaseArea.getCentreX() - 40, phaseArea.getCentreY() - 40, 80, 80, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(phaseArea.getCentreX() - 39, phaseArea.getCentreY() - 39, 78, 78, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(phaseArea.getCentreX() - 37, phaseArea.getCentreY() - 37, 74, 74, 2.0f, 1.0f);
}

void SynthesizerComponent::drawFilterSectionBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same style as oscillator backgrounds
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Main background (dark gray)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Create raised effect with gradients
    auto raisedBounds = masterOutlineBounds.reduced(4.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff505050).withAlpha(0.7f));
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
    
    // Add "Filter" label to top center of module
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 30, raisedBounds.getY() + 5, 60, 20);
    
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
    
    // Engraved 3D text effect - same as oscillator
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("FILTER", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("FILTER", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("FILTER", textBounds, juce::Justification::centred, true);
}

void SynthesizerComponent::drawMacroKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same style as oscillator backgrounds but no screws
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Main background (dark gray)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Create raised effect with gradients
    auto raisedBounds = masterOutlineBounds.reduced(4.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff505050).withAlpha(0.7f));
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
    
    // Add "MACRO" label to top center of module
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 35, raisedBounds.getY() + 5, 70, 20);
    
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
    
    // Engraved 3D text effect - same as oscillator
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("MACRO", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("MACRO", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("MACRO", textBounds, juce::Justification::centred, true);
}

//==============================================================================
// Macro Mapping System Implementation

void SynthesizerComponent::createMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    macroControls.createMacroMapping(macroIndex, targetSlider);
}

juce::Slider* SynthesizerComponent::findSliderAt(juce::Point<int> position)
{
    // List of all sliders to check for drop detection
    std::vector<juce::Slider*> allSliders = {
        // Volume and oscillator controls
        &volumeControlsVolumeKnob, &volumeControlsDetuneKnob, &volumeControlsStereoWidthKnob, &volumeControlsPanKnob,
        &phaseControlsPhaseKnob, &secondOscillator.getVolumeKnob(), &secondOscillator.getDetuneKnob(), &secondOscillator.getStereoKnob(), &secondOscillator.getPanKnob(), &secondOscillator.getPhaseKnob(),

        // ADSR controls
        &adsrAttackKnob, &adsrDecayKnob, &adsrSustainKnob, &adsrReleaseKnob,
        &secondOscillator.getAttackKnob(), &secondOscillator.getDecayKnob(), &secondOscillator.getSustainKnob(), &secondOscillator.getReleaseKnob(),
        
        // Filter controls
        &filterModule.filterCutoffKnob, &filterModule.filterResonanceKnob,
        
        // EQ controls - using only the NEW knobs to avoid duplicates
        &eq1NewFreqKnob, &eq1NewQKnob, &eq1NewGainKnob, &eq2NewFreqKnob, &eq2NewQKnob, &eq2NewGainKnob,
        
        // Effects controls
        &chorusModule.chorusRateKnob, &chorusModule.chorusDelay1Knob, &chorusModule.chorusDelay2Knob, &chorusModule.chorusDepthKnob, &chorusModule.chorusFeedKnob, &chorusModule.chorusLpfKnob, &chorusModule.chorusMixKnob,
        &flangerModule.flangerMixKnob, &flangerModule.flangerRateKnob, &flangerModule.flangerDepthKnob, &flangerModule.flangerFeedbackKnob, &flangerModule.flangerPhaseKnob,
        &phaserModule.phaserMixKnob, &phaserModule.phaserRateKnob, &phaserModule.phaserDepth1Knob, &phaserModule.phaserDepth2Knob, &phaserModule.phaserFeedbackKnob, &phaserModule.phaserPhaseKnob, &phaserModule.phaserFrequencyKnob,
        &compressorModule.compressorThresholdKnob, &compressorModule.compressorRatioKnob, &compressorModule.compressorAttackKnob, &compressorModule.compressorReleaseKnob, &compressorModule.compressorGainKnob, &compressorModule.compressorMixKnob,
        &reverbModule.reverbMixKnob, &reverbModule.reverbSizeKnob, &reverbModule.reverbPreDelayKnob, &reverbModule.reverbLowCutKnob, &reverbModule.reverbHighCutKnob, &reverbModule.reverbDampKnob, &reverbModule.reverbWidthKnob
    };
    
    // Use MacroSystem to find slider at position with coordinate conversion
    return macroControls.getMacroSystem().findSliderAt(position, allSliders);
}



void SynthesizerComponent::triggerParameterUpdate(juce::Slider* slider, double newValue)
{
    // Mirror the logic from sliderValueChanged to update audio processor parameters
    // without sending slider notifications (to avoid visual movement)
    
    if (slider == &volumeControlsVolumeKnob)
    {
        audioProcessor.setOsc1Volume(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsDetuneKnob)
    {
        audioProcessor.setOsc1Detune(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsStereoWidthKnob)
    {
        audioProcessor.setOsc1StereoWidth(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setOsc1Pan(static_cast<float>(newValue));
    }
    else if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setOsc1Phase(static_cast<float>(newValue));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Attack(static_cast<float>(newValue));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Decay(static_cast<float>(newValue));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Sustain(static_cast<float>(newValue));
        }

        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Release(static_cast<float>(newValue));
        }

        updateEnvelopeDisplay();
    }
    // Add more slider checks as needed for other parameters
    // For now, focusing on the main commonly-used parameters
    // Filter controls now handled by FilterControlComponent
    // EQ parameters
    else if (slider == &eq1FreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1QKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1GainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2FreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2QKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2GainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    // New EQ knobs
    else if (slider == &eq1NewFreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1NewQKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1NewGainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewFreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewQKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewGainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    // Oscillator 2 controls now handled by SecondOscillatorComponent
    // Chorus FX - handled by ChorusComponent
    else if (slider == &chorusModule.chorusRateKnob)
    {
        audioProcessor.setChorusRate(static_cast<float>(newValue));
    }
    else if (slider == &chorusModule.chorusDelay1Knob)
    {
        audioProcessor.setChorusDelay1(static_cast<float>(newValue));
    }
    else if (slider == &chorusModule.chorusDelay2Knob)
    {
        audioProcessor.setChorusDelay2(static_cast<float>(newValue));
    }
    else if (slider == &chorusModule.chorusDepthKnob)
    {
        audioProcessor.setChorusDepth(static_cast<float>(newValue));
    }
    else if (slider == &chorusModule.chorusFeedKnob)
    {
        audioProcessor.setChorusFeedback(static_cast<float>(newValue));
    }
    else if (slider == &chorusModule.chorusLpfKnob)
    {
        float filterFreq = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 20.0f, 20000.0f);
        audioProcessor.setChorusLPF(filterFreq);
    }
    else if (slider == &chorusModule.chorusMixKnob)
    {
        audioProcessor.setChorusMix(static_cast<float>(newValue));
    }
    // Flanger FX - now handled by FlangerComponent
    // Phaser FX - now handled by PhaserComponent
    // Compressor FX - handled by CompressorComponent
    else if (slider == &compressorModule.compressorThresholdKnob)
    {
        audioProcessor.setCompressorThreshold(static_cast<float>(newValue));
    }
    else if (slider == &compressorModule.compressorRatioKnob)
    {
        float ratio = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 20.0f);
        audioProcessor.setCompressorRatio(ratio);
    }
    else if (slider == &compressorModule.compressorAttackKnob)
    {
        float attack = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 0.1f, 100.0f);
        audioProcessor.setCompressorAttack(attack);
    }
    else if (slider == &compressorModule.compressorReleaseKnob)
    {
        float release = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 1000.0f);
        audioProcessor.setCompressorRelease(release);
    }
    else if (slider == &compressorModule.compressorGainKnob)
    {
        audioProcessor.setCompressorGain(static_cast<float>(newValue));
    }
    else if (slider == &compressorModule.compressorMixKnob)
    {
        audioProcessor.setCompressorMix(static_cast<float>(newValue));
    }
    // Delay FX
    // Reverb FX - now handled by ReverbComponent
}

MacroMapping* SynthesizerComponent::findMacroMappingAtPosition(juce::Point<int> position)
{
    return macroControls.getMacroSystem().findMacroMappingAtPosition(position);
}


void SynthesizerComponent::updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition)
{
    macroControls.getMacroSystem().updateMappingRange(mapping, dragPosition);
    repaint();
}

// Preset Management Implementation

void SynthesizerComponent::showSavePresetDialog()
{
    // Create default filename with timestamp
    juce::String defaultName = "MyPreset_" + juce::String(juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S"));
    juce::File defaultFile = audioProcessor.getPresetDirectory().getChildFile(defaultName + ".sxs2");
    
    auto chooser = std::make_shared<juce::FileChooser>("Save Preset",
                                                       defaultFile,
                                                       "*.sxs2",
                                                       true);
    
    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;
                                
                            if (file.existsAsFile())
                            {
                                // File already exists, ask for confirmation
                                int result = juce::AlertWindow::showYesNoCancelBox(
                                    juce::AlertWindow::QuestionIcon,
                                    "File Exists",
                                    "The file \"" + file.getFileName() + "\" already exists. Do you want to replace it?",
                                    "Replace",
                                    "Cancel",
                                    "",
                                    nullptr,
                                    nullptr);
                                    
                                if (result != 1) // If not "Replace"
                                    return;
                            }
                            
                            // Ensure the file has the correct extension
                            if (!file.hasFileExtension(".sxs2"))
                            {
                                file = file.withFileExtension(".sxs2");
                            }
                            
                            // Save the preset
                            if (audioProcessor.savePresetToFile(file, "User created preset"))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset saved successfully to:\n" + file.getFullPathName());
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to save preset to:\n" + file.getFullPathName());
                                });
                            }
                        });
}

void SynthesizerComponent::showLoadPresetDialog()
{
    auto chooser = std::make_shared<juce::FileChooser>("Load Preset",
                                                       audioProcessor.getPresetDirectory(),
                                                       "*.sxs2",
                                                       true);
    
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;
                                
                            if (audioProcessor.loadPreset(file.getFullPathName()))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset '" + file.getFileNameWithoutExtension() + "' loaded successfully!");
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to load preset: " + file.getFileName());
                                });
                            }
                        });
}

void SynthesizerComponent::showInitPresetDialog()
{
    juce::AlertWindow::showYesNoCancelBox(
        juce::AlertWindow::QuestionIcon,
        "Initialize Synthesizer",
        "This will reset all parameters to their default values. Any unsaved changes will be lost.\n\nAre you sure you want to continue?",
        "Yes, Initialize",
        "Cancel",
        juce::String(),
        this,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1) // Yes button clicked
            {
                audioProcessor.initializeAllParameters();
                updatePresetDisplay();
            }
        })
    );
}

void SynthesizerComponent::updatePresetDisplay()
{
    effectsPresetNameLabel.setText(audioProcessor.getCurrentPresetName(), 
                                 juce::dontSendNotification);
    repaint();
}

void SynthesizerComponent::updateAllGuiControls()
{
    // Main synthesizer controls (using correct control names)
    volumeControlsVolumeKnob.setValue(audioProcessor.getOsc1Volume(), juce::dontSendNotification);
    volumeControlsDetuneKnob.setValue(audioProcessor.getOsc1Detune(), juce::dontSendNotification);
    volumeControlsStereoWidthKnob.setValue(audioProcessor.getOsc1StereoWidth(), juce::dontSendNotification);
    volumeControlsPanKnob.setValue(audioProcessor.getOsc1Pan(), juce::dontSendNotification);
    phaseControlsPhaseKnob.setValue(audioProcessor.getOsc1Phase(), juce::dontSendNotification);
    
    // Main ADSR envelope
    adsrAttackKnob.setValue(audioProcessor.getOsc1Attack(), juce::dontSendNotification);
    adsrDecayKnob.setValue(audioProcessor.getOsc1Decay(), juce::dontSendNotification);
    adsrSustainKnob.setValue(audioProcessor.getOsc1Sustain(), juce::dontSendNotification);
    adsrReleaseKnob.setValue(audioProcessor.getOsc1Release(), juce::dontSendNotification);
    
    // Oscillator 1 controls
    pulseWidthSlider.setValue(audioProcessor.getOsc1PulseWidth(), juce::dontSendNotification);
    
    // Update oscillator 1 wave type buttons
    int osc1Type = audioProcessor.getOsc1Type();
    waveTypeSineButton.setToggleState(osc1Type == 0, juce::dontSendNotification);
    waveTypeSawButton.setToggleState(osc1Type == 1, juce::dontSendNotification);
    waveTypeSquareButton.setToggleState(osc1Type == 2, juce::dontSendNotification);
    waveTypeTriangleButton.setToggleState(osc1Type == 3, juce::dontSendNotification);
    waveTypeWhiteNoiseButton.setToggleState(osc1Type == 4, juce::dontSendNotification);
    waveTypePinkNoiseButton.setToggleState(osc1Type == 5, juce::dontSendNotification);
    waveTypeRandomPhaseButton.setToggleState(osc1Type == 6, juce::dontSendNotification);
    
    // Oscillator 2 controls now handled by SecondOscillatorComponent
    secondOscillator.updateAllGuiControls();
    
    // Filter controls - handled by FilterControlComponent
    filterModule.syncWithDSPState();
    
    // Effects controls - update the knobs with loaded preset values
    
    // Chorus controls - handled by ChorusComponent
    chorusModule.syncWithDSPState();
    
    // Compressor controls - handled by CompressorComponent
    compressorModule.syncWithDSPState();
    
    // Delay controls - handled by DelayComponent
    delayModule.syncWithDSPState();
    
    
    // EQ controls
    eq1FreqKnob.setValue(audioProcessor.getEQ1Frequency(), juce::dontSendNotification);
    eq1QKnob.setValue(audioProcessor.getEQ1Q(), juce::dontSendNotification);
    eq1GainKnob.setValue(audioProcessor.getEQ1Gain(), juce::dontSendNotification);
    eq2FreqKnob.setValue(audioProcessor.getEQ2Frequency(), juce::dontSendNotification);
    eq2QKnob.setValue(audioProcessor.getEQ2Q(), juce::dontSendNotification);
    eq2GainKnob.setValue(audioProcessor.getEQ2Gain(), juce::dontSendNotification);
    eqOnOffButton.setToggleState(audioProcessor.getEQEnabled(), juce::dontSendNotification);
    
    // Flanger controls - now handled by FlangerComponent
    flangerModule.syncWithDSPState();
    
    // Phaser controls - now handled by PhaserComponent
    phaserModule.syncWithDSPState();
    
    // Reverb controls - now handled by ReverbComponent
    reverbModule.syncWithDSPState();
    
    // Macro controls (commented out - not implemented in processor)
    // macro1Knob.setValue(audioProcessor.getMacro1(), juce::dontSendNotification);
    // macro2Knob.setValue(audioProcessor.getMacro2(), juce::dontSendNotification);
    // macro3Knob.setValue(audioProcessor.getMacro3(), juce::dontSendNotification);
    // macro4Knob.setValue(audioProcessor.getMacro4(), juce::dontSendNotification);
    // macro5Knob.setValue(audioProcessor.getMacro5(), juce::dontSendNotification);
    // macro6Knob.setValue(audioProcessor.getMacro6(), juce::dontSendNotification);
    // macro7Knob.setValue(audioProcessor.getMacro7(), juce::dontSendNotification);
    // macro8Knob.setValue(audioProcessor.getMacro8(), juce::dontSendNotification);
    
    // Update envelope display
    updateEnvelopeDisplay();
    
    // Update preset display
    updatePresetDisplay();
    
    // Trigger a repaint to update the visuals
    repaint();
}

