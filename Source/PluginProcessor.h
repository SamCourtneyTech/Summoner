#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"
class SummonerAudioProcessor : public juce::AudioProcessor
{
public:
    SummonerAudioProcessor();
    ~SummonerAudioProcessor() override;
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
    int getResponseCount() const { return responses.size(); }

    juce::AudioProcessorValueTreeState parameters;

private:

    Oscillator oscillator;
    float currentFrequency = 440.0f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
    double currentSampleRate = 0.0; 

    void updateFilter();

    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    juce::CriticalSection responseLock;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerAudioProcessor)
};



