#pragma once
#include <JuceHeader.h>
#include <array>
#include "SettingsComponent.h"

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
        reset();
    }
    
    void setSampleRate(double newSampleRate)
    {
        sampleRate = newSampleRate;
        updateCoeff();
    }
    
    void setCutoffFrequency(float cutoff)
    {
        cutoffFreq = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.4), cutoff);
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
            
        // Apply input scaling to prevent numerical issues
        inputSample = juce::jlimit(-1.0f, 1.0f, inputSample);
        
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
            
            // Apply gentle saturation for stability
            output = std::tanh(output * 0.8f) * 1.25f;
            
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
            
            // Apply gentle saturation for stability
            output = std::tanh(output * 0.8f) * 1.25f;
            
            return output;
        }
        
        if (filterSlope == SLOPE_12DB)
        {
            // Single 12dB stage
            float output = inputSample * a0 + z1;
            
            // Update delay line
            z1 = inputSample * a1 + z2 - b1 * output;
            z2 = inputSample * a2 - b2 * output;
            
            // Apply gentle saturation for stability
            output = std::tanh(output * 0.8f) * 1.25f;
            
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
            
            // Apply gentle saturation for stability
            output = std::tanh(output * 0.8f) * 1.25f;
            
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
        float c = 1.0f / std::tan(omega * 0.5f);
        float cSq = c * c;
        float cQ = c / q; // Use Q factor instead of sqrt(2)
        
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
            // Higher Q = more feedback = more resonance
            feedbackGain = juce::jlimit(0.0f, 0.95f, (q - 0.707f) / 19.293f * 0.9f);
            
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
        
        // Clamp coefficients to prevent instability
        a0 = juce::jlimit(-2.0f, 2.0f, a0);
        a1 = juce::jlimit(-2.0f, 2.0f, a1);
        a2 = juce::jlimit(-2.0f, 2.0f, a2);
        b1 = juce::jlimit(-1.99f, 1.99f, b1);
        b2 = juce::jlimit(-1.99f, 1.99f, b2);
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
        osc2Volume = volume; 
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
        // Convert 0.0-1.0 resonance to Q factor (0.707 to 20.0)
        // 0.0 = 0.707 (no resonance), 1.0 = 20.0 (high resonance)
        return 0.707f + (resonance * 19.293f); // 19.293 = 20.0 - 0.707
    }

    SettingsComponent settingsComponent;
    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    mutable juce::CriticalSection responseLock;

    // Synthesizer components
    juce::Synthesiser synthesiser;
    float synthVolume = 0.5f;
    float synthDetune = 0.0f;
    float synthStereoWidth = 0.5f;
    float synthPan = 0.0f;
    float synthPhase = 0.0f;
    float synthAttack = 0.1f;
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
    int osc1Type = 0; // 0 = sine, 1 = saw
    float osc1Volume = 0.5f;
    
    // Second oscillator parameters
    float osc2Volume = 0.0f; // 0.0 to 1.0
    bool osc2Enabled = false; // true when sine button is toggled
    int osc2Type = 0; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
    int osc2VoiceCount = 1; // 1 to 16 unison voices
    float osc2Detune = 0.0f; // 0.0 to 1.0, controls detune amount
    float osc2Stereo = 0.5f; // 0.0 to 1.0, controls stereo width
    float osc2Pan = 0.0f; // -1.0 to 1.0, controls left/right panning
    int osc2Octave = 0; // -4 to +4 octaves
    int osc2Semitone = 0; // -12 to +12 semitones
    int osc2FineTune = 0; // -100 to +100 cents
    bool osc2RandomPhase = true; // true = random phase, false = fixed phase
    float osc2Phase = 0.0f; // 0.0 to 360.0 degrees, fixed phase starting point
    float osc2Attack = 0.1f;
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
            level = velocity * 0.15;
            
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
                            voiceSample = (float)(std::sin(unisonAngles[voice]) * level);
                        }
                        else if (osc1Type == 1) // Saw wave
                        {
                            // Sawtooth wave: variable ramp using pulse width
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            if (osc1PulseWidth < 0.5) {
                                // More reverse saw character
                                auto adjustedAngle = normalizedAngle * (1.0 - osc1PulseWidth) + osc1PulseWidth;
                                voiceSample = (float)((2.0 * (1.0 - adjustedAngle) - 1.0) * level);
                            } else {
                                // More forward saw character
                                auto adjustedAngle = normalizedAngle * osc1PulseWidth;
                                voiceSample = (float)((2.0 * adjustedAngle - 1.0) * level);
                            }
                        }
                        else if (osc1Type == 2) // Square wave
                        {
                            // Square wave: variable duty cycle using pulse width
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            voiceSample = (float)((normalizedAngle < osc1PulseWidth ? 1.0 : -1.0) * level);
                        }
                        else if (osc1Type == 3) // Triangle wave
                        {
                            // Triangle wave: variable peak position using pulse width
                            auto normalizedAngle = std::fmod(unisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                            if (normalizedAngle < osc1PulseWidth)
                                voiceSample = (float)((normalizedAngle / osc1PulseWidth * 2.0 - 1.0) * level); // Rising: -1 to 1
                            else
                                voiceSample = (float)((1.0 - (normalizedAngle - osc1PulseWidth) / (1.0 - osc1PulseWidth)) * 2.0 - 1.0) * level; // Falling: 1 to -1
                        }
                        else if (osc1Type == 4) // White noise
                        {
                            // White noise: random values between -1 and 1 (same for all unison voices)
                            voiceSample = (float)((random.nextFloat() * 2.0f - 1.0f) * level);
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
                            
                            voiceSample = (float)(pink * 0.11f * level); // Scale down to prevent clipping
                        }
                        else
                        {
                            // Default to sine wave for unknown oscillator types
                            voiceSample = (float)(std::sin(unisonAngles[voice]) * level);
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

                    // Apply Oscillator 1 volume
                    leftSample *= osc1Volume;
                    rightSample *= osc1Volume;
                    
                    // Apply envelope to oscillator 1 ONLY
                    auto envelopeValue = envelope.getNextSample();
                    leftSample *= envelopeValue;
                    rightSample *= envelopeValue;
                    
                    // Apply overall pan to oscillator 1 ONLY (-50 to +50 range)
                    float normalizedPan = pan / 50.0f; // Convert to -1.0 to +1.0 range
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
                                voiceSample = (float)(std::sin(osc2UnisonAngles[voice]) * osc2Volume);
                            }
                            else if (osc2Type == 1) // Saw wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                voiceSample = (float)((2.0 * normalizedAngle - 1.0) * osc2Volume);
                            }
                            else if (osc2Type == 2) // Square wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                voiceSample = (float)((normalizedAngle < 0.5 ? 1.0 : -1.0) * osc2Volume);
                            }
                            else if (osc2Type == 3) // Triangle wave
                            {
                                auto normalizedAngle = std::fmod(osc2UnisonAngles[voice], 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                                if (normalizedAngle < 0.5)
                                    voiceSample = (float)((normalizedAngle * 4.0 - 1.0) * osc2Volume);
                                else
                                    voiceSample = (float)((3.0 - normalizedAngle * 4.0) * osc2Volume);
                            }
                            else if (osc2Type == 4) // White noise
                            {
                                // White noise: same for all unison voices
                                voiceSample = (float)((random.nextFloat() * 2.0f - 1.0f) * osc2Volume);
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
                                
                                voiceSample = (float)(pink * 0.11f * osc2Volume);
                            }
                            else
                            {
                                // Default to sine wave for unknown types
                                voiceSample = (float)(std::sin(osc2UnisonAngles[voice]) * osc2Volume);
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
                voiceOsc1Filter.reset();
            }
            if (osc2FilterInstance != nullptr)
            {
                voiceOsc2Filter.setSampleRate(getSampleRate());
                voiceOsc2Filter.setCutoffFrequency(osc2FilterInstance->getCutoffFrequency());
                voiceOsc2Filter.setResonance(osc2FilterInstance->getResonance());
                voiceOsc2Filter.setFilterType(osc2FilterInstance->getFilterType());
                voiceOsc2Filter.setFilterSlope(osc2FilterInstance->getFilterSlope());
                voiceOsc2Filter.reset();
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
        int osc1Type = 0; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
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
        bool osc2Enabled = false;
        int osc2Type = 0; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
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