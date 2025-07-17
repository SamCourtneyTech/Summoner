#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "LFOComponent.h"

class SummonerXSerum2AudioProcessor;


class ADSREnvelopeComponent : public juce::Component
{
public:
    ADSREnvelopeComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void updateEnvelope(float attack, float decay, float sustain, float release);
    
private:
    float attackTime = 0.1f;
    float decayTime = 0.2f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.3f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSREnvelopeComponent)
};

class SynthesizerComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // Mouse events for octave control
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
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
    void layoutSecondOscillator(juce::Rectangle<int>& bounds);
    
    void drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawSecondOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PitchControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOsc2PhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);

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
    class DigitalScreenLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
        {
            auto bounds = button.getLocalBounds();
            
            // Digital screen tab styling
            if (button.isFrontTab())
            {
                // Active tab - bright green
                g.setColour(juce::Colour(0xff00ff00).withAlpha(0.9f));
                g.fillRect(bounds);
                g.setColour(juce::Colour(0xff000000));
            }
            else
            {
                // Inactive tab - darker green
                g.setColour(juce::Colour(0xff003300).withAlpha(0.7f));
                g.fillRect(bounds);
                g.setColour(juce::Colour(0xff00aa00));
            }
            
            // Draw text with digital font style
            g.setFont(juce::Font("Courier New", 9.7f, juce::Font::bold)); // 10.8f * 0.9 = 9.72f
            g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
            
            // Add scan line effect
            for (int y = bounds.getY(); y < bounds.getBottom(); y += 3)
            {
                g.setColour(juce::Colour(0xff000000).withAlpha(0.1f));
                g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
            }
        }
    };
    
    DigitalScreenLookAndFeel digitalScreenLookAndFeel;
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    ADSREnvelopeComponent adsrEnvelopeVisualizer;
    
    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    ADSREnvelopeComponent osc2AdsrEnvelopeVisualizer;
    
    // LFO MODULE
    LFOModuleComponent lfoModule;
    
    // EFFECTS MODULE
    juce::TabbedComponent effectsModule;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;
    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    juce::Slider volumeControlsVolumeKnob;
    juce::Label volumeControlsVolumeLabel;
    juce::Slider volumeControlsDetuneKnob;
    juce::Label volumeControlsDetuneLabel;
    juce::Slider volumeControlsStereoWidthKnob;
    juce::Label volumeControlsStereoWidthLabel;
    juce::Slider volumeControlsPanKnob;
    juce::Label volumeControlsPanLabel;
    // PHASE CONTROLS GROUP - (STATIONARY)
    juce::Slider phaseControlsPhaseKnob;
    juce::Label phaseControlsPhaseLabel;
    // ADSR KNOBS GROUP - Row 3 (MOVEABLE)
    juce::Slider adsrAttackKnob;
    juce::Label adsrAttackLabel;
    juce::Slider adsrDecayKnob;
    juce::Label adsrDecayLabel;
    juce::Slider adsrSustainKnob;
    juce::Label adsrSustainLabel;
    juce::Slider adsrReleaseKnob;
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
    
    // SECOND OSCILLATOR CONTROLS - Row 6 (MOVEABLE)
    juce::TextButton osc2SineButton;
    juce::TextButton osc2SawButton;
    juce::TextButton osc2SquareButton;
    juce::TextButton osc2TriangleButton;
    juce::TextButton osc2WhiteNoiseButton;
    juce::TextButton osc2PinkNoiseButton;
    juce::Slider osc2VolumeKnob;
    juce::Label osc2VolumeLabel;
    juce::Slider osc2DetuneKnob;
    juce::Label osc2DetuneLabel;
    juce::Slider osc2StereoKnob;
    juce::Label osc2StereoLabel;
    juce::Label osc2VoicesLabel;
    juce::Label osc2VoicesValueLabel;
    juce::Slider osc2PanKnob;
    juce::Label osc2PanLabel;
    juce::Label osc2OctaveLabel;
    juce::Label osc2OctaveValueLabel;
    juce::Label osc2SemitoneLabel;
    juce::Label osc2SemitoneValueLabel;
    juce::Label osc2FineTuneLabel;
    juce::Label osc2FineTuneValueLabel;
    juce::TextButton osc2RandomPhaseButton;
    juce::Slider osc2PhaseKnob;
    juce::Label osc2PhaseLabel;
    juce::Label osc2TitleLabel;
    juce::Slider osc2AttackKnob;
    juce::Label osc2AttackLabel;
    juce::Slider osc2DecayKnob;
    juce::Label osc2DecayLabel;
    juce::Slider osc2SustainKnob;
    juce::Label osc2SustainLabel;
    juce::Slider osc2ReleaseKnob;
    juce::Label osc2ReleaseLabel;
    juce::TextButton osc2AdsrLinkButton;
    
    // Filter controls
    juce::Slider filterCutoffKnob;
    juce::Label filterCutoffLabel;
    juce::Slider filterResonanceKnob;
    juce::Label filterResonanceLabel;
    juce::TextButton osc1FilterEnableButton;
    juce::TextButton osc2FilterEnableButton;
    juce::TextButton filterLPButton;
    juce::TextButton filterHPButton;
    juce::TextButton filterBPButton;
    juce::TextButton filterNotchButton;
    juce::TextButton filterCombButton;
    juce::TextButton filterFormantButton;
    juce::TextButton filter12dBButton;
    juce::TextButton filter24dBButton;
    
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
    
    // Oscillator 2 ADSR original values (stored when linking is enabled)
    float osc2OriginalAttack = 0.1f;
    float osc2OriginalDecay = 0.2f;
    float osc2OriginalSustain = 0.7f;
    float osc2OriginalRelease = 0.3f;
    
    // Oscillator 2 pitch control state
    int osc2OctaveValue = 0;
    bool isDraggingOsc2Octave = false;
    int dragStartOsc2Octave = 0;
    
    int osc2SemitoneValue = 0;
    bool isDraggingOsc2Semitone = false;
    int dragStartOsc2Semitone = 0;
    
    int osc2FineTuneValue = 0;
    bool isDraggingOsc2FineTune = false;
    int dragStartOsc2FineTune = 0;
    
    int osc2VoiceCountValue = 1;
    bool isDraggingOsc2VoiceCount = false;
    int dragStartOsc2VoiceCount = 0;
    
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
