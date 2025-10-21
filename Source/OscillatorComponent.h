#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "UI/ADSREnvelopeComponent.h"

class SummonerXSerum2AudioProcessor;

class OscillatorComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    explicit OscillatorComponent(SummonerXSerum2AudioProcessor& processor, int oscillatorNumber);
    ~OscillatorComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    void updateEnvelopeDisplay();
    void updateAllGuiControls();
    void updateWaveButtonStates(juce::Button* activeButton, std::vector<juce::Button*> inactiveButtons);
    void updateAdsrLink();
    void updatePitchControl(int& parameter, int delta, int step, int minVal, int maxVal);
    void updatePitchDisplay();

    // Mouse interaction for oscillator 1 pitch controls
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    int oscNumber; // 1 or 2

    void initializeCommonControls();
    void initializeOscillator1Controls();
    void initializeOscillator2Controls();

    // Custom look and feel
    CustomKnobLookAndFeel customKnobLookAndFeel;
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;
    LEDLabelLookAndFeel ledLabelLookAndFeel;
    LEDNumberLookAndFeel ledNumberLookAndFeel;

    // ADSR Envelope Visualizer
    ADSREnvelopeComponent adsrEnvelopeVisualizer;

    // Wave type buttons
    juce::TextButton sineButton;
    juce::TextButton sawButton;
    juce::TextButton squareButton;
    juce::TextButton triangleButton;
    juce::TextButton whiteNoiseButton;
    juce::TextButton pinkNoiseButton;
    juce::TextButton randomPhaseButton;

    // Volume controls
    RestrictedHitSlider volumeKnob;
    juce::Label volumeLabel;
    RestrictedHitSlider detuneKnob;
    juce::Label detuneLabel;
    RestrictedHitSlider stereoWidthKnob;
    juce::Label stereoWidthLabel;
    RestrictedHitSlider panKnob;
    juce::Label panLabel;

    // Phase controls
    RestrictedHitSlider phaseKnob;
    juce::Label phaseLabel;

    // ADSR controls
    RestrictedHitSlider attackKnob;
    juce::Label attackLabel;
    RestrictedHitSlider decayKnob;
    juce::Label decayLabel;
    RestrictedHitSlider sustainKnob;
    juce::Label sustainLabel;
    RestrictedHitSlider releaseKnob;
    juce::Label releaseLabel;

    // Pitch controls (labels that respond to mouse)
    juce::Label octaveLabel;
    juce::Label octaveValueLabel;
    juce::Label semitoneLabel;
    juce::Label semitoneValueLabel;
    juce::Label fineTuneLabel;
    juce::Label fineTuneValueLabel;
    juce::Label voiceCountLabel;
    juce::Label voiceCountValueLabel;

    // Special controls for oscillator 2
    juce::Label titleLabel; // "OSCILLATOR 2" title
    juce::TextButton adsrLinkButton; // Link to osc 1 ADSR

    // Control state for pitch dragging
    int octaveValue = 0;
    bool isDraggingOctave = false;
    int dragStartY = 0;
    int dragStartOctave = 0;

    int semitoneValue = 0;
    bool isDraggingSemitone = false;
    int dragStartSemitone = 0;

    int fineTuneValue = 0;
    bool isDraggingFineTune = false;
    int dragStartFineTune = 0;

    int voiceCountValue = 1;
    bool isDraggingVoiceCount = false;
    int dragStartVoiceCount = 0;

    // Original ADSR values (for osc 2 link functionality)
    float originalAttack = 0.1f;
    float originalDecay = 0.2f;
    float originalSustain = 0.7f;
    float originalRelease = 0.3f;

    // Helper methods for layout
    void layoutWaveTypeButtons(juce::Rectangle<int>& bounds);
    void layoutADSREnvelope(juce::Rectangle<int>& bounds);
    void layoutADSRKnobs(juce::Rectangle<int>& bounds);
    void layoutVolumeKnobs(juce::Rectangle<int>& bounds);
    void layoutOctaveControls(juce::Rectangle<int>& bounds);
    void layoutPhaseControls(juce::Rectangle<int>& bounds);
    void layoutSecondOscillator(juce::Rectangle<int>& bounds);

    // Helper methods for drawing
    void drawOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorComponent)
};