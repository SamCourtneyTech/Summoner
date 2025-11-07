#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;
class LEDLabelLookAndFeel;
class LEDNumberLookAndFeel;

class PitchControlsComponent : public juce::Component
{
public:
    explicit PitchControlsComponent(SummonerXSerum2AudioProcessor& processor,
                                    LEDLabelLookAndFeel& labelLookAndFeel,
                                    LEDNumberLookAndFeel& numberLookAndFeel);
    ~PitchControlsComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Mouse interaction for draggable labels
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    // Update all GUI controls from processor state
    void updateAllGuiControls();

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    LEDLabelLookAndFeel& ledLabelLookAndFeel;
    LEDNumberLookAndFeel& ledNumberLookAndFeel;

    // PITCH CONTROLS - Labels
    juce::Label pitchControlsOctaveLabel;
    juce::Label pitchControlsOctaveValueLabel;
    juce::Label pitchControlsSemitoneLabel;
    juce::Label pitchControlsSemitoneValueLabel;
    juce::Label pitchControlsFineTuneLabel;
    juce::Label pitchControlsFineTuneValueLabel;
    juce::Label pitchControlsVoiceCountLabel;
    juce::Label pitchControlsVoiceCountValueLabel;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchControlsComponent)
};
