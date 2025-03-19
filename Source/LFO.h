#pragma once
#include <JuceHeader.h>

class LFO {
public:
    enum class Waveform {
        Sine,
        Triangle,
        Saw,
        Square
    };

    LFO() : phase(0.0f), frequency(1.0f), sampleRate(44100.0), waveform(Waveform::Sine) {}

    void prepare(double sampleRate) {
        this->sampleRate = sampleRate;
        updatePhaseIncrement();
    }

    void setFrequency(float freq) {
        frequency = freq;
        updatePhaseIncrement();
    }

    void setWaveform(Waveform wf) {
        waveform = wf;
    }

    float getNextSample() {
        float output = 0.0f;

        switch (waveform) {
        case Waveform::Sine:
            output = std::sin(phase * juce::MathConstants<float>::twoPi);
            break;
        case Waveform::Triangle:
            output = 2.0f * std::abs(2.0f * (phase - std::floor(phase + 0.5f))) - 1.0f;
            break;
        case Waveform::Saw:
            output = 2.0f * (phase - std::floor(phase)) - 1.0f;
            break;
        case Waveform::Square:
            output = phase < 0.5f ? 1.0f : -1.0f;
            break;
        }

        phase += phaseIncrement;
        if (phase >= 1.0f) phase -= 1.0f;

        return output;
    }

private:
    float phase;
    float frequency;
    double sampleRate;
    float phaseIncrement;
    Waveform waveform;

    void updatePhaseIncrement() {
        phaseIncrement = frequency / static_cast<float>(sampleRate);
    }
};