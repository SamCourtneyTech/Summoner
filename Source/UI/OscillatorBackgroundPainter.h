#pragma once
#include <JuceHeader.h>

class OscillatorBackgroundPainter
{
public:
    OscillatorBackgroundPainter() = default;
    ~OscillatorBackgroundPainter() = default;

    // Main paint method for oscillator 1 background
    static void paintOscillator1Background(juce::Graphics& g,
                                          juce::Rectangle<int> bounds,
                                          float oscillatorBackgroundOffsetX,
                                          float oscillatorBackgroundOffsetY,
                                          float adsrKnobsGroupOffsetX,
                                          float adsrKnobsGroupOffsetY,
                                          float volumeControlsGroupOffsetX,
                                          float volumeControlsGroupOffsetY,
                                          float pitchControlsGroupOffsetX,
                                          float pitchControlsGroupOffsetY);

private:
    // Helper methods for drawing different parts
    static void drawMetallicPanel(juce::Graphics& g, juce::Rectangle<float> bounds);
    static void drawPanelScrews(juce::Graphics& g, juce::Rectangle<float> bounds);
    static void drawOscillatorLabel(juce::Graphics& g, juce::Rectangle<float> bounds);
    static void drawADSRKnobSlots(juce::Graphics& g, juce::Rectangle<int> adsrSection);
    static void drawVolumeKnobSlots(juce::Graphics& g, juce::Rectangle<int> volumeSection);
    static void drawPhaseKnobSlot(juce::Graphics& g, juce::Rectangle<int> phaseRow);
    static void drawTuningControlBoxes(juce::Graphics& g, juce::Rectangle<int> tuningSectionBounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorBackgroundPainter)
};
