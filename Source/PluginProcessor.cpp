#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>

SummonerXSerum2AudioProcessor::SummonerXSerum2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    settingsComponent(*this)
{
}

SummonerXSerum2AudioProcessor::~SummonerXSerum2AudioProcessor()
{
}

const juce::String SummonerXSerum2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SummonerXSerum2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SummonerXSerum2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SummonerXSerum2AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SummonerXSerum2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SummonerXSerum2AudioProcessor::getNumPrograms()
{
    return 1;
}

int SummonerXSerum2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void SummonerXSerum2AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SummonerXSerum2AudioProcessor::getProgramName(int index)
{
    return {};
}

void SummonerXSerum2AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void SummonerXSerum2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize the internal synthesizer
    synthesiser.clearVoices();
    synthesiser.clearSounds();
    
    // Add 4 voices for polyphony
    for (int i = 0; i < 4; ++i)
        synthesiser.addVoice(new SineWaveVoice());
    
    // Add the sound
    synthesiser.addSound(new SineWaveSound());
    
    // Set sample rate
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
}

void SummonerXSerum2AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SummonerXSerum2AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void SummonerXSerum2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Clear the buffer first
    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    // Render the synthesizer
    synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Apply volume control
    buffer.applyGain(synthVolume);
}

void SummonerXSerum2AudioProcessor::updateEnvelopeParameters()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setEnvelopeParameters(synthAttack, synthDecay, synthSustain, synthRelease);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOscillatorType()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOscillatorType(oscillatorType);
        }
    }
}

void SummonerXSerum2AudioProcessor::updatePulseWidth()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setPulseWidth(pulseWidth);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOctave()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOctave(octave);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateSemitone()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setSemitone(semitone);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateFineTune()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setFineTune(fineTune);
        }
    }
}

bool SummonerXSerum2AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SummonerXSerum2AudioProcessor::createEditor()
{
    return new SummonerXSerum2AudioProcessorEditor(*this);
}

void SummonerXSerum2AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void SummonerXSerum2AudioProcessor::setResponses(const std::vector<std::map<std::string, std::string>>& newResponses)
{
    juce::ScopedLock lock(responseLock);
    responses = newResponses;
    currentResponseIndex = 0;
}

void SummonerXSerum2AudioProcessor::applyResponseAtIndex(int index)
{
    juce::ScopedLock lock(responseLock);
    if (index >= 0 && index < responses.size())
    {
        currentResponseIndex = index;
        // Apply response to internal synthesizer here
    }
}

void SummonerXSerum2AudioProcessor::nextResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex < responses.size() - 1)
    {
        currentResponseIndex++;
        // Apply response to internal synthesizer here
    }
}

void SummonerXSerum2AudioProcessor::previousResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex > 0)
    {
        currentResponseIndex--;
        // Apply response to internal synthesizer here
    }
}

void SummonerXSerum2AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SummonerXSerum2AudioProcessor();
}