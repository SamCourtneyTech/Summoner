#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"

class Voice {
public:
    Voice() : noteNumber(-1), isActive(false) {}

    void prepare(double sampleRate) {
        oscillator1.prepare(sampleRate);
        oscillator2.prepare(sampleRate);
    }

    void setNoteNumber(int note) { noteNumber = note; }
    int getNoteNumber() const { return noteNumber; }
    bool getIsActive() const { return isActive; }

    void noteOn(float freq, double sampleRate) {
        currentFrequency = freq;
        oscillator1.setFrequency(freq, sampleRate);
        float detuneCents = detunePtr->load();
        float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f);
        oscillator2.setFrequency(freq * detuneFactor, sampleRate);
        oscillator1.noteOn();
        oscillator2.noteOn();
        isActive = true;
    }

    void noteOff() {
        oscillator1.noteOff();
        oscillator2.noteOff();
    }

    float getNextSample() {
        if (!isActive) return 0.0f;

        float osc1Output = oscillator1.getNextSample() * osc1LevelPtr->load();
        float osc2Output = oscillator2.getNextSample() * osc2LevelPtr->load();
        float mixedOutput = (osc1Output + osc2Output) * 0.5f;

        // Check if both oscillators are done with their envelopes
        if (!oscillator1.getIsActive() && !oscillator2.getIsActive()) {
            isActive = false;
        }

        return mixedOutput;
    }

    float getEnvelopeValue() {
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

    // Parameter pointers
    std::atomic<float>* detunePtr = nullptr;
    std::atomic<float>* osc1LevelPtr = nullptr;
    std::atomic<float>* osc2LevelPtr = nullptr;
};