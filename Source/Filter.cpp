#include "Filter.h"

Filter::Filter() {}

Filter::~Filter() {}

void Filter::prepare(double sampleRate) {
    this->sampleRate = sampleRate;
    filter.prepare({ sampleRate, 2, static_cast<juce::uint32>(sampleRate) });
    updateFilter();
}

void Filter::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
}

void Filter::setFilterType(Filter::FilterType type) {
    filterType = type;
    updateFilter();
}

void Filter::setCutoffFrequency(float frequency) {
    cutoffFrequency = frequency;
    updateFilter();
}

void Filter::setResonance(float resonance) {
    this->resonance = resonance;
    updateFilter();
}

void Filter::setADSRMix(float mix) {
    this->adsrMix = mix;
}

void Filter::updateFilter() {
    switch (filterType) {
    case Filter::FilterType::LowPass:
        coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency, resonance);
        break;
    case Filter::FilterType::HighPass:
        coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoffFrequency, resonance);
        break;
    case Filter::FilterType::BandPass:
        coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, cutoffFrequency, resonance);
        break;
    case Filter::FilterType::Notch:
        coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, cutoffFrequency, resonance);
        break;
    }
    filter.coefficients = coefficients;
}