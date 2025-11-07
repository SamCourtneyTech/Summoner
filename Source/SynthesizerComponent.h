#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "LFOComponent.h"
#include "UI/DraggableMacroSymbol.h"
#include "UI/MacroSystem.h"
#include "UI/ADSREnvelopeComponent.h"
#include "UI/ParametricEQComponent.h"
#include "UI/DistortionComponent.h"
#include "UI/DelayComponent.h"
#include "UI/CompressorComponent.h"
#include "UI/ChorusComponent.h"
#include "UI/ReverbComponent.h"
#include "UI/PhaserComponent.h"
#include "UI/FlangerComponent.h"
#include "UI/FilterControlComponent.h"
#include "UI/MacroControlsComponent.h"
#include "UI/SecondOscillatorComponent.h"
#include "UI/VolumeControlsComponent.h"
#include "UI/SimpleKnobLookAndFeel.h"
#include "UI/DigitalScreenLookAndFeel.h"
#include "UI/DigitalButtonLookAndFeel.h"
#include "UI/DigitalKnobLookAndFeel.h"
#include "UI/GreenDigitalKnobLookAndFeel.h"
#include "UI/GreenDigitalButtonLookAndFeel.h"
#include "UI/GreenLEDNumberLookAndFeel.h"
#include "UI/DigitalFilterKnobLookAndFeel.h"
#include "UI/EngravedLabelLookAndFeel.h"
#include "UI/EQControlsComponent.h"
#include "UI/PresetManagementComponent.h"

class SummonerXSerum2AudioProcessor;
class SynthesizerComponent;



class SynthesizerComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
    friend class ParametricEQComponent;
    friend class DistortionComponent;
    friend class DelayComponent;
    friend class CompressorComponent;
    friend class ChorusComponent;
    friend class ReverbComponent;
    friend class PhaserComponent;
    friend class FlangerComponent;
    friend class FilterControlComponent;
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse interaction for arc resizing
    bool hitTest(int x, int y) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // Helper method to update envelope display
    void updateEnvelopeDisplay();
    
    // Helper methods for grouped component positioning and background drawing
    void layoutWaveTypeButtons(juce::Rectangle<int>& bounds);
    void layoutADSREnvelope(juce::Rectangle<int>& bounds);
    void layoutADSRKnobs(juce::Rectangle<int>& bounds);
    void layoutVolumeKnobs(juce::Rectangle<int>& bounds);
    void layoutOctaveControls(juce::Rectangle<int>& bounds);
    void layoutPhaseControls(juce::Rectangle<int>& bounds);
    void layoutLFOModule(juce::Rectangle<int>& bounds);
    void layoutEffectsModule(juce::Rectangle<int>& bounds);
    void layoutDistortionControls(juce::Rectangle<int>& bounds);
    void layoutEQControls(juce::Rectangle<int>& bounds);
    void layoutSecondOscillator(juce::Rectangle<int>& bounds);
    void updateEffectsTabVisibility();
    
    void drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawSecondOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PitchControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawFilterSectionBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawMacroKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;
    
    // Custom look and feel for wave buttons
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;
    
    // Custom look and feel for LED labels
    LEDLabelLookAndFeel ledLabelLookAndFeel;
    LEDNumberLookAndFeel ledNumberLookAndFeel;

    // Custom look and feel for digital screen effects
    DigitalScreenLookAndFeel digitalScreenLookAndFeel;

    // Digital button look and feel for smooth digital display appearance
    DigitalButtonLookAndFeel digitalButtonLookAndFeel;

    // Digital knob look and feel for chorus module
    DigitalKnobLookAndFeel digitalKnobLookAndFeel;

    // Green digital knob look and feel for chorus module
    GreenDigitalKnobLookAndFeel greenDigitalKnobLookAndFeel;

    // Green digital button look and feel for chorus module
    GreenDigitalButtonLookAndFeel greenDigitalButtonLookAndFeel;

    // Green LED number look and feel for distortion type selector
    GreenLEDNumberLookAndFeel greenLEDNumberLookAndFeel;
    
    // Simple knob look and feel instance for macro knobs
    SimpleKnobLookAndFeel simpleKnobLookAndFeel;

    // Digital filter knob look and feel with glow text and background
    DigitalFilterKnobLookAndFeel digitalFilterKnobLookAndFeel;

    // Engraved label look and feel for macro knobs
    EngravedLabelLookAndFeel engravedLabelLookAndFeel;
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    ADSREnvelopeComponent adsrEnvelopeVisualizer;

    // LFO MODULE
    LFOModuleComponent lfoModule;
    
    // EFFECTS MODULE
    juce::TabbedComponent effectsModule { juce::TabbedButtonBar::TabsAtTop };

    // Preset management component
    PresetManagementComponent presetManagement;
    
    // Effects module border component
    class EffectsBorderComponent : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds();
            
            // Draw realistic 3D metallic border around the effects module (even larger border)
            // Outermost dark frame (thicker)
            g.setColour(juce::Colour(0xff1a1a1a));
            g.drawRect(bounds, 6);
            
            // Second layer - dark gray
            g.setColour(juce::Colour(0xff303030));
            g.drawRect(bounds.reduced(6), 3);
            
            // Third layer - medium gray
            g.setColour(juce::Colour(0xff505050));
            g.drawRect(bounds.reduced(9), 2);
            
            // Inner light highlight (metallic shine)
            g.setColour(juce::Colour(0xff909090));
            g.drawRect(bounds.reduced(11), 2);
            
            // Inner shadow
            g.setColour(juce::Colour(0xff0a0a0a));
            g.drawRect(bounds.reduced(13), 1);
        }
    };
    
    EffectsBorderComponent effectsBorder;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;

    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    VolumeControlsComponent volumeControls;

    // PHASE CONTROLS GROUP - (STATIONARY)
    RestrictedHitSlider phaseControlsPhaseKnob;
    juce::Label phaseControlsPhaseLabel;
    // ADSR KNOBS GROUP - Row 3 (MOVEABLE)
    RestrictedHitSlider adsrAttackKnob;
    juce::Label adsrAttackLabel;
    RestrictedHitSlider adsrDecayKnob;
    juce::Label adsrDecayLabel;
    RestrictedHitSlider adsrSustainKnob;
    juce::Label adsrSustainLabel;
    RestrictedHitSlider adsrReleaseKnob;
    juce::Label adsrReleaseLabel;
    
    // Pulse width control
    juce::Slider pulseWidthSlider;
    juce::Label pulseWidthLabel;
    
    // PITCH CONTROLS GROUP - Row 5 (MOVEABLE)
    juce::Label pitchControlsOctaveLabel;
    juce::Label pitchControlsOctaveValueLabel;
    juce::Label pitchControlsSemitoneLabel;
    juce::Label pitchControlsSemitoneValueLabel;
    juce::Label pitchControlsFineTuneLabel;
    juce::Label pitchControlsFineTuneValueLabel;
    juce::Label pitchControlsVoiceCountLabel;
    juce::Label pitchControlsVoiceCountValueLabel;
    
    // WAVE TYPE BUTTONS GROUP - Row 1 (MOVEABLE)
    juce::TextButton waveTypeSineButton;
    juce::TextButton waveTypeSawButton;
    juce::TextButton waveTypeSquareButton;
    juce::TextButton waveTypeTriangleButton;
    juce::TextButton waveTypeWhiteNoiseButton;
    juce::TextButton waveTypePinkNoiseButton;
    juce::TextButton waveTypeRandomPhaseButton;

    // SECOND OSCILLATOR - Component
    SecondOscillatorComponent secondOscillator;

    // Macro controls component
    MacroControlsComponent macroControls;
    
public:
    void createMacroMapping(int macroIndex, juce::Slider* targetSlider);
    juce::Slider* findSliderAt(juce::Point<int> position);
    void triggerParameterUpdate(juce::Slider* slider, double newValue);
    void updateAllGuiControls();

