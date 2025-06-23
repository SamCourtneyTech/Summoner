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

private:
    std::map<std::string, int> parameterMap;
    void enumerateParameters();
    void setParameterByName(const std::pair<std::string, float>& paramData);
    float parseValue(const std::string& value);

    SettingsComponent settingsComponent;
    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    mutable juce::CriticalSection responseLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerXSerum2AudioProcessor)
};