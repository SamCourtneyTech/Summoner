#pragma once
#include <JuceHeader.h>

class Filter {
public:
    enum class FilterType { LowPass, HighPass, BandPass, Notch };

    Filter();
    ~Filter();

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);

    void setFilterType(FilterType type);
    void setCutoffFrequency(float frequency);
    void setResonance(float resonance);
    void setADSRMix(float mix);

private:
    FilterType filterType = FilterType::LowPass;
    double sampleRate = 44100.0;
    float cutoffFrequency = 2000.0f;
    float resonance = 1.0f;
    float adsrMix = 0.0f;

    juce::dsp::IIR::Filter<float> filter;
    juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

    void updateFilter();
};