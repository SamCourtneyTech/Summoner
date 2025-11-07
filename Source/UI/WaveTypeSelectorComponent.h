#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel.h"

class SummonerXSerum2AudioProcessor;

class WaveTypeSelectorComponent : public juce::Component, private juce::Button::Listener
{
public:
    explicit WaveTypeSelectorComponent(SummonerXSerum2AudioProcessor& processor);
    ~WaveTypeSelectorComponent() override;

    void resized() override;

    // Public accessors for the buttons
    juce::TextButton& getSineButton() { return waveTypeSineButton; }
    juce::TextButton& getSawButton() { return waveTypeSawButton; }
    juce::TextButton& getSquareButton() { return waveTypeSquareButton; }
    juce::TextButton& getTriangleButton() { return waveTypeTriangleButton; }
    juce::TextButton& getWhiteNoiseButton() { return waveTypeWhiteNoiseButton; }
    juce::TextButton& getPinkNoiseButton() { return waveTypePinkNoiseButton; }
    juce::TextButton& getRandomPhaseButton() { return waveTypeRandomPhaseButton; }

private:
    void buttonClicked(juce::Button* button) override;

    SummonerXSerum2AudioProcessor& audioProcessor;

    // Custom look and feel for wave buttons
    WaveButtonLookAndFeel customWaveButtonLookAndFeel;

    // Wave type buttons
    juce::TextButton waveTypeSineButton;
    juce::TextButton waveTypeSawButton;
    juce::TextButton waveTypeSquareButton;
    juce::TextButton waveTypeTriangleButton;
    juce::TextButton waveTypeWhiteNoiseButton;
    juce::TextButton waveTypePinkNoiseButton;
    juce::TextButton waveTypeRandomPhaseButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveTypeSelectorComponent)
};
