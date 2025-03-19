#pragma once
#include <JuceHeader.h>
#include <random>

class Oscillator {
public:
    enum class Waveform { Sine, Saw, Square, Triangle, Pulse25, WhiteNoise, PinkNoise };

    Oscillator() = default;

    void setFrequency(float freq, double sampleRate) {
        frequency = freq;
        phaseIncrement = (2.0 * juce::MathConstants<double>::pi * frequency) / sampleRate;
    }

    void prepare(double sampleRate) {
        currentSampleRate = sampleRate;
        phase = 0.0;
        envelope.setSampleRate(sampleRate);

        // Initialize Pink Noise Filter State
        for (int i = 0; i < 6; ++i) {
            pinkNoiseState[i] = 0.0f;
        }
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
        case Waveform::WhiteNoise:
            output = random.nextFloat() * 2.0f - 1.0f;
            break;
        case Waveform::PinkNoise:
            output = generatePinkNoise();
            break;
        }

        output *= envelope.getNextSample();
        phase += phaseIncrement;
        if (phase >= 2.0 * juce::MathConstants<double>::pi) {
            phase -= 2.0 * juce::MathConstants<double>::pi;
        }
        return output;
    }

    // Add method to get the current envelope value
    float getEnvelopeValue() {  // Remove 'const'
        return envelope.getNextSample();
    }

private:
    float frequency = 440.0f;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    double currentSampleRate = 44100.0;
    juce::ADSR envelope;
    Waveform waveform = Waveform::Sine;
    juce::Random random; // For white noise

    // Pink noise filter state
    float pinkNoiseState[6];

    float generatePinkNoise() {
        float white = random.nextFloat() * 2.0f - 1.0f;
        pinkNoiseState[0] = 0.99886f * pinkNoiseState[0] + white * 0.0555179f;
        pinkNoiseState[1] = 0.99332f * pinkNoiseState[1] + white * 0.0750759f;
        pinkNoiseState[2] = 0.96900f * pinkNoiseState[2] + white * 0.1538520f;
        pinkNoiseState[3] = 0.86650f * pinkNoiseState[3] + white * 0.3104856f;
        pinkNoiseState[4] = 0.55000f * pinkNoiseState[4] + white * 0.5329522f;
        pinkNoiseState[5] = -0.7616f * pinkNoiseState[5] - white * 0.0168980f;
        return pinkNoiseState[0] + pinkNoiseState[1] + pinkNoiseState[2] + pinkNoiseState[3] + pinkNoiseState[4] + pinkNoiseState[5] + white * 0.5362f;
    }
};