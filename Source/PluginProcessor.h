#pragma once
#include <JuceHeader.h>
#include <array>
#include "SettingsComponent.h"

// One-pole smoothing filter for parameter smoothing
class OnePoleSmoothing
{
public:
    OnePoleSmoothing() = default;
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        updateTimeConstant();
    }
    
    void setTimeConstantMs(float timeMs)
    {
        timeConstantMs = timeMs;
        updateTimeConstant();
    }
    
    void setTarget(float newTarget)
    {
        target = newTarget;
    }
    
    float getNextValue()
    {
        current = current + coefficient * (target - current);
        return current;
    }
    
    void reset(float initialValue = 0.0f)
    {
        current = initialValue;
        target = initialValue;
    }
    
    float getCurrentValue() const { return current; }
    
private:
    void updateTimeConstant()
    {
        if (sampleRate > 0.0 && timeConstantMs > 0.0f)
        {
            // One-pole coefficient calculation
            float timeConstantSamples = timeConstantMs * 0.001f * static_cast<float>(sampleRate);
            coefficient = 1.0f - std::exp(-1.0f / timeConstantSamples);
        }
        else
        {
            coefficient = 1.0f; // No smoothing
        }
    }
    
    double sampleRate = 44100.0;
    float timeConstantMs = 10.0f; // Default 10ms smoothing time
    float coefficient = 0.1f;
    float current = 0.0f;
    float target = 0.0f;
};

// Variable slope filter with separate LP and HP modes
class SimpleStableFilter
{
public:
    enum FilterType
    {
        OFF = 0,
        LOWPASS = 1,
        HIGHPASS = 2,
        BANDPASS = 3,
        NOTCH = 4,
        COMB = 5,
        FORMANT = 6
    };
    
    enum FilterSlope
    {
        SLOPE_12DB = 0,
        SLOPE_24DB = 1
    };
    
    SimpleStableFilter()
    {
        cutoffSmoothing.setSampleRate(44100.0);
        cutoffSmoothing.setTimeConstantMs(10.0f); // 10ms smoothing time
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        cutoffSmoothing.setSampleRate(newSampleRate);
        updateCoeff();
    }
    
    void setCutoffFrequency(float cutoff)
    {
        float limitedCutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.4), cutoff);
        cutoffSmoothing.setTarget(limitedCutoff);
        
        // For user knob movements, jump immediately to the new value
        // Smoothing will handle sample-to-sample changes during audio processing
        cutoffFreq = limitedCutoff;
        cutoffSmoothing.reset(limitedCutoff);
        updateCoeff();
    }
    
    void setResonance(float resonance)
    {
        // Q factor: 0.707 = no resonance, higher values = more resonance
        q = juce::jlimit(0.1f, 20.0f, resonance);
        updateCoeff();
    }
    
    void setFilterType(FilterType type)
    {
        filterType = type;
        updateCoeff();
    }
    
    void setFilterSlope(FilterSlope slope)
    {
        filterSlope = slope;
        updateCoeff();
    }
    
    void setSmoothingTimeMs(float timeMs)
    {
        cutoffSmoothing.setTimeConstantMs(timeMs);
    }
    
    // Getter methods for per-voice filter synchronization
    float getCutoffFrequency() const { return cutoffFreq; }
    FilterType getFilterType() const { return filterType; }
    FilterSlope getFilterSlope() const { return filterSlope; }
    float getResonance() const { return q; }
    
    void reset()
    {
        z1 = 0.0f;
        z2 = 0.0f;
        z3 = 0.0f;
        z4 = 0.0f;
        
        // Reset smoothing to current cutoff frequency
        cutoffSmoothing.reset(cutoffFreq);
        
        // Clear comb filter delay line
        if (!delayLine.empty())
        {
            std::fill(delayLine.begin(), delayLine.end(), 0.0f);
            delayIndex = 0;
        }
        
        // Clear formant filter state
        formant1_z1 = 0.0f; formant1_z2 = 0.0f;
        formant2_z1 = 0.0f; formant2_z2 = 0.0f;
        formant3_z1 = 0.0f; formant3_z2 = 0.0f;
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        // Not used in our implementation
    }
    
    float processSample(float inputSample, int channel)
    {
        if (filterType == OFF)
            return inputSample;
            
        // Update smoothed cutoff frequency and recalculate coefficients if needed
        float newCutoffFreq = cutoffSmoothing.getNextValue();
        if (std::abs(newCutoffFreq - cutoffFreq) > 0.01f) // Very small threshold for smooth updates
        {
            cutoffFreq = newCutoffFreq;
            updateCoeff();
        }
            
        
        // Handle comb filter separately as it uses a different processing method
        if (filterType == COMB)
        {
            if (delayLine.empty())
                return inputSample;
                
            // Read delayed sample from delay line
            float delayedSample = delayLine[delayIndex];
            
            // Comb filter output: input + feedback * delayed
            float output = inputSample + feedbackGain * delayedSample;
            
            // Write new sample to delay line
            delayLine[delayIndex] = inputSample;
            
            // Update delay index (circular buffer)
            delayIndex = (delayIndex + 1) % delayLineSize;
            
            
            return output;
        }
        
        // Handle formant filter separately as it uses multiple parallel bandpass filters
        if (filterType == FORMANT)
        {
            // Process through 3 parallel bandpass filters (formants)
            // Formant 1
            float f1_output = inputSample * formant1_a0 + formant1_z1;
            formant1_z1 = inputSample * formant1_a1 + formant1_z2 - formant1_b1 * f1_output;
            formant1_z2 = inputSample * formant1_a2 - formant1_b2 * f1_output;
            
            // Formant 2
            float f2_output = inputSample * formant2_a0 + formant2_z1;
            formant2_z1 = inputSample * formant2_a1 + formant2_z2 - formant2_b1 * f2_output;
            formant2_z2 = inputSample * formant2_a2 - formant2_b2 * f2_output;
            
            // Formant 3
            float f3_output = inputSample * formant3_a0 + formant3_z1;
            formant3_z1 = inputSample * formant3_a1 + formant3_z2 - formant3_b1 * f3_output;
            formant3_z2 = inputSample * formant3_a2 - formant3_b2 * f3_output;
            
            // Mix the three formants with emphasis on lower formants
            float output = f1_output * 0.5f + f2_output * 0.35f + f3_output * 0.15f;
            
            
            return output;
        }
        
        if (filterSlope == SLOPE_12DB)
        {
            // Single 12dB stage
            float output = inputSample * a0 + z1;
            
            // Update delay line
            z1 = inputSample * a1 + z2 - b1 * output;
            z2 = inputSample * a2 - b2 * output;
            
            
            return output;
        }
        else // SLOPE_24DB
        {
            // First 12dB stage
            float stage1 = inputSample * a0 + z1;
            z1 = inputSample * a1 + z2 - b1 * stage1;
            z2 = inputSample * a2 - b2 * stage1;
            
            // Second 12dB stage (cascaded)
            float output = stage1 * a0 + z3;
            z3 = stage1 * a1 + z4 - b1 * output;
            z4 = stage1 * a2 - b2 * output;
            
            
            return output;
        }
    }

private:
    void updateCoeff()
    {
        if (filterType == OFF)
        {
            a0 = 1.0f; a1 = 0.0f; a2 = 0.0f;
            b1 = 0.0f; b2 = 0.0f;
            return;
        }
        
        // Standard 12dB/octave filter design with Q factor
        float omega = 2.0f * juce::MathConstants<float>::pi * cutoffFreq / static_cast<float>(sampleRate);
        omega = juce::jlimit(0.001f, juce::MathConstants<float>::pi * 0.99f, omega); // Prevent extreme values
        float c = 1.0f / std::tan(omega * 0.5f);
        float cSq = c * c;
        float safeQ = juce::jlimit(0.5f, 3.0f, q); // Conservative Q range for stability
        float cQ = c / safeQ;
        
        if (filterType == LOWPASS)
        {
            // 12dB/octave low-pass coefficients with Q
            float norm = 1.0f / (1.0f + cQ + cSq);
            a0 = norm;
            a1 = 2.0f * a0;
            a2 = a0;
            b1 = 2.0f * (1.0f - cSq) * norm;
            b2 = (1.0f - cQ + cSq) * norm;
        }
        else if (filterType == HIGHPASS)
        {
            // 12dB/octave high-pass coefficients with Q
            float norm = 1.0f / (1.0f + cQ + cSq);
            a0 = cSq * norm;
            a1 = -2.0f * a0;
            a2 = a0;
            b1 = 2.0f * (1.0f - cSq) * norm;
            b2 = (1.0f - cQ + cSq) * norm;
        }
        else if (filterType == BANDPASS)
        {
            // 12dB/octave band-pass coefficients with Q
            float norm = 1.0f / (1.0f + cQ + cSq);
            a0 = cQ * norm;
            a1 = 0.0f;
            a2 = -a0;
            b1 = 2.0f * (1.0f - cSq) * norm;
            b2 = (1.0f - cQ + cSq) * norm;
        }
        else if (filterType == NOTCH)
        {
            // 12dB/octave notch (band-reject) coefficients with Q
            float norm = 1.0f / (1.0f + cQ + cSq);
            a0 = (1.0f + cSq) * norm;
            a1 = 2.0f * (1.0f - cSq) * norm;
            a2 = a0;
            b1 = a1; // Same as a1 for notch
            b2 = (1.0f - cQ + cSq) * norm;
        }
        else if (filterType == COMB)
        {
            // Comb filter setup - uses delay line instead of biquad coefficients
            // Calculate delay time in samples based on cutoff frequency
            float delayTimeMs = 1000.0f / cutoffFreq; // Convert frequency to period in ms
            int newDelaySize = static_cast<int>(delayTimeMs * sampleRate / 1000.0f);
            
            // Clamp delay size to reasonable range (1ms to 50ms)
            newDelaySize = juce::jlimit(static_cast<int>(sampleRate * 0.001f), 
                                       static_cast<int>(sampleRate * 0.05f), 
                                       newDelaySize);
            
            // Only resize if delay size changed
            if (newDelaySize != delayLineSize)
            {
                delayLineSize = newDelaySize;
                delayLine.resize(delayLineSize);
                std::fill(delayLine.begin(), delayLine.end(), 0.0f);
                delayIndex = 0;
            }
            
            // Set feedback gain based on Q factor (resonance)
            // Higher Q = more feedback = more resonance, but keep very conservative
            feedbackGain = juce::jlimit(0.0f, 0.5f, (safeQ - 0.707f) / 1.293f * 0.4f);
            
            // Clear biquad coefficients for comb filter
            a0 = 1.0f; a1 = 0.0f; a2 = 0.0f;
            b1 = 0.0f; b2 = 0.0f;
            return;
        }
        else if (filterType == FORMANT)
        {
            // Formant filter setup - 3 parallel bandpass filters for vowel simulation
            // Cutoff frequency controls the vowel character by shifting formant frequencies
            
            // Base formant frequencies for /a/ vowel (can be modulated by cutoff)
            float formantShift = cutoffFreq / 1000.0f; // Normalize around 1000Hz
            float f1_freq = 730.0f * formantShift;  // First formant
            float f2_freq = 1090.0f * formantShift; // Second formant
            float f3_freq = 2440.0f * formantShift; // Third formant
            
            // Clamp formant frequencies to reasonable range
            f1_freq = juce::jlimit(200.0f, static_cast<float>(sampleRate * 0.4f), f1_freq);
            f2_freq = juce::jlimit(400.0f, static_cast<float>(sampleRate * 0.4f), f2_freq);
            f3_freq = juce::jlimit(800.0f, static_cast<float>(sampleRate * 0.4f), f3_freq);
            
            // Calculate bandpass coefficients for each formant
            // Using moderate Q for realistic vowel formants
            float formantQ = 4.0f + (q - 0.707f) / 19.293f * 16.0f; // Q range: 4-20
            
            // Formant 1 coefficients
            {
                float omega = 2.0f * juce::MathConstants<float>::pi * f1_freq / static_cast<float>(sampleRate);
                float c = 1.0f / std::tan(omega * 0.5f);
                float cSq = c * c;
                float cQ = c / formantQ;
                float norm = 1.0f / (1.0f + cQ + cSq);
                formant1_a0 = cQ * norm;
                formant1_a1 = 0.0f;
                formant1_a2 = -formant1_a0;
                formant1_b1 = 2.0f * (1.0f - cSq) * norm;
                formant1_b2 = (1.0f - cQ + cSq) * norm;
            }
            
            // Formant 2 coefficients
            {
                float omega = 2.0f * juce::MathConstants<float>::pi * f2_freq / static_cast<float>(sampleRate);
                float c = 1.0f / std::tan(omega * 0.5f);
                float cSq = c * c;
                float cQ = c / formantQ;
                float norm = 1.0f / (1.0f + cQ + cSq);
                formant2_a0 = cQ * norm;
                formant2_a1 = 0.0f;
                formant2_a2 = -formant2_a0;
                formant2_b1 = 2.0f * (1.0f - cSq) * norm;
                formant2_b2 = (1.0f - cQ + cSq) * norm;
            }
            
            // Formant 3 coefficients
            {
                float omega = 2.0f * juce::MathConstants<float>::pi * f3_freq / static_cast<float>(sampleRate);
                float c = 1.0f / std::tan(omega * 0.5f);
                float cSq = c * c;
                float cQ = c / formantQ;
                float norm = 1.0f / (1.0f + cQ + cSq);
                formant3_a0 = cQ * norm;
                formant3_a1 = 0.0f;
                formant3_a2 = -formant3_a0;
                formant3_b1 = 2.0f * (1.0f - cSq) * norm;
                formant3_b2 = (1.0f - cQ + cSq) * norm;
            }
            
            // Clamp formant coefficients
            formant1_a0 = juce::jlimit(-2.0f, 2.0f, formant1_a0);
            formant1_a2 = juce::jlimit(-2.0f, 2.0f, formant1_a2);
            formant1_b1 = juce::jlimit(-1.99f, 1.99f, formant1_b1);
            formant1_b2 = juce::jlimit(-1.99f, 1.99f, formant1_b2);
            
            formant2_a0 = juce::jlimit(-2.0f, 2.0f, formant2_a0);
            formant2_a2 = juce::jlimit(-2.0f, 2.0f, formant2_a2);
            formant2_b1 = juce::jlimit(-1.99f, 1.99f, formant2_b1);
            formant2_b2 = juce::jlimit(-1.99f, 1.99f, formant2_b2);
            
            formant3_a0 = juce::jlimit(-2.0f, 2.0f, formant3_a0);
            formant3_a2 = juce::jlimit(-2.0f, 2.0f, formant3_a2);
            formant3_b1 = juce::jlimit(-1.99f, 1.99f, formant3_b1);
            formant3_b2 = juce::jlimit(-1.99f, 1.99f, formant3_b2);
            
            // Clear main biquad coefficients for formant filter
            a0 = 1.0f; a1 = 0.0f; a2 = 0.0f;
            b1 = 0.0f; b2 = 0.0f;
            return;
        }
        
        // Coefficients should be stable due to conservative Q range and omega limiting
    }
    
    double sampleRate = 44100.0;
    float cutoffFreq = 1000.0f;
    float q = 0.707f; // Q factor for resonance
    FilterType filterType = OFF;
    FilterSlope filterSlope = SLOPE_12DB;
    
    // Filter coefficients
    float a0 = 1.0f, a1 = 0.0f, a2 = 0.0f;
    float b1 = 0.0f, b2 = 0.0f;
    
    // State variables (z3, z4 for 24dB filters)
    float z1 = 0.0f, z2 = 0.0f, z3 = 0.0f, z4 = 0.0f;
    
    // One-pole smoothing for cutoff frequency
    OnePoleSmoothing cutoffSmoothing;
    
    // Comb filter delay line
    std::vector<float> delayLine;
    int delayLineSize = 0;
    int delayIndex = 0;
    float feedbackGain = 0.0f;
    
    // Formant filter state (3 parallel bandpass filters for vowel formants)
    float formant1_z1 = 0.0f, formant1_z2 = 0.0f;
    float formant2_z1 = 0.0f, formant2_z2 = 0.0f;
    float formant3_z1 = 0.0f, formant3_z2 = 0.0f;
    float formant1_a0 = 1.0f, formant1_a1 = 0.0f, formant1_a2 = 0.0f;
    float formant1_b1 = 0.0f, formant1_b2 = 0.0f;
    float formant2_a0 = 1.0f, formant2_a1 = 0.0f, formant2_a2 = 0.0f;
    float formant2_b1 = 0.0f, formant2_b2 = 0.0f;
    float formant3_a0 = 1.0f, formant3_a1 = 0.0f, formant3_a2 = 0.0f;
    float formant3_b1 = 0.0f, formant3_b2 = 0.0f;
};

