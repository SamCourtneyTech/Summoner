#pragma once
#include <JuceHeader.h>
#include "Voice.h"
#include "LFO.h"

class SummonerAudioProcessor : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
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

    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::OwnedArray<Voice> voices;
    LFO lfo;
    float currentFrequency = 440.0f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
    double currentSampleRate = 0.0;

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize = 0;
    int delayWritePosition = 0;

    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> distortionToneFilter;
    float lastDistortionTone = -1.0f;

    juce::dsp::Compressor<float> compressor;
    juce::dsp::Gain<float> compressorMakeupGain;

    juce::dsp::Chorus<float> chorus;

    juce::dsp::Phaser<float> phaser;

    juce::AudioBuffer<float> flangerBuffer;
    int flangerBufferSize = 0;
    int flangerWritePosition = 0;
    LFO flangerLFO;
    float flangerPhase = 0.0f;

    float lastFilterCutoff = -1.0f;
    float lastFilterResonance = -1.0f;
    int lastFilterType = -1;

    // Map to track voices assigned to each note (for unison)
    std::map<int, std::vector<Voice*>> noteToVoices;

    void updateFilter();
    void updateNumVoices();
    Voice* findVoiceToSteal();

    std::vector<std::map<std::string, std::string>> responses;
    int currentResponseIndex = 0;
    juce::CriticalSection responseLock;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerAudioProcessor)
};