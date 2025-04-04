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
    std::make_unique<juce::AudioParameterFloat>("attack", "Attack", 0.01f, 5.0f, 0.01f),
    std::make_unique<juce::AudioParameterFloat>("decay", "Decay", 0.01f, 5.0f, 0.30f),
    std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", 0.0f, 1.0f, 0.94f),
    std::make_unique<juce::AudioParameterFloat>("release", "Release", 0.01f, 5.0f, 0.01f),
    std::make_unique<juce::AudioParameterFloat>("waveform", "Waveform", 0.0f, 1.0f, 0.20f),
    std::make_unique<juce::AudioParameterFloat>("waveform2", "Waveform 2", 0.0f, 1.0f, 0.20f),
    std::make_unique<juce::AudioParameterFloat>("waveform3", "Waveform 3", 0.0f, 1.0f, 0.20f),
    std::make_unique<juce::AudioParameterFloat>("noiseWaveform", "Noise Waveform", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("subWaveform", "Sub Waveform", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("detune", "Detune", -100.0f, 100.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("osc1Level", "Osc1 Level", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("osc2Level", "Osc2 Level", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("osc3Level", "Osc3 Level", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("noiseLevel", "Noise Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("subLevel", "Sub Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("osc1Octave", "Osc1 Octave", -4.0f, 4.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("osc2Octave", "Osc2 Octave", -4.0f, 4.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("osc3Octave", "Osc3 Octave", -4.0f, 4.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("subOctave", "Sub Octave", -4.0f, 4.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Rate", 0.1f, 20.0f, 1.0f),
    std::make_unique<juce::AudioParameterChoice>("lfoWaveform", "LFO Waveform",
        juce::StringArray("Sine", "Triangle", "Saw", "Square"), 0),
    std::make_unique<juce::AudioParameterFloat>("distortionDrive", "Distortion Drive", 1.0f, 10.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("distortionTone", "Distortion Tone", 500.0f, 20000.0f, 5000.0f),
    std::make_unique<juce::AudioParameterFloat>("distortionMix", "Distortion Mix", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 0.0f, 1000.0f, 300.0f),
    std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Delay Feedback", 0.0f, 0.9f, 0.3f),
    std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", 0.1f, 10.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("chorusDelay", "Chorus Delay", 1.0f, 20.0f, 7.0f),
    std::make_unique<juce::AudioParameterFloat>("phaserRate", "Phaser Rate", 0.1f, 10.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("phaserDepth", "Phaser Depth", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("phaserMix", "Phaser Mix", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("flangerRate", "Flanger Rate", 0.1f, 10.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("flangerDepth", "Flanger Depth", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("flangerMix", "Flanger Mix", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("flangerDelay", "Flanger Delay", 0.1f, 5.0f, 1.0f),
    std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "Reverb Room Size", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Reverb Damping", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("reverbWetLevel", "Reverb Wet Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("reverbDryLevel", "Reverb Dry Level", 0.0f, 1.0f, 0.4f),
    std::make_unique<juce::AudioParameterFloat>("compressorThreshold", "Compressor Threshold", -60.0f, 0.0f, -24.0f),
    std::make_unique<juce::AudioParameterFloat>("compressorRatio", "Compressor Ratio", 1.0f, 10.0f, 2.0f),
    std::make_unique<juce::AudioParameterFloat>("compressorAttack", "Compressor Attack", 0.1f, 100.0f, 10.0f),
    std::make_unique<juce::AudioParameterFloat>("compressorRelease", "Compressor Release", 10.0f, 1000.0f, 100.0f),
    std::make_unique<juce::AudioParameterFloat>("compressorMakeupGain", "Compressor Makeup Gain", 0.0f, 24.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 20.0f, 20000.0f, 7077.26f),
    std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 10.0f, 1.00f),
    std::make_unique<juce::AudioParameterChoice>("filterType", "Filter Type",
        juce::StringArray("Low Pass", "High Pass", "Band Pass", "Notch"), 0),
    std::make_unique<juce::AudioParameterInt>("numVoices", "Num Voices", 1, 16, 8),
    std::make_unique<juce::AudioParameterInt>("osc1UnisonVoices", "Osc1 Unison Voices", 1, 8, 1),
    std::make_unique<juce::AudioParameterFloat>("osc1UnisonDetune", "Osc1 Unison Detune", 0.0f, 50.0f, 0.0f),
    std::make_unique<juce::AudioParameterInt>("osc2UnisonVoices", "Osc2 Unison Voices", 1, 8, 1),
    std::make_unique<juce::AudioParameterFloat>("osc2UnisonDetune", "Osc2 Unison Detune", 0.0f, 50.0f, 0.0f),
    std::make_unique<juce::AudioParameterInt>("osc3UnisonVoices", "Osc3 Unison Voices", 1, 8, 1),
    std::make_unique<juce::AudioParameterFloat>("osc3UnisonDetune", "Osc3 Unison Detune", 0.0f, 50.0f, 0.0f),
    // Modulation Matrix Parameters
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc1Freq", "LFO to Osc1 Freq", 0.0f, 100.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc2Freq", "LFO to Osc2 Freq", 0.0f, 100.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc3Freq", "LFO to Osc3 Freq", 0.0f, 100.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc1Level", "LFO to Osc1 Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc2Level", "LFO to Osc2 Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToOsc3Level", "LFO to Osc3 Level", 0.0f, 1.0f, 0.0f),
    std::make_unique<juce::AudioParameterFloat>("lfoToFilterCutoff", "LFO to Filter Cutoff", 0.0f, 12.0f, 0.0f)
        })
{
    parameters.addParameterListener("numVoices", this);
}



void SummonerAudioProcessor::updateNumVoices() {
    int newNumVoices = static_cast<int>(*parameters.getRawParameterValue("numVoices"));

    for (auto* voice : voices) {
        voice->noteOff();
    }
    voices.clear();

    for (int i = 0; i < newNumVoices; ++i) {
        voices.add(new Voice());
        if (currentSampleRate > 0.0) {
            voices[i]->prepare(currentSampleRate);
            voices[i]->setParameters(parameters);
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

void SummonerAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "filterCutoff" || parameterID == "filterResonance" || parameterID == "filterType")
    {
        updateFilter();
    }
    else if (parameterID == "numVoices")
    {
        updateNumVoices();
    }
    else if (parameterID == "reverbRoomSize")
    {
        reverbParams.roomSize = newValue;
        reverb.setParameters(reverbParams);
    }
    else if (parameterID == "reverbDamping")
    {
        reverbParams.damping = newValue;
        reverb.setParameters(reverbParams);
    }
    else if (parameterID == "reverbWetLevel")
    {
        reverbParams.wetLevel = newValue;
        reverb.setParameters(reverbParams);
    }
    else if (parameterID == "reverbDryLevel")
    {
        reverbParams.dryLevel = newValue;
        reverb.setParameters(reverbParams);
    }
    else if (parameterID == "compressorThreshold")
    {
        compressor.setThreshold(newValue);
    }
    else if (parameterID == "compressorRatio")
    {
        compressor.setRatio(newValue);
    }
    else if (parameterID == "compressorAttack")
    {
        compressor.setAttack(newValue);
    }
    else if (parameterID == "compressorRelease")
    {
        compressor.setRelease(newValue);
    }
    else if (parameterID == "compressorMakeupGain")
    {
        compressorMakeupGain.setGainDecibels(newValue);
    }
    else if (parameterID == "chorusRate")
    {
        chorus.setRate(newValue);
    }
    else if (parameterID == "chorusDepth")
    {
        chorus.setDepth(newValue);
    }
    else if (parameterID == "chorusMix")
    {
        chorus.setMix(newValue);
    }
    else if (parameterID == "chorusDelay")
    {
        chorus.setCentreDelay(newValue * 1000.0f); // Convert to milliseconds
    }
    else if (parameterID == "phaserRate")
    {
        phaser.setRate(newValue);
    }
    else if (parameterID == "phaserDepth")
    {
        phaser.setDepth(newValue);
    }
    else if (parameterID == "phaserMix")
    {
        phaser.setMix(newValue);
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
    // Filter cutoff modulation is now handled in processBlock
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
    currentSampleRate = sampleRate;
    voices.clear();
    updateNumVoices();
    for (auto* voice : voices) {
        voice->prepare(sampleRate);
        voice->setParameters(parameters);
    }

    filter.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    distortionToneFilter.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });

    delayBufferSize = static_cast<int>(sampleRate * 2.0);
    delayBuffer.setSize(2, delayBufferSize);
    delayBuffer.clear();
    delayWritePosition = 0;

    flangerBufferSize = static_cast<int>(sampleRate * 0.1);
    flangerBuffer.setSize(2, flangerBufferSize);
    flangerBuffer.clear();
    flangerWritePosition = 0;
    flangerLFO.prepare(sampleRate);

    reverb.reset();
    compressor.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    compressorMakeupGain.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    chorus.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    phaser.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    limiter.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 }); // Initialize limiter
    limiter.setThreshold(-0.1f); // Set threshold to prevent clipping
    limiter.setRelease(50.0f); // Moderate release time

    lfo.prepare(sampleRate);
}

void SummonerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();
    buffer.clear();

    float attack = parameters.getRawParameterValue("attack")->load();
    float decay = parameters.getRawParameterValue("decay")->load();
    float sustain = parameters.getRawParameterValue("sustain")->load();
    float release = parameters.getRawParameterValue("release")->load();
    for (auto* voice : voices) {
        voice->setADSR(attack, decay, sustain, release);
    }

    float waveformValue1 = parameters.getRawParameterValue("waveform")->load();
    Oscillator::Waveform wf1;
    if (waveformValue1 <= 0.16f) wf1 = Oscillator::Waveform::Sine;
    else if (waveformValue1 <= 0.33f) wf1 = Oscillator::Waveform::Saw;
    else if (waveformValue1 <= 0.50f) wf1 = Oscillator::Waveform::Square;
    else if (waveformValue1 <= 0.66f) wf1 = Oscillator::Waveform::Triangle;
    else if (waveformValue1 <= 0.83f) wf1 = Oscillator::Waveform::Pulse25;
    else wf1 = Oscillator::Waveform::Pulse15;

    float waveformValue2 = parameters.getRawParameterValue("waveform2")->load();
    Oscillator::Waveform wf2;
    if (waveformValue2 <= 0.16f) wf2 = Oscillator::Waveform::Sine;
    else if (waveformValue2 <= 0.33f) wf2 = Oscillator::Waveform::Saw;
    else if (waveformValue2 <= 0.50f) wf2 = Oscillator::Waveform::Square;
    else if (waveformValue2 <= 0.66f) wf2 = Oscillator::Waveform::Triangle;
    else if (waveformValue2 <= 0.83f) wf2 = Oscillator::Waveform::Pulse25;
    else wf2 = Oscillator::Waveform::Pulse15;

    float waveformValue3 = parameters.getRawParameterValue("waveform3")->load();
    Oscillator::Waveform wf3;
    if (waveformValue3 <= 0.16f) wf3 = Oscillator::Waveform::Sine;
    else if (waveformValue3 <= 0.33f) wf3 = Oscillator::Waveform::Saw;
    else if (waveformValue3 <= 0.50f) wf3 = Oscillator::Waveform::Square;
    else if (waveformValue3 <= 0.66f) wf3 = Oscillator::Waveform::Triangle;
    else if (waveformValue3 <= 0.83f) wf3 = Oscillator::Waveform::Pulse25;
    else wf3 = Oscillator::Waveform::Pulse15;

    float noiseWaveformValue = parameters.getRawParameterValue("noiseWaveform")->load();
    NoiseOscillator::Waveform nwf = (noiseWaveformValue <= 0.5f) ? NoiseOscillator::Waveform::WhiteNoise : NoiseOscillator::Waveform::PinkNoise;

    float subWaveformValue = parameters.getRawParameterValue("subWaveform")->load();
    SubOscillator::Waveform swf;
    if (subWaveformValue <= 0.16f) swf = SubOscillator::Waveform::Sine;
    else if (subWaveformValue <= 0.33f) swf = SubOscillator::Waveform::RoundedSine;
    else if (subWaveformValue <= 0.50f) swf = SubOscillator::Waveform::Triangle;
    else if (subWaveformValue <= 0.66f) swf = SubOscillator::Waveform::Saw;
    else if (subWaveformValue <= 0.83f) swf = SubOscillator::Waveform::Square;
    else swf = SubOscillator::Waveform::Pulse33;

    for (auto* voice : voices) {
        voice->setWaveform(wf1, wf2, wf3, nwf, swf);
    }

    float lfoRate = parameters.getRawParameterValue("lfoRate")->load();
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

    float delayTimeMs = parameters.getRawParameterValue("delayTime")->load();
    float delayFeedback = parameters.getRawParameterValue("delayFeedback")->load();
    float delayMix = parameters.getRawParameterValue("delayMix")->load();
    int delaySamples = juce::jmax(1, static_cast<int>(juce::jlimit(0.0f, 1000.0f, delayTimeMs) * currentSampleRate / 1000.0f));

    reverbParams.roomSize = parameters.getRawParameterValue("reverbRoomSize")->load();
    reverbParams.damping = parameters.getRawParameterValue("reverbDamping")->load();
    reverbParams.wetLevel = parameters.getRawParameterValue("reverbWetLevel")->load();
    reverbParams.dryLevel = parameters.getRawParameterValue("reverbDryLevel")->load();
    reverb.setParameters(reverbParams);

    float distortionDrive = parameters.getRawParameterValue("distortionDrive")->load();
    float distortionTone = parameters.getRawParameterValue("distortionTone")->load();
    float distortionMix = parameters.getRawParameterValue("distortionMix")->load();

    if (distortionTone != lastDistortionTone) {
        *distortionToneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, distortionTone, 0.707f);
        lastDistortionTone = distortionTone;
    }

    compressor.setThreshold(parameters.getRawParameterValue("compressorThreshold")->load());
    compressor.setRatio(parameters.getRawParameterValue("compressorRatio")->load());
    compressor.setAttack(parameters.getRawParameterValue("compressorAttack")->load());
    compressor.setRelease(parameters.getRawParameterValue("compressorRelease")->load());
    compressorMakeupGain.setGainDecibels(parameters.getRawParameterValue("compressorMakeupGain")->load());

    chorus.setRate(parameters.getRawParameterValue("chorusRate")->load());
    chorus.setDepth(parameters.getRawParameterValue("chorusDepth")->load());
    chorus.setMix(parameters.getRawParameterValue("chorusMix")->load());
    chorus.setCentreDelay(parameters.getRawParameterValue("chorusDelay")->load());

    phaser.setRate(parameters.getRawParameterValue("phaserRate")->load());
    phaser.setDepth(parameters.getRawParameterValue("phaserDepth")->load());
    phaser.setMix(parameters.getRawParameterValue("phaserMix")->load());

    float flangerRate = parameters.getRawParameterValue("flangerRate")->load();
    float flangerDepth = parameters.getRawParameterValue("flangerDepth")->load();
    float flangerMix = parameters.getRawParameterValue("flangerMix")->load();
    float flangerDelayMs = parameters.getRawParameterValue("flangerDelay")->load();
    flangerLFO.setFrequency(flangerRate);

    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            int noteNumber = msg.getNoteNumber();
            float freq = juce::MidiMessage::getMidiNoteInHertz(noteNumber);

            if (noteToVoices.find(noteNumber) != noteToVoices.end()) {
                for (auto* voice : noteToVoices[noteNumber]) {
                    voice->noteOff();
                    voice->setNoteNumber(-1);
                }
                noteToVoices.erase(noteNumber);
            }

            Voice* freeVoice = findVoiceToSteal();
            if (freeVoice) {
                freeVoice->setNoteNumber(noteNumber);
                freeVoice->noteOn(freq, getSampleRate());
                noteToVoices[noteNumber] = { freeVoice };
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

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // Sum voices with enhanced normalization
    for (int sample = 0; sample < numSamples; ++sample) {
        float lfoValue = lfo.getNextSample();

        // Apply LFO to all active voices for frequency modulation
        for (auto* voice : voices) {
            if (voice->getIsActive()) {
                voice->setLFOValue(lfoValue, currentSampleRate);
            }
        }

        float mixedOutput = 0.0f;
        int activeVoices = 0;

        for (auto* voice : voices) {
            if (voice->getIsActive()) {
                float voiceOutput = voice->getNextSample();
                mixedOutput += voiceOutput;
                activeVoices++;
            }
        }

        // Enhanced normalization: scale down more aggressively
        if (activeVoices > 0) {
            mixedOutput /= (std::sqrt(static_cast<float>(activeVoices)) * 0.5f * activeVoices);
        }

        leftChannel[sample] = mixedOutput;
        rightChannel[sample] = mixedOutput;
    }

    // Apply LFO to filter cutoff
    float baseCutoff = parameters.getRawParameterValue("filterCutoff")->load();
    float lfoToCutoff = parameters.getRawParameterValue("lfoToFilterCutoff")->load();
    float lfoValue = lfo.getNextSample();
    float modulatedCutoff = baseCutoff * std::pow(2.0f, lfoValue * lfoToCutoff / 12.0f);
    modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);

    float resonance = parameters.getRawParameterValue("filterResonance")->load();
    resonance = juce::jlimit(0.1f, 5.0f, resonance); // Limit resonance to prevent excessive gain
    int filterTypeIdx = parameters.getParameter("filterType")->convertFrom0to1(
        parameters.getParameter("filterType")->getValue()
    );

    if (modulatedCutoff != lastFilterCutoff || resonance != lastFilterResonance || filterTypeIdx != lastFilterType) {
        lastFilterCutoff = modulatedCutoff;
        lastFilterResonance = resonance;
        lastFilterType = filterTypeIdx;

        juce::dsp::IIR::Coefficients<float>::Ptr newCoefficients;
        switch (filterTypeIdx) {
        case 0: // Low Pass
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, modulatedCutoff, resonance);
            break;
        case 1: // High Pass
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, modulatedCutoff, resonance);
            break;
        case 2: // Band Pass
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, modulatedCutoff, resonance);
            break;
        case 3: // Notch
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(currentSampleRate, modulatedCutoff, resonance);
            break;
        default:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, modulatedCutoff, resonance);
        }
        *filter.state = *newCoefficients;
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);

    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Apply distortion with reduced drive impact
    for (int sample = 0; sample < numSamples; ++sample) {
        float drive = juce::jlimit(1.0f, 5.0f, distortionDrive); // Limit drive to prevent excessive distortion
        leftChannel[sample] = std::tanh(leftChannel[sample] * drive);
        rightChannel[sample] = std::tanh(rightChannel[sample] * drive);
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

        delayedLeft = juce::jlimit(-1.0f, 1.0f, delayedLeft);
        delayedRight = juce::jlimit(-1.0f, 1.0f, delayedRight);

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

    // Apply limiter as the final stage
    limiter.process(context);
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
