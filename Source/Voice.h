#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"
#include "NoiseOscillator.h"
#include "SubOscillator.h"

class Voice {
public:
    Voice() : noteNumber(-1), isActive(false), fadeOutSamples(0), fadeOutStep(0.0f) {}

    void prepare(double sampleRate) {
        for (auto& osc : oscillator1s) osc.prepare(sampleRate);
        for (auto& osc : oscillator2s) osc.prepare(sampleRate);
        for (auto& osc : oscillator3s) osc.prepare(sampleRate);
        noiseOscillator.prepare(sampleRate);
        subOscillator.prepare(sampleRate);
        fadeOutSamples = static_cast<int>(0.005 * sampleRate);
        fadeOutStep = 1.0f / fadeOutSamples;
    }

    void setParameters(juce::AudioProcessorValueTreeState& params) {
        parameters = &params;
    }

    void setNoteNumber(int note) { noteNumber = note; }
    int getNoteNumber() const { return noteNumber; }
    bool getIsActive() const { return isActive; }
    bool isInRelease() const {
        return oscillator1s[0].getIsActive() && oscillator1s[0].getEnvelopeValue() < 0.1f;
    }

    void noteOn(float freq, double sampleRate) {
        currentFrequency = freq;

        int osc1Octave = static_cast<int>(std::round(*parameters->getRawParameterValue("osc1Octave")));
        int osc2Octave = static_cast<int>(std::round(*parameters->getRawParameterValue("osc2Octave")));
        int osc3Octave = static_cast<int>(std::round(*parameters->getRawParameterValue("osc3Octave")));
        int subOctave = static_cast<int>(std::round(*parameters->getRawParameterValue("subOctave")));

        float osc1OctaveShift = std::pow(2.0f, static_cast<float>(osc1Octave));
        float osc2OctaveShift = std::pow(2.0f, static_cast<float>(osc2Octave));
        float osc3OctaveShift = std::pow(2.0f, static_cast<float>(osc3Octave));
        float subOctaveShift = std::pow(2.0f, static_cast<float>(subOctave));

        int osc1UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc1UnisonVoices"));
        float osc1UnisonDetune = *parameters->getRawParameterValue("osc1UnisonDetune");
        for (int i = 0; i < osc1UnisonVoices; ++i) {
            float detuneOffset = 0.0f;
            if (osc1UnisonVoices > 1) {
                float step = osc1UnisonDetune / (osc1UnisonVoices - 1);
                detuneOffset = (i * step) - (osc1UnisonDetune / 2.0f);
            }
            float detuneFactor = std::pow(2.0f, detuneOffset / 1200.0f);
            oscillator1s[i].setFrequency(freq * detuneFactor * osc1OctaveShift, sampleRate);
            oscillator1s[i].noteOn();
        }

        int osc2UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc2UnisonVoices"));
        float osc2UnisonDetune = *parameters->getRawParameterValue("osc2UnisonDetune");
        float detuneCents = *parameters->getRawParameterValue("detune");
        float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f);
        for (int i = 0; i < osc2UnisonVoices; ++i) {
            float detuneOffset = 0.0f;
            if (osc2UnisonVoices > 1) {
                float step = osc2UnisonDetune / (osc2UnisonVoices - 1);
                detuneOffset = (i * step) - (osc2UnisonDetune / 2.0f);
            }
            float unisonDetuneFactor = std::pow(2.0f, detuneOffset / 1200.0f);
            oscillator2s[i].setFrequency(freq * detuneFactor * unisonDetuneFactor * osc2OctaveShift, sampleRate);
            oscillator2s[i].noteOn();
        }

        int osc3UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc3UnisonVoices"));
        float osc3UnisonDetune = *parameters->getRawParameterValue("osc3UnisonDetune");
        for (int i = 0; i < osc3UnisonVoices; ++i) {
            float detuneOffset = 0.0f;
            if (osc3UnisonVoices > 1) {
                float step = osc3UnisonDetune / (osc3UnisonVoices - 1);
                detuneOffset = (i * step) - (osc3UnisonDetune / 2.0f);
            }
            float unisonDetuneFactor = std::pow(2.0f, detuneOffset / 1200.0f);
            oscillator3s[i].setFrequency(freq * detuneFactor * 1.01f * unisonDetuneFactor * osc3OctaveShift, sampleRate);
            oscillator3s[i].noteOn();
        }

