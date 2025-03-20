#pragma once
#include <JuceHeader.h>
#include <random>

class Oscillator {
public:
    enum class Waveform { Sine, Saw, Square, Triangle, Pulse25 };

    Oscillator() = default;

    void setFrequency(float freq, double sampleRate) {
        frequency = freq;
        phaseIncrement = (2.0 * juce::MathConstants<double>::pi * frequency) / sampleRate;
    }

    void prepare(double sampleRate) {
        currentSampleRate = sampleRate;
        phase = 0.0;
        envelope.setSampleRate(sampleRate);
    }

    void setADSR(float attack, float decay, float sustain, float release) {
        envelope.setParameters({ attack, decay, sustain, release });
    }

    void noteOn() { envelope.noteOn(); }
    void noteOff() { envelope.noteOff(); }

    void setWaveform(Waveform newWaveform) { waveform = newWaveform; }

    float getNextSample() {
        float output = 0.0f;

        switch (waveform) {
        case Waveform::Sine:
            output = std::sin(phase);
            break;
        case Waveform::Saw:
            output = (2.0f * (phase / (2.0 * juce::MathConstants<double>::pi)) - 1.0f);
            break;
        case Waveform::Square:
            output = (std::sin(phase) >= 0.0f) ? 1.0f : -1.0f;
            break;
        case Waveform::Triangle:
            output = (2.0f * std::abs(2.0f * (phase / (2.0 * juce::MathConstants<double>::pi)) - 1.0f) - 1.0f);
            break;
        case Waveform::Pulse25:
            output = (std::sin(phase) >= 0.5f) ? 1.0f : -1.0f;
            break;
        }

        // Update the stored envelope value
        currentEnvelopeValue = envelope.getNextSample();
        output *= currentEnvelopeValue;

        phase += phaseIncrement;
        if (phase >= 2.0 * juce::MathConstants<double>::pi) {
            phase -= 2.0 * juce::MathConstants<double>::pi;
        }
        return output;
    }

    bool getIsActive() const {
        return envelope.isActive();
    }

    float getEnvelopeValue() const {
        return currentEnvelopeValue;
    }

private:
    float frequency = 440.0f;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    double currentSampleRate = 44100.0;
    juce::ADSR envelope;
    Waveform waveform = Waveform::Sine;
    float currentEnvelopeValue = 0.0f; // Store the current envelope value
};