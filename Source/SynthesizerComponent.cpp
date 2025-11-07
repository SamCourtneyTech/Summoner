#include "SynthesizerComponent.h"
#include "PluginProcessor.h"
#include <set>


SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor),
      effectsModule(juce::TabbedButtonBar::TabsAtTop),
      secondOscillator(*this, processor, &customKnobLookAndFeel, &customWaveButtonLookAndFeel, &ledLabelLookAndFeel, &ledNumberLookAndFeel),
      macroControls(*this, processor, &simpleKnobLookAndFeel, &engravedLabelLookAndFeel),
      volumeControls(processor, &customKnobLookAndFeel, &ledLabelLookAndFeel),
      pitchControls(processor, ledLabelLookAndFeel, ledNumberLookAndFeel),
      eqControls(*this, processor),
      presetManagement(processor)
{
    addAndMakeVisible(secondOscillator);
    addAndMakeVisible(macroControls);
    addAndMakeVisible(volumeControls);
    addAndMakeVisible(pitchControls);

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

    // Set initial bounds (will be properly laid out in resized())
    parametricEQ.setBounds(0, 95, 300, 200);

    // Initialize EQ controls component
    eqControls.setLookAndFeels(&greenDigitalKnobLookAndFeel, &greenDigitalButtonLookAndFeel);
    eqControls.setParametricEQ(&parametricEQ);
    eqControls.syncWithDSPState();
    eqControls.setBounds(0, 0, 300, 500); // Set bounds to cover the tab area
    equalizerTab->addAndMakeVisible(eqControls);
    
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

    // EFFECTS PRESET CONTROLS - now handled by PresetManagementComponent
    presetManagement.getPrevButton().setLookAndFeel(&customWaveButtonLookAndFeel);
    presetManagement.getNextButton().setLookAndFeel(&customWaveButtonLookAndFeel);
    presetManagement.getSaveButton().setLookAndFeel(&customWaveButtonLookAndFeel);
    presetManagement.getLoadButton().setLookAndFeel(&customWaveButtonLookAndFeel);
    presetManagement.getInitButton().setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(presetManagement);

    // Add border component behind effects module
    addAndMakeVisible(effectsBorder);
    addAndMakeVisible(effectsModule);
    
    // Initialize envelope display with default values
    updateEnvelopeDisplay();

    // Initialize preset display - now handled by PresetManagementComponent (initialized in its constructor)
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
    phaseControlsPhaseLabel.setLookAndFeel(nullptr);
    adsrAttackLabel.setLookAndFeel(nullptr);
    adsrDecayLabel.setLookAndFeel(nullptr);
    adsrSustainLabel.setLookAndFeel(nullptr);
    adsrReleaseLabel.setLookAndFeel(nullptr);
    // Oscillator 2 cleanup now handled by SecondOscillatorComponent
    // Macro controls cleanup now handled by MacroControlsComponent
    // Preset management cleanup now handled by PresetManagementComponent

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
    if (slider == &phaseControlsPhaseKnob)
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
    // Pitch controls now handled by PitchControlsComponent
    // Oscillator 2 pitch controls now handled by SecondOscillatorComponent
    // Reverb type cycling now handled by ReverbComponent
    // Phaser poles value label clicking now handled by PhaserComponent
}

void SynthesizerComponent::mouseDrag(const juce::MouseEvent& event)
{
    // Pitch controls dragging now handled by PitchControlsComponent
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
    // Pitch control drag states now handled by PitchControlsComponent
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

    // Set the component bounds - it will handle internal layout
    volumeControls.setBounds(offsetVolumeSection);
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

    // Set the pitch controls component bounds
    pitchControls.setBounds(offsetPitchControlsSection);
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
    
    // Preset controls area above the effects module - now handled by PresetManagementComponent
    auto presetControlsHeight = 35;
    auto presetY = centerY - presetControlsHeight - 10; // 10px gap above effects module

    // Position the preset management component
    auto buttonWidth = 40;
    auto buttonHeight = 25;
    auto labelWidth = 120;
    auto spacing = 10;
    auto totalPresetWidth = (4 * buttonWidth) + labelWidth + (4 * spacing);
    auto presetStartX = centerX + borderPadding + (effectsWidth - totalPresetWidth) / 2;

    presetManagement.setBounds(presetStartX, presetY, totalPresetWidth, presetControlsHeight);
    
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

// drawOctaveControlsBackground removed - now handled by PitchControlsComponent

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
        &volumeControls.getVolumeKnob(), &volumeControls.getDetuneKnob(), &volumeControls.getStereoWidthKnob(), &volumeControls.getPanKnob(),
        &phaseControlsPhaseKnob, &secondOscillator.getVolumeKnob(), &secondOscillator.getDetuneKnob(), &secondOscillator.getStereoKnob(), &secondOscillator.getPanKnob(), &secondOscillator.getPhaseKnob(),

        // ADSR controls
        &adsrAttackKnob, &adsrDecayKnob, &adsrSustainKnob, &adsrReleaseKnob,
        &secondOscillator.getAttackKnob(), &secondOscillator.getDecayKnob(), &secondOscillator.getSustainKnob(), &secondOscillator.getReleaseKnob(),
        
        // Filter controls
        &filterModule.filterCutoffKnob, &filterModule.filterResonanceKnob,
        
        // EQ controls - using only the NEW knobs to avoid duplicates
        &eqControls.getEQ1NewFreqKnob(), &eqControls.getEQ1NewQKnob(), &eqControls.getEQ1NewGainKnob(),
        &eqControls.getEQ2NewFreqKnob(), &eqControls.getEQ2NewQKnob(), &eqControls.getEQ2NewGainKnob(),
        
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
    
    if (slider == &volumeControls.getVolumeKnob())
    {
        audioProcessor.setOsc1Volume(static_cast<float>(newValue));
    }
    else if (slider == &volumeControls.getDetuneKnob())
    {
        audioProcessor.setOsc1Detune(static_cast<float>(newValue));
    }
    else if (slider == &volumeControls.getStereoWidthKnob())
    {
        audioProcessor.setOsc1StereoWidth(static_cast<float>(newValue));
    }
    else if (slider == &volumeControls.getPanKnob())
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
    // EQ parameters - now handled by EQControlsComponent
    else if (slider == &eqControls.getEQ1FreqKnob())
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ1QKnob())
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ1GainKnob())
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2FreqKnob())
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2QKnob())
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2GainKnob())
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    // New EQ knobs
    else if (slider == &eqControls.getEQ1NewFreqKnob())
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ1NewQKnob())
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ1NewGainKnob())
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2NewFreqKnob())
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2NewQKnob())
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eqControls.getEQ2NewGainKnob())
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

// Preset Management - now handled by PresetManagementComponent

void SynthesizerComponent::updateAllGuiControls()
{
    // Main synthesizer controls (using correct control names)
    volumeControls.updateAllGuiControls();
    pitchControls.updateAllGuiControls();
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
    
    
    // EQ controls - now handled by EQControlsComponent
    eqControls.syncWithDSPState();
    
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

    // Update preset display - now handled by PresetManagementComponent
    presetManagement.updatePresetDisplay();

    // Trigger a repaint to update the visuals
    repaint();
}