private:
    
    // Arc interaction methods
    MacroMapping* findMacroMappingAtPosition(juce::Point<int> position);
    void updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition);
    
    // Arc dragging state
    MacroMapping* draggedMapping = nullptr;
    bool isDraggingArc = false;
    juce::Point<int> dragStartPosition;
    juce::Point<int> dragStartPoint;
    bool draggingMinRange = false;
    
    // Chorus effect controls - now handled by ChorusComponent
    ChorusComponent chorusModule;
    
    // Compressor effect controls
    CompressorComponent compressorModule;
    
    // Delay effect controls
    DelayComponent delayModule;
    
    // Distortion effect controls
    DistortionComponent distortionModule;

    // EQ effect controls - now handled by EQControlsComponent
    ParametricEQComponent parametricEQ;
    EQControlsComponent eqControls;

    // Flanger effect controls - now handled by FlangerComponent
    FlangerComponent flangerModule;
    
    // Phaser effect controls - now handled by PhaserComponent
    PhaserComponent phaserModule;
    
    // Reverb effect controls - now handled by ReverbComponent
    ReverbComponent reverbModule;
    
    // Filter controls - now handled by FilterControlComponent
    FilterControlComponent filterModule;
    
    // Octave control state
    int octaveValue = 0;
    bool isDraggingOctave = false;
    int dragStartY = 0;
    int dragStartOctave = 0;
    
    // Semitone control state
    int semitoneValue = 0;
    bool isDraggingSemitone = false;
    int dragStartSemitone = 0;
    
    // Fine tune control state
    int fineTuneValue = 0;
    bool isDraggingFineTune = false;
    int dragStartFineTune = 0;
    
    // Voice count control state
    int voiceCountValue = 1;
    bool isDraggingVoiceCount = false;
    int dragStartVoiceCount = 0;
    
    // Distortion type control state - now handled by DistortionComponent
    int reverbTypeValue = 2; // 1-5 for different reverb types (1=Plate, 2=Hall, 3=Vintage, 4=Room, 5=Ambience)
    bool isDraggingReverbType = false;
    int phaserPolesValue = 4; // 1-16 for number of poles
    bool isDraggingPhaserPoles = false;
    // dragStartDistortionType moved to DistortionComponent
    int dragStartReverbType = 0;
    int dragStartPhaserPoles = 0;
    
    // Cached bounds for each section to share between resized() and paint()
    juce::Rectangle<int> waveButtonsBounds;
    juce::Rectangle<int> adsrEnvelopeBounds;
    juce::Rectangle<int> adsrKnobsBounds;
    juce::Rectangle<int> volumeKnobsBounds;
    juce::Rectangle<int> octaveControlsBounds;
    juce::Rectangle<int> phaseControlsBounds;
    juce::Rectangle<int> secondOscillatorBounds;
    juce::Rectangle<int> osc2PitchControlsBounds;
    juce::Rectangle<int> osc2PhaseControlsBounds;
    juce::Rectangle<int> filterSectionBounds;
    juce::Rectangle<int> macroKnobsBounds;
    
    // MOVEABLE COMPONENT GROUPS - Offset controls for easy repositioning
    // Wave Type Buttons Group (Row 1 - Top row with SIN, SAW, SQR, TRI, WHT, PNK, RND PHASE)
    float waveTypeButtonsGroupOffsetX = 0.0f;
    float waveTypeButtonsGroupOffsetY = 25.0f;    // MOVED DOWN 25 pixels (reduced from 40)
    
    // ADSR Envelope Visualizer Group (Row 2 - Visual envelope display)
    float adsrEnvelopeGroupOffsetX = 0.0f;
    float adsrEnvelopeGroupOffsetY = 25.0f;       // MOVED DOWN 25 pixels (reduced from 40)
    
    // ADSR Knobs Group (Row 3 - Attack, Decay, Sustain, Release knobs)
    float adsrKnobsGroupOffsetX = 0.0f;
    float adsrKnobsGroupOffsetY = 25.0f;          // MOVED DOWN 25 pixels (reduced from 40)
    
    // Volume Controls Group (Row 4 - Volume, Detune, Stereo Width, Pan knobs)
    float volumeControlsGroupOffsetX = 0.0f;
    float volumeControlsGroupOffsetY = 25.0f;     // MOVED DOWN 25 pixels (reduced from 40)
    
    // Pitch Controls Group (Row 5 - Octave, Semitone, Fine Tune, Voice Count sliders)
    float pitchControlsGroupOffsetX = 0.0f;
    float pitchControlsGroupOffsetY = 18.0f;      // MOVED DOWN 18 pixels (tiny adjustment)
    
    // Second Oscillator Group (Row 6 - Simple sine wave and volume)
    float secondOscillatorGroupOffsetX = 0.0f;
    float secondOscillatorGroupOffsetY = 0.0f;   // Reset to 0 for top-right positioning
    
    // STATIONARY COMPONENTS - These remain in place
    // Phase Controls Group (Phase knob + button - stays put)
    float phaseControlsGroupOffsetX = 0.0f;
    float phaseControlsGroupOffsetY = 0.0f;
    
    // Oscillator background paint (main background - stays put)
    float oscillatorBackgroundOffsetX = 10.0f;    // X offset for background alignment with wood border
    float oscillatorBackgroundOffsetY = -10.0f;   // Y offset for background alignment with wood border

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};