// Dynamics compressor with optional multiband processing
class CompressorEffect
{
public:
    CompressorEffect()
    {
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        // Initialize parameter smoothing
        thresholdSmoothing.setSampleRate(sampleRate);
        thresholdSmoothing.setTimeConstantMs(10.0f);
        
        ratioSmoothing.setSampleRate(sampleRate);
        ratioSmoothing.setTimeConstantMs(10.0f);
        
        attackSmoothing.setSampleRate(sampleRate);
        attackSmoothing.setTimeConstantMs(5.0f);
        
        releaseSmoothing.setSampleRate(sampleRate);
        releaseSmoothing.setTimeConstantMs(5.0f);
        
        makeupGainSmoothing.setSampleRate(sampleRate);
        makeupGainSmoothing.setTimeConstantMs(10.0f);
        
        mixSmoothing.setSampleRate(sampleRate);
        mixSmoothing.setTimeConstantMs(20.0f);
        
        // Initialize RMS buffers (10ms window)
        int rmsBufferSize = static_cast<int>(sampleRate * 0.01);
        rmsBuffer[0].resize(rmsBufferSize, 0.0f);
        rmsBuffer[1].resize(rmsBufferSize, 0.0f);
        rmsBufferSize_ = rmsBufferSize;
        
        // Initialize multiband RMS buffers
        rmsBufferLow[0].resize(rmsBufferSize, 0.0f);
        rmsBufferLow[1].resize(rmsBufferSize, 0.0f);
        rmsBufferMid[0].resize(rmsBufferSize, 0.0f);
        rmsBufferMid[1].resize(rmsBufferSize, 0.0f);
        rmsBufferHigh[0].resize(rmsBufferSize, 0.0f);
        rmsBufferHigh[1].resize(rmsBufferSize, 0.0f);
        
        // Initialize 3-band crossover filters for multiband processing
        initializeCrossoverFilters();
        
        // Reset all processing state to safe defaults
        reset();
    }
    
    void setEnabled(bool enabled) { isEnabled = enabled; }
    void setThreshold(float thresholdDb) { 
        threshold = juce::jlimit(-60.0f, 0.0f, thresholdDb);
        thresholdSmoothing.setTarget(threshold);
    }
    void setRatio(float ratioValue) { 
        ratio = juce::jlimit(1.0f, 20.0f, ratioValue);
        ratioSmoothing.setTarget(ratio);
    }
    void setAttack(float attackMs) { 
        attack = juce::jlimit(0.1f, 100.0f, attackMs);
        attackSmoothing.setTarget(attack);
    }
    void setRelease(float releaseMs) { 
        release = juce::jlimit(10.0f, 1000.0f, releaseMs);
        releaseSmoothing.setTarget(release);
    }
    void setMakeupGain(float gainDb) { 
        makeupGain = juce::jlimit(0.0f, 30.0f, gainDb);
        makeupGainSmoothing.setTarget(makeupGain);
    }
    void setMix(float mixValue) { 
        wetMix = juce::jlimit(0.0f, 1.0f, mixValue);
        mixSmoothing.setTarget(wetMix);
    }
    void setMultiband(bool enabled) { multibandEnabled = enabled; }
    
    void reset()
    {
        // Clear RMS buffers and reset all state
        for (int ch = 0; ch < 2; ++ch)
        {
            std::fill(rmsBuffer[ch].begin(), rmsBuffer[ch].end(), 0.0f);
            std::fill(rmsBufferLow[ch].begin(), rmsBufferLow[ch].end(), 0.0f);
            std::fill(rmsBufferMid[ch].begin(), rmsBufferMid[ch].end(), 0.0f);
            std::fill(rmsBufferHigh[ch].begin(), rmsBufferHigh[ch].end(), 0.0f);
            
            rmsIndex[ch] = 0;
            rmsIndexLow[ch] = 0;
            rmsIndexMid[ch] = 0;
            rmsIndexHigh[ch] = 0;
            
            rmsSum[ch] = 0.0f;
            rmsSumLow[ch] = 0.0f;
            rmsSumMid[ch] = 0.0f;
            rmsSumHigh[ch] = 0.0f;
            
            gainReduction[ch] = 1.0f;
            gainReductionLow[ch] = 1.0f;
            gainReductionMid[ch] = 1.0f;
            gainReductionHigh[ch] = 1.0f;
        }
        
        // Reset parameter smoothing
        thresholdSmoothing.reset();
        ratioSmoothing.reset();
        attackSmoothing.reset();
        releaseSmoothing.reset();
        makeupGainSmoothing.reset();
        mixSmoothing.reset();
        for (int ch = 0; ch < 2; ++ch)
        {
            if (!rmsBuffer[ch].empty())
                std::fill(rmsBuffer[ch].begin(), rmsBuffer[ch].end(), 0.0f);
            rmsIndex[ch] = 0;
            rmsSum[ch] = 0.0f;
            
            // Clear multiband RMS buffers
            if (!rmsBufferLow[ch].empty())
                std::fill(rmsBufferLow[ch].begin(), rmsBufferLow[ch].end(), 0.0f);
            if (!rmsBufferMid[ch].empty())
                std::fill(rmsBufferMid[ch].begin(), rmsBufferMid[ch].end(), 0.0f);
            if (!rmsBufferHigh[ch].empty())
                std::fill(rmsBufferHigh[ch].begin(), rmsBufferHigh[ch].end(), 0.0f);
            
            rmsIndexLow[ch] = rmsIndexMid[ch] = rmsIndexHigh[ch] = 0;
            rmsSumLow[ch] = rmsSumMid[ch] = rmsSumHigh[ch] = 0.0f;
        }
        
        // Reset envelope followers
        gainReduction[0] = gainReduction[1] = 1.0f;
        gainReductionLow[0] = gainReductionLow[1] = 1.0f;
        gainReductionMid[0] = gainReductionMid[1] = 1.0f;
        gainReductionHigh[0] = gainReductionHigh[1] = 1.0f;
        
        // Reset smoothing
        thresholdSmoothing.reset(threshold);
        ratioSmoothing.reset(ratio);
        attackSmoothing.reset(attack);
        releaseSmoothing.reset(release);
        makeupGainSmoothing.reset(makeupGain);
        mixSmoothing.reset(wetMix);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled || sampleRate <= 0.0)
            return;
            
        const int numSamples = buffer.getNumSamples();
        const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
        
        // Additional safety checks
        if (numSamples <= 0 || numChannels <= 0 || rmsBufferSize_ <= 0)
            return;
            
        // Temporarily disable multiband to isolate the issue
        if (false && multibandEnabled) // Force disable multiband for debugging
        {
            processMultiband(buffer, numSamples, numChannels);
        }
        else
        {
            processSingleband(buffer, numSamples, numChannels);
        }
    }
    
