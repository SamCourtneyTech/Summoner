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
    std::make_unique<juce::AudioParameterFloat>("osc1Level", "Osc1 Level", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("osc2Level", "Osc2 Level", 0.0f, 1.0f, 0.5f),
    // LFO parameters
    std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate", 0.1f, 20.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoDepth", "LFO Depth", 0.0f, 5000.0f, 0.0f),
    std::make_unique<juce::AudioParameterChoice>("lfoWaveform", "LFO Waveform",
        juce::StringArray("Sine", "Triangle", "Saw", "Square"), 0),
        // Distortion parameters
        std::make_unique<juce::AudioParameterFloat>("distortionDrive", "Distortion Drive", 1.0f, 10.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("distortionTone", "Distortion Tone", 500.0f, 20000.0f, 5000.0f),
        std::make_unique<juce::AudioParameterFloat>("distortionMix", "Distortion Mix", 0.0f, 1.0f, 0.5f),
        // Delay parameters
        std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 0.0f, 1000.0f, 300.0f),
        std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Delay Feedback", 0.0f, 0.9f, 0.3f),
        std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", 0.0f, 1.0f, 0.5f),
        // Chorus parameters
        std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", 0.1f, 10.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("chorusDelay", "Chorus Delay", 1.0f, 20.0f, 7.0f),
        // Phaser parameters
        std::make_unique<juce::AudioParameterFloat>("phaserRate", "Phaser Rate", 0.1f, 10.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("phaserDepth", "Phaser Depth", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("phaserMix", "Phaser Mix", 0.0f, 1.0f, 0.5f),
        // Flanger parameters
        std::make_unique<juce::AudioParameterFloat>("flangerRate", "Flanger Rate", 0.1f, 10.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flangerDepth", "Flanger Depth", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flangerMix", "Flanger Mix", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("flangerDelay", "Flanger Delay", 0.1f, 5.0f, 1.0f),
        // Reverb parameters
        std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "Reverb Room Size", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Reverb Damping", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("reverbWetLevel", "Reverb Wet Level", 0.0f, 1.0f, 0.33f),
        std::make_unique<juce::AudioParameterFloat>("reverbDryLevel", "Reverb Dry Level", 0.0f, 1.0f, 0.4f),
        // Compressor parameters
        std::make_unique<juce::AudioParameterFloat>("compressorThreshold", "Compressor Threshold", -60.0f, 0.0f, -24.0f),
        std::make_unique<juce::AudioParameterFloat>("compressorRatio", "Compressor Ratio", 1.0f, 10.0f, 2.0f),
        std::make_unique<juce::AudioParameterFloat>("compressorAttack", "Compressor Attack", 0.1f, 100.0f, 10.0f),
        std::make_unique<juce::AudioParameterFloat>("compressorRelease", "Compressor Release", 10.0f, 1000.0f, 100.0f),
        std::make_unique<juce::AudioParameterFloat>("compressorMakeupGain", "Compressor Makeup Gain", 0.0f, 24.0f, 0.0f),
        // Filter parameters
        std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 20.0f, 20000.0f, 7077.26f),
        std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 10.0f, 1.00f),
        std::make_unique<juce::AudioParameterFloat>("filterADSRMix", "Filter ADSR Mix", 0.0f, 1.0f, 1.00f),
        std::make_unique<juce::AudioParameterFloat>("filterADSRDepth", "Filter ADSR Depth", 0.0f, 10000.0f, 10000.0f),
        std::make_unique<juce::AudioParameterChoice>("filterType", "Filter Type",
            juce::StringArray("Low Pass", "High Pass", "Band Pass", "Notch"), 0),
            // Number of voices
            std::make_unique<juce::AudioParameterInt>("numVoices", "Num Voices", 1, 16, 8),
            // Unison parameters
            std::make_unique<juce::AudioParameterInt>("unisonVoices", "Unison Voices", 1, 8, 1), // 1 = off, 2-8 = unison voices
            std::make_unique<juce::AudioParameterFloat>("unisonDetune", "Unison Detune", 0.0f, 50.0f, 0.0f) // Detune in cents
        })
{
    parameters.addParameterListener("numVoices", this);
}

