#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"
#include "NoiseOscillator.h"
#include "SubOscillator.h"

class Voice {
public:
    Voice() : noteNumber(-1), isActive(false), fadeOutSamples(0), fadeOutStep(0.0f), unisonDetuneOffset(0.0f) {}

    void prepare(double sampleRate) {
        oscillator1.prepare(sampleRate);
        oscillator2.prepare(sampleRate);
        oscillator3.prepare(sampleRate);
        noiseOscillator.prepare(sampleRate);
        subOscillator.prepare(sampleRate);
        fadeOutSamples = static_cast<int>(0.005 * sampleRate);
        fadeOutStep = 1.0f / fadeOutSamples;
    }

    void setNoteNumber(int note) { noteNumber = note; }
    int getNoteNumber() const { return noteNumber; }
    bool getIsActive() const { return isActive; }
    bool isInRelease() const {
        return oscillator1.getIsActive() && oscillator1.getEnvelopeValue() < 0.1f;
    }

    void setUnisonDetuneOffset(float offset) {
        unisonDetuneOffset = offset;
    }

    void noteOn(float freq, double sampleRate) {
        currentFrequency = freq;
        float unisonDetuneFactor = std::pow(2.0f, unisonDetuneOffset / 1200.0f);
        float detunedFreq = freq * unisonDetuneFactor;

        oscillator1.setFrequency(detunedFreq, sampleRate);
        float detuneCents = detunePtr->load();
        float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f);
        oscillator2.setFrequency(detunedFreq * detuneFactor, sampleRate);
        oscillator3.setFrequency(detunedFreq * detuneFactor * 1.01f, sampleRate);
        subOscillator.setFrequency(detunedFreq * 0.5f, sampleRate); // Sub-oscillator at half frequency
        oscillator1.noteOn();
        oscillator2.noteOn();
        oscillator3.noteOn();
        noiseOscillator.noteOn();
        subOscillator.noteOn();
        isActive = true;
        fadeOutCounter = 0;
        amplitude = 1.0f;
    }

    void noteOff() {
        oscillator1.noteOff();
        oscillator2.noteOff();
        oscillator3.noteOff();
        noiseOscillator.noteOff();
        subOscillator.noteOff();
    }

    void startFadeOut() {
        fadeOutCounter = fadeOutSamples;
    }

    float getNextSample() {
        if (!isActive) return 0.0f;

        float osc1Output = oscillator1.getNextSample() * osc1LevelPtr->load();
        float osc2Output = oscillator2.getNextSample() * osc2LevelPtr->load();
        float osc3Output = oscillator3.getNextSample() * osc3LevelPtr->load();
        float noiseOutput = noiseOscillator.getNextSample() * noiseLevelPtr->load();
        float subOutput = subOscillator.getNextSample() * subLevelPtr->load();
        float mixedOutput = (osc1Output + osc2Output + osc3Output + noiseOutput + subOutput) / 5.0f;

        if (fadeOutCounter > 0) {
            amplitude = juce::jmax(0.0f, amplitude - fadeOutStep);
            fadeOutCounter--;
            if (fadeOutCounter <= 0) {
                isActive = false;
            }
        }

        if (!oscillator1.getIsActive() && !oscillator2.getIsActive() && !oscillator3.getIsActive() &&
            !noiseOscillator.getIsActive() && !subOscillator.getIsActive() && fadeOutCounter <= 0) {
            isActive = false;
        }

        return mixedOutput * amplitude;
    }

    float getEnvelopeValue() const {
        return oscillator1.getEnvelopeValue();
    }

    void setADSR(float attack, float decay, float sustain, float release) {
        oscillator1.setADSR(attack, decay, sustain, release);
        oscillator2.setADSR(attack, decay, sustain, release);
        oscillator3.setADSR(attack, decay, sustain, release);
        noiseOscillator.setADSR(attack, decay, sustain, release);
        subOscillator.setADSR(attack, decay, sustain, release);
    }

    void setWaveform(Oscillator::Waveform waveform1, Oscillator::Waveform waveform2, Oscillator::Waveform waveform3,
        NoiseOscillator::Waveform noiseWaveform, SubOscillator::Waveform subWaveform) {
        oscillator1.setWaveform(waveform1);
        oscillator2.setWaveform(waveform2);
        oscillator3.setWaveform(waveform3);
        noiseOscillator.setWaveform(noiseWaveform);
        subOscillator.setWaveform(subWaveform);
    }

    void setParameterPointers(std::atomic<float>* detune, std::atomic<float>* osc1Level, std::atomic<float>* osc2Level,
        std::atomic<float>* osc3Level, std::atomic<float>* noiseLevel, std::atomic<float>* subLevel) {
        detunePtr = detune;
        osc1LevelPtr = osc1Level;
        osc2LevelPtr = osc2Level;
        osc3LevelPtr = osc3Level;
        noiseLevelPtr = noiseLevel;
        subLevelPtr = subLevel;
    }

private:
    Oscillator oscillator1;
    Oscillator oscillator2;
    Oscillator oscillator3;
    NoiseOscillator noiseOscillator;
    SubOscillator subOscillator;
    float currentFrequency = 0.0f;
    int noteNumber;
    bool isActive;
    float unisonDetuneOffset;

    int fadeOutSamples;
    float fadeOutStep;
    int fadeOutCounter = 0;
    float amplitude = 1.0f;

    std::atomic<float>* detunePtr = nullptr;
    std::atomic<float>* osc1LevelPtr = nullptr;
    std::atomic<float>* osc2LevelPtr = nullptr;
    std::atomic<float>* osc3LevelPtr = nullptr;
    std::atomic<float>* noiseLevelPtr = nullptr;
    std::atomic<float>* subLevelPtr = nullptr;
};