private:
    void processSingleband(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed parameters
            float currentThreshold = thresholdSmoothing.getNextValue();
            float currentRatio = ratioSmoothing.getNextValue();
            float currentAttack = attackSmoothing.getNextValue();
            float currentRelease = releaseSmoothing.getNextValue();
            float currentMakeupGain = makeupGainSmoothing.getNextValue();
            float currentMix = mixSmoothing.getNextValue();
            
            // Calculate envelope coefficients
            float attackCoeff = std::exp(-1.0f / (currentAttack * 0.001f * static_cast<float>(sampleRate)));
            float releaseCoeff = std::exp(-1.0f / (currentRelease * 0.001f * static_cast<float>(sampleRate)));
            
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float inputSample = buffer.getSample(ch, sample);
                float drySample = inputSample;
                
                float compressedSample = compressSample(inputSample, currentThreshold, currentRatio, 
                                                       attackCoeff, releaseCoeff, ch, 
                                                       rmsBuffer[ch], rmsIndex[ch], rmsSum[ch], gainReduction[ch]);
                
                // Apply makeup gain
                float makeupGainLinear = std::pow(10.0f, currentMakeupGain / 20.0f);
                compressedSample *= makeupGainLinear;
                
                // Mix dry and wet (parallel compression)
                float outputSample = drySample * (1.0f - currentMix) + compressedSample * currentMix;
                
                buffer.setSample(ch, sample, outputSample);
            }
        }
    }
    
    void processMultiband(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed parameters
            float currentThreshold = thresholdSmoothing.getNextValue();
            float currentRatio = ratioSmoothing.getNextValue();
            float currentAttack = attackSmoothing.getNextValue();
            float currentRelease = releaseSmoothing.getNextValue();
            float currentMakeupGain = makeupGainSmoothing.getNextValue();
            float currentMix = mixSmoothing.getNextValue();
            
            // Calculate envelope coefficients
            float attackCoeff = std::exp(-1.0f / (currentAttack * 0.001f * static_cast<float>(sampleRate)));
            float releaseCoeff = std::exp(-1.0f / (currentRelease * 0.001f * static_cast<float>(sampleRate)));
            
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float inputSample = buffer.getSample(ch, sample);
                float drySample = inputSample;
                
                // Split into frequency bands
                float lowBand = inputSample;
                lowBand = lowBandFilter[ch][0].processSample(lowBand, ch);
                lowBand = lowBandFilter[ch][1].processSample(lowBand, ch);
                
                float midBand = inputSample;
                midBand = midBandFilterHP[ch][0].processSample(midBand, ch);
                midBand = midBandFilterHP[ch][1].processSample(midBand, ch);
                midBand = midBandFilterLP[ch][0].processSample(midBand, ch);
                midBand = midBandFilterLP[ch][1].processSample(midBand, ch);
                
                float highBand = inputSample;
                highBand = highBandFilter[ch][0].processSample(highBand, ch);
                highBand = highBandFilter[ch][1].processSample(highBand, ch);
                
                // OTT-style per-band processing with more aggressive characteristics
                // Low band: Very aggressive for that signature OTT bass punch
                float lowThreshold = currentThreshold - 8.0f; // More compression
                float lowRatio = juce::jmax(4.0f, currentRatio * 1.8f); // Much higher ratio, minimum 4:1
                float compressedLow = compressSample(lowBand, lowThreshold, lowRatio,
                                                   attackCoeff * 0.5f, releaseCoeff * 1.2f, ch, // Faster attack, slower release
                                                   rmsBufferLow[ch], rmsIndexLow[ch], rmsSumLow[ch], gainReductionLow[ch]);
                
                // Mid band: Aggressive for vocal/instrument clarity (OTT's strength)
                float midThreshold = currentThreshold - 3.0f; // More sensitive
                float midRatio = juce::jmax(3.0f, currentRatio * 1.2f); // Higher ratio, minimum 3:1
                float compressedMid = compressSample(midBand, midThreshold, midRatio,
                                                   attackCoeff * 0.4f, releaseCoeff, ch, // Very fast attack
                                                   rmsBufferMid[ch], rmsIndexMid[ch], rmsSumMid[ch], gainReductionMid[ch]);
                
                // High band: Moderate but present compression for sparkle
                float highThreshold = currentThreshold + 2.0f; // Still gentler but more active
                float highRatio = juce::jmax(2.0f, currentRatio * 0.9f); // Moderate ratio, minimum 2:1
                float compressedHigh = compressSample(highBand, highThreshold, highRatio,
                                                    attackCoeff * 0.8f, releaseCoeff, ch, // Medium-fast attack
                                                    rmsBufferHigh[ch], rmsIndexHigh[ch], rmsSumHigh[ch], gainReductionHigh[ch]);
                
                // Apply conservative band balancing (no boosts to prevent gain stacking)
                compressedLow *= 0.95f;   // Slight reduction to prevent bass buildup
                compressedMid *= 1.0f;    // Keep mids neutral 
                compressedHigh *= 0.98f;  // Slight high reduction
                
                // Sum the compressed bands with safety checks
                float compressedSample = compressedLow + compressedMid + compressedHigh;
                
                // Safety check after band summing
                if (!std::isfinite(compressedSample) || std::abs(compressedSample) > 100.0f)
                {
                    compressedSample = 0.0f; // Return silence if something went wrong
                }
                else
                {
                    // Apply gain cut early in chain to prevent saturation/limiting from being hit too hard
                    float gainCutEarly = std::pow(10.0f, -12.0f / 20.0f); // 12dB cut before saturation
                    compressedSample *= gainCutEarly;
                    
                    // Add OTT-style harmonic saturation
                    if (std::abs(compressedSample) > 0.005f)
                    {
                        float saturation = 0.08f; // More noticeable saturation for OTT character
                        compressedSample = std::tanh(compressedSample * (1.0f + saturation)) / (1.0f + saturation);
                    }
                    
                    // Soft limiting with OTT-style characteristics
                    compressedSample = std::tanh(compressedSample * 0.9f) * 0.9f;
                    
                    // Apply additional 6dB cut after processing (total 18dB with early cut)
                    float gainCutLate = std::pow(10.0f, -6.0f / 20.0f); // Additional 6dB cut
                    compressedSample *= gainCutLate;
                    
                    // Final safety check
                    if (!std::isfinite(compressedSample))
                    {
                        compressedSample = 0.0f;
                    }
                }
                
                // Apply makeup gain
                float makeupGainLinear = std::pow(10.0f, currentMakeupGain / 20.0f);
                compressedSample *= makeupGainLinear;
                
                // Mix dry and wet (parallel compression)
                float outputSample = drySample * (1.0f - currentMix) + compressedSample * currentMix;
                
                buffer.setSample(ch, sample, outputSample);
            }
        }
    }
    
    float compressSample(float inputSample, float threshold, float ratio, 
                        float attackCoeff, float releaseCoeff, int channel,
                        std::vector<float>& rmsBuffer, int& rmsIndex, float& rmsSum, float& gainReduction)
    {
        // RMS Detection (10ms window)
        float inputSquared = inputSample * inputSample;
        
        // Update RMS circular buffer
        if (rmsBufferSize_ > 0)
        {
            rmsSum -= rmsBuffer[rmsIndex]; // Remove old sample
            rmsBuffer[rmsIndex] = inputSquared; // Add new sample
            rmsSum += inputSquared;
            rmsIndex = (rmsIndex + 1) % rmsBufferSize_;
            
            // Calculate RMS level in dB
            float rmsValue = std::sqrt(rmsSum / static_cast<float>(rmsBufferSize_));
            float rmsDb = rmsValue > 0.0f ? 20.0f * std::log10(rmsValue) : -100.0f;
            
            // True OTT-style dual compression with proper upward/downward sections
            float gainChangeDb = 0.0f;
            
            // Split threshold into upward and downward sections like real OTT
            float upwardThreshold = threshold - 15.0f; // 15dB below for upward section
            float downwardThreshold = threshold; // Main threshold for downward
            
            // Downward compression (compress loud signals above threshold)
            if (rmsDb > downwardThreshold)
            {
                float overshoot = rmsDb - downwardThreshold;
                float downwardRatio = juce::jmax(2.0f, ratio); // Minimum 2:1 for downward
                gainChangeDb -= overshoot * (1.0f - 1.0f / downwardRatio);
            }
            
            // Upward compression (OTT's signature - aggressively boost quiet parts)
            if (rmsDb < upwardThreshold && rmsDb > -50.0f) // Don't boost noise floor
            {
                float undershoot = upwardThreshold - rmsDb;
                float upwardRatio = juce::jmax(3.0f, ratio * 0.8f); // Strong upward compression
                
                // More aggressive upward compression curve - this is the OTT magic
                float upwardGain = undershoot * (1.0f - 1.0f / upwardRatio);
                
                // Apply controlled scaling to prevent excessive gain buildup
                float quietnessScale = juce::jlimit(0.1f, 0.6f, (upwardThreshold - rmsDb) / 30.0f);
                gainChangeDb += upwardGain * quietnessScale * 0.2f; // Controlled upward compression to prevent gain stacking
            }
            
            // Middle zone (between upward and downward thresholds) - gentle expansion
            if (rmsDb >= upwardThreshold && rmsDb <= downwardThreshold)
            {
                // Controlled expansion in the middle zone to prevent gain stacking
                float middleZoneWidth = downwardThreshold - upwardThreshold;
                float positionInZone = (rmsDb - upwardThreshold) / middleZoneWidth; // 0-1
                float expansionAmount = (1.0f - positionInZone) * 0.05f; // Reduced expansion to control gain
                gainChangeDb += expansionAmount;
            }
            
            // Convert to linear gain change
            float targetGainReduction = std::pow(10.0f, gainChangeDb / 20.0f);
            
            // Apply attack/release envelope
            if (targetGainReduction < gainReduction)
            {
                // Attack (gain reduction increasing or boost decreasing)
                gainReduction = targetGainReduction + (gainReduction - targetGainReduction) * attackCoeff;
            }
            else
            {
                // Release (gain reduction decreasing or boost increasing)  
                gainReduction = targetGainReduction + (gainReduction - targetGainReduction) * releaseCoeff;
            }
            
            // Apply compression/expansion with safety checks
            float outputSample = inputSample * gainReduction;
            
            // Safety check for NaN, infinity, or excessive values
            if (!std::isfinite(outputSample) || std::abs(outputSample) > 10.0f)
            {
                return 0.0f; // Return silence if something went wrong
            }
            
            return outputSample;
        }
        
        return inputSample;
    }
    
    void initializeCrossoverFilters()
    {
        // Crossover frequencies: 200Hz and 2kHz
        float lowCrossover = 200.0f;
        float highCrossover = 2000.0f;
        
        for (int ch = 0; ch < 2; ++ch)
        {
            // Low band: Lowpass at 200Hz (24dB/octave Linkwitz-Riley)
            lowBandFilter[ch][0].setSampleRate(sampleRate);
            lowBandFilter[ch][0].setCutoffFrequency(lowCrossover);
            lowBandFilter[ch][0].setResonance(0.707f);
            lowBandFilter[ch][0].setFilterType(SimpleStableFilter::LOWPASS);
            lowBandFilter[ch][0].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            lowBandFilter[ch][1].setSampleRate(sampleRate);
            lowBandFilter[ch][1].setCutoffFrequency(lowCrossover);
            lowBandFilter[ch][1].setResonance(0.707f);
            lowBandFilter[ch][1].setFilterType(SimpleStableFilter::LOWPASS);
            lowBandFilter[ch][1].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            // Mid band: Highpass at 200Hz + Lowpass at 2kHz
            midBandFilterHP[ch][0].setSampleRate(sampleRate);
            midBandFilterHP[ch][0].setCutoffFrequency(lowCrossover);
            midBandFilterHP[ch][0].setResonance(0.707f);
            midBandFilterHP[ch][0].setFilterType(SimpleStableFilter::HIGHPASS);
            midBandFilterHP[ch][0].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            midBandFilterHP[ch][1].setSampleRate(sampleRate);
            midBandFilterHP[ch][1].setCutoffFrequency(lowCrossover);
            midBandFilterHP[ch][1].setResonance(0.707f);
            midBandFilterHP[ch][1].setFilterType(SimpleStableFilter::HIGHPASS);
            midBandFilterHP[ch][1].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            midBandFilterLP[ch][0].setSampleRate(sampleRate);
            midBandFilterLP[ch][0].setCutoffFrequency(highCrossover);
            midBandFilterLP[ch][0].setResonance(0.707f);
            midBandFilterLP[ch][0].setFilterType(SimpleStableFilter::LOWPASS);
            midBandFilterLP[ch][0].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            midBandFilterLP[ch][1].setSampleRate(sampleRate);
            midBandFilterLP[ch][1].setCutoffFrequency(highCrossover);
            midBandFilterLP[ch][1].setResonance(0.707f);
            midBandFilterLP[ch][1].setFilterType(SimpleStableFilter::LOWPASS);
            midBandFilterLP[ch][1].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            // High band: Highpass at 2kHz (24dB/octave Linkwitz-Riley)
            highBandFilter[ch][0].setSampleRate(sampleRate);
            highBandFilter[ch][0].setCutoffFrequency(highCrossover);
            highBandFilter[ch][0].setResonance(0.707f);
            highBandFilter[ch][0].setFilterType(SimpleStableFilter::HIGHPASS);
            highBandFilter[ch][0].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
            
            highBandFilter[ch][1].setSampleRate(sampleRate);
            highBandFilter[ch][1].setCutoffFrequency(highCrossover);
            highBandFilter[ch][1].setResonance(0.707f);
            highBandFilter[ch][1].setFilterType(SimpleStableFilter::HIGHPASS);
            highBandFilter[ch][1].setFilterSlope(SimpleStableFilter::SLOPE_12DB);
        }
    }
    
    // Parameters
    bool isEnabled = false;
    float threshold = -20.0f; // dB
    float ratio = 4.0f; // ratio
    float attack = 5.0f; // ms
    float release = 100.0f; // ms
    float makeupGain = 0.0f; // dB
    float wetMix = 1.0f; // 0.0 to 1.0
    bool multibandEnabled = false;
    
    // Parameter smoothing
    OnePoleSmoothing thresholdSmoothing, ratioSmoothing, attackSmoothing, releaseSmoothing, makeupGainSmoothing, mixSmoothing;
    
    // Processing state
    double sampleRate = 44100.0;
    std::vector<float> rmsBuffer[2]; // RMS calculation buffers
    int rmsBufferSize_ = 0;
    int rmsIndex[2] = {0, 0};
    float rmsSum[2] = {0.0f, 0.0f};
    float gainReduction[2] = {1.0f, 1.0f}; // Linear gain reduction values
    
    // Multiband crossover filters (2 stages each for 24dB/octave Linkwitz-Riley)
    SimpleStableFilter lowBandFilter[2][2];     // Low band: LP @ 200Hz (2 stages)
    SimpleStableFilter midBandFilterHP[2][2];   // Mid band: HP @ 200Hz (2 stages)
    SimpleStableFilter midBandFilterLP[2][2];   // Mid band: LP @ 2kHz (2 stages)  
    SimpleStableFilter highBandFilter[2][2];    // High band: HP @ 2kHz (2 stages)
    
    // Multiband RMS buffers (separate per band)
    std::vector<float> rmsBufferLow[2], rmsBufferMid[2], rmsBufferHigh[2];
    int rmsIndexLow[2] = {0, 0}, rmsIndexMid[2] = {0, 0}, rmsIndexHigh[2] = {0, 0};
    float rmsSumLow[2] = {0.0f, 0.0f}, rmsSumMid[2] = {0.0f, 0.0f}, rmsSumHigh[2] = {0.0f, 0.0f};
    float gainReductionLow[2] = {1.0f, 1.0f}, gainReductionMid[2] = {1.0f, 1.0f}, gainReductionHigh[2] = {1.0f, 1.0f};
};

// Multi-mode distortion effect with pre/post filtering
class DistortionEffect
{
public:
    enum DistortionType
    {
        TUBE = 1,           // "TUBE"
        SOFTCLIP = 2,       // "SOFTCLIP"  
        HARDCLIP = 3,       // "HARDCLIP"
        DIODE1 = 4,         // "DIODE 1"
        DIODE2 = 5,         // "DIODE 2"
        LINEAR_FOLD = 6,    // "LINEAR FOLD"
        SINE_FOLD = 7,      // "SINE FOLD"
        ZERO_SQUARE = 8,    // "ZERO-SQUARE"
        DOWNSAMPLE = 9,     // "DOWNSAMPLE"
        ASYMMETRIC = 10,    // "ASYMMETRIC"
        RECTIFY = 11,       // "RECTIFY"
        SINE_SHAPER = 12,   // "SINE SHAPER"
        STOMP_BOX = 13,     // "STOMP BOX"
        TAPE_SAT = 14,      // "TAPE SAT"
        OVERDRIVE = 15,     // "OVERDRIVE"
        SOFT_SAT = 16       // "SOFT SAT"
    };
    
    enum FilterPosition
    {
        FILTER_OFF = 0,
        FILTER_PRE = 1,
        FILTER_POST = 2
    };
    
    DistortionEffect()
    {
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        // Initialize parameter smoothing
        driveSmoothing.setSampleRate(sampleRate);
        driveSmoothing.setTimeConstantMs(10.0f);
        
        mixSmoothing.setSampleRate(sampleRate);
        mixSmoothing.setTimeConstantMs(20.0f);
        
        // Initialize filters
        preFilter[0].setSampleRate(sampleRate);
        preFilter[1].setSampleRate(sampleRate);
        postFilter[0].setSampleRate(sampleRate);
        postFilter[1].setSampleRate(sampleRate);
        
        updateFilters();
        reset();
    }
    
    void setEnabled(bool enabled) { isEnabled = enabled; }
    void setType(int type) { 
        distortionType = juce::jlimit(1, 16, type);
    }
    void setDrive(float driveValue) { 
        drive = juce::jlimit(0.0f, 100.0f, driveValue);
        driveSmoothing.setTarget(drive);
    }
    void setMix(float mixValue) { 
        wetMix = juce::jlimit(0.0f, 1.0f, mixValue);
        mixSmoothing.setTarget(wetMix);
    }
    
    // Filter controls
    void setFilterPosition(FilterPosition position) { 
        FilterPosition oldPosition = filterPosition;
        filterPosition = position;
        
        // Reset filter states when switching positions to ensure clean transition
        if (oldPosition != position)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                preFilter[ch].reset();
                postFilter[ch].reset();
            }
        }
        
        updateFilters();
    }
    void setFilterType(SimpleStableFilter::FilterType type) { 
        filterType = type;
        updateFilters();
    }
    void setFilterFrequency(float freq) { 
        filterFreq = juce::jlimit(20.0f, 20000.0f, freq);
        updateFilters();
    }
    void setFilterQ(float q) { 
        filterQ = juce::jlimit(0.1f, 30.0f, q);
        updateFilters();
    }
    
    void reset()
    {
        // Reset parameter smoothing
        driveSmoothing.reset(drive);
        mixSmoothing.reset(wetMix);
        
        // Reset filters
        for (int ch = 0; ch < 2; ++ch)
        {
            preFilter[ch].reset();
            postFilter[ch].reset();
        }
        
        // Reset bitcrusher state
        bitcrushHold[0] = bitcrushHold[1] = 0.0f;
        bitcrushCounter[0] = bitcrushCounter[1] = 0;
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled || sampleRate <= 0.0)
            return;
            
        const int numSamples = buffer.getNumSamples();
        const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
        
        if (numSamples <= 0 || numChannels <= 0)
            return;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float currentDrive = driveSmoothing.getNextValue();
            float currentMix = mixSmoothing.getNextValue();
            
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float inputSample = buffer.getSample(ch, sample);
                float drySample = inputSample;
                float processedSample = inputSample;
                
                // Pre-filtering (will pass through unchanged if filter is OFF)
                processedSample = preFilter[ch].processSample(processedSample, ch);
                
                // Apply distortion
                processedSample = applyDistortion(processedSample, currentDrive, ch);
                
                // Post-filtering (will pass through unchanged if filter is OFF)
                processedSample = postFilter[ch].processSample(processedSample, ch);
                
                // Mix dry and wet
                float outputSample = drySample * (1.0f - currentMix) + processedSample * currentMix;
                
                // Safety check
                if (!std::isfinite(outputSample))
                {
                    outputSample = 0.0f;
                }
                
                buffer.setSample(ch, sample, outputSample);
            }
        }
    }
    