void SummonerAudioProcessor::updateNumVoices() {
    int newNumVoices = static_cast<int>(*parameters.getRawParameterValue("numVoices"));

    // Clear existing voices
    for (auto* voice : voices) {
        voice->noteOff();
    }
    voices.clear();

    // Add new voices based on parameter
    for (int i = 0; i < newNumVoices; ++i) {
        voices.add(new Voice());
        // Only prepare if sample rate is valid
        if (currentSampleRate > 0.0) {
            voices[i]->prepare(currentSampleRate);
            voices[i]->setParameterPointers(
                parameters.getRawParameterValue("detune"),
                parameters.getRawParameterValue("osc1Level"),
                parameters.getRawParameterValue("osc2Level")
            );
        }
    }
}

Voice* SummonerAudioProcessor::findVoiceToSteal() {
    Voice* bestVoice = nullptr;
    float lowestAmplitude = std::numeric_limits<float>::max();

    // First, look for an inactive voice
    for (auto* voice : voices) {
        if (!voice->getIsActive()) {
            bestVoice = voice;
            break;
        }
    }

    // If no inactive voice, steal the voice with the lowest amplitude
    if (!bestVoice) {
        for (auto* voice : voices) {
            float envValue = voice->getEnvelopeValue();
            if (envValue < lowestAmplitude) {
                bestVoice = voice;
                lowestAmplitude = envValue;
            }
        }
    }

    // If we found a voice to steal and it's active, apply fade-out
    if (bestVoice && bestVoice->getIsActive()) {
        bestVoice->startFadeOut();
    }

    return bestVoice;
}

void SummonerAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    if (parameterID == "numVoices" && currentSampleRate > 0.0) {
        updateNumVoices();
    }
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

void SummonerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    lfo.prepare(sampleRate);
    currentSampleRate = sampleRate;

    updateNumVoices(); // Initialize voices based on parameter

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    filter.prepare(spec);
    updateFilter();

    delayBufferSize = static_cast<int>(2.0 * sampleRate);
    delayBuffer.setSize(2, delayBufferSize);
    delayBuffer.clear();
    delayWritePosition = 0;

    flangerBufferSize = static_cast<int>(0.0075 * sampleRate) + 1;
    flangerBuffer.setSize(2, flangerBufferSize);
    flangerBuffer.clear();
    flangerWritePosition = 0;
    flangerLFO.prepare(sampleRate);
    flangerLFO.setWaveform(LFO::Waveform::Sine);

    reverb.reset();
    reverbParams.roomSize = *parameters.getRawParameterValue("reverbRoomSize");
    reverbParams.damping = *parameters.getRawParameterValue("reverbDamping");
    reverbParams.wetLevel = *parameters.getRawParameterValue("reverbWetLevel");
    reverbParams.dryLevel = *parameters.getRawParameterValue("reverbDryLevel");
    reverb.setParameters(reverbParams);

    distortionToneFilter.prepare(spec);
    lastDistortionTone = -1.0f;

    compressor.prepare(spec);
    compressor.setThreshold(*parameters.getRawParameterValue("compressorThreshold"));
    compressor.setRatio(*parameters.getRawParameterValue("compressorRatio"));
    compressor.setAttack(*parameters.getRawParameterValue("compressorAttack"));
    compressor.setRelease(*parameters.getRawParameterValue("compressorRelease"));

    compressorMakeupGain.prepare(spec);
    compressorMakeupGain.setGainDecibels(*parameters.getRawParameterValue("compressorMakeupGain"));

    chorus.prepare(spec);
    chorus.setRate(*parameters.getRawParameterValue("chorusRate"));
    chorus.setDepth(*parameters.getRawParameterValue("chorusDepth"));
    chorus.setMix(*parameters.getRawParameterValue("chorusMix"));
    chorus.setCentreDelay(*parameters.getRawParameterValue("chorusDelay"));

    phaser.prepare(spec);
    phaser.setRate(*parameters.getRawParameterValue("phaserRate"));
    phaser.setDepth(*parameters.getRawParameterValue("phaserDepth"));
    phaser.setMix(*parameters.getRawParameterValue("phaserMix"));

    DBG("Synth prepared with sample rate: " << sampleRate);
}

void SummonerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();
    buffer.clear();

    float attack = *parameters.getRawParameterValue("attack");
    float decay = *parameters.getRawParameterValue("decay");
    float sustain = *parameters.getRawParameterValue("sustain");
    float release = *parameters.getRawParameterValue("release");
    for (auto* voice : voices) {
        voice->setADSR(attack, decay, sustain, release);
    }

    float waveformValue1 = *parameters.getRawParameterValue("waveform");
    Oscillator::Waveform wf1;
    if (waveformValue1 <= 0.16f) wf1 = Oscillator::Waveform::Sine;
    else if (waveformValue1 <= 0.32f) wf1 = Oscillator::Waveform::Saw;
    else if (waveformValue1 <= 0.48f) wf1 = Oscillator::Waveform::Square;
    else if (waveformValue1 <= 0.64f) wf1 = Oscillator::Waveform::Triangle;
    else if (waveformValue1 <= 0.80f) wf1 = Oscillator::Waveform::Pulse25;
    else if (waveformValue1 <= 0.90f) wf1 = Oscillator::Waveform::WhiteNoise;
    else wf1 = Oscillator::Waveform::PinkNoise;

    float waveformValue2 = *parameters.getRawParameterValue("waveform2");
    Oscillator::Waveform wf2;
    if (waveformValue2 <= 0.16f) wf2 = Oscillator::Waveform::Sine;
    else if (waveformValue2 <= 0.32f) wf2 = Oscillator::Waveform::Saw;
    else if (waveformValue2 <= 0.48f) wf2 = Oscillator::Waveform::Square;
    else if (waveformValue2 <= 0.64f) wf2 = Oscillator::Waveform::Triangle;
    else if (waveformValue2 <= 0.80f) wf2 = Oscillator::Waveform::Pulse25;
    else if (waveformValue2 <= 0.90f) wf2 = Oscillator::Waveform::WhiteNoise;
    else wf2 = Oscillator::Waveform::PinkNoise;

    for (auto* voice : voices) {
        voice->setWaveform(wf1, wf2);
    }

    float lfoRate = *parameters.getRawParameterValue("lfoRate");
    float lfoDepth = *parameters.getRawParameterValue("lfoDepth");
    int lfoWaveformIdx = parameters.getParameter("lfoWaveform")->convertFrom0to1(
        parameters.getParameter("lfoWaveform")->getValue()
    );
    lfo.setFrequency(lfoRate);
    switch (lfoWaveformIdx) {
    case 0: lfo.setWaveform(LFO::Waveform::Sine); break;
    case 1: lfo.setWaveform(LFO::Waveform::Triangle); break;
    case 2: lfo.setWaveform(LFO::Waveform::Saw); break;
    case 3: lfo.setWaveform(LFO::Waveform::Square); break;
    }

    float delayTimeMs = *parameters.getRawParameterValue("delayTime");
    float delayFeedback = *parameters.getRawParameterValue("delayFeedback");
    float delayMix = *parameters.getRawParameterValue("delayMix");
    int delaySamples = static_cast<int>(delayTimeMs * currentSampleRate / 1000.0f);

    reverbParams.roomSize = *parameters.getRawParameterValue("reverbRoomSize");
    reverbParams.damping = *parameters.getRawParameterValue("reverbDamping");
    reverbParams.wetLevel = *parameters.getRawParameterValue("reverbWetLevel");
    reverbParams.dryLevel = *parameters.getRawParameterValue("reverbDryLevel");
    reverb.setParameters(reverbParams);

    float distortionDrive = *parameters.getRawParameterValue("distortionDrive");
    float distortionTone = *parameters.getRawParameterValue("distortionTone");
    float distortionMix = *parameters.getRawParameterValue("distortionMix");

    if (distortionTone != lastDistortionTone) {
        *distortionToneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, distortionTone, 0.707f);
        lastDistortionTone = distortionTone;
    }

    compressor.setThreshold(*parameters.getRawParameterValue("compressorThreshold"));
    compressor.setRatio(*parameters.getRawParameterValue("compressorRatio"));
    compressor.setAttack(*parameters.getRawParameterValue("compressorAttack"));
    compressor.setRelease(*parameters.getRawParameterValue("compressorRelease"));
    compressorMakeupGain.setGainDecibels(*parameters.getRawParameterValue("compressorMakeupGain"));

    chorus.setRate(*parameters.getRawParameterValue("chorusRate"));
    chorus.setDepth(*parameters.getRawParameterValue("chorusDepth"));
    chorus.setMix(*parameters.getRawParameterValue("chorusMix"));
    chorus.setCentreDelay(*parameters.getRawParameterValue("chorusDelay"));

    phaser.setRate(*parameters.getRawParameterValue("phaserRate"));
    phaser.setDepth(*parameters.getRawParameterValue("phaserDepth"));
    phaser.setMix(*parameters.getRawParameterValue("phaserMix"));

    float flangerRate = *parameters.getRawParameterValue("flangerRate");
    float flangerDepth = *parameters.getRawParameterValue("flangerDepth");
    float flangerMix = *parameters.getRawParameterValue("flangerMix");
    float flangerDelayMs = *parameters.getRawParameterValue("flangerDelay");
    flangerLFO.setFrequency(flangerRate);

    // Get unison parameters
    int unisonVoices = static_cast<int>(*parameters.getRawParameterValue("unisonVoices"));
    float unisonDetune = *parameters.getRawParameterValue("unisonDetune");

    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            int noteNumber = msg.getNoteNumber();
            float freq = juce::MidiMessage::getMidiNoteInHertz(noteNumber);

            // Clear any existing voices for this note (in case of retrigger)
            if (noteToVoices.find(noteNumber) != noteToVoices.end()) {
                for (auto* voice : noteToVoices[noteNumber]) {
                    voice->noteOff();
                    voice->setNoteNumber(-1);
                }
                noteToVoices.erase(noteNumber);
            }

            // Allocate unison voices
            std::vector<Voice*> assignedVoices;
            for (int i = 0; i < unisonVoices; ++i) {
                Voice* freeVoice = findVoiceToSteal();
                if (freeVoice) {
                    // Calculate detune offset for this unison voice
                    float detuneOffset = 0.0f;
                    if (unisonVoices > 1) {
                        // Spread detune symmetrically: e.g., for 3 voices: -detune, 0, +detune
                        float step = unisonDetune / (unisonVoices - 1);
                        detuneOffset = (i * step) - (unisonDetune / 2.0f);
                    }
                    freeVoice->setUnisonDetuneOffset(detuneOffset);
                    freeVoice->setNoteNumber(noteNumber);
                    freeVoice->noteOn(freq, getSampleRate());
                    assignedVoices.push_back(freeVoice);
                }
            }
            if (!assignedVoices.empty()) {
                noteToVoices[noteNumber] = assignedVoices;
            }
        }
        else if (msg.isNoteOff()) {
            int noteNumber = msg.getNoteNumber();
            if (noteToVoices.find(noteNumber) != noteToVoices.end()) {
                for (auto* voice : noteToVoices[noteNumber]) {
                    voice->noteOff();
                    voice->setNoteNumber(-1);
                }
                noteToVoices.erase(noteNumber);
            }
        }
    }

    float baseCutoff = *parameters.getRawParameterValue("filterCutoff");
    float filterADSRMix = *parameters.getRawParameterValue("filterADSRMix");
    float filterADSRDepth = *parameters.getRawParameterValue("filterADSRDepth");

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // Adjust amplitude based on number of unison voices to prevent clipping
    float amplitudeScale = 1.0f / std::sqrt(static_cast<float>(unisonVoices));

    for (int sample = 0; sample < numSamples; ++sample) {
        float mixedOutput = 0.0f;
        float maxEnvValue = 0.0f;

        for (auto* voice : voices) {
            if (voice->getIsActive()) {
                mixedOutput += voice->getNextSample() * amplitudeScale;
                float envValue = voice->getEnvelopeValue();
                maxEnvValue = std::max(maxEnvValue, envValue);
            }
        }

        float modulatedCutoff = baseCutoff;
        if (filterADSRMix > 0.0f) {
            float modulationAmount = maxEnvValue * filterADSRDepth;
            modulatedCutoff = baseCutoff + (modulationAmount * filterADSRMix);
        }
        float lfoAmount = lfo.getNextSample() * lfoDepth;
        modulatedCutoff += lfoAmount;
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);

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

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    for (int sample = 0; sample < numSamples; ++sample) {
        leftChannel[sample] = std::tanh(leftChannel[sample] * distortionDrive);
        rightChannel[sample] = std::tanh(rightChannel[sample] * distortionDrive);
    }

    distortionToneFilter.process(context);

    auto* dryLeftChannel = dryBuffer.getWritePointer(0);
    auto* dryRightChannel = dryBuffer.getWritePointer(1);
    for (int sample = 0; sample < numSamples; ++sample) {
        leftChannel[sample] = dryLeftChannel[sample] * (1.0f - distortionMix) + leftChannel[sample] * distortionMix;
        rightChannel[sample] = dryRightChannel[sample] * (1.0f - distortionMix) + rightChannel[sample] * distortionMix;
    }

    for (int sample = 0; sample < numSamples; ++sample) {
        int delayReadPosition = (delayWritePosition - delaySamples + delayBufferSize) % delayBufferSize;
        float delayedLeft = delayBuffer.getSample(0, delayReadPosition);
        float delayedRight = delayBuffer.getSample(1, delayReadPosition);

        float inputLeft = leftChannel[sample];
        float inputRight = rightChannel[sample];

        delayBuffer.setSample(0, delayWritePosition, inputLeft + delayedLeft * delayFeedback);
        delayBuffer.setSample(1, delayWritePosition, inputRight + delayedRight * delayFeedback);

        leftChannel[sample] = inputLeft * (1.0f - delayMix) + delayedLeft * delayMix;
        rightChannel[sample] = inputRight * (1.0f - delayMix) + delayedRight * delayMix;

        delayWritePosition = (delayWritePosition + 1) % delayBufferSize;
    }

    chorus.process(context);
    phaser.process(context);

    for (int sample = 0; sample < numSamples; ++sample) {
        float lfoValue = flangerLFO.getNextSample();
        float delayModulation = (lfoValue * flangerDepth * flangerDelayMs * 0.5f);
        float totalDelayMs = flangerDelayMs + delayModulation;
        totalDelayMs = juce::jlimit(0.1f, 7.5f, totalDelayMs);
        float delaySamplesFloat = totalDelayMs * currentSampleRate / 1000.0f;

        float readPosition = flangerWritePosition - delaySamplesFloat;
        while (readPosition < 0) readPosition += flangerBufferSize;
        while (readPosition >= flangerBufferSize) readPosition -= flangerBufferSize;

        int readIndex = static_cast<int>(readPosition);
        float fraction = readPosition - readIndex;
        int nextReadIndex = (readIndex + 1) % flangerBufferSize;

        readIndex = juce::jlimit(0, flangerBufferSize - 1, readIndex);
        nextReadIndex = juce::jlimit(0, flangerBufferSize - 1, nextReadIndex);

        float delayedLeft = flangerBuffer.getSample(0, readIndex) + fraction * (flangerBuffer.getSample(0, nextReadIndex) - flangerBuffer.getSample(0, readIndex));
        float delayedRight = flangerBuffer.getSample(1, readIndex) + fraction * (flangerBuffer.getSample(1, nextReadIndex) - flangerBuffer.getSample(1, readIndex));

        flangerBuffer.setSample(0, flangerWritePosition, leftChannel[sample]);
        flangerBuffer.setSample(1, flangerWritePosition, rightChannel[sample]);

        leftChannel[sample] = leftChannel[sample] * (1.0f - flangerMix) + delayedLeft * flangerMix;
        rightChannel[sample] = rightChannel[sample] * (1.0f - flangerMix) + delayedRight * flangerMix;

        flangerWritePosition = (flangerWritePosition + 1) % flangerBufferSize;
    }

    reverb.processStereo(leftChannel, rightChannel, numSamples);
    compressor.process(context);
    compressorMakeupGain.process(context);
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
