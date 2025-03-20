#pragma once
#include <JuceHeader.h>
#include <random>

class NoiseOscillator {
public:
    enum class Waveform { WhiteNoise, PinkNoise };

    NoiseOscillator() = default;

    void prepare(double sampleRate) {
        currentSampleRate = sampleRate;
        envelope.setSampleRate(sampleRate);
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
        case Waveform::WhiteNoise:
            output = random.nextFloat() * 2.0f - 1.0f;
            break;
        case Waveform::PinkNoise:
            output = generatePinkNoise();
            break;
        }

        currentEnvelopeValue = envelope.getNextSample();
        output *= currentEnvelopeValue;

        return output;
    }

    bool getIsActive() const {
        return envelope.isActive();
    }

    float getEnvelopeValue() const {
        return currentEnvelopeValue;
    }

private:
    double currentSampleRate = 44100.0;
    juce::ADSR envelope;
    Waveform waveform = Waveform::WhiteNoise;
    juce::Random random;
    float pinkNoiseState[6] = { 0.0f };
    float currentEnvelopeValue = 0.0f;

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