private:
    float applyDistortion(float input, float driveAmount, int channel)
    {
        if (driveAmount <= 0.0f) return input;
        
        // Convert drive (0-100) to coloration amount - focus on character, not just gain
        // Lower values still produce obvious coloration
        float driveScale = 0.5f + (driveAmount / 100.0f) * 3.0f; // Range: 0.5 to 3.5 (much more reasonable)
        float driven = input * driveScale;
        
        switch (distortionType)
        {
            case TUBE:
                return tubeSaturation(driven);
                
            case SOFTCLIP:
                return softSaturation(driven);
                
            case HARDCLIP:
                return hardClipping(driven);
                
            case DIODE1:
                return diode1Distortion(driven);
                
            case DIODE2:
                return diode2Distortion(driven);
                
            case LINEAR_FOLD:
                return linearFolding(driven);
                
            case SINE_FOLD:
                return sineFolding(driven);
                
            case ZERO_SQUARE:
                return zeroSquareDistortion(driven);
                
            case DOWNSAMPLE:
                return decimateDistortion(driven, channel);
                
            case ASYMMETRIC:
                return asymmetricDistortion(driven);
                
            case RECTIFY:
                return rectifyDistortion(driven);
                
            case SINE_SHAPER:
                return sineShaping(driven);
                
            case STOMP_BOX:
                return stompBoxDistortion(driven);
                
            case TAPE_SAT:
                return tapeSaturation(driven);
                
            case OVERDRIVE:
                return overdriveSaturation(driven);
                
            case SOFT_SAT:
                return softSaturation(driven);
                
            default:
                return tubeSaturation(driven);
        }
    }
    
    // Distortion algorithms
    float tubeSaturation(float x)
    {
        // AGGRESSIVE tube saturation - maximum coloration, not subtlety!
        // Multiple stages of processing for rich harmonic content
        
        // Stage 1: Pre-emphasis to enhance mid-range tube character
        float preEmphasized = x + (x * x * x) * 0.4f; // Cubic nonlinearity for warmth
        
        // Stage 2: Asymmetric tube-style saturation with heavy coloration
        float tubeSignal;
        if (preEmphasized > 0.0f)
        {
            // Positive: Heavy 2nd harmonic generation + soft limiting
            float drive = preEmphasized * 2.5f; // More aggressive drive internally
            tubeSignal = std::tanh(drive) * 0.7f;
            
            // Add strong even harmonics (2nd, 4th)
            float harmonic2 = tubeSignal * tubeSignal * 0.6f;           // Strong 2nd harmonic
            float harmonic4 = tubeSignal * tubeSignal * tubeSignal * tubeSignal * 0.2f; // 4th harmonic
            tubeSignal += harmonic2 + harmonic4;
        }
        else
        {
            // Negative: Harder clipping with odd harmonics
            float drive = preEmphasized * 3.0f; // Even more aggressive on negative
            tubeSignal = std::tanh(drive) * 0.6f;
            
            // Add strong odd harmonics (3rd, 5th)
            float harmonic3 = tubeSignal * tubeSignal * tubeSignal * 0.4f;   // Strong 3rd
            float harmonic5 = std::pow(std::abs(tubeSignal), 5) * (tubeSignal < 0 ? -0.15f : 0.15f); // 5th
            tubeSignal += harmonic3 + harmonic5;
        }
        
        // Stage 3: Tube-style compression and warmth
        float compressed = tubeSignal / (1.0f + std::abs(tubeSignal) * 0.8f);
        
        // Stage 4: Add tube "glow" - subtle high-frequency rolling off with saturation
        float glow = compressed + std::sin(compressed * 6.28f) * 0.1f; // Subtle intermodulation
        
        // Final soft limiting to prevent overload while maintaining character
        return std::tanh(glow * 1.2f) * 0.8f;
    }
    
    float softSaturation(float x)
    {
        return x / (1.0f + std::abs(x));
    }
    
    float hardClipping(float x)
    {
        return juce::jlimit(-0.7f, 0.7f, x);
    }
    
    float diode1Distortion(float x)
    {
        // Asymmetric diode clipping - more aggressive on positive
        if (x > 0.4f)
            return 0.4f + std::tanh((x - 0.4f) * 3.0f) * 0.3f;
        else if (x < -0.6f)
            return -0.6f + std::tanh((x + 0.6f) * 2.0f) * 0.2f;
        return x;
    }
    
    float diode2Distortion(float x)
    {
        // Different diode characteristics - softer knee
        if (x > 0.3f)
            return 0.3f + std::tanh((x - 0.3f) * 2.0f) * 0.4f;
        else if (x < -0.4f)
            return -0.4f + std::tanh((x + 0.4f) * 2.5f) * 0.3f;
        return x;
    }
    
    float linearFolding(float x)
    {
        while (std::abs(x) > 1.0f)
        {
            if (x > 1.0f)
                x = 2.0f - x;
            else if (x < -1.0f)
                x = -2.0f - x;
        }
        return x;
    }
    
    float sineFolding(float x)
    {
        if (std::abs(x) > 1.0f)
        {
            return std::sin(x * juce::MathConstants<float>::pi);
        }
        return x;
    }
    
    float zeroSquareDistortion(float x)
    {
        return x > 0.0f ? 1.0f : -1.0f;
    }
    
    float decimateDistortion(float x, int channel)
    {
        // Reduce effective sample rate (downsample)
        if (++bitcrushCounter[channel] >= 8)
        {
            bitcrushHold[channel] = x;
            bitcrushCounter[channel] = 0;
        }
        return bitcrushHold[channel];
    }
    
    float asymmetricDistortion(float x)
    {
        if (x > 0.0f)
            return std::tanh(x * 2.0f);
        else
            return std::tanh(x * 0.5f);
    }
    
    float rectifyDistortion(float x)
    {
        return std::abs(x);
    }
    
    float sineShaping(float x)
    {
        return std::sin(x * juce::MathConstants<float>::pi * 0.5f);
    }
    
    float stompBoxDistortion(float x)
    {
        // Classic stomp box overdrive with soft knee
        return std::tanh(x * 1.5f) * 0.8f;
    }
    
    float tapeSaturation(float x)
    {
        // Tape-style saturation with even harmonics
        float saturated = std::tanh(x * 0.8f);
        return saturated + saturated * saturated * 0.1f; // Add 2nd harmonic
    }
    
    float overdriveSaturation(float x)
    {
        if (x > 0.0f)
            return 1.0f - std::exp(-x);
        else
            return -1.0f + std::exp(x);
    }
    
    void updateFilters()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            // Only configure the active filter to ensure proper state
            if (filterPosition == FILTER_PRE)
            {
                preFilter[ch].setCutoffFrequency(filterFreq);
                preFilter[ch].setResonance(filterQ);
                preFilter[ch].setFilterType(filterType);
                // Ensure post filter is off
                postFilter[ch].setFilterType(SimpleStableFilter::OFF);
            }
            else if (filterPosition == FILTER_POST)
            {
                postFilter[ch].setCutoffFrequency(filterFreq);
                postFilter[ch].setResonance(filterQ);
                postFilter[ch].setFilterType(filterType);
                // Ensure pre filter is off
                preFilter[ch].setFilterType(SimpleStableFilter::OFF);
            }
            else // FILTER_OFF
            {
                preFilter[ch].setFilterType(SimpleStableFilter::OFF);
                postFilter[ch].setFilterType(SimpleStableFilter::OFF);
            }
        }
    }
    
    // Parameters
    bool isEnabled = false;
    int distortionType = TUBE;
    float drive = 50.0f; // 0-100
    float wetMix = 1.0f; // 0-1
    
    // Filter parameters
    FilterPosition filterPosition = FILTER_OFF;
    SimpleStableFilter::FilterType filterType = SimpleStableFilter::LOWPASS;
    float filterFreq = 800.0f;  // Lower frequency to make LP filtering more obvious
    float filterQ = 2.0f;       // Higher Q for more pronounced filtering
    
    // Parameter smoothing
    OnePoleSmoothing driveSmoothing, mixSmoothing;
    
    // Processing state
    double sampleRate = 44100.0;
    SimpleStableFilter preFilter[2], postFilter[2];
    
    // Bitcrusher state
    float bitcrushHold[2] = {0.0f, 0.0f};
    int bitcrushCounter[2] = {0, 0};
};

// Stereo delay effect with ping-pong, filtering, and tempo sync
class DelayEffect
{
public:
    enum DelayMode
    {
        NORMAL = 0,
        PING_PONG = 1
    };
    
    DelayEffect()
    {
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        // Initialize parameter smoothing
        feedbackSmoothing.setSampleRate(sampleRate);
        feedbackSmoothing.setTimeConstantMs(10.0f);
        
        mixSmoothing.setSampleRate(sampleRate);
        mixSmoothing.setTimeConstantMs(20.0f);
        
        // Allocate delay buffers - maximum 2 seconds at any sample rate
        int maxDelaySize = static_cast<int>(sampleRate * 2.0);
        delayBufferL.resize(maxDelaySize, 0.0f);
        delayBufferR.resize(maxDelaySize, 0.0f);
        delayBufferSize = maxDelaySize;
        
        // Initialize filters for feedback path
        feedbackFilterL.setSampleRate(sampleRate);
        feedbackFilterR.setSampleRate(sampleRate);
        updateFilters();
        
        reset();
    }
    
    void setEnabled(bool enabled) { isEnabled = enabled; }
    
    void setFeedback(float feedback) { 
        feedbackAmount = juce::jlimit(0.0f, 0.95f, feedback);
        feedbackSmoothing.setTarget(feedbackAmount);
    }
    
    void setMix(float mixValue) { 
        wetMix = juce::jlimit(0.0f, 1.0f, mixValue);
        mixSmoothing.setTarget(wetMix);
    }
    
    void setDelayMode(DelayMode mode) { delayMode = mode; }
    
    void setLeftTime(float timeMs) { 
        leftTimeMs = juce::jlimit(1.0f, 2000.0f, timeMs);
        updateDelayTimes();
    }
    
    void setRightTime(float timeMs) { 
        rightTimeMs = juce::jlimit(1.0f, 2000.0f, timeMs);
        updateDelayTimes();
    }
    
    void setLinked(bool linked) { 
        isLinked = linked;
        if (linked) {
            setRightTime(leftTimeMs);
        }
    }
    
    void setBpmSync(bool sync) { bpmSync = sync; }
    
    void setLeftTriplet(bool triplet) { leftIsTriplet = triplet; updateDelayTimes(); }
    void setLeftDotted(bool dotted) { leftIsDotted = dotted; updateDelayTimes(); }
    void setRightTriplet(bool triplet) { rightIsTriplet = triplet; updateDelayTimes(); }
    void setRightDotted(bool dotted) { rightIsDotted = dotted; updateDelayTimes(); }
    
    // Filter controls
    void setFilterFreq(float freq) { 
        filterFreq = juce::jlimit(20.0f, 20000.0f, freq);
        updateFilters();
    }
    
    void setFilterQ(float q) { 
        filterQ = juce::jlimit(0.1f, 30.0f, q);
        updateFilters();
    }
    
    void reset()
    {
        // Clear delay buffers
        std::fill(delayBufferL.begin(), delayBufferL.end(), 0.0f);
        std::fill(delayBufferR.begin(), delayBufferR.end(), 0.0f);
        delayIndexL = delayIndexR = 0;
        
        // Reset parameter smoothing
        feedbackSmoothing.reset(feedbackAmount);
        mixSmoothing.reset(wetMix);
        
        // Reset filters
        feedbackFilterL.reset();
        feedbackFilterR.reset();
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled || sampleRate <= 0.0)
            return;
            
        const int numSamples = buffer.getNumSamples();
        const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
        
        if (numSamples <= 0 || numChannels <= 0 || delayBufferSize <= 0)
            return;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float currentFeedback = feedbackSmoothing.getNextValue();
            float currentMix = mixSmoothing.getNextValue();
            
            // Process stereo or mono
            float inputL = buffer.getSample(0, sample);
            float inputR = numChannels > 1 ? buffer.getSample(1, sample) : inputL;
            
            float outputL, outputR;
            
            if (delayMode == PING_PONG)
            {
                processPingPong(inputL, inputR, outputL, outputR, currentFeedback);
            }
            else
            {
                processNormal(inputL, inputR, outputL, outputR, currentFeedback);
            }
            
            // Mix dry and wet
            float finalL = inputL * (1.0f - currentMix) + outputL * currentMix;
            float finalR = inputR * (1.0f - currentMix) + outputR * currentMix;
            
            // Safety checks
            if (!std::isfinite(finalL)) finalL = 0.0f;
            if (!std::isfinite(finalR)) finalR = 0.0f;
            
            buffer.setSample(0, sample, finalL);
            if (numChannels > 1)
                buffer.setSample(1, sample, finalR);
        }
    }
    
