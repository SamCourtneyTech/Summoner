#pragma once
#include <JuceHeader.h>

class Oscillator {
public:
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
    void noteOn() {
        envelope.noteOn();
    }
    void noteOff() {
        envelope.noteOff();
    }
    float getNextSample() {
        float output = std::sin(phase) * envelope.getNextSample();
        phase += phaseIncrement;
        if (phase >= 2.0 * juce::MathConstants<double>::pi) {
            phase -= 2.0 * juce::MathConstants<double>::pi;
        }
        return output;
    }
private:
    float frequency = 440.0f;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    double currentSampleRate = 44100.0;
    juce::ADSR envelope;
};