        subOscillator.setFrequency(freq * 0.5f * subOctaveShift, sampleRate);
        noiseOscillator.noteOn();
        subOscillator.noteOn();
        isActive = true;
        fadeOutCounter = 0;
        amplitude = 1.0f;
    }

    void noteOff() {
        for (auto& osc : oscillator1s) osc.noteOff();
        for (auto& osc : oscillator2s) osc.noteOff();
        for (auto& osc : oscillator3s) osc.noteOff();
        noiseOscillator.noteOff();
        subOscillator.noteOff();
    }

    void startFadeOut() {
        fadeOutCounter = fadeOutSamples;
    }

    float getNextSample() {
        if (!isActive) return 0.0f;

        float osc1Output = 0.0f;
        int osc1UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc1UnisonVoices"));
        for (int i = 0; i < osc1UnisonVoices; ++i) {
            osc1Output += oscillator1s[i].getNextSample();
        }
        osc1Output = (osc1UnisonVoices > 0) ? osc1Output / osc1UnisonVoices : 0.0f;
        osc1Output *= *parameters->getRawParameterValue("osc1Level");

        float osc2Output = 0.0f;
        int osc2UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc2UnisonVoices"));
        for (int i = 0; i < osc2UnisonVoices; ++i) {
            osc2Output += oscillator2s[i].getNextSample();
        }
        osc2Output = (osc2UnisonVoices > 0) ? osc2Output / osc2UnisonVoices : 0.0f;
        osc2Output *= *parameters->getRawParameterValue("osc2Level");

        float osc3Output = 0.0f;
        int osc3UnisonVoices = static_cast<int>(*parameters->getRawParameterValue("osc3UnisonVoices"));
        for (int i = 0; i < osc3UnisonVoices; ++i) {
            osc3Output += oscillator3s[i].getNextSample();
        }
        osc3Output = (osc3UnisonVoices > 0) ? osc3Output / osc3UnisonVoices : 0.0f;
        osc3Output *= *parameters->getRawParameterValue("osc3Level");

        float noiseOutput = noiseOscillator.getNextSample() * *parameters->getRawParameterValue("noiseLevel");
        float subOutput = subOscillator.getNextSample() * *parameters->getRawParameterValue("subLevel");
        float mixedOutput = (osc1Output + osc2Output + osc3Output + noiseOutput + subOutput);

        if (fadeOutCounter > 0) {
            amplitude = juce::jmax(0.0f, amplitude - fadeOutStep);
            fadeOutCounter--;
            if (fadeOutCounter <= 0) {
                isActive = false;
            }
        }

        if (!oscillator1s[0].getIsActive() && !oscillator2s[0].getIsActive() && !oscillator3s[0].getIsActive() &&
            !noiseOscillator.getIsActive() && !subOscillator.getIsActive() && fadeOutCounter <= 0) {
            isActive = false;
        }

        return mixedOutput * amplitude;
    }

    float getEnvelopeValue() const {
        return oscillator1s[0].getEnvelopeValue();
    }

    void setADSR(float attack, float decay, float sustain, float release) {
        for (auto& osc : oscillator1s) osc.setADSR(attack, decay, sustain, release);
        for (auto& osc : oscillator2s) osc.setADSR(attack, decay, sustain, release);
        for (auto& osc : oscillator3s) osc.setADSR(attack, decay, sustain, release);
        noiseOscillator.setADSR(attack, decay, sustain, release);
        subOscillator.setADSR(attack, decay, sustain, release);
    }

    void setWaveform(Oscillator::Waveform waveform1, Oscillator::Waveform waveform2, Oscillator::Waveform waveform3,
        NoiseOscillator::Waveform noiseWaveform, SubOscillator::Waveform subWaveform) {
        for (auto& osc : oscillator1s) osc.setWaveform(waveform1);
        for (auto& osc : oscillator2s) osc.setWaveform(waveform2);
        for (auto& osc : oscillator3s) osc.setWaveform(waveform3);
        noiseOscillator.setWaveform(noiseWaveform);
        subOscillator.setWaveform(subWaveform);
    }

private:
    static constexpr int maxUnisonVoices = 8;
    std::array<Oscillator, maxUnisonVoices> oscillator1s;
    std::array<Oscillator, maxUnisonVoices> oscillator2s;
    std::array<Oscillator, maxUnisonVoices> oscillator3s;
    NoiseOscillator noiseOscillator;
    SubOscillator subOscillator;
    float currentFrequency = 0.0f;
    int noteNumber;
    bool isActive;

    int fadeOutSamples;
    float fadeOutStep;
    int fadeOutCounter = 0;
    float amplitude = 1.0f;

    juce::AudioProcessorValueTreeState* parameters = nullptr;
};