private:
    void processNormal(float inputL, float inputR, float& outputL, float& outputR, float feedback)
    {
        // Read delayed samples
        float delayedL = delayBufferL[delayIndexL];
        float delayedR = delayBufferR[delayIndexR];
        
        // Apply feedback filtering
        delayedL = feedbackFilterL.processSample(delayedL, 0);
        delayedR = feedbackFilterR.processSample(delayedR, 1);
        
        // Feedback path
        float feedbackL = inputL + delayedL * feedback;
        float feedbackR = inputR + delayedR * feedback;
        
        // Write to delay buffers
        delayBufferL[delayIndexL] = feedbackL;
        delayBufferR[delayIndexR] = feedbackR;
        
        // Advance delay indices
        delayIndexL = (delayIndexL + 1) % leftDelaySamples;
        delayIndexR = (delayIndexR + 1) % rightDelaySamples;
        
        // Output delayed signals
        outputL = delayedL;
        outputR = delayedR;
    }
    
    void processPingPong(float inputL, float inputR, float& outputL, float& outputR, float feedback)
    {
        // Read delayed samples
        float delayedL = delayBufferL[delayIndexL];
        float delayedR = delayBufferR[delayIndexR];
        
        // Apply feedback filtering
        delayedL = feedbackFilterL.processSample(delayedL, 0);
        delayedR = feedbackFilterR.processSample(delayedR, 1);
        
        // Ping-pong: cross-feed the delays
        float feedbackL = inputL + delayedR * feedback; // Right delay feeds left
        float feedbackR = inputR + delayedL * feedback; // Left delay feeds right
        
        // Write to delay buffers
        delayBufferL[delayIndexL] = feedbackL;
        delayBufferR[delayIndexR] = feedbackR;
        
        // Advance delay indices
        delayIndexL = (delayIndexL + 1) % leftDelaySamples;
        delayIndexR = (delayIndexR + 1) % rightDelaySamples;
        
        // Output delayed signals
        outputL = delayedL;
        outputR = delayedR;
    }
    
    void updateDelayTimes()
    {
        if (sampleRate <= 0.0) return;
        
        float leftTime = leftTimeMs;
        float rightTime = rightTimeMs;
        
        // Apply triplet/dotted modifiers
        if (leftIsTriplet) leftTime *= 2.0f / 3.0f;
        if (leftIsDotted) leftTime *= 1.5f;
        if (rightIsTriplet) rightTime *= 2.0f / 3.0f;
        if (rightIsDotted) rightTime *= 1.5f;
        
        // Convert to samples
        leftDelaySamples = juce::jlimit(1, delayBufferSize - 1, static_cast<int>(leftTime * 0.001f * sampleRate));
        rightDelaySamples = juce::jlimit(1, delayBufferSize - 1, static_cast<int>(rightTime * 0.001f * sampleRate));
    }
    
    void updateFilters()
    {
        feedbackFilterL.setCutoffFrequency(filterFreq);
        feedbackFilterL.setResonance(filterQ);
        feedbackFilterL.setFilterType(SimpleStableFilter::LOWPASS);
        
        feedbackFilterR.setCutoffFrequency(filterFreq);
        feedbackFilterR.setResonance(filterQ);
        feedbackFilterR.setFilterType(SimpleStableFilter::LOWPASS);
    }
    
    // Parameters
    bool isEnabled = false;
    DelayMode delayMode = NORMAL;
    float feedbackAmount = 0.3f;
    float wetMix = 0.3f;
    float leftTimeMs = 250.0f;
    float rightTimeMs = 250.0f;
    bool isLinked = true;
    bool bpmSync = false;
    bool leftIsTriplet = false;
    bool leftIsDotted = false;
    bool rightIsTriplet = false;
    bool rightIsDotted = false;
    float filterFreq = 8000.0f;
    float filterQ = 0.707f;
    
    // Parameter smoothing
    OnePoleSmoothing feedbackSmoothing, mixSmoothing;
    
    // Processing state
    double sampleRate = 44100.0;
    std::vector<float> delayBufferL, delayBufferR;
    int delayBufferSize = 0;
    int delayIndexL = 0, delayIndexR = 0;
    int leftDelaySamples = 0, rightDelaySamples = 0;
    
    // Feedback filters
    SimpleStableFilter feedbackFilterL, feedbackFilterR;
};

// Multi-tap stereo chorus effect
class ChorusEffect
{
public:
    ChorusEffect()
    {
        // Initialize LFO phases - offset by 90 degrees for stereo width
        lfoPhase[0] = 0.0f;
        lfoPhase[1] = juce::MathConstants<float>::pi * 0.5f; // 90 degree offset
        
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        // Initialize parameter smoothing
        rateSmoothing.setSampleRate(sampleRate);
        rateSmoothing.setTimeConstantMs(20.0f);
        
        depthSmoothing.setSampleRate(sampleRate);
        depthSmoothing.setTimeConstantMs(20.0f);
        
        feedbackSmoothing.setSampleRate(sampleRate);
        feedbackSmoothing.setTimeConstantMs(20.0f);
        
        mixSmoothing.setSampleRate(sampleRate);
        mixSmoothing.setTimeConstantMs(20.0f);
        
        lpfCutoffSmoothing.setSampleRate(sampleRate);
        lpfCutoffSmoothing.setTimeConstantMs(5.0f); // Much faster smoothing for gentle transitions
        
        // Allocate delay buffers - maximum 100ms at any sample rate
        int maxDelaySize = static_cast<int>(sampleRate * 0.1); // 100ms
        delayBuffer[0].resize(maxDelaySize, 0.0f);
        delayBuffer[1].resize(maxDelaySize, 0.0f);
        delayBufferSize = maxDelaySize;
        
        // Initialize LPF
        lpf[0].setSampleRate(sampleRate);
        lpf[1].setSampleRate(sampleRate);
        updateLPF();
    }
    
    void setEnabled(bool enabled) { isEnabled = enabled; }
    void setRate(float rate) { 
        rateHz = juce::jlimit(0.1f, 10.0f, rate);
        rateSmoothing.setTarget(rateHz);
    }
    void setDelay1(float delayMs) { 
        delay1Ms = juce::jlimit(1.0f, 50.0f, delayMs);
    }
    void setDelay2(float delayMs) { 
        delay2Ms = juce::jlimit(1.0f, 50.0f, delayMs);
    }
    void setDepth(float depth) { 
        depthMs = juce::jlimit(0.0f, 20.0f, depth);
        depthSmoothing.setTarget(depthMs);
    }
    void setFeedback(float feedback) { 
        feedbackAmount = juce::jlimit(0.0f, 0.95f, feedback);
        feedbackSmoothing.setTarget(feedbackAmount);
    }
    void setLPFCutoff(float cutoffHz) { 
        lpfCutoff = juce::jlimit(200.0f, 20000.0f, cutoffHz);
        
        // Update immediately - no smoothing for instant response
        currentLPFCutoff = lpfCutoff;
        updateLPF();
    }
    void setMix(float mix) { 
        wetMix = juce::jlimit(0.0f, 1.0f, mix);
        mixSmoothing.setTarget(wetMix);
    }
    
    void reset()
    {
        // Clear delay buffers
        for (int ch = 0; ch < 2; ++ch)
        {
            if (!delayBuffer[ch].empty())
                std::fill(delayBuffer[ch].begin(), delayBuffer[ch].end(), 0.0f);
            delayIndex[ch] = 0;
        }
        
        // Reset LFO phases
        lfoPhase[0] = 0.0f;
        lfoPhase[1] = juce::MathConstants<float>::pi * 0.5f;
        
        // Reset filters
        lpf[0].reset();
        lpf[1].reset();
        
        // Reset smoothing
        rateSmoothing.reset(rateHz);
        depthSmoothing.reset(depthMs);
        feedbackSmoothing.reset(feedbackAmount);
        mixSmoothing.reset(wetMix);
        lpfCutoffSmoothing.reset(lpfCutoff);
        
        // Initialize current LPF cutoff and update filters
        currentLPFCutoff = lpfCutoff;
        if (sampleRate > 0.0)
            updateLPF();
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled || sampleRate <= 0.0)
            return;
            
        const int numSamples = buffer.getNumSamples();
        const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
        
        // LPF is updated immediately in setLPFCutoff() - no per-sample updates needed
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed parameters
            float currentRate = rateSmoothing.getNextValue();
            float currentDepth = depthSmoothing.getNextValue();
            float currentFeedback = feedbackSmoothing.getNextValue();
            float currentMix = mixSmoothing.getNextValue();
            
            // Calculate LFO increment
            float lfoIncrement = 2.0f * juce::MathConstants<float>::pi * currentRate / static_cast<float>(sampleRate);
            
            for (int ch = 0; ch < numChannels; ++ch)
            {
                // Read input sample
                float inputSample = buffer.getSample(ch, sample);
                float drySample = inputSample;
                
                // Update LFO
                lfoPhase[ch] += lfoIncrement;
                if (lfoPhase[ch] >= 2.0f * juce::MathConstants<float>::pi)
                    lfoPhase[ch] -= 2.0f * juce::MathConstants<float>::pi;
                
                // Calculate modulated delay time
                float lfoValue = std::sin(lfoPhase[ch]);
                float baseDelay = (ch == 0) ? delay1Ms : delay2Ms;
                float modulatedDelayMs = baseDelay + (currentDepth * lfoValue);
                
                // Convert to samples and clamp
                float delaySamples = modulatedDelayMs * static_cast<float>(sampleRate) / 1000.0f;
                delaySamples = juce::jlimit(1.0f, static_cast<float>(delayBufferSize - 1), delaySamples);
                
                // Calculate interpolated delay tap
                int delayInt = static_cast<int>(delaySamples);
                float delayFrac = delaySamples - delayInt;
                
                int readPos1 = (delayIndex[ch] - delayInt + delayBufferSize) % delayBufferSize;
                int readPos2 = (readPos1 - 1 + delayBufferSize) % delayBufferSize;
                
                float sample1 = delayBuffer[ch][readPos1];
                float sample2 = delayBuffer[ch][readPos2];
                float delayedSample = sample1 + delayFrac * (sample2 - sample1); // Linear interpolation
                
                // Apply feedback
                float feedbackSample = inputSample + (currentFeedback * delayedSample);
                
                // Write to delay buffer
                delayBuffer[ch][delayIndex[ch]] = feedbackSample;
                
                // Create wet signal and apply lowpass filter to wet signal (if cutoff is low enough to matter)
                float wetSample;
                if (currentLPFCutoff < 18000.0f) // Only filter if cutoff is meaningfully low
                {
                    wetSample = lpf[ch].processSample(delayedSample, ch);
                }
                else
                {
                    wetSample = delayedSample; // Bypass filter when at high frequencies
                }
                
                // Mix dry and wet
                float outputSample = drySample * (1.0f - currentMix) + wetSample * currentMix;
                
                // Write back to buffer
                buffer.setSample(ch, sample, outputSample);
                
                // Advance delay index
                delayIndex[ch] = (delayIndex[ch] + 1) % delayBufferSize;
            }
        }
    }
    
private:
    void updateLPF()
    {
        lpf[0].setCutoffFrequency(currentLPFCutoff);
        lpf[0].setResonance(0.707f); // No resonance - clean filtering
        lpf[0].setFilterType(SimpleStableFilter::LOWPASS);
        lpf[0].setFilterSlope(SimpleStableFilter::SLOPE_12DB); // Standard 12dB slope
        
        lpf[1].setCutoffFrequency(currentLPFCutoff);
        lpf[1].setResonance(0.707f); // No resonance - clean filtering
        lpf[1].setFilterType(SimpleStableFilter::LOWPASS);
        lpf[1].setFilterSlope(SimpleStableFilter::SLOPE_12DB); // Standard 12dB slope
    }
    
    // Parameters
    bool isEnabled = false;
    float rateHz = 2.0f;
    float delay1Ms = 20.0f;
    float delay2Ms = 30.0f;
    float depthMs = 5.0f;
    float feedbackAmount = 0.2f;
    float lpfCutoff = 20000.0f;
    float wetMix = 0.5f;
    float currentLPFCutoff = 20000.0f;
    
    // Parameter smoothing
    OnePoleSmoothing rateSmoothing, depthSmoothing, feedbackSmoothing, mixSmoothing, lpfCutoffSmoothing;
    
    // Processing state
    double sampleRate = 44100.0;
    std::vector<float> delayBuffer[2]; // Stereo delay buffers
    int delayBufferSize = 0;
    int delayIndex[2] = {0, 0};
    float lfoPhase[2] = {0.0f, 0.0f}; // LFO phases for L/R channels
    
    // Built-in lowpass filters
    SimpleStableFilter lpf[2];
};

class SummonerXSerum2AudioProcessor : public juce::AudioProcessor
{
public:
    SummonerXSerum2AudioProcessor();
    ~SummonerXSerum2AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    std::function<void()> onPresetApplied;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setResponses(const std::vector<std::map<std::string, std::string>>& newResponses);
    void applyResponseAtIndex(int index);
    void nextResponse();
    void previousResponse();

    int getCurrentResponseIndex() const { return currentResponseIndex; }
    int getResponseCount() const { 
        juce::ScopedLock lock(responseLock); 
        return static_cast<int>(responses.size()); 
    }
    
    // Synthesizer controls
    void setSynthVolume(float volume) { synthVolume = volume; }
    float getSynthVolume() const { return synthVolume; }
    
    void setSynthDetune(float detune) { 
        synthDetune = detune; 
        updateDetune();
    }
    float getSynthDetune() const { return synthDetune; }
    
    void setSynthStereoWidth(float width) { 
        synthStereoWidth = width; 
        updateStereoWidth();
    }
    float getSynthStereoWidth() const { return synthStereoWidth; }
    
    void setSynthPan(float pan) { 
        synthPan = pan; 
        updatePan();
    }
    float getSynthPan() const { return synthPan; }
    
    void setSynthPhase(float phase) { 
        synthPhase = phase; 
        updatePhase();
    }
    float getSynthPhase() const { return synthPhase; }
    
    void setSynthAttack(float attack) { 
        synthAttack = attack; 
        updateEnvelopeParameters();
    }
    float getSynthAttack() const { return synthAttack; }
    
    void setSynthDecay(float decay) { 
        synthDecay = decay; 
        updateEnvelopeParameters();
    }
    float getSynthDecay() const { return synthDecay; }
    
    void setSynthSustain(float sustain) { 
        synthSustain = sustain; 
        updateEnvelopeParameters();
    }
    float getSynthSustain() const { return synthSustain; }
    
    void setSynthRelease(float release) { 
        synthRelease = release; 
        updateEnvelopeParameters();
    }
    float getSynthRelease() const { return synthRelease; }
    
    // Oscillator 1 controls
    void setOsc1Type(int type) { 
        osc1Type = type; 
        updateOsc1Type();
    }
    int getOsc1Type() const { return osc1Type; }
    
    void setOsc1PulseWidth(float width) { 
        osc1PulseWidth = width; 
        updateOsc1PulseWidth();
    }
    float getOsc1PulseWidth() const { return osc1PulseWidth; }
    
    void setOsc1Octave(int oct) { 
        osc1Octave = oct; 
        updateOsc1Octave();
    }
    int getOsc1Octave() const { return osc1Octave; }
    
    void setOsc1Semitone(int semi) { 
        osc1Semitone = semi; 
        updateOsc1Semitone();
    }
    int getOsc1Semitone() const { return osc1Semitone; }
    
    void setOsc1FineTune(int fine) { 
        osc1FineTune = fine; 
        updateOsc1FineTune();
    }
    int getOsc1FineTune() const { return osc1FineTune; }
    
    void setOsc1RandomPhase(bool random) { 
        osc1RandomPhase = random; 
        updateOsc1RandomPhase();
    }
    bool getOsc1RandomPhase() const { return osc1RandomPhase; }
    
    void setOsc1VoiceCount(int count) { 
        osc1VoiceCount = count; 
        updateOsc1VoiceCount();
    }
    int getOsc1VoiceCount() const { return osc1VoiceCount; }

    void setOsc1Volume(float volume) {
        osc1Volume = volume;
        updateOsc1Volume();
    }
    float getOsc1Volume() const { return osc1Volume; }
    
    // Second oscillator controls
    void setOsc2Volume(float volume) { 
        osc2Volume = volume; // Made same as OSC1 - removed gain reduction
        updateOsc2Parameters();
    }
    float getOsc2Volume() const { return osc2Volume; }
    
    void setOsc2Detune(float detune) { 
        osc2Detune = detune; 
        updateOsc2Parameters();
    }
    float getOsc2Detune() const { return osc2Detune; }
    
    void setOsc2Stereo(float stereo) { 
        osc2Stereo = stereo; 
        updateOsc2Parameters();
    }
    float getOsc2Stereo() const { return osc2Stereo; }
    
    void setOsc2Pan(float pan) { 
        osc2Pan = pan; 
        updateOsc2Parameters();
    }
    float getOsc2Pan() const { return osc2Pan; }
    
    void setOsc2Octave(int octave) { 
        osc2Octave = juce::jlimit(-4, 4, octave); 
        updateOsc2Parameters();
    }
    int getOsc2Octave() const { return osc2Octave; }
    
    void setOsc2Semitone(int semitone) { 
        osc2Semitone = juce::jlimit(-12, 12, semitone); 
        updateOsc2Parameters();
    }
    int getOsc2Semitone() const { return osc2Semitone; }
    
    void setOsc2FineTune(int fineTune) { 
        osc2FineTune = juce::jlimit(-100, 100, fineTune); 
        updateOsc2Parameters();
    }
    int getOsc2FineTune() const { return osc2FineTune; }
    
    void setOsc2RandomPhase(bool randomPhase) { 
        osc2RandomPhase = randomPhase; 
        updateOsc2Parameters();
    }
    bool getOsc2RandomPhase() const { return osc2RandomPhase; }
    
    void setOsc2Phase(float phase) { 
        osc2Phase = phase; 
        updateOsc2Parameters();
    }
    float getOsc2Phase() const { return osc2Phase; }
    
    void setOsc2Enabled(bool enabled) { 
        osc2Enabled = enabled; 
        updateOsc2Parameters();
    }
    bool getOsc2Enabled() const { return osc2Enabled; }
    
    // Oscillator 2 ADSR controls
    void setOsc2Attack(float attack) {
        osc2Attack = attack;
        updateOsc2EnvelopeParameters();
    }
    float getOsc2Attack() const { return osc2Attack; }
    
    void setOsc2Decay(float decay) {
        osc2Decay = decay;
        updateOsc2EnvelopeParameters();
    }
    float getOsc2Decay() const { return osc2Decay; }
    
    void setOsc2Sustain(float sustain) {
        osc2Sustain = sustain;
        updateOsc2EnvelopeParameters();
    }
    float getOsc2Sustain() const { return osc2Sustain; }
    
    void setOsc2Release(float release) {
        osc2Release = release;
        updateOsc2EnvelopeParameters();
    }
    float getOsc2Release() const { return osc2Release; }
    
    // Oscillator 2 wave type control
    void setOsc2Type(int type) {
        osc2Type = type;
        updateOsc2Parameters();
    }
    int getOsc2Type() const { return osc2Type; }
    
    // Oscillator 2 unison voices control
    void setOsc2VoiceCount(int count) {
        osc2VoiceCount = count;
        updateOsc2Parameters();
    }
    int getOsc2VoiceCount() const { return osc2VoiceCount; }
    
    // Filter controls
    void setFilterCutoff(float cutoff) {
        filterCutoff = cutoff;
        updateFilterParameters();
    }
    float getFilterCutoff() const { return filterCutoff; }
    
    void setFilterResonance(float resonance) {
        filterResonance = resonance;
        updateFilterParameters();
    }
    float getFilterResonance() const { return filterResonance; }
    
    void setOsc1FilterEnabled(bool enabled) {
        osc1FilterEnabled = enabled;
        updateFilterRouting();
    }
    bool getOsc1FilterEnabled() const { return osc1FilterEnabled; }
    
    void setOsc2FilterEnabled(bool enabled) {
        osc2FilterEnabled = enabled;
        updateFilterRouting();
    }
    bool getOsc2FilterEnabled() const { return osc2FilterEnabled; }
    
    void setFilterLPEnabled(bool enabled) {
        filterLPEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterLPEnabled() const { return filterLPEnabled; }
    
    void setFilterHPEnabled(bool enabled) {
        filterHPEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterHPEnabled() const { return filterHPEnabled; }
    
    void setFilterBPEnabled(bool enabled) {
        filterBPEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterBPEnabled() const { return filterBPEnabled; }
    
    void setFilterNotchEnabled(bool enabled) {
        filterNotchEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterNotchEnabled() const { return filterNotchEnabled; }
    
    void setFilterCombEnabled(bool enabled) {
        filterCombEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterCombEnabled() const { return filterCombEnabled; }
    
    void setFilterFormantEnabled(bool enabled) {
        filterFormantEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilterFormantEnabled() const { return filterFormantEnabled; }
    
    void setFilter12dBEnabled(bool enabled) {
        filter12dBEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilter12dBEnabled() const { return filter12dBEnabled; }
    
    void setFilter24dBEnabled(bool enabled) {
        filter24dBEnabled = enabled;
        updateFilterParameters();
    }
    bool getFilter24dBEnabled() const { return filter24dBEnabled; }

    // Chorus effect controls
    void setChorusEnabled(bool enabled) {
        chorusEnabled = enabled;
        chorus.setEnabled(enabled);
    }
    bool getChorusEnabled() const { return chorusEnabled; }
    
    void setChorusRate(float rate) {
        chorusRate = rate;
        chorus.setRate(rate);
    }
    float getChorusRate() const { return chorusRate; }
    
    void setChorusDelay1(float delayMs) {
        chorusDelay1 = delayMs;
        chorus.setDelay1(delayMs);
    }
    float getChorusDelay1() const { return chorusDelay1; }
    
    void setChorusDelay2(float delayMs) {
        chorusDelay2 = delayMs;
        chorus.setDelay2(delayMs);
    }
    float getChorusDelay2() const { return chorusDelay2; }
    
    void setChorusDepth(float depth) {
        chorusDepth = depth;
        chorus.setDepth(depth);
    }
    float getChorusDepth() const { return chorusDepth; }
    
    void setChorusFeedback(float feedback) {
        chorusFeedback = feedback;
        chorus.setFeedback(feedback);
    }
    float getChorusFeedback() const { return chorusFeedback; }
    
    void setChorusLPF(float cutoffHz) {
        chorusLPF = cutoffHz;
        chorus.setLPFCutoff(cutoffHz);
    }
    float getChorusLPF() const { return chorusLPF; }
    
    void setChorusMix(float mix) {
        chorusMix = mix;
        chorus.setMix(mix);
    }
    float getChorusMix() const { return chorusMix; }

    // Compressor effect controls
    void setCompressorEnabled(bool enabled) {
        compressorEnabled = enabled;
        compressor.setEnabled(enabled);
    }
    bool getCompressorEnabled() const { return compressorEnabled; }
    
    void setCompressorThreshold(float thresholdDb) {
        compressorThreshold = thresholdDb;
        compressor.setThreshold(thresholdDb);
    }
    float getCompressorThreshold() const { return compressorThreshold; }
    
    void setCompressorRatio(float ratio) {
        compressorRatio = ratio;
        compressor.setRatio(ratio);
    }
    float getCompressorRatio() const { return compressorRatio; }
    
    void setCompressorAttack(float attackMs) {
        compressorAttack = attackMs;
        compressor.setAttack(attackMs);
    }
    float getCompressorAttack() const { return compressorAttack; }
    
    void setCompressorRelease(float releaseMs) {
        compressorRelease = releaseMs;
        compressor.setRelease(releaseMs);
    }
    float getCompressorRelease() const { return compressorRelease; }
    
    void setCompressorGain(float gainDb) {
        compressorGain = gainDb;
        compressor.setMakeupGain(gainDb);
    }
    float getCompressorGain() const { return compressorGain; }
    
    void setCompressorMix(float mix) {
        compressorMix = mix;
        compressor.setMix(mix);
    }
    float getCompressorMix() const { return compressorMix; }
    
    void setCompressorMultiband(bool enabled) {
        compressorMultiband = enabled;
        compressor.setMultiband(enabled);
    }
    bool getCompressorMultiband() const { return compressorMultiband; }
    
    // Distortion effect control methods
    void setDistortionEnabled(bool enabled) {
        distortionEnabled = enabled;
        distortion.setEnabled(enabled);
    }
    bool getDistortionEnabled() const { return distortionEnabled; }
    
    void setDistortionType(int type) {
        distortionType = juce::jlimit(1, 16, type);
        distortion.setType(type);
    }
    int getDistortionType() const { return distortionType; }
    
    void setDistortionDrive(float drive) {
        distortionDrive = juce::jlimit(0.0f, 100.0f, drive);
        distortion.setDrive(drive);
    }
    float getDistortionDrive() const { return distortionDrive; }
    
    void setDistortionMix(float mix) {
        distortionMix = juce::jlimit(0.0f, 1.0f, mix);
        distortion.setMix(mix);
    }
    float getDistortionMix() const { return distortionMix; }
    
    void setDistortionFilterPosition(int position) {
        distortionFilterPosition = juce::jlimit(0, 2, position);
        distortion.setFilterPosition(static_cast<DistortionEffect::FilterPosition>(position));
    }
    int getDistortionFilterPosition() const { return distortionFilterPosition; }
    
    void setDistortionFilterType(int type) {
        distortionFilterType = juce::jlimit(1, 3, type);
        SimpleStableFilter::FilterType filterType = SimpleStableFilter::LOWPASS;
        if (type == 2) filterType = SimpleStableFilter::BANDPASS;
        else if (type == 3) filterType = SimpleStableFilter::HIGHPASS;
        distortion.setFilterType(filterType);
    }
    int getDistortionFilterType() const { return distortionFilterType; }
    
    void setDistortionFilterFreq(float freq) {
        distortionFilterFreq = juce::jlimit(20.0f, 20000.0f, freq);
        distortion.setFilterFrequency(freq);
    }
    float getDistortionFilterFreq() const { return distortionFilterFreq; }
    
    void setDistortionFilterQ(float q) {
        distortionFilterQ = juce::jlimit(0.1f, 30.0f, q);
        distortion.setFilterQ(q);
    }
    float getDistortionFilterQ() const { return distortionFilterQ; }
    
    // Delay effect control methods
    void setDelayEnabled(bool enabled) {
        delayEnabled = enabled;
        delay.setEnabled(enabled);
    }
    bool getDelayEnabled() const { return delayEnabled; }
    
    void setDelayFeedback(float feedback) {
        delayFeedback = juce::jlimit(0.0f, 0.95f, feedback);
        delay.setFeedback(feedback);
    }
    float getDelayFeedback() const { return delayFeedback; }
    
    void setDelayMix(float mix) {
        delayMix = juce::jlimit(0.0f, 1.0f, mix);
        delay.setMix(mix);
    }
    float getDelayMix() const { return delayMix; }
    
    void setDelayPingPong(bool pingPong) {
        delayPingPong = pingPong;
        delay.setDelayMode(pingPong ? DelayEffect::PING_PONG : DelayEffect::NORMAL);
    }
    bool getDelayPingPong() const { return delayPingPong; }
    
    void setDelayLeftTime(float timeMs) {
        delayLeftTime = juce::jlimit(1.0f, 2000.0f, timeMs);
        delay.setLeftTime(timeMs);
    }
    float getDelayLeftTime() const { return delayLeftTime; }
    
    void setDelayRightTime(float timeMs) {
        delayRightTime = juce::jlimit(1.0f, 2000.0f, timeMs);
        delay.setRightTime(timeMs);
    }
    float getDelayRightTime() const { return delayRightTime; }
    
    void setDelaySync(bool sync) {
        delaySync = sync;
        delay.setBpmSync(sync);
    }
    bool getDelaySync() const { return delaySync; }
    
    void setDelayTriplet(bool triplet) {
        delayTriplet = triplet;
        delay.setLeftTriplet(triplet);
    }
    bool getDelayTriplet() const { return delayTriplet; }
    
    void setDelayDotted(bool dotted) {
        delayDotted = dotted;
        delay.setLeftDotted(dotted);
    }
    bool getDelayDotted() const { return delayDotted; }
    
    void setDelayRTriplet(bool triplet) {
        delayRTriplet = triplet;
        delay.setRightTriplet(triplet);
    }
    bool getDelayRTriplet() const { return delayRTriplet; }
    
    void setDelayRDotted(bool dotted) {
        delayRDotted = dotted;
        delay.setRightDotted(dotted);
    }
    bool getDelayRDotted() const { return delayRDotted; }
    
    void setDelayFilterFreq(float freq) {
        delayFilterFreq = juce::jlimit(20.0f, 20000.0f, freq);
        delay.setFilterFreq(freq);
    }
    float getDelayFilterFreq() const { return delayFilterFreq; }
    
    void setDelayFilterQ(float q) {
        delayFilterQ = juce::jlimit(0.1f, 30.0f, q);
        delay.setFilterQ(q);
    }
    float getDelayFilterQ() const { return delayFilterQ; }

private:
    std::map<std::string, int> parameterMap;
    void enumerateParameters();
    void setParameterByName(const std::pair<std::string, float>& paramData);
    float parseValue(const std::string& value);
    
    void updateEnvelopeParameters();
    void updateOsc1Type();
    void updateOsc1PulseWidth();
    void updateOsc1Octave();
    void updateOsc1Semitone();
    void updateOsc1FineTune();
    void updateOsc1RandomPhase();
    void updateOsc1VoiceCount();
    void updateDetune();
    void updateStereoWidth();
    void updatePan();
    void updatePhase();
    void updateOsc1Volume();
    void updateOsc2Parameters();
    void updateOsc2EnvelopeParameters();
    void updateFilterParameters();
    void updateFilterRouting();
    
    // Helper function to convert resonance (0.0-1.0) to Q factor
    float resonanceToQ(float resonance) const {
        // Convert 0.0-1.0 resonance to Q factor (0.707 to 2.0)
        // 0.0 = 0.707 (no resonance), 1.0 = 2.0 (mild resonance, no self-oscillation)
        return 0.707f + (resonance * 1.293f); // 1.293 = 2.0 - 0.707
    }

    SettingsComponent settingsComponent;
    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    mutable juce::CriticalSection responseLock;

    // Synthesizer components
    juce::Synthesiser synthesiser;
    float synthVolume = 3.0f; // +25 dB louder (approximately)
    float synthDetune = 0.0f;
    float synthStereoWidth = 0.5f;
    float synthPan = 0.0f;
    float synthPhase = 0.0f;
    float synthAttack = 0.01f;
    float synthDecay = 0.2f;
    float synthSustain = 0.7f;
    float synthRelease = 0.3f;
    
    // Oscillator 1 parameters
    float osc1PulseWidth = 0.5f;
    int osc1Octave = 0; // -4 to +4 octaves
    int osc1Semitone = 0; // -12 to +12 semitones
    int osc1FineTune = 0; // -100 to +100 cents
    bool osc1RandomPhase = true; // true = random phase, false = consistent phase
    int osc1VoiceCount = 1; // 1 to 16 unison voices
    int osc1Type = 1; // 0 = sine, 1 = saw
    float osc1Volume = 0.5f;
    
    // Second oscillator parameters
    float osc2Volume = 0.0f; // 0.0 to 1.0
    bool osc2Enabled = true; // true when wave button is toggled
    int osc2Type = 1; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
    int osc2VoiceCount = 1; // 1 to 16 unison voices
    float osc2Detune = 0.0f; // 0.0 to 1.0, controls detune amount
    float osc2Stereo = 0.5f; // 0.0 to 1.0, controls stereo width
    float osc2Pan = 0.0f; // -1.0 to 1.0, controls left/right panning
    int osc2Octave = 0; // -4 to +4 octaves
    int osc2Semitone = 0; // -12 to +12 semitones
    int osc2FineTune = 0; // -100 to +100 cents
    bool osc2RandomPhase = true; // true = random phase, false = fixed phase
    float osc2Phase = 0.0f; // 0.0 to 360.0 degrees, fixed phase starting point
    float osc2Attack = 0.01f;
    float osc2Decay = 0.2f;
    float osc2Sustain = 0.7f;
    float osc2Release = 0.3f;
    
    // Filter parameters
    float filterCutoff = 1000.0f; // 20Hz to 20kHz
    float filterResonance = 0.0f; // Resonance: 0.0 to 1.0 (0.0 = no resonance)
    bool filterLPEnabled = true; // LP filter enabled by default
    bool filterHPEnabled = false; // HP filter disabled by default
    bool filterBPEnabled = false; // BP filter disabled by default
    bool filterNotchEnabled = false; // Notch filter disabled by default
    bool filterCombEnabled = false; // Comb filter disabled by default
    bool filterFormantEnabled = false; // Formant filter disabled by default
    bool filter12dBEnabled = true; // 12dB slope enabled by default
    bool filter24dBEnabled = false; // 24dB slope disabled by default
    bool osc1FilterEnabled = false; // OSC 1 filter disabled by default
    bool osc2FilterEnabled = false; // OSC 2 filter disabled by default
    SimpleStableFilter osc1Filter; // Separate filter instance for OSC1
    SimpleStableFilter osc2Filter; // Separate filter instance for OSC2
    
    // Chorus effect parameters
    bool chorusEnabled = false;
    float chorusRate = 2.0f; // 0.1 to 10.0 Hz
    float chorusDelay1 = 20.0f; // 1.0 to 50.0 ms
    float chorusDelay2 = 30.0f; // 1.0 to 50.0 ms
    float chorusDepth = 5.0f; // 0.0 to 20.0 ms
    float chorusFeedback = 0.2f; // 0.0 to 0.95
    float chorusLPF = 20000.0f; // 200.0 to 20000.0 Hz
    float chorusMix = 0.5f; // 0.0 to 1.0
    ChorusEffect chorus; // Chorus effect instance
    
    // Compressor effect parameters
    bool compressorEnabled = false;
    float compressorThreshold = -20.0f; // -60.0 to 0.0 dB
    float compressorRatio = 4.0f; // 1.0 to 20.0
    float compressorAttack = 5.0f; // 0.1 to 100.0 ms
    float compressorRelease = 100.0f; // 10.0 to 1000.0 ms
    float compressorGain = 0.0f; // 0.0 to 30.0 dB
    float compressorMix = 1.0f; // 0.0 to 1.0
    bool compressorMultiband = false;
    CompressorEffect compressor; // Compressor effect instance
    
    // Distortion effect parameters
    bool distortionEnabled = false;
    int distortionType = 1; // 1-16 for different distortion types
    float distortionDrive = 50.0f; // 0.0 to 100.0
    float distortionMix = 1.0f; // 0.0 to 1.0
    int distortionFilterPosition = 0; // 0=Off, 1=Pre, 2=Post
    int distortionFilterType = 1; // 1=LP, 2=BP, 3=HP
    float distortionFilterFreq = 1000.0f; // 20.0 to 20000.0 Hz
    float distortionFilterQ = 0.707f; // 0.1 to 30.0
    DistortionEffect distortion; // Distortion effect instance
    
    // Delay effect parameters
    bool delayEnabled = false;
    float delayFeedback = 0.3f; // 0.0 to 0.95
    float delayMix = 0.3f; // 0.0 to 1.0
    bool delayPingPong = false; // Normal vs ping-pong mode
    float delayLeftTime = 250.0f; // 1.0 to 2000.0 ms
    float delayRightTime = 250.0f; // 1.0 to 2000.0 ms
    bool delaySync = false; // BPM sync enabled/disabled
    bool delayTriplet = false; // Triplet timing for left channel
    bool delayDotted = false; // Dotted timing for left channel
    bool delayRTriplet = false; // Triplet timing for right channel
    bool delayRDotted = false; // Dotted timing for right channel
    float delayFilterFreq = 8000.0f; // 20.0 to 20000.0 Hz
    float delayFilterQ = 0.707f; // 0.1 to 30.0
    DelayEffect delay; // Delay effect instance
    
    // Temporary buffers for separate oscillator processing
    juce::AudioBuffer<float> osc1Buffer;
    juce::AudioBuffer<float> osc2Buffer;
    
    struct SineWaveSound : public juce::SynthesiserSound
    {
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };
    
    struct SineWaveVoice : public juce::SynthesiserVoice
    {
        SineWaveVoice()
        {
            envelope.setSampleRate(44100.0);
            envelope.setParameters({0.1f, 0.8f, 0.3f, 0.5f});
            
            // Initialize per-voice filter instances
            voiceOsc1Filter.setSampleRate(44100.0);
            voiceOsc2Filter.setSampleRate(44100.0);
        }
        
        bool canPlaySound(juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<SineWaveSound*>(sound) != nullptr;
        }
        
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
        {
            // Calculate oscillator 1 frequency with all its modifications
            frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            // Apply oscillator 1 octave shift: each octave doubles/halves the frequency
            frequency *= std::pow(2.0, osc1Octave);
            // Apply oscillator 1 semitone shift: each semitone is 2^(1/12) frequency ratio
            frequency *= std::pow(2.0, osc1Semitone / 12.0);
            // Apply oscillator 1 fine tune shift: each cent is 2^(1/1200) frequency ratio
            frequency *= std::pow(2.0, osc1FineTune / 1200.0);
            level = velocity * 0.15 * 3.16f; // Increased by 10dB
            
            // Initialize oscillator 1 unison voices with detuning
            for (int i = 0; i < maxUnisonVoices; ++i)
            {
                // Calculate detune amount: spread voices based on detune parameter
                double detuneCents = 0.0;
                if (osc1VoiceCount > 1)
                {
                    // Max detune of 50 cents * detune parameter (0.0 to 1.0)
                    double maxDetune = 50.0 * detune;
                    detuneCents = (i - (osc1VoiceCount - 1) / 2.0) * (maxDetune * 2.0 / (osc1VoiceCount - 1));
                }
                
                // Apply detuning
                unisonFrequencies[i] = frequency * std::pow(2.0, detuneCents / 1200.0);
                unisonDeltas[i] = unisonFrequencies[i] * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
                
                // Set initial phase for oscillator 1
                if (osc1RandomPhase)
                {
                    unisonAngles[i] = random.nextFloat() * 2.0 * juce::MathConstants<double>::pi;
                }
                else
                {
                    // Convert fixed phase from degrees to radians
                    unisonAngles[i] = fixedPhase * juce::MathConstants<double>::pi / 180.0;
                }
            }
            
            // Keep legacy variables for oscillator 1 compatibility
            angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
            currentAngle = osc1RandomPhase ? random.nextFloat() * 2.0 * juce::MathConstants<double>::pi : (fixedPhase * juce::MathConstants<double>::pi / 180.0);
            
            // Initialize second oscillator with INDEPENDENT frequency and pitch offsets
            double osc2BaseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            // Apply oscillator 2 octave shift: each octave doubles/halves the frequency
            osc2BaseFrequency *= std::pow(2.0, osc2Octave);
            // Apply oscillator 2 semitone shift: each semitone is 2^(1/12) frequency ratio
            osc2BaseFrequency *= std::pow(2.0, osc2Semitone / 12.0);
            // Apply oscillator 2 fine tune shift: each cent is 2^(1/1200) frequency ratio
            osc2BaseFrequency *= std::pow(2.0, osc2FineTune / 1200.0);
            osc2AngleDelta = osc2BaseFrequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
            osc2CurrentAngle = osc2RandomPhase ? random.nextFloat() * 2.0 * juce::MathConstants<double>::pi : (osc2Phase * juce::MathConstants<double>::pi / 180.0);
            
            // Initialize oscillator 2 unison voices with controllable detuning
            for (int i = 0; i < maxOsc2UnisonVoices; ++i)
            {
                // Calculate detune amount for osc2: spread voices based on detune parameter
                double osc2DetuneCents = 0.0;
                if (osc2VoiceCount > 1)
                {
                    // Max detune of 50 cents * detune parameter (0.0 to 1.0)
                    double maxOsc2Detune = 50.0 * osc2Detune;
                    osc2DetuneCents = (i - (osc2VoiceCount - 1) / 2.0) * (maxOsc2Detune * 2.0 / (osc2VoiceCount - 1));
                }
                
                // Apply detuning to osc2
                osc2UnisonFrequencies[i] = osc2BaseFrequency * std::pow(2.0, osc2DetuneCents / 1200.0);
                osc2UnisonDeltas[i] = osc2UnisonFrequencies[i] * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
                
                // Set initial phase for oscillator 2 (random or fixed based on setting)
                osc2UnisonAngles[i] = osc2RandomPhase ? random.nextFloat() * 2.0 * juce::MathConstants<double>::pi : (osc2Phase * juce::MathConstants<double>::pi / 180.0);
            }
            
            envelope.setSampleRate(getSampleRate());
            envelope.noteOn();
            
            osc2Envelope.setSampleRate(getSampleRate());
            osc2Envelope.noteOn();
        }
        
        void stopNote(float, bool allowTailOff) override
        {
            envelope.noteOff();
            osc2Envelope.noteOff();
            
            if (!allowTailOff)
            {
                clearCurrentNote();
                angleDelta = 0.0;
                osc2AngleDelta = 0.0;
            }
        }
        
        void pitchWheelMoved(int) override {}
        void controllerMoved(int, int) override {}
        
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
        {
            if (angleDelta != 0.0)
            {
                while (--numSamples >= 0)
                {
                    float leftSample = 0.0f;
                    float rightSample = 0.0f;
                    
                    // Sum all active oscillator 1 unison voices with stereo panning
                    for (int voice = 0; voice < osc1VoiceCount; ++voice)
                    {
                        float voiceSample;
                        
                        // Generate waveform based on oscillator 1 type for this voice
                        if (osc1Type == 0) // Sine wave
                        {
                            voiceSample = (float)(std::sin(unisonAngles[voice]) * osc1Volume * 3.16f / 6.31f);
                        }
                        else if (osc1Type == 1) // Saw wave
                        {
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            voiceSample = (float)((2.0 * normalizedAngle - 1.0) * osc1Volume * 3.16f / 6.31f);
                        }
                        else if (osc1Type == 2) // Square wave
                        {
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            voiceSample = (float)((normalizedAngle < 0.5 ? 1.0 : -1.0) * osc1Volume * 3.16f / 6.31f);
                        }
                        else if (osc1Type == 3) // Triangle wave
                        {
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            if (normalizedAngle < 0.5)
                                voiceSample = (float)((normalizedAngle * 4.0 - 1.0) * osc1Volume * 3.16f / 6.31f);
                            else
                                voiceSample = (float)((3.0 - normalizedAngle * 4.0) * osc1Volume * 3.16f / 6.31f);
                        }
                        else if (osc1Type == 4) // White noise
                        {
                            // White noise: random values between -1 and 1 (same for all unison voices)
                            voiceSample = (float)((random.nextFloat() * 2.0f - 1.0f) * osc1Volume * 3.16f / 6.31f);
                        }
                        else if (osc1Type == 5) // Pink noise
                        {
                            // Pink noise using Paul Kellett's method (same for all unison voices)
                            float white = random.nextFloat() * 2.0f - 1.0f;
                            
                            pinkFilter[0] = 0.99886f * pinkFilter[0] + white * 0.0555179f;
                            pinkFilter[1] = 0.99332f * pinkFilter[1] + white * 0.0750759f;
                            pinkFilter[2] = 0.96900f * pinkFilter[2] + white * 0.1538520f;
                            pinkFilter[3] = 0.86650f * pinkFilter[3] + white * 0.3104856f;
                            pinkFilter[4] = 0.55000f * pinkFilter[4] + white * 0.5329522f;
                            pinkFilter[5] = -0.7616f * pinkFilter[5] - white * 0.0168980f;
                            
                            float pink = pinkFilter[0] + pinkFilter[1] + pinkFilter[2] + pinkFilter[3] + pinkFilter[4] + pinkFilter[5] + pinkFilter[6] + white * 0.5362f;
                            pinkFilter[6] = white * 0.115926f;
                            
                            voiceSample = (float)(pink * 0.11f * osc1Volume * 3.16f / 6.31f);
                        }
                        else
                        {
                            // Default to sine wave for unknown oscillator types
                            voiceSample = (float)(std::sin(unisonAngles[voice]) * osc1Volume * 3.16f / 6.31f);
                        }
                        
                        // Calculate stereo panning for this voice
                        float voicePan = 0.0f; // Center by default
                        if (osc1VoiceCount > 1)
                        {
                            // Spread voices across stereo field based on stereo width
                            voicePan = (voice - (osc1VoiceCount - 1) / 2.0f) / ((osc1VoiceCount - 1) / 2.0f) * stereoWidth;
                            voicePan = juce::jlimit(-1.0f, 1.0f, voicePan);
                        }
                        
                        // Apply panning (equal power panning)
                        float leftGain = std::cos((voicePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                        float rightGain = std::sin((voicePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                        
                        leftSample += voiceSample * leftGain;
                        rightSample += voiceSample * rightGain;
                        
                        // Update angle for this voice
                        unisonAngles[voice] += unisonDeltas[voice];
                    }
                    
                    // Apply level scaling to prevent clipping from multiple oscillator 1 voices
                    leftSample /= std::sqrt((float)osc1VoiceCount);
                    rightSample /= std::sqrt((float)osc1VoiceCount);
                    
                    // Apply envelope to oscillator 1 ONLY
                    auto envelopeValue = envelope.getNextSample();
                    leftSample *= envelopeValue;
                    rightSample *= envelopeValue;
                    
                    // Apply overall pan to oscillator 1 (-1.0 to +1.0 range like OSC2)
                    float normalizedPan = pan;
                    normalizedPan = juce::jlimit(-1.0f, 1.0f, normalizedPan);
                    
                    // Apply overall pan using equal power panning to oscillator 1
                    float panLeftGain = std::cos((normalizedPan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                    float panRightGain = std::sin((normalizedPan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                    
                    leftSample *= panLeftGain;
                    rightSample *= panRightGain;
                    
                    // Apply filter to OSC1 if enabled (using per-voice filter)
                    if (osc1FilterEnabled)
                    {
                        leftSample = voiceOsc1Filter.processSample(leftSample, 0);
                        rightSample = voiceOsc1Filter.processSample(rightSample, 1);
                    }
                    
                    // Add second oscillator if enabled with its own envelope and unison voices
                    if (osc2Enabled && osc2Volume > 0.0f && osc2AngleDelta != 0.0)
                    {
                        float osc2LeftSample = 0.0f;
                        float osc2RightSample = 0.0f;
                        
                        // Sum all active oscillator 2 unison voices
                        for (int voice = 0; voice < osc2VoiceCount; ++voice)
                        {
                            float voiceSample;
                            
                            // Generate waveform based on oscillator 2 type for this voice
                            if (osc2Type == 0) // Sine wave
                            {
                                voiceSample = (float)(std::sin(osc2UnisonAngles[voice]) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else if (osc2Type == 1) // Saw wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                voiceSample = (float)((2.0 * normalizedAngle - 1.0) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else if (osc2Type == 2) // Square wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                voiceSample = (float)((normalizedAngle < 0.5 ? 1.0 : -1.0) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else if (osc2Type == 3) // Triangle wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                if (normalizedAngle < 0.5)
                                    voiceSample = (float)((normalizedAngle * 4.0 - 1.0) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                                else
                                    voiceSample = (float)((3.0 - normalizedAngle * 4.0) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else if (osc2Type == 4) // White noise
                            {
                                // White noise: same for all unison voices
                                voiceSample = (float)((random.nextFloat() * 2.0f - 1.0f) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else if (osc2Type == 5) // Pink noise
                            {
                                // Pink noise: same for all unison voices
                                float white = random.nextFloat() * 2.0f - 1.0f;
                                
                                osc2PinkFilter[0] = 0.99886f * osc2PinkFilter[0] + white * 0.0555179f;
                                osc2PinkFilter[1] = 0.99332f * osc2PinkFilter[1] + white * 0.0750759f;
                                osc2PinkFilter[2] = 0.96900f * osc2PinkFilter[2] + white * 0.1538520f;
                                osc2PinkFilter[3] = 0.86650f * osc2PinkFilter[3] + white * 0.3104856f;
                                osc2PinkFilter[4] = 0.55000f * osc2PinkFilter[4] + white * 0.5329522f;
                                osc2PinkFilter[5] = -0.7616f * osc2PinkFilter[5] - white * 0.0168980f;
                                
                                float pink = osc2PinkFilter[0] + osc2PinkFilter[1] + osc2PinkFilter[2] + osc2PinkFilter[3] + osc2PinkFilter[4] + osc2PinkFilter[5] + osc2PinkFilter[6] + white * 0.5362f;
                                osc2PinkFilter[6] = white * 0.115926f;
                                
                                voiceSample = (float)(pink * 0.11f * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            else
                            {
                                // Default to sine wave for unknown types
                                voiceSample = (float)(std::sin(osc2UnisonAngles[voice]) * osc2Volume * 3.16f / 6.31f); // +10dB then -16dB = -6dB net
                            }
                            
                            // Calculate stereo panning for this osc2 voice
                            float voicePan = 0.0f; // Center by default
                            if (osc2VoiceCount > 1)
                            {
                                // Spread voices across stereo field based on stereo width
                                voicePan = (voice - (osc2VoiceCount - 1) / 2.0f) / ((osc2VoiceCount - 1) / 2.0f) * osc2Stereo;
                                voicePan = juce::jlimit(-1.0f, 1.0f, voicePan);
                            }
                            
                            // Apply panning (equal power panning)
                            float leftGain = std::cos((voicePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                            float rightGain = std::sin((voicePan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                            
                            osc2LeftSample += voiceSample * leftGain;
                            osc2RightSample += voiceSample * rightGain;
                            
                            // Update angle for this voice
                            osc2UnisonAngles[voice] += osc2UnisonDeltas[voice];
                        }
                        
                        // Apply level scaling to prevent clipping from multiple osc2 voices
                        osc2LeftSample /= std::sqrt((float)osc2VoiceCount);
                        osc2RightSample /= std::sqrt((float)osc2VoiceCount);
                        
                        // Apply osc2 envelope
                        auto osc2EnvelopeValue = osc2Envelope.getNextSample();
                        osc2LeftSample *= osc2EnvelopeValue;
                        osc2RightSample *= osc2EnvelopeValue;
                        
                        // Apply overall pan control for oscillator 2
                        float osc2PanLeftGain = std::cos((osc2Pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                        float osc2PanRightGain = std::sin((osc2Pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f);
                        
                        osc2LeftSample *= osc2PanLeftGain;
                        osc2RightSample *= osc2PanRightGain;
                        
                        // Apply filter to OSC2 if enabled (using per-voice filter)
                        if (osc2FilterEnabled)
                        {
                            osc2LeftSample = voiceOsc2Filter.processSample(osc2LeftSample, 0);
                            osc2RightSample = voiceOsc2Filter.processSample(osc2RightSample, 1);
                        }
                        
                        // Add to main mix
                        leftSample += osc2LeftSample;
                        rightSample += osc2RightSample;
                        
                        // Update legacy oscillator 2 angle for compatibility
                        osc2CurrentAngle += osc2AngleDelta;
                    }
                    
                    // Output to stereo channels
                    if (outputBuffer.getNumChannels() >= 2)
                    {
                        outputBuffer.addSample(0, startSample, leftSample);
                        outputBuffer.addSample(1, startSample, rightSample);
                    }
                    else if (outputBuffer.getNumChannels() == 1)
                    {
                        // Mono output - mix left and right
                        outputBuffer.addSample(0, startSample, (leftSample + rightSample) * 0.5f);
                    }
                    
                    ++startSample;
                    
                    if (!envelope.isActive() && !osc2Envelope.isActive())
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        osc2AngleDelta = 0.0;
                        break;
                    }
                }
            }
        }
        
        void setEnvelopeParameters(float attack, float decay, float sustain, float release)
        {
            envelope.setParameters({attack, decay, sustain, release});
        }
        
        void setOsc1Type(int type)
        {
            osc1Type = type;
        }
        
        void setOsc1PulseWidth(float width)
        {
            osc1PulseWidth = width;
        }
        
        void setOsc1Octave(int oct)
        {
            osc1Octave = oct;
        }
        
        void setOsc1Semitone(int semi)
        {
            osc1Semitone = semi;
        }
        
        void setOsc1FineTune(int fine)
        {
            osc1FineTune = fine;
        }
        
        void setOsc1RandomPhase(bool random)
        {
            osc1RandomPhase = random;
        }
        
        void setOsc1VoiceCount(int count)
        {
            osc1VoiceCount = juce::jlimit(1, 16, count);
        }

        void setOsc1Volume(float volume)
        {
            osc1Volume = volume;
        }
        
        void setDetune(float detuneAmount)
        {
            detune = detuneAmount;
        }
        
        void setStereoWidth(float width)
        {
            stereoWidth = width;
        }
        
        void setPan(float panValue)
        {
            pan = panValue;
        }
        
        void setPhase(float phaseValue)
        {
            fixedPhase = phaseValue;
        }
        
        void setOsc2Volume(float volume)
        {
            osc2Volume = volume;
        }
        
        void setOsc2Enabled(bool enabled)
        {
            osc2Enabled = enabled;
        }
        
        void setOsc2EnvelopeParameters(float attack, float decay, float sustain, float release)
        {
            osc2Envelope.setParameters({attack, decay, sustain, release});
        }
        
        void setOsc2Type(int type)
        {
            osc2Type = type;
        }
        
        void setOsc2VoiceCount(int count)
        {
            osc2VoiceCount = juce::jlimit(1, 16, count);
        }
        
        void setOsc2Detune(float detune)
        {
            osc2Detune = detune;
        }
        
        void setOsc2Stereo(float stereo)
        {
            osc2Stereo = stereo;
        }
        
        void setOsc2Pan(float pan)
        {
            osc2Pan = pan;
        }
        
        void setOsc2Octave(int octave)
        {
            osc2Octave = juce::jlimit(-4, 4, octave);
        }
        
        void setOsc2Semitone(int semitone)
        {
            osc2Semitone = juce::jlimit(-12, 12, semitone);
        }
        
        void setOsc2FineTune(int fineTune)
        {
            osc2FineTune = juce::jlimit(-100, 100, fineTune);
        }
        
        void setOsc2RandomPhase(bool randomPhase)
        {
            osc2RandomPhase = randomPhase;
        }
        
        void setOsc2Phase(float phase)
        {
            osc2Phase = phase;
        }
        
        void setFilterRouting(bool osc1ToFilter, bool osc2ToFilter, SimpleStableFilter* osc1FilterPtr, SimpleStableFilter* osc2FilterPtr)
        {
            osc1FilterEnabled = osc1ToFilter;
            osc2FilterEnabled = osc2ToFilter;
            osc1FilterInstance = osc1FilterPtr;
            osc2FilterInstance = osc2FilterPtr;
            
            // Update per-voice filters with the same parameters as the main filters
            updatePerVoiceFilters();
        }
        
        void updatePerVoiceFilters()
        {
            if (osc1FilterInstance != nullptr)
            {
                voiceOsc1Filter.setSampleRate(getSampleRate());
                voiceOsc1Filter.setCutoffFrequency(osc1FilterInstance->getCutoffFrequency());
                voiceOsc1Filter.setResonance(osc1FilterInstance->getResonance());
                voiceOsc1Filter.setFilterType(osc1FilterInstance->getFilterType());
                voiceOsc1Filter.setFilterSlope(osc1FilterInstance->getFilterSlope());
                // Don't reset - preserve filter state to avoid artifacts
            }
            if (osc2FilterInstance != nullptr)
            {
                voiceOsc2Filter.setSampleRate(getSampleRate());
                voiceOsc2Filter.setCutoffFrequency(osc2FilterInstance->getCutoffFrequency());
                voiceOsc2Filter.setResonance(osc2FilterInstance->getResonance());
                voiceOsc2Filter.setFilterType(osc2FilterInstance->getFilterType());
                voiceOsc2Filter.setFilterSlope(osc2FilterInstance->getFilterSlope());
                // Don't reset - preserve filter state to avoid artifacts
            }
        }
        
        
    private:
        double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
        double frequency = 0.0;
        
        // Oscillator 1 unison voice arrays (support up to 16 voices)
        static constexpr int maxUnisonVoices = 16;
        std::array<double, maxUnisonVoices> unisonAngles;
        std::array<double, maxUnisonVoices> unisonDeltas;
        std::array<double, maxUnisonVoices> unisonFrequencies;
        int osc1Type = 1; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
        float osc1PulseWidth = 0.5f;
        int osc1Octave = 0; // -4 to +4 octaves
        int osc1Semitone = 0; // -12 to +12 semitones
        int osc1FineTune = 0; // -100 to +100 cents
        bool osc1RandomPhase = true; // true = random phase, false = consistent phase
        int osc1VoiceCount = 1; // Number of unison voices (1-16)
        float osc1Volume = 0.5f; // Volume for oscillator 1 (0.0 to 1.0)
        float detune = 0.0f; // Detune amount (0.0 = no detune, 1.0 = max detune)
        float stereoWidth = 0.5f; // Stereo width (0.0 = mono, 1.0 = full stereo)
        float pan = 0.0f; // Pan position (-50 = left, 0 = center, 50 = right)
        float fixedPhase = 0.0f; // Fixed phase in degrees (0-360)
        juce::ADSR envelope;
        juce::ADSR osc2Envelope;
        juce::Random random;
        
        // Second oscillator parameters
        float osc2Volume = 0.0f;
        bool osc2Enabled = true;
        int osc2Type = 1; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
        int osc2VoiceCount = 1; // Number of unison voices (1-16)
        float osc2Detune = 0.0f; // Detune amount (0.0 to 1.0)
        float osc2Stereo = 0.5f; // Stereo width (0.0 to 1.0)
        float osc2Pan = 0.0f; // Pan position (-1.0 to 1.0)
        int osc2Octave = 0; // Octave offset (-4 to +4)
        int osc2Semitone = 0; // Semitone offset (-12 to +12)
        int osc2FineTune = 0; // Fine tune offset (-100 to +100 cents)
        bool osc2RandomPhase = true; // true = random phase, false = fixed phase
        float osc2Phase = 0.0f; // Fixed phase starting point (0.0 to 360.0 degrees)
        double osc2CurrentAngle = 0.0;
        double osc2AngleDelta = 0.0;
        
        // Oscillator 2 unison voice arrays (support up to 16 voices)
        static constexpr int maxOsc2UnisonVoices = 16;
        std::array<double, maxOsc2UnisonVoices> osc2UnisonAngles;
        std::array<double, maxOsc2UnisonVoices> osc2UnisonDeltas;
        std::array<double, maxOsc2UnisonVoices> osc2UnisonFrequencies;
        
        // Pink noise generation state for osc2 (Paul Kellett's method)
        float osc2PinkFilter[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        
        // Pink noise generation state (Paul Kellett's method)
        float pinkFilter[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        
        // Filter routing
        bool osc1FilterEnabled = false;
        bool osc2FilterEnabled = false;
        SimpleStableFilter* osc1FilterInstance = nullptr;
        SimpleStableFilter* osc2FilterInstance = nullptr;
        
        // Per-voice filter instances to prevent cross-voice interference
        SimpleStableFilter voiceOsc1Filter;
        SimpleStableFilter voiceOsc2Filter;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerXSerum2AudioProcessor)
};