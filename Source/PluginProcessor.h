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

private:
    std::map<std::string, int> parameterMap;
    void enumerateParameters();
    void setParameterByName(const std::pair<std::string, float>& paramData);
    float parseValue(const std::string& value);

    SettingsComponent settingsComponent;
    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    mutable juce::CriticalSection responseLock;

    // Synthesizer components
    juce::Synthesiser synthesiser;
    float synthVolume = 0.5f;
    
    struct SineWaveSound : public juce::SynthesiserSound
    {
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };
    
    struct SineWaveVoice : public juce::SynthesiserVoice
    {
        bool canPlaySound(juce::SynthesiserSound* sound) override
        {
            return dynamic_cast<SineWaveSound*>(sound) != nullptr;
        }
        
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
        {
            frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
            level = velocity * 0.15;
            tailOff = 0.0;
            angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / getSampleRate();
            currentAngle = 0.0;
        }
        
        void stopNote(float, bool allowTailOff) override
        {
            if (allowTailOff)
            {
                if (tailOff == 0.0)
                    tailOff = 1.0;
            }
            else
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
                if (tailOff > 0.0)
                {
                    while (--numSamples >= 0)
                    {
                        auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);
                        
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample(i, startSample, currentSample);
                        
                        currentAngle += angleDelta;
                        ++startSample;
                        
                        tailOff *= 0.99;
                        
                        if (tailOff <= 0.005)
                        {
                            clearCurrentNote();
                            angleDelta = 0.0;
                            break;
                        }
                    }
                }
                else
                {
                    while (--numSamples >= 0)
                    {
                        auto currentSample = (float)(std::sin(currentAngle) * level);
                        
                        for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                            outputBuffer.addSample(i, startSample, currentSample);
                        
                        currentAngle += angleDelta;
                        ++startSample;
                    }
                }
            }
        }
        
    private:
        double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
        double frequency = 0.0;
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerXSerum2AudioProcessor)
};