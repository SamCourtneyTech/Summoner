#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"

class Voice {
public:
    Voice() : noteNumber(-1), isActive(false), fadeOutSamples(0), fadeOutStep(0.0f), unisonDetuneOffset(0.0f) {}

    void prepare(double sampleRate) {
        oscillator1.prepare(sampleRate);
        oscillator2.prepare(sampleRate);
        fadeOutSamples = static_cast<int>(0.005 * sampleRate); // 5ms fade-out
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
        // Apply unison detune offset
        float unisonDetuneFactor = std::pow(2.0f, unisonDetuneOffset / 1200.0f);
        float detunedFreq = freq * unisonDetuneFactor;

        oscillator1.setFrequency(detunedFreq, sampleRate);
        float detuneCents = detunePtr->load();
        float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f);
        oscillator2.setFrequency(detunedFreq * detuneFactor, sampleRate);
        oscillator1.noteOn();
        oscillator2.noteOn();
        isActive = true;
        fadeOutCounter = 0; // Reset fade-out
        amplitude = 1.0f;
    }

    void noteOff() {
        oscillator1.noteOff();
        oscillator2.noteOff();
    }

    void startFadeOut() {
        fadeOutCounter = fadeOutSamples;
    }

    float getNextSample() {
        if (!isActive) return 0.0f;

        float osc1Output = oscillator1.getNextSample() * osc1LevelPtr->load();
        float osc2Output = oscillator2.getNextSample() * osc2LevelPtr->load();
        float mixedOutput = (osc1Output + osc2Output) * 0.5f;

        // Apply fade-out if active
        if (fadeOutCounter > 0) {
            amplitude = juce::jmax(0.0f, amplitude - fadeOutStep);
            fadeOutCounter--;
            if (fadeOutCounter <= 0) {
                isActive = false; // Deactivate voice after fade-out
            }
        }

        // Check if both oscillators are done with their envelopes
        if (!oscillator1.getIsActive() && !oscillator2.getIsActive() && fadeOutCounter <= 0) {
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
    }

    void setWaveform(Oscillator::Waveform waveform1, Oscillator::Waveform waveform2) {
        oscillator1.setWaveform(waveform1);
        oscillator2.setWaveform(waveform2);
    }

    void setParameterPointers(std::atomic<float>* detune, std::atomic<float>* osc1Level, std::atomic<float>* osc2Level) {
        detunePtr = detune;
        osc1LevelPtr = osc1Level;
        osc2LevelPtr = osc2Level;
    }

private:
    Oscillator oscillator1;
    Oscillator oscillator2;
    float currentFrequency = 0.0f;
    int noteNumber;
    bool isActive;
    float unisonDetuneOffset; // Detune offset for unison voices (in cents)

    // Fade-out variables
    int fadeOutSamples; // Number of samples for fade-out
    float fadeOutStep;  // Amplitude decrement per sample
    int fadeOutCounter = 0; // Current fade-out position
    float amplitude = 1.0f; // Current amplitude during fade-out

    // Parameter pointers
    std::atomic<float>* detunePtr = nullptr;
    std::atomic<float>* osc1LevelPtr = nullptr;
    std::atomic<float>* osc2LevelPtr = nullptr;
};