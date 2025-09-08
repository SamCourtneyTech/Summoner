#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class ParametricEQComponent : public juce::Component
{
public:
    enum FilterType
    {
        Shelf,
        Peak,
        Pass
    };
    
    struct EQBand
    {
        float frequency = 1000.0f;
        float gain = 0.0f;
        float q = 1.0f;
        FilterType filterType = Peak;
        bool isLeftBand = true;
        juce::Point<float> graphPosition;
    };
    
    ParametricEQComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void updateBandFromKnobs(int bandIndex);
    void updateKnobsFromBand(int bandIndex);
    void setBandFilterType(int bandIndex, FilterType type);
    void setParentSynthesizer(SynthesizerComponent* parent) { parentSynthesizer = parent; }
    void syncWithDSPState(); // Sync visual component with DSP state
    
    EQBand& getBand(int index) { return bands[index]; }
    const EQBand& getBand(int index) const { return bands[index]; }
    
private:
    EQBand bands[2]; // Two bands (left and right)
    int draggedBand = -1;
    juce::Rectangle<float> graphArea;
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    float frequencyToX(float frequency) const;
    float xToFrequency(float x) const;
    float gainToY(float gain) const;
    float yToGain(float y) const;
    
    void drawFrequencyResponse(juce::Graphics& g);
    void drawBandHandle(juce::Graphics& g, const EQBand& band, int bandIndex);
    int getBandAtPoint(juce::Point<float> point);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricEQComponent)
};