#pragma once
#include <JuceHeader.h>
#include "ADSREnvelopeComponent.h"

class SynthesizerComponent;
class SummonerXSerum2AudioProcessor;

class Oscillator1Component : public juce::Component,
                              private juce::Slider::Listener,
                              private juce::Button::Listener
{
public:
    explicit Oscillator1Component(SynthesizerComponent& parent,
                                  SummonerXSerum2AudioProcessor& processor,
                                  juce::LookAndFeel* customKnobLF,
                                  juce::LookAndFeel* waveButtonLF,
                                  juce::LookAndFeel* ledLabelLF,
                                  juce::LookAndFeel* ledNumberLF);
    ~Oscillator1Component() override;

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
    juce::Rectangle<int> getWaveButtonsBounds() const { return waveButtonsBounds; }
    juce::Rectangle<int> getPitchControlsBounds() const { return pitchControlsBounds; }
    juce::Rectangle<int> getPhaseControlsBounds() const { return phaseControlsBounds; }
    juce::Rectangle<int> getAdsrKnobsBounds() const { return adsrKnobsBounds; }
    juce::Rectangle<int> getAdsrEnvelopeBounds() const { return adsrEnvelopeBounds; }

    // Getters for accessing controls (needed for macro system)
    juce::Slider& getAttackKnob() { return adsrAttackKnob; }
    juce::Slider& getDecayKnob() { return adsrDecayKnob; }
    juce::Slider& getSustainKnob() { return adsrSustainKnob; }
    juce::Slider& getReleaseKnob() { return adsrReleaseKnob; }
    juce::Slider& getPhaseKnob() { return phaseControlsPhaseKnob; }
    juce::TextButton& getRandomPhaseButton() { return waveTypeRandomPhaseButton; }
    ADSREnvelopeComponent& getAdsrEnvelopeVisualizer() { return adsrEnvelopeVisualizer; }

private:
    SynthesizerComponent& parentComponent;
    SummonerXSerum2AudioProcessor& audioProcessor;

    // Look and feel references (owned by parent)
    juce::LookAndFeel* customKnobLookAndFeel;
    juce::LookAndFeel* customWaveButtonLookAndFeel;
    juce::LookAndFeel* ledLabelLookAndFeel;
    juce::LookAndFeel* ledNumberLookAndFeel;

    // Wave type buttons
    juce::TextButton waveTypeSineButton;
    juce::TextButton waveTypeSawButton;
    juce::TextButton waveTypeSquareButton;
    juce::TextButton waveTypeTriangleButton;
    juce::TextButton waveTypeWhiteNoiseButton;
    juce::TextButton waveTypePinkNoiseButton;
    juce::TextButton waveTypeRandomPhaseButton;

    // Pitch controls
    juce::Label pitchControlsOctaveLabel;
    juce::Label pitchControlsOctaveValueLabel;
    juce::Label pitchControlsSemitoneLabel;
    juce::Label pitchControlsSemitoneValueLabel;
    juce::Label pitchControlsFineTuneLabel;
    juce::Label pitchControlsFineTuneValueLabel;
    juce::Label pitchControlsVoiceCountLabel;
    juce::Label pitchControlsVoiceCountValueLabel;

    // Phase controls
    juce::Slider phaseControlsPhaseKnob;
    juce::Label phaseControlsPhaseLabel;

    // ADSR controls
    juce::Slider adsrAttackKnob;
    juce::Label adsrAttackLabel;
    juce::Slider adsrDecayKnob;
    juce::Label adsrDecayLabel;
    juce::Slider adsrSustainKnob;
    juce::Label adsrSustainLabel;
    juce::Slider adsrReleaseKnob;
    juce::Label adsrReleaseLabel;

    // ADSR envelope visualizer
    ADSREnvelopeComponent adsrEnvelopeVisualizer;

    // Pitch control state
    int octaveValue = 0;
    bool isDraggingOctave = false;
    int dragStartOctave = 0;
    int dragStartY = 0;

    int semitoneValue = 0;
    bool isDraggingSemitone = false;
    int dragStartSemitone = 0;

    int fineTuneValue = 0;
    bool isDraggingFineTune = false;
    int dragStartFineTune = 0;

    int voiceCountValue = 1;
    bool isDraggingVoiceCount = false;
    int dragStartVoiceCount = 0;

    // Cached bounds for background drawing
    juce::Rectangle<int> waveButtonsBounds;
    juce::Rectangle<int> pitchControlsBounds;
    juce::Rectangle<int> phaseControlsBounds;
    juce::Rectangle<int> adsrKnobsBounds;
    juce::Rectangle<int> adsrEnvelopeBounds;

    // Helper methods
    void initializeOsc1Controls();
    void handleWaveButtonClick(juce::Button* button);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator1Component)
};
