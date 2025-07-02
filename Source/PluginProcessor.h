#pragma once
#include <JuceHeader.h>
#include "SettingsComponent.h"

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
    
    void setOscillatorType(int type) { 
        oscillatorType = type; 
        updateOscillatorType();
    }
    int getOscillatorType() const { return oscillatorType; }
    
    void setPulseWidth(float width) { 
        pulseWidth = width; 
        updatePulseWidth();
    }
    float getPulseWidth() const { return pulseWidth; }
    
    void setOctave(int oct) { 
        octave = oct; 
        updateOctave();
    }
    int getOctave() const { return octave; }

private:
    std::map<std::string, int> parameterMap;
    void enumerateParameters();
    void setParameterByName(const std::pair<std::string, float>& paramData);
    float parseValue(const std::string& value);
    
    void updateEnvelopeParameters();
    void updateOscillatorType();
    void updatePulseWidth();
    void updateOctave();

    SettingsComponent settingsComponent;
    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    mutable juce::CriticalSection responseLock;

    // Synthesizer components
    juce::Synthesiser synthesiser;
    float synthVolume = 0.5f;
    float synthAttack = 0.1f;
    float synthDecay = 0.2f;
    float synthSustain = 0.7f;
    float synthRelease = 0.3f;
    float pulseWidth = 0.5f;
    int octave = 0; // -4 to +4 octaves
    int oscillatorType = 0; // 0 = sine, 1 = saw
    
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
        }
        
        bool canPlaySound(juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<SineWaveSound*>(sound) != nullptr;
        }
        
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
        {
            frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            // Apply octave shift: each octave doubles/halves the frequency
            frequency *= std::pow(2.0, octave);
            level = velocity * 0.15;
            angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
            currentAngle = 0.0;
            
            envelope.setSampleRate(getSampleRate());
            envelope.noteOn();
        }
        
        void stopNote(float, bool allowTailOff) override
        {
            envelope.noteOff();
            
            if (!allowTailOff)
            {
                clearCurrentNote();
                angleDelta = 0.0;
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
                    float currentSample;
                    
                    // Generate waveform based on oscillator type
                    if (oscillatorType == 0) // Sine wave
                    {
                        currentSample = (float)(std::sin(currentAngle) * level);
                    }
                    else if (oscillatorType == 1) // Saw wave
                    {
                        // Sawtooth wave: variable ramp using pulse width (0.5 = normal saw, <0.5 = reverse bias, >0.5 = forward bias)
                        auto normalizedAngle = std::fmod(currentAngle, 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                        if (pulseWidth < 0.5) {
                            // More reverse saw character
                            auto adjustedAngle = normalizedAngle * (1.0 - pulseWidth) + pulseWidth;
                            currentSample = (float)((2.0 * (1.0 - adjustedAngle) - 1.0) * level);
                        } else {
                            // More forward saw character
                            auto adjustedAngle = normalizedAngle * pulseWidth;
                            currentSample = (float)((2.0 * adjustedAngle - 1.0) * level);
                        }
                    }
                    else if (oscillatorType == 2) // Square wave
                    {
                        // Square wave: variable duty cycle using pulse width
                        auto normalizedAngle = std::fmod(currentAngle, 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                        currentSample = (float)((normalizedAngle < pulseWidth ? 1.0 : -1.0) * level);
                    }
                    else if (oscillatorType == 3) // Triangle wave
                    {
                        // Triangle wave: variable peak position using pulse width
                        auto normalizedAngle = std::fmod(currentAngle, 2.0 * juce::MathConstants<double>::pi) / (2.0 * juce::MathConstants<double>::pi);
                        if (normalizedAngle < pulseWidth)
                            currentSample = (float)((normalizedAngle / pulseWidth * 2.0 - 1.0) * level); // Rising: -1 to 1
                        else
                            currentSample = (float)((1.0 - (normalizedAngle - pulseWidth) / (1.0 - pulseWidth)) * 2.0 - 1.0) * level; // Falling: 1 to -1
                    }
                    else if (oscillatorType == 4) // White noise
                    {
                        // White noise: random values between -1 and 1
                        currentSample = (float)((random.nextFloat() * 2.0f - 1.0f) * level);
                    }
                    else if (oscillatorType == 5) // Pink noise
                    {
                        // Pink noise using Paul Kellett's method
                        float white = random.nextFloat() * 2.0f - 1.0f;
                        
                        pinkFilter[0] = 0.99886f * pinkFilter[0] + white * 0.0555179f;
                        pinkFilter[1] = 0.99332f * pinkFilter[1] + white * 0.0750759f;
                        pinkFilter[2] = 0.96900f * pinkFilter[2] + white * 0.1538520f;
                        pinkFilter[3] = 0.86650f * pinkFilter[3] + white * 0.3104856f;
                        pinkFilter[4] = 0.55000f * pinkFilter[4] + white * 0.5329522f;
                        pinkFilter[5] = -0.7616f * pinkFilter[5] - white * 0.0168980f;
                        
                        float pink = pinkFilter[0] + pinkFilter[1] + pinkFilter[2] + pinkFilter[3] + pinkFilter[4] + pinkFilter[5] + pinkFilter[6] + white * 0.5362f;
                        pinkFilter[6] = white * 0.115926f;
                        
                        currentSample = (float)(pink * 0.11f * level); // Scale down to prevent clipping
                    }
                    else
                    {
                        // Default to sine wave for unknown oscillator types
                        currentSample = (float)(std::sin(currentAngle) * level);
                    }
                    
                    auto envelopeValue = envelope.getNextSample();
                    currentSample *= envelopeValue;
                    
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                    
                    if (!envelope.isActive())
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
        }
        
        void setEnvelopeParameters(float attack, float decay, float sustain, float release)
        {
            envelope.setParameters({attack, decay, sustain, release});
        }
        
        void setOscillatorType(int type)
        {
            oscillatorType = type;
        }
        
        void setPulseWidth(float width)
        {
            pulseWidth = width;
        }
        
        void setOctave(int oct)
        {
            octave = oct;
        }
        
    private:
        double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
        double frequency = 0.0;
        int oscillatorType = 0; // 0 = sine, 1 = saw, 2 = square, 3 = triangle, 4 = white noise, 5 = pink noise
        float pulseWidth = 0.5f;
        int octave = 0; // -4 to +4 octaves
        juce::ADSR envelope;
        juce::Random random;
        
        // Pink noise generation state (Paul Kellett's method)
        float pinkFilter[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerXSerum2AudioProcessor)
};