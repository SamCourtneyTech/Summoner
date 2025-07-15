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
    
    // Add voices for polyphony (fixed at 8 for good polyphony)
    for (int i = 0; i < 8; ++i)
        synthesiser.addVoice(new SineWaveVoice());
    
    // Add the sound
    synthesiser.addSound(new SineWaveSound());
    
    // Set sample rate
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    
    // Initialize both filter instances
    // Determine initial filter type
    SimpleStableFilter::FilterType initialFilterType = SimpleStableFilter::OFF;
    if (filterLPEnabled)
        initialFilterType = SimpleStableFilter::LOWPASS;
    else if (filterHPEnabled)
        initialFilterType = SimpleStableFilter::HIGHPASS;
    else if (filterBPEnabled)
        initialFilterType = SimpleStableFilter::BANDPASS;
    
    osc1Filter.setSampleRate(sampleRate);
    osc1Filter.setCutoffFrequency(filterCutoff);
    osc1Filter.setResonance(resonanceToQ(filterResonance));
    osc1Filter.setFilterType(initialFilterType);
    
    osc2Filter.setSampleRate(sampleRate);
    osc2Filter.setCutoffFrequency(filterCutoff);
    osc2Filter.setResonance(resonanceToQ(filterResonance));
    osc2Filter.setFilterType(initialFilterType);
    
    // Initialize temporary buffers for oscillator separation
    osc1Buffer.setSize(2, samplesPerBlock);
    osc2Buffer.setSize(2, samplesPerBlock);
    
    // Initialize filter routing for all voices
    updateFilterRouting();
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
    
    // Render the synthesizer (filtering happens inside voices)
    synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    // Count active voices for polyphonic scaling
    int activeVoices = 0;
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (synthesiser.getVoice(i)->isVoiceActive())
            activeVoices++;
    }
    
    // Apply polyphonic scaling to prevent overload with multiple voices
    if (activeVoices > 1)
    {
        float polyScale = 1.0f / std::sqrt((float)activeVoices);
        buffer.applyGain(polyScale);
    }
    
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

void SummonerXSerum2AudioProcessor::updateOsc1Type()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1Type(osc1Type);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1PulseWidth()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1PulseWidth(osc1PulseWidth);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1Octave()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1Octave(osc1Octave);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1Semitone()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1Semitone(osc1Semitone);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1FineTune()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1FineTune(osc1FineTune);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1RandomPhase()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1RandomPhase(osc1RandomPhase);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1VoiceCount()
{
    // Update oscillator 1 unison voice count for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1VoiceCount(osc1VoiceCount);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc1Volume()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc1Volume(osc1Volume);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateDetune()
{
    // Update detune amount for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setDetune(synthDetune);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateStereoWidth()
{
    // Update stereo width for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setStereoWidth(synthStereoWidth);
        }
    }
}

void SummonerXSerum2AudioProcessor::updatePan()
{
    // Update pan for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setPan(synthPan);
        }
    }
}

void SummonerXSerum2AudioProcessor::updatePhase()
{
    // Update phase for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setPhase(synthPhase);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc2Parameters()
{
    // Update oscillator 2 parameters for all existing voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc2Volume(osc2Volume);
            voice->setOsc2Enabled(osc2Enabled);
            voice->setOsc2Type(osc2Type);
            voice->setOsc2VoiceCount(osc2VoiceCount);
            voice->setOsc2Detune(osc2Detune);
            voice->setOsc2Stereo(osc2Stereo);
            voice->setOsc2Pan(osc2Pan);
            voice->setOsc2Octave(osc2Octave);
            voice->setOsc2Semitone(osc2Semitone);
            voice->setOsc2FineTune(osc2FineTune);
            voice->setOsc2RandomPhase(osc2RandomPhase);
            voice->setOsc2Phase(osc2Phase);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateOsc2EnvelopeParameters()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setOsc2EnvelopeParameters(osc2Attack, osc2Decay, osc2Sustain, osc2Release);
        }
    }
}

void SummonerXSerum2AudioProcessor::updateFilterParameters()
{
    // Reset filters before updating parameters
    osc1Filter.reset();
    osc2Filter.reset();
    
    // Determine filter type
    SimpleStableFilter::FilterType filterType = SimpleStableFilter::OFF;
    if (filterLPEnabled)
        filterType = SimpleStableFilter::LOWPASS;
    else if (filterHPEnabled)
        filterType = SimpleStableFilter::HIGHPASS;
    else if (filterBPEnabled)
        filterType = SimpleStableFilter::BANDPASS;
    
    osc1Filter.setCutoffFrequency(filterCutoff);
    osc1Filter.setResonance(resonanceToQ(filterResonance));
    osc1Filter.setFilterType(filterType);
    osc2Filter.setCutoffFrequency(filterCutoff);
    osc2Filter.setResonance(resonanceToQ(filterResonance));
    osc2Filter.setFilterType(filterType);
    updateFilterRouting();
    
    // Update per-voice filters for all active voices
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->updatePerVoiceFilters();
        }
    }
}

void SummonerXSerum2AudioProcessor::updateFilterRouting()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setFilterRouting(osc1FilterEnabled, osc2FilterEnabled, &osc1Filter, &osc2Filter);
            voice->updatePerVoiceFilters();
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