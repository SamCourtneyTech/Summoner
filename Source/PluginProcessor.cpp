#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h> // Add this for juce::dsp

SummonerAudioProcessor::SummonerAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "PARAMETERS", {
    // Oscillator/ADSR parameters
    std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.01f, 5.0f, 0.01f),
    std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.01f, 5.0f, 0.30f),
    std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 0.94f),
    std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.01f, 5.0f, 0.01f),
    std::make_unique<juce::AudioParameterFloat>("waveform", "Waveform", 0.0f, 1.0f, 0.20f),
    std::make_unique<juce::AudioParameterFloat>("waveform2", "Waveform 2", 0.0f, 1.0f, 0.20f),
    std::make_unique<juce::AudioParameterFloat>("detune", "Detune", -100.0f, 100.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("osc1Level", "Osc1 Level", 0.0f, 1.0f, 0.5f), // New parameter
    std::make_unique<juce::AudioParameterFloat>("osc2Level", "Osc2 Level", 0.0f, 1.0f, 0.5f), // New parameter
    // Filter parameters
    std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 20.0f, 20000.0f, 7077.26f),
    std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 10.0f, 1.00f),
    std::make_unique<juce::AudioParameterFloat>("filterADSRMix", "Filter ADSR Mix", 0.0f, 1.0f, 1.00f),
    std::make_unique<juce::AudioParameterFloat>("filterADSRDepth", "Filter ADSR Depth", 0.0f, 10000.0f, 10000.0f),
    std::make_unique<juce::AudioParameterChoice>("filterType", "Filter Type",
        juce::StringArray("Low Pass", "High Pass", "Band Pass", "Notch"), 0)
        })
{
}

SummonerAudioProcessor::~SummonerAudioProcessor()
{
}

const juce::String SummonerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SummonerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SummonerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SummonerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SummonerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SummonerAudioProcessor::getNumPrograms()
{
    return 1;
}

int SummonerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SummonerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SummonerAudioProcessor::getProgramName(int index)
{
    return {};
}

void SummonerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void SummonerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    oscillator1.prepare(sampleRate);
    oscillator2.prepare(sampleRate); // Prepare the second oscillator
    oscillator1.setFrequency(currentFrequency, sampleRate);
    oscillator2.setFrequency(currentFrequency, sampleRate); // Same frequency for now
    oscillator1.setADSR(
        *parameters.getRawParameterValue("attack"),
        *parameters.getRawParameterValue("decay"),
        *parameters.getRawParameterValue("sustain"),
        *parameters.getRawParameterValue("release")
    );
    oscillator2.setADSR(
        *parameters.getRawParameterValue("attack"),
        *parameters.getRawParameterValue("decay"),
        *parameters.getRawParameterValue("sustain"),
        *parameters.getRawParameterValue("release")
    );

    // Prepare the filter
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    filter.prepare(spec);
    updateFilter(); // Initial filter update

    DBG("Synth prepared with sample rate: " << sampleRate);
}

void SummonerAudioProcessor::updateFilter()
{
    float cutoff = *parameters.getRawParameterValue("filterCutoff");
    float resonance = *parameters.getRawParameterValue("filterResonance");
    int filterTypeIdx = parameters.getParameter("filterType")->convertFrom0to1(
        parameters.getParameter("filterType")->getValue()
    );

    // Check if parameters have changed
    if (cutoff == lastFilterCutoff && resonance == lastFilterResonance && filterTypeIdx == lastFilterType)
        return;

    // Update last known values
    lastFilterCutoff = cutoff;
    lastFilterResonance = resonance;
    lastFilterType = filterTypeIdx;

    juce::dsp::IIR::Coefficients<float>::Ptr newCoefficients;
    switch (filterTypeIdx)
    {
    case 0: // Low Pass
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoff, resonance);
        break;
    case 1: // High Pass
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, cutoff, resonance);
        break;
    case 2: // Band Pass
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, cutoff, resonance);
        break;
    case 3: // Notch
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(currentSampleRate, cutoff, resonance);
        break;
    default:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoff, resonance);
    }

    *filter.state = *newCoefficients;
}

void SummonerAudioProcessor::releaseResources()
{
    // Release synthesizer resources here
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SummonerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    DBG("Checking bus layouts...");
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    return true;
}
#endif

void SummonerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();
    buffer.clear();

    oscillator1.setADSR(
        *parameters.getRawParameterValue("attack"),
        *parameters.getRawParameterValue("decay"),
        *parameters.getRawParameterValue("sustain"),
        *parameters.getRawParameterValue("release")
    );
    oscillator2.setADSR(
        *parameters.getRawParameterValue("attack"),
        *parameters.getRawParameterValue("decay"),
        *parameters.getRawParameterValue("sustain"),
        *parameters.getRawParameterValue("release")
    );

    float waveformValue1 = *parameters.getRawParameterValue("waveform");
    if (waveformValue1 <= 0.16f)
        oscillator1.setWaveform(Oscillator::Waveform::Sine);
    else if (waveformValue1 <= 0.32f)
        oscillator1.setWaveform(Oscillator::Waveform::Saw);
    else if (waveformValue1 <= 0.48f)
        oscillator1.setWaveform(Oscillator::Waveform::Square);
    else if (waveformValue1 <= 0.64f)
        oscillator1.setWaveform(Oscillator::Waveform::Triangle);
    else if (waveformValue1 <= 0.80f)
        oscillator1.setWaveform(Oscillator::Waveform::Pulse25);
    else if (waveformValue1 <= 0.90f)
        oscillator1.setWaveform(Oscillator::Waveform::WhiteNoise);
    else
        oscillator1.setWaveform(Oscillator::Waveform::PinkNoise);

    float waveformValue2 = *parameters.getRawParameterValue("waveform2");
    if (waveformValue2 <= 0.16f)
        oscillator2.setWaveform(Oscillator::Waveform::Sine);
    else if (waveformValue2 <= 0.32f)
        oscillator2.setWaveform(Oscillator::Waveform::Saw);
    else if (waveformValue2 <= 0.48f)
        oscillator2.setWaveform(Oscillator::Waveform::Square);
    else if (waveformValue2 <= 0.64f)
        oscillator2.setWaveform(Oscillator::Waveform::Triangle);
    else if (waveformValue2 <= 0.80f)
        oscillator2.setWaveform(Oscillator::Waveform::Pulse25);
    else if (waveformValue2 <= 0.90f)
        oscillator2.setWaveform(Oscillator::Waveform::WhiteNoise);
    else
        oscillator2.setWaveform(Oscillator::Waveform::PinkNoise);

    float detuneCents = *parameters.getRawParameterValue("detune");
    float osc1Level = *parameters.getRawParameterValue("osc1Level");
    float osc2Level = *parameters.getRawParameterValue("osc2Level");

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            currentFrequency = juce::MidiMessage::getMidiNoteInHertz(msg.getNoteNumber());
            oscillator1.setFrequency(currentFrequency, getSampleRate());
            float detuneFactor = std::pow(2.0f, detuneCents / 1200.0f);
            float detunedFrequency = currentFrequency * detuneFactor;
            oscillator2.setFrequency(detunedFrequency, getSampleRate());
            oscillator1.noteOn();
            oscillator2.noteOn();
        }
        else if (msg.isNoteOff())
        {
            oscillator1.noteOff();
            oscillator2.noteOff();
        }
    }

    float baseCutoff = *parameters.getRawParameterValue("filterCutoff");
    float filterADSRMix = *parameters.getRawParameterValue("filterADSRMix");
    float filterADSRDepth = *parameters.getRawParameterValue("filterADSRDepth");

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float osc1Output = oscillator1.getNextSample() * osc1Level;
        float osc2Output = oscillator2.getNextSample() * osc2Level;
        float mixedOutput = (osc1Output + osc2Output) * 0.5f; // Sum and scale to avoid clipping

        float modulatedCutoff = baseCutoff;
        if (filterADSRMix > 0.0f)
        {
            float envAmount = oscillator1.getEnvelopeValue();
            float modulationAmount = envAmount * filterADSRDepth;
            modulatedCutoff = baseCutoff + (modulationAmount * filterADSRMix);
            modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
        }

        lastFilterCutoff = -1.0f;
        *parameters.getRawParameterValue("filterCutoff") = modulatedCutoff;
        updateFilter();
        *parameters.getRawParameterValue("filterCutoff") = baseCutoff;

        leftChannel[sample] = mixedOutput;
        rightChannel[sample] = mixedOutput;
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
}

bool SummonerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SummonerAudioProcessor::createEditor()
{
    return new SummonerAudioProcessorEditor(*this);
}

void SummonerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void SummonerAudioProcessor::setResponses(const std::vector<std::map<std::string, std::string>>& newResponses)
{
    juce::ScopedLock lock(responseLock);
    responses = newResponses;
    currentResponseIndex = 0;
    if (!responses.empty())
    {
        // Apply the first response to synthesizer here
        DBG("Applied response 0");
    }
}

void SummonerAudioProcessor::applyResponseAtIndex(int index)
{
    juce::ScopedLock lock(responseLock);
    if (index >= 0 && index < responses.size())
    {
        currentResponseIndex = index;
        // Apply the response at index to synthesizer here
        DBG("Applied response " << index);
    }
}

void SummonerAudioProcessor::nextResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex < responses.size() - 1)
    {
        currentResponseIndex++;
        // Apply the next response to  synthesizer here
        DBG("Applied next response " << currentResponseIndex);
    }
}

void SummonerAudioProcessor::previousResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex > 0)
    {
        currentResponseIndex--;
        // Apply the previous response to  synthesizer here
        DBG("Applied previous response " << currentResponseIndex);
    }
}

void SummonerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SummonerAudioProcessor();
}
