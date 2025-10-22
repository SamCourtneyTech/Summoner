#pragma once
#include <JuceHeader.h>
#include "ADSREnvelopeComponent.h"

class SynthesizerComponent;
class SummonerXSerum2AudioProcessor;

class SecondOscillatorComponent : public juce::Component,
                                   private juce::Slider::Listener,
                                   private juce::Button::Listener
{
public:
    explicit SecondOscillatorComponent(SynthesizerComponent& parent,
                                      SummonerXSerum2AudioProcessor& processor,
                                      juce::LookAndFeel* customKnobLF,
                                      juce::LookAndFeel* waveButtonLF,
                                      juce::LookAndFeel* ledLabelLF,
                                      juce::LookAndFeel* ledNumberLF);
    ~SecondOscillatorComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Mouse handling for pitch controls
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    // Event handlers
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    // Public interface for parent component
    void updateEnvelopeDisplay();
    void updateAllGuiControls();

    // Bounds accessors for parent's background drawing
    juce::Rectangle<int> getBounds() const { return getLocalBounds(); }
    juce::Rectangle<int> getPitchControlsBounds() const { return osc2PitchControlsBounds; }
    juce::Rectangle<int> getPhaseControlsBounds() const { return osc2PhaseControlsBounds; }

    // Getters for accessing controls (needed for macro system)
    juce::Slider& getVolumeKnob() { return osc2VolumeKnob; }
    juce::Slider& getDetuneKnob() { return osc2DetuneKnob; }
    juce::Slider& getStereoKnob() { return osc2StereoKnob; }
    juce::Slider& getPanKnob() { return osc2PanKnob; }
    juce::Slider& getAttackKnob() { return osc2AttackKnob; }
    juce::Slider& getDecayKnob() { return osc2DecayKnob; }
    juce::Slider& getSustainKnob() { return osc2SustainKnob; }
    juce::Slider& getReleaseKnob() { return osc2ReleaseKnob; }
    juce::Slider& getPhaseKnob() { return osc2PhaseKnob; }
    juce::TextButton& getAdsrLinkButton() { return osc2AdsrLinkButton; }

private:
    SynthesizerComponent& parentComponent;
    SummonerXSerum2AudioProcessor& audioProcessor;

    // Look and feel references (owned by parent)
    juce::LookAndFeel* customKnobLookAndFeel;
    juce::LookAndFeel* customWaveButtonLookAndFeel;
    juce::LookAndFeel* ledLabelLookAndFeel;
    juce::LookAndFeel* ledNumberLookAndFeel;

    // Wave type buttons
    juce::TextButton osc2SineButton;
    juce::TextButton osc2SawButton;
    juce::TextButton osc2SquareButton;
    juce::TextButton osc2TriangleButton;
    juce::TextButton osc2WhiteNoiseButton;
    juce::TextButton osc2PinkNoiseButton;

    // Volume/Detune/Stereo/Pan controls
    juce::Slider osc2VolumeKnob;
    juce::Label osc2VolumeLabel;
    juce::Slider osc2DetuneKnob;
    juce::Label osc2DetuneLabel;
    juce::Slider osc2StereoKnob;
    juce::Label osc2StereoLabel;
    juce::Slider osc2PanKnob;
    juce::Label osc2PanLabel;

    // Pitch controls
    juce::Label osc2OctaveLabel;
    juce::Label osc2OctaveValueLabel;
    juce::Label osc2SemitoneLabel;
    juce::Label osc2SemitoneValueLabel;
    juce::Label osc2FineTuneLabel;
    juce::Label osc2FineTuneValueLabel;
    juce::Label osc2VoicesLabel;
    juce::Label osc2VoicesValueLabel;

    // Phase controls
    juce::TextButton osc2RandomPhaseButton;
    juce::Slider osc2PhaseKnob;
    juce::Label osc2PhaseLabel;

    // Title label
    juce::Label osc2TitleLabel;

    // ADSR controls
    juce::Slider osc2AttackKnob;
    juce::Label osc2AttackLabel;
    juce::Slider osc2DecayKnob;
    juce::Label osc2DecayLabel;
    juce::Slider osc2SustainKnob;
    juce::Label osc2SustainLabel;
    juce::Slider osc2ReleaseKnob;
    juce::Label osc2ReleaseLabel;
    juce::TextButton osc2AdsrLinkButton;

    // ADSR envelope visualizer
    ADSREnvelopeComponent osc2AdsrEnvelopeVisualizer;

    // Pitch control state
    int osc2OctaveValue = 0;
    bool isDraggingOsc2Octave = false;
    int dragStartOsc2Octave = 0;
    int dragStartY = 0;

    int osc2SemitoneValue = 0;
    bool isDraggingOsc2Semitone = false;
    int dragStartOsc2Semitone = 0;

    int osc2FineTuneValue = 0;
    bool isDraggingOsc2FineTune = false;
    int dragStartOsc2FineTune = 0;

    int osc2VoiceCountValue = 1;
    bool isDraggingOsc2VoiceCount = false;
    int dragStartOsc2VoiceCount = 0;

    // ADSR original values (stored when linking is enabled)
    float osc2OriginalAttack = 0.1f;
    float osc2OriginalDecay = 0.2f;
    float osc2OriginalSustain = 0.7f;
    float osc2OriginalRelease = 0.3f;

    // Cached bounds for background drawing
    juce::Rectangle<int> osc2PitchControlsBounds;
    juce::Rectangle<int> osc2PhaseControlsBounds;

    // Helper methods
    void initializeOsc2Controls();
    void handleWaveButtonClick(juce::Button* button);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SecondOscillatorComponent)
};
