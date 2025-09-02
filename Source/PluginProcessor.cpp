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
    // Initialize parameter mapping system
    initializeParameterMap();
    
    // Initialize preset system
    presetDirectory = getPresetDirectory();
    refreshPresetList();
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
    else if (filterNotchEnabled)
        initialFilterType = SimpleStableFilter::NOTCH;
    else if (filterCombEnabled)
        initialFilterType = SimpleStableFilter::COMB;
    else if (filterFormantEnabled)
        initialFilterType = SimpleStableFilter::FORMANT;
    
    // Determine initial filter slope
    SimpleStableFilter::FilterSlope initialFilterSlope = SimpleStableFilter::SLOPE_12DB;
    if (filter24dBEnabled)
        initialFilterSlope = SimpleStableFilter::SLOPE_24DB;
    
    osc1Filter.setSampleRate(sampleRate);
    osc1Filter.setCutoffFrequency(filterCutoff);
    osc1Filter.setResonance(resonanceToQ(filterResonance));
    osc1Filter.setFilterType(initialFilterType);
    osc1Filter.setFilterSlope(initialFilterSlope);
    
    osc2Filter.setSampleRate(sampleRate);
    osc2Filter.setCutoffFrequency(filterCutoff);
    osc2Filter.setResonance(resonanceToQ(filterResonance));
    osc2Filter.setFilterType(initialFilterType);
    osc2Filter.setFilterSlope(initialFilterSlope);
    
    // Initialize temporary buffers for oscillator separation
    osc1Buffer.setSize(2, samplesPerBlock);
    osc2Buffer.setSize(2, samplesPerBlock);
    
    // Initialize chorus effect
    chorus.setSampleRate(sampleRate);
    chorus.setEnabled(chorusEnabled);
    chorus.setRate(chorusRate);
    chorus.setDelay1(chorusDelay1);
    chorus.setDelay2(chorusDelay2);
    chorus.setDepth(chorusDepth);
    chorus.setFeedback(chorusFeedback);
    chorus.setLPFCutoff(chorusLPF);
    chorus.setMix(chorusMix);
    
    // Initialize flanger effect
    flanger.setSampleRate(sampleRate);
    flanger.setEnabled(flangerEnabled);
    flanger.setRate(flangerRate);
    flanger.setDepth(flangerDepth);
    flanger.setFeedback(flangerFeedback);
    flanger.setMix(flangerMix);
    flanger.setPhase(flangerPhase);
    
    // Initialize phaser effect
    phaser.setSampleRate(sampleRate);
    phaser.setEnabled(phaserEnabled);
    phaser.setRate(phaserRate);
    phaser.setDepth1(phaserDepth1);
    phaser.setDepth2(phaserDepth2);
    phaser.setFeedback(phaserFeedback);
    phaser.setMix(phaserMix);
    phaser.setPhase(phaserPhase);
    phaser.setFrequency(phaserFrequency);
    phaser.setPoles(phaserPoles);
    
    // Initialize compressor effect
    compressor.setSampleRate(sampleRate);
    compressor.setEnabled(compressorEnabled);
    compressor.setThreshold(compressorThreshold);
    compressor.setRatio(compressorRatio);
    compressor.setAttack(compressorAttack);
    compressor.setRelease(compressorRelease);
    compressor.setMakeupGain(compressorGain);
    compressor.setMix(compressorMix);
    compressor.setMultiband(compressorMultiband);
    
    // Initialize distortion effect
    distortion.setSampleRate(sampleRate);
    distortion.setEnabled(distortionEnabled);
    distortion.setType(distortionType);
    distortion.setDrive(distortionDrive);
    distortion.setMix(distortionMix);
    distortion.setFilterPosition(static_cast<DistortionEffect::FilterPosition>(distortionFilterPosition));
    SimpleStableFilter::FilterType filterType = SimpleStableFilter::LOWPASS;
    if (distortionFilterType == 2) filterType = SimpleStableFilter::BANDPASS;
    else if (distortionFilterType == 3) filterType = SimpleStableFilter::HIGHPASS;
    distortion.setFilterType(filterType);
    distortion.setFilterFrequency(distortionFilterFreq);
    distortion.setFilterQ(distortionFilterQ);
    
    // Initialize delay effect
    delay.setSampleRate(sampleRate);
    delay.setEnabled(delayEnabled);
    delay.setFeedback(delayFeedback);
    delay.setMix(delayMix);
    delay.setDelayMode(delayPingPong ? DelayEffect::PING_PONG : DelayEffect::NORMAL);
    delay.setLeftTime(delayLeftTime);
    delay.setRightTime(delayRightTime);
    delay.setBpmSync(delaySync);
    delay.setLeftTriplet(delayTriplet);
    delay.setLeftDotted(delayDotted);
    delay.setRightTriplet(delayRTriplet);
    delay.setRightDotted(delayRDotted);
    delay.setFilterFreq(delayFilterFreq);
    delay.setFilterQ(delayFilterQ);
    
    // Initialize reverb effect
    reverb.setSampleRate(sampleRate);
    reverb.setEnabled(reverbEnabled);
    reverb.setMix(reverbMix / 100.0f); // Convert from percentage to 0-1
    reverb.setType(static_cast<ReverbEffect::ReverbType>(reverbType));
    reverb.setLowCut(reverbLowCut);
    reverb.setHighCut(reverbHighCut);
    reverb.setSize(reverbSize / 100.0f); // Convert from percentage to 0-1
    reverb.setPreDelay(reverbPreDelay);
    reverb.setDamping(reverbDamping / 100.0f); // Convert from percentage to 0-1
    reverb.setWidth(reverbWidth / 100.0f); // Convert from percentage to 0-1
    
    // Initialize EQ effect
    eq.setSampleRate(sampleRate);
    eq.setEnabled(eqEnabled);
    eq.setBand1Enabled(eq1Enabled);
    eq.setBand2Enabled(eq2Enabled);
    eq.setBand1Frequency(eq1Frequency);
    eq.setBand1Q(eq1Q);
    eq.setBand1Gain(eq1Gain);
    BiquadFilter::FilterType eq1FilterType = BiquadFilter::PEAK;
    if (eq1Type == 0) eq1FilterType = BiquadFilter::PEAK;
    else if (eq1Type == 1) eq1FilterType = BiquadFilter::LOW_SHELF;
    else if (eq1Type == 2) eq1FilterType = BiquadFilter::HIGH_PASS;
    eq.setBand1Type(eq1FilterType);
    
    eq.setBand2Frequency(eq2Frequency);
    eq.setBand2Q(eq2Q);
    eq.setBand2Gain(eq2Gain);
    BiquadFilter::FilterType eq2FilterType = BiquadFilter::PEAK;
    if (eq2Type == 0) eq2FilterType = BiquadFilter::PEAK;
    else if (eq2Type == 1) eq2FilterType = BiquadFilter::HIGH_SHELF;
    else if (eq2Type == 2) eq2FilterType = BiquadFilter::LOW_PASS;
    eq.setBand2Type(eq2FilterType);
    
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
    
    // Apply chorus effect
    chorus.processBlock(buffer);
    
    // Apply flanger effect
    flanger.processBlock(buffer);
    
    // Apply phaser effect
    phaser.processBlock(buffer);
    
    // Apply compressor effect
    compressor.processBlock(buffer);
    
    // Apply distortion effect
    distortion.processBlock(buffer);
    
    // Apply delay effect
    delay.processBlock(buffer);
    
    // Apply reverb effect
    reverb.processBlock(buffer);
    
    // Apply EQ effect
    eq.processBlock(buffer);
    
    // Apply volume control
    buffer.applyGain(masterVolume);
}

void SummonerXSerum2AudioProcessor::updateEnvelopeParameters()
{
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synthesiser.getVoice(i)))
        {
            voice->setEnvelopeParameters(osc1Attack, osc1Decay, osc1Sustain, osc1Release);
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
            voice->setDetune(osc1Detune);
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
            voice->setStereoWidth(osc1StereoWidth);
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
            voice->setPan(osc1Pan);
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
            voice->setPhase(osc1Phase);
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
    // Don't reset filters - preserve state to avoid audio artifacts
    
    // Determine filter type
    SimpleStableFilter::FilterType filterType = SimpleStableFilter::OFF;
    if (filterLPEnabled)
        filterType = SimpleStableFilter::LOWPASS;
    else if (filterHPEnabled)
        filterType = SimpleStableFilter::HIGHPASS;
    else if (filterBPEnabled)
        filterType = SimpleStableFilter::BANDPASS;
    else if (filterNotchEnabled)
        filterType = SimpleStableFilter::NOTCH;
    else if (filterCombEnabled)
        filterType = SimpleStableFilter::COMB;
    else if (filterFormantEnabled)
        filterType = SimpleStableFilter::FORMANT;
    
    // Determine filter slope
    SimpleStableFilter::FilterSlope filterSlope = SimpleStableFilter::SLOPE_12DB;
    if (filter24dBEnabled)
        filterSlope = SimpleStableFilter::SLOPE_24DB;
    
    osc1Filter.setCutoffFrequency(filterCutoff);
    osc1Filter.setResonance(resonanceToQ(filterResonance));
    osc1Filter.setFilterType(filterType);
    osc1Filter.setFilterSlope(filterSlope);
    osc2Filter.setCutoffFrequency(filterCutoff);
    osc2Filter.setResonance(resonanceToQ(filterResonance));
    osc2Filter.setFilterType(filterType);
    osc2Filter.setFilterSlope(filterSlope);
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
        auto [successful, failed] = applyResponseParameters(responses[index]);
        juce::ignoreUnused(successful, failed);
    }
}

void SummonerXSerum2AudioProcessor::nextResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex < responses.size() - 1)
    {
        currentResponseIndex++;
        auto [successful, failed] = applyResponseParameters(responses[currentResponseIndex]);
        juce::ignoreUnused(successful, failed);
    }
}

void SummonerXSerum2AudioProcessor::previousResponse()
{
    juce::ScopedLock lock(responseLock);
    if (currentResponseIndex > 0)
    {
        currentResponseIndex--;
        auto [successful, failed] = applyResponseParameters(responses[currentResponseIndex]);
        juce::ignoreUnused(successful, failed);
    }
}

void SummonerXSerum2AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

void SummonerXSerum2AudioProcessor::initializeParameterMap()
{
    // Main Oscillator 1 Parameters (9)
    parameterMap["masterVolume"] = {ParameterInfo::FLOAT, 0.0f, 5.0f, [this](float v) { setMasterVolume(v); }};
    parameterMap["osc1Detune"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setOsc1Detune(v); }};
    parameterMap["osc1StereoWidth"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc1StereoWidth(v); }};
    parameterMap["osc1Pan"] = {ParameterInfo::FLOAT, -1.0f, 1.0f, [this](float v) { setOsc1Pan(v); }};
    parameterMap["osc1Phase"] = {ParameterInfo::FLOAT, 0.0f, 360.0f, [this](float v) { setOsc1Phase(v); }};
    parameterMap["osc1Attack"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc1Attack(v); }};
    parameterMap["osc1Decay"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc1Decay(v); }};
    parameterMap["osc1Sustain"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc1Sustain(v); }};
    parameterMap["osc1Release"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc1Release(v); }};

    // Oscillator 1 Parameters (8)
    parameterMap["osc1Type"] = {ParameterInfo::INT, 0.0f, 10.0f, [this](float v) { setOsc1Type((int)v); }};
    parameterMap["osc1PulseWidth"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc1PulseWidth(v); }};
    parameterMap["osc1Octave"] = {ParameterInfo::INT, -4.0f, 4.0f, [this](float v) { setOsc1Octave((int)v); }};
    parameterMap["osc1Semitone"] = {ParameterInfo::INT, -12.0f, 12.0f, [this](float v) { setOsc1Semitone((int)v); }};
    parameterMap["osc1FineTune"] = {ParameterInfo::INT, -100.0f, 100.0f, [this](float v) { setOsc1FineTune((int)v); }};
    parameterMap["osc1RandomPhase"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setOsc1RandomPhase(v > 0.5f); }};
    parameterMap["osc1VoiceCount"] = {ParameterInfo::INT, 1.0f, 16.0f, [this](float v) { setOsc1VoiceCount((int)v); }};
    parameterMap["osc1Volume"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc1Volume(v); }};

    // Oscillator 2 Parameters (16)
    parameterMap["osc2Enabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setOsc2Enabled(v > 0.5f); }};
    parameterMap["osc2Type"] = {ParameterInfo::INT, 0.0f, 10.0f, [this](float v) { setOsc2Type((int)v); }};
    parameterMap["osc2Volume"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc2Volume(v); }};
    parameterMap["osc2Detune"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setOsc2Detune(v); }};
    parameterMap["osc2Stereo"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc2Stereo(v); }};
    parameterMap["osc2Pan"] = {ParameterInfo::FLOAT, -1.0f, 1.0f, [this](float v) { setOsc2Pan(v); }};
    parameterMap["osc2Octave"] = {ParameterInfo::INT, -4.0f, 4.0f, [this](float v) { setOsc2Octave((int)v); }};
    parameterMap["osc2Semitone"] = {ParameterInfo::INT, -12.0f, 12.0f, [this](float v) { setOsc2Semitone((int)v); }};
    parameterMap["osc2FineTune"] = {ParameterInfo::INT, -100.0f, 100.0f, [this](float v) { setOsc2FineTune((int)v); }};
    parameterMap["osc2RandomPhase"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setOsc2RandomPhase(v > 0.5f); }};
    parameterMap["osc2Phase"] = {ParameterInfo::FLOAT, 0.0f, 360.0f, [this](float v) { setOsc2Phase(v); }};
    parameterMap["osc2Attack"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc2Attack(v); }};
    parameterMap["osc2Decay"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc2Decay(v); }};
    parameterMap["osc2Sustain"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setOsc2Sustain(v); }};
    parameterMap["osc2Release"] = {ParameterInfo::FLOAT, 0.0f, 10.0f, [this](float v) { setOsc2Release(v); }};
    parameterMap["osc2VoiceCount"] = {ParameterInfo::INT, 1.0f, 16.0f, [this](float v) { setOsc2VoiceCount((int)v); }};

    // Filter Parameters (12)
    parameterMap["filterCutoff"] = {ParameterInfo::FLOAT, 20.0f, 20000.0f, [this](float v) { setFilterCutoff(v); }};
    parameterMap["filterResonance"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setFilterResonance(v); }};
    parameterMap["osc1FilterEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setOsc1FilterEnabled(v > 0.5f); }};
    parameterMap["osc2FilterEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setOsc2FilterEnabled(v > 0.5f); }};
    parameterMap["filterLPEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterLPEnabled(v > 0.5f); }};
    parameterMap["filterHPEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterHPEnabled(v > 0.5f); }};
    parameterMap["filterBPEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterBPEnabled(v > 0.5f); }};
    parameterMap["filterNotchEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterNotchEnabled(v > 0.5f); }};
    parameterMap["filterCombEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterCombEnabled(v > 0.5f); }};
    parameterMap["filterFormantEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilterFormantEnabled(v > 0.5f); }};
    parameterMap["filter12dBEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilter12dBEnabled(v > 0.5f); }};
    parameterMap["filter24dBEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFilter24dBEnabled(v > 0.5f); }};

    // Chorus Effect Parameters (8)
    parameterMap["chorusEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setChorusEnabled(v > 0.5f); }};
    parameterMap["chorusRate"] = {ParameterInfo::FLOAT, 0.1f, 10.0f, [this](float v) { setChorusRate(v); }};
    parameterMap["chorusDelay1"] = {ParameterInfo::FLOAT, 1.0f, 50.0f, [this](float v) { setChorusDelay1(v); }};
    parameterMap["chorusDelay2"] = {ParameterInfo::FLOAT, 1.0f, 50.0f, [this](float v) { setChorusDelay2(v); }};
    parameterMap["chorusDepth"] = {ParameterInfo::FLOAT, 0.0f, 20.0f, [this](float v) { setChorusDepth(v); }};
    parameterMap["chorusFeedback"] = {ParameterInfo::FLOAT, 0.0f, 0.95f, [this](float v) { setChorusFeedback(v); }};
    parameterMap["chorusLPF"] = {ParameterInfo::FLOAT, 200.0f, 20000.0f, [this](float v) { setChorusLPF(v); }};
    parameterMap["chorusMix"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setChorusMix(v); }};

    // Flanger Effect Parameters (6)
    parameterMap["flangerEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setFlangerEnabled(v > 0.5f); }};
    parameterMap["flangerRate"] = {ParameterInfo::FLOAT, 0.1f, 10.0f, [this](float v) { setFlangerRate(v); }};
    parameterMap["flangerDepth"] = {ParameterInfo::FLOAT, 0.1f, 10.0f, [this](float v) { setFlangerDepth(v); }};
    parameterMap["flangerFeedback"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setFlangerFeedback(v); }};
    parameterMap["flangerMix"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setFlangerMix(v); }};
    parameterMap["flangerPhase"] = {ParameterInfo::FLOAT, 0.0f, 360.0f, [this](float v) { setFlangerPhase(v); }};

    // Phaser Effect Parameters (9)
    parameterMap["phaserEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setPhaserEnabled(v > 0.5f); }};
    parameterMap["phaserRate"] = {ParameterInfo::FLOAT, 0.1f, 10.0f, [this](float v) { setPhaserRate(v); }};
    parameterMap["phaserDepth1"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setPhaserDepth1(v); }};
    parameterMap["phaserDepth2"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setPhaserDepth2(v); }};
    parameterMap["phaserFeedback"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setPhaserFeedback(v); }};
    parameterMap["phaserMix"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setPhaserMix(v); }};
    parameterMap["phaserPhase"] = {ParameterInfo::FLOAT, 0.0f, 360.0f, [this](float v) { setPhaserPhase(v); }};
    parameterMap["phaserFrequency"] = {ParameterInfo::FLOAT, 20.0f, 2000.0f, [this](float v) { setPhaserFrequency(v); }};
    parameterMap["phaserPoles"] = {ParameterInfo::INT, 1.0f, 16.0f, [this](float v) { setPhaserPoles((int)v); }};

    // Compressor Effect Parameters (8)
    parameterMap["compressorEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setCompressorEnabled(v > 0.5f); }};
    parameterMap["compressorThreshold"] = {ParameterInfo::FLOAT, -60.0f, 0.0f, [this](float v) { setCompressorThreshold(v); }};
    parameterMap["compressorRatio"] = {ParameterInfo::FLOAT, 1.0f, 20.0f, [this](float v) { setCompressorRatio(v); }};
    parameterMap["compressorAttack"] = {ParameterInfo::FLOAT, 0.1f, 100.0f, [this](float v) { setCompressorAttack(v); }};
    parameterMap["compressorRelease"] = {ParameterInfo::FLOAT, 10.0f, 1000.0f, [this](float v) { setCompressorRelease(v); }};
    parameterMap["compressorGain"] = {ParameterInfo::FLOAT, 0.0f, 30.0f, [this](float v) { setCompressorGain(v); }};
    parameterMap["compressorMix"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setCompressorMix(v); }};
    parameterMap["compressorMultiband"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setCompressorMultiband(v > 0.5f); }};

    // Distortion Effect Parameters (8)
    parameterMap["distortionEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDistortionEnabled(v > 0.5f); }};
    parameterMap["distortionType"] = {ParameterInfo::INT, 1.0f, 16.0f, [this](float v) { setDistortionType((int)v); }};
    parameterMap["distortionDrive"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setDistortionDrive(v); }};
    parameterMap["distortionMix"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setDistortionMix(v); }};
    parameterMap["distortionFilterPosition"] = {ParameterInfo::INT, 0.0f, 2.0f, [this](float v) { setDistortionFilterPosition((int)v); }};
    parameterMap["distortionFilterType"] = {ParameterInfo::INT, 1.0f, 3.0f, [this](float v) { setDistortionFilterType((int)v); }};
    parameterMap["distortionFilterFreq"] = {ParameterInfo::FLOAT, 20.0f, 20000.0f, [this](float v) { setDistortionFilterFreq(v); }};
    parameterMap["distortionFilterQ"] = {ParameterInfo::FLOAT, 0.1f, 30.0f, [this](float v) { setDistortionFilterQ(v); }};

    // Delay Effect Parameters (13)
    parameterMap["delayEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayEnabled(v > 0.5f); }};
    parameterMap["delayFeedback"] = {ParameterInfo::FLOAT, 0.0f, 0.95f, [this](float v) { setDelayFeedback(v); }};
    parameterMap["delayMix"] = {ParameterInfo::FLOAT, 0.0f, 1.0f, [this](float v) { setDelayMix(v); }};
    parameterMap["delayPingPong"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayPingPong(v > 0.5f); }};
    parameterMap["delayLeftTime"] = {ParameterInfo::FLOAT, 1.0f, 2000.0f, [this](float v) { setDelayLeftTime(v); }};
    parameterMap["delayRightTime"] = {ParameterInfo::FLOAT, 1.0f, 2000.0f, [this](float v) { setDelayRightTime(v); }};
    parameterMap["delaySync"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelaySync(v > 0.5f); }};
    parameterMap["delayTriplet"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayTriplet(v > 0.5f); }};
    parameterMap["delayDotted"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayDotted(v > 0.5f); }};
    parameterMap["delayRTriplet"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayRTriplet(v > 0.5f); }};
    parameterMap["delayRDotted"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setDelayRDotted(v > 0.5f); }};
    parameterMap["delayFilterFreq"] = {ParameterInfo::FLOAT, 20.0f, 20000.0f, [this](float v) { setDelayFilterFreq(v); }};
    parameterMap["delayFilterQ"] = {ParameterInfo::FLOAT, 0.1f, 30.0f, [this](float v) { setDelayFilterQ(v); }};

    // Reverb Effect Parameters (9)
    parameterMap["reverbEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setReverbEnabled(v > 0.5f); }};
    parameterMap["reverbMix"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setReverbMix(v); }};
    parameterMap["reverbType"] = {ParameterInfo::INT, 1.0f, 5.0f, [this](float v) { setReverbType((int)v); }};
    parameterMap["reverbLowCut"] = {ParameterInfo::FLOAT, 20.0f, 1000.0f, [this](float v) { setReverbLowCut(v); }};
    parameterMap["reverbHighCut"] = {ParameterInfo::FLOAT, 1000.0f, 20000.0f, [this](float v) { setReverbHighCut(v); }};
    parameterMap["reverbSize"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setReverbSize(v); }};
    parameterMap["reverbPreDelay"] = {ParameterInfo::FLOAT, 0.0f, 200.0f, [this](float v) { setReverbPreDelay(v); }};
    parameterMap["reverbDamping"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setReverbDamping(v); }};
    parameterMap["reverbWidth"] = {ParameterInfo::FLOAT, 0.0f, 100.0f, [this](float v) { setReverbWidth(v); }};

    // EQ Effect Parameters (11)
    parameterMap["eqEnabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setEQEnabled(v > 0.5f); }};
    parameterMap["eq1Enabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setEQ1Enabled(v > 0.5f); }};
    parameterMap["eq1Frequency"] = {ParameterInfo::FLOAT, 20.0f, 20000.0f, [this](float v) { setEQ1Frequency(v); }};
    parameterMap["eq1Q"] = {ParameterInfo::FLOAT, 0.1f, 30.0f, [this](float v) { setEQ1Q(v); }};
    parameterMap["eq1Gain"] = {ParameterInfo::FLOAT, -15.0f, 15.0f, [this](float v) { setEQ1Gain(v); }};
    parameterMap["eq1Type"] = {ParameterInfo::INT, 0.0f, 2.0f, [this](float v) { setEQ1Type((int)v); }};
    parameterMap["eq2Enabled"] = {ParameterInfo::BOOL, 0.0f, 1.0f, [this](float v) { setEQ2Enabled(v > 0.5f); }};
    parameterMap["eq2Frequency"] = {ParameterInfo::FLOAT, 20.0f, 20000.0f, [this](float v) { setEQ2Frequency(v); }};
    parameterMap["eq2Q"] = {ParameterInfo::FLOAT, 0.1f, 30.0f, [this](float v) { setEQ2Q(v); }};
    parameterMap["eq2Gain"] = {ParameterInfo::FLOAT, -15.0f, 15.0f, [this](float v) { setEQ2Gain(v); }};
    parameterMap["eq2Type"] = {ParameterInfo::INT, 0.0f, 2.0f, [this](float v) { setEQ2Type((int)v); }};
}

bool SummonerXSerum2AudioProcessor::setParameterByName(const std::string& name, const std::string& value)
{
    float floatValue = parseStringValue(value);
    return setParameterByName(name, floatValue);
}

bool SummonerXSerum2AudioProcessor::setParameterByName(const std::string& name, float value)
{
    auto it = parameterMap.find(name);
    if (it == parameterMap.end())
    {
        DBG("Parameter not found: " << name);
        return false;
    }

    if (!validateParameterValue(name, value))
    {
        DBG("Parameter value out of range for " << name << ": " << value);
        return false;
    }

    try 
    {
        it->second.setter(value);
        DBG("Successfully set parameter " << name << " to " << value);
        return true;
    }
    catch (...)
    {
        DBG("Exception occurred setting parameter " << name);
        return false;
    }
}

float SummonerXSerum2AudioProcessor::parseStringValue(const std::string& value)
{
    // Handle boolean strings
    if (value == "true" || value == "True" || value == "TRUE" || value == "on" || value == "On" || value == "ON")
        return 1.0f;
    if (value == "false" || value == "False" || value == "FALSE" || value == "off" || value == "Off" || value == "OFF")
        return 0.0f;

    // Try to parse as float
    try 
    {
        return std::stof(value);
    }
    catch (...)
    {
        DBG("Failed to parse value: " << value);
        return 0.0f;
    }
}

bool SummonerXSerum2AudioProcessor::validateParameterValue(const std::string& name, float value)
{
    auto it = parameterMap.find(name);
    if (it == parameterMap.end())
        return false;

    const auto& info = it->second;
    return value >= info.minValue && value <= info.maxValue;
}

std::pair<int, int> SummonerXSerum2AudioProcessor::applyResponseParameters(const std::map<std::string, std::string>& response)
{
    int successfulUpdates = 0;
    int failedUpdates = 0;
    
    DBG("Applying AI response with " << response.size() << " parameters");
    
    for (const auto& param : response)
    {
        bool success = setParameterByName(param.first, param.second);
        if (success)
        {
            successfulUpdates++;
        }
        else
        {
            failedUpdates++;
            DBG("Failed to apply parameter: " << param.first << " = " << param.second);
        }
    }
    
    DBG("Parameter application complete: " << successfulUpdates << " successful, " << failedUpdates << " failed");
    
    // Trigger UI update if there are successful parameter changes
    if (successfulUpdates > 0)
    {
        // This will notify the editor that parameters have changed
        updateHostDisplay();
    }
    
    return {successfulUpdates, failedUpdates};
}

// Preset Management System Implementation

juce::File SummonerXSerum2AudioProcessor::getPresetDirectory()
{
    // Create presets folder in user documents directory
    juce::File userDocs = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    juce::File presetDir = userDocs.getChildFile("SummonerXSerum2").getChildFile("Presets");
    
    if (!presetDir.exists())
        presetDir.createDirectory();
        
    return presetDir;
}

void SummonerXSerum2AudioProcessor::refreshPresetList()
{
    availablePresets.clear();
    
    if (presetDirectory.exists() && presetDirectory.isDirectory())
    {
        juce::Array<juce::File> files;
        presetDirectory.findChildFiles(files, juce::File::findFiles, false, "*.sxs2");
        
        for (auto& file : files)
            availablePresets.push_back(file);
            
        // Sort by name
        std::sort(availablePresets.begin(), availablePresets.end(), 
                  [](const juce::File& a, const juce::File& b) {
                      return a.getFileNameWithoutExtension() < b.getFileNameWithoutExtension();
                  });
    }
    
    // Update current preset index if we have a current preset name
    currentPresetIndex = -1;
    for (int i = 0; i < (int)availablePresets.size(); ++i)
    {
        if (availablePresets[i].getFileNameWithoutExtension() == currentPresetName)
        {
            currentPresetIndex = i;
            break;
        }
    }
}

juce::ValueTree SummonerXSerum2AudioProcessor::createPresetData()
{
    juce::ValueTree preset("PRESET");
    
    // Add metadata
    preset.setProperty("name", currentPresetName, nullptr);
    preset.setProperty("creationTime", juce::Time::getCurrentTime().toISO8601(false), nullptr);
    preset.setProperty("version", "1.0", nullptr);
    
    // Add ALL synthesizer parameters - use parameter map to ensure completeness
    juce::ValueTree params("PARAMETERS");
    
    // Save every single parameter from the parameter map
    for (const auto& param : parameterMap)
    {
        const std::string& paramName = param.first;
        // Get current value using the getter methods
        if (paramName == "masterVolume") params.setProperty("masterVolume", masterVolume, nullptr);
        else if (paramName == "osc1Detune") params.setProperty("osc1Detune", osc1Detune, nullptr);
        else if (paramName == "osc1StereoWidth") params.setProperty("osc1StereoWidth", osc1StereoWidth, nullptr);
        else if (paramName == "osc1Pan") params.setProperty("osc1Pan", osc1Pan, nullptr);
        else if (paramName == "osc1Phase") params.setProperty("osc1Phase", osc1Phase, nullptr);
        else if (paramName == "osc1Attack") params.setProperty("osc1Attack", osc1Attack, nullptr);
        else if (paramName == "osc1Decay") params.setProperty("osc1Decay", osc1Decay, nullptr);
        else if (paramName == "osc1Sustain") params.setProperty("osc1Sustain", osc1Sustain, nullptr);
        else if (paramName == "osc1Release") params.setProperty("osc1Release", osc1Release, nullptr);
        
        else if (paramName == "osc1Type") params.setProperty("osc1Type", osc1Type, nullptr);
        else if (paramName == "osc1PulseWidth") params.setProperty("osc1PulseWidth", osc1PulseWidth, nullptr);
        else if (paramName == "osc1Octave") params.setProperty("osc1Octave", osc1Octave, nullptr);
        else if (paramName == "osc1Semitone") params.setProperty("osc1Semitone", osc1Semitone, nullptr);
        else if (paramName == "osc1FineTune") params.setProperty("osc1FineTune", osc1FineTune, nullptr);
        else if (paramName == "osc1RandomPhase") params.setProperty("osc1RandomPhase", osc1RandomPhase, nullptr);
        else if (paramName == "osc1VoiceCount") params.setProperty("osc1VoiceCount", osc1VoiceCount, nullptr);
        else if (paramName == "osc1Volume") params.setProperty("osc1Volume", osc1Volume, nullptr);
        
        else if (paramName == "osc2Enabled") params.setProperty("osc2Enabled", osc2Enabled, nullptr);
        else if (paramName == "osc2Type") params.setProperty("osc2Type", osc2Type, nullptr);
        else if (paramName == "osc2Volume") params.setProperty("osc2Volume", osc2Volume, nullptr);
        else if (paramName == "osc2Detune") params.setProperty("osc2Detune", osc2Detune, nullptr);
        else if (paramName == "osc2Stereo") params.setProperty("osc2Stereo", osc2Stereo, nullptr);
        else if (paramName == "osc2Pan") params.setProperty("osc2Pan", osc2Pan, nullptr);
        else if (paramName == "osc2Octave") params.setProperty("osc2Octave", osc2Octave, nullptr);
        else if (paramName == "osc2Semitone") params.setProperty("osc2Semitone", osc2Semitone, nullptr);
        else if (paramName == "osc2FineTune") params.setProperty("osc2FineTune", osc2FineTune, nullptr);
        else if (paramName == "osc2RandomPhase") params.setProperty("osc2RandomPhase", osc2RandomPhase, nullptr);
        else if (paramName == "osc2Phase") params.setProperty("osc2Phase", osc2Phase, nullptr);
        else if (paramName == "osc2Attack") params.setProperty("osc2Attack", osc2Attack, nullptr);
        else if (paramName == "osc2Decay") params.setProperty("osc2Decay", osc2Decay, nullptr);
        else if (paramName == "osc2Sustain") params.setProperty("osc2Sustain", osc2Sustain, nullptr);
        else if (paramName == "osc2Release") params.setProperty("osc2Release", osc2Release, nullptr);
        else if (paramName == "osc2VoiceCount") params.setProperty("osc2VoiceCount", osc2VoiceCount, nullptr);
        
        else if (paramName == "filterCutoff") params.setProperty("filterCutoff", filterCutoff, nullptr);
        else if (paramName == "filterResonance") params.setProperty("filterResonance", filterResonance, nullptr);
        else if (paramName == "osc1FilterEnabled") params.setProperty("osc1FilterEnabled", osc1FilterEnabled, nullptr);
        else if (paramName == "osc2FilterEnabled") params.setProperty("osc2FilterEnabled", osc2FilterEnabled, nullptr);
        else if (paramName == "filterLPEnabled") params.setProperty("filterLPEnabled", filterLPEnabled, nullptr);
        else if (paramName == "filterHPEnabled") params.setProperty("filterHPEnabled", filterHPEnabled, nullptr);
        else if (paramName == "filterBPEnabled") params.setProperty("filterBPEnabled", filterBPEnabled, nullptr);
        else if (paramName == "filterNotchEnabled") params.setProperty("filterNotchEnabled", filterNotchEnabled, nullptr);
        else if (paramName == "filterCombEnabled") params.setProperty("filterCombEnabled", filterCombEnabled, nullptr);
        else if (paramName == "filterFormantEnabled") params.setProperty("filterFormantEnabled", filterFormantEnabled, nullptr);
        else if (paramName == "filter12dBEnabled") params.setProperty("filter12dBEnabled", filter12dBEnabled, nullptr);
        else if (paramName == "filter24dBEnabled") params.setProperty("filter24dBEnabled", filter24dBEnabled, nullptr);
        
        else if (paramName == "chorusEnabled") params.setProperty("chorusEnabled", chorusEnabled, nullptr);
        else if (paramName == "chorusRate") params.setProperty("chorusRate", chorusRate, nullptr);
        else if (paramName == "chorusDelay1") params.setProperty("chorusDelay1", chorusDelay1, nullptr);
        else if (paramName == "chorusDelay2") params.setProperty("chorusDelay2", chorusDelay2, nullptr);
        else if (paramName == "chorusDepth") params.setProperty("chorusDepth", chorusDepth, nullptr);
        else if (paramName == "chorusFeedback") params.setProperty("chorusFeedback", chorusFeedback, nullptr);
        else if (paramName == "chorusLPF") params.setProperty("chorusLPF", chorusLPF, nullptr);
        else if (paramName == "chorusMix") params.setProperty("chorusMix", chorusMix, nullptr);
        
        else if (paramName == "flangerEnabled") params.setProperty("flangerEnabled", flangerEnabled, nullptr);
        else if (paramName == "flangerRate") params.setProperty("flangerRate", flangerRate, nullptr);
        else if (paramName == "flangerDepth") params.setProperty("flangerDepth", flangerDepth, nullptr);
        else if (paramName == "flangerFeedback") params.setProperty("flangerFeedback", flangerFeedback, nullptr);
        else if (paramName == "flangerMix") params.setProperty("flangerMix", flangerMix, nullptr);
        else if (paramName == "flangerPhase") params.setProperty("flangerPhase", flangerPhase, nullptr);
        
        else if (paramName == "phaserEnabled") params.setProperty("phaserEnabled", phaserEnabled, nullptr);
        else if (paramName == "phaserRate") params.setProperty("phaserRate", phaserRate, nullptr);
        else if (paramName == "phaserDepth1") params.setProperty("phaserDepth1", phaserDepth1, nullptr);
        else if (paramName == "phaserDepth2") params.setProperty("phaserDepth2", phaserDepth2, nullptr);
        else if (paramName == "phaserFeedback") params.setProperty("phaserFeedback", phaserFeedback, nullptr);
        else if (paramName == "phaserMix") params.setProperty("phaserMix", phaserMix, nullptr);
        else if (paramName == "phaserPhase") params.setProperty("phaserPhase", phaserPhase, nullptr);
        else if (paramName == "phaserFrequency") params.setProperty("phaserFrequency", phaserFrequency, nullptr);
        else if (paramName == "phaserPoles") params.setProperty("phaserPoles", phaserPoles, nullptr);
        
        else if (paramName == "compressorEnabled") params.setProperty("compressorEnabled", compressorEnabled, nullptr);
        else if (paramName == "compressorThreshold") params.setProperty("compressorThreshold", compressorThreshold, nullptr);
        else if (paramName == "compressorRatio") params.setProperty("compressorRatio", compressorRatio, nullptr);
        else if (paramName == "compressorAttack") params.setProperty("compressorAttack", compressorAttack, nullptr);
        else if (paramName == "compressorRelease") params.setProperty("compressorRelease", compressorRelease, nullptr);
        else if (paramName == "compressorGain") params.setProperty("compressorGain", compressorGain, nullptr);
        else if (paramName == "compressorMix") params.setProperty("compressorMix", compressorMix, nullptr);
        else if (paramName == "compressorMultiband") params.setProperty("compressorMultiband", compressorMultiband, nullptr);
        
        else if (paramName == "distortionEnabled") params.setProperty("distortionEnabled", distortionEnabled, nullptr);
        else if (paramName == "distortionType") params.setProperty("distortionType", distortionType, nullptr);
        else if (paramName == "distortionDrive") params.setProperty("distortionDrive", distortionDrive, nullptr);
        else if (paramName == "distortionMix") params.setProperty("distortionMix", distortionMix, nullptr);
        else if (paramName == "distortionFilterPosition") params.setProperty("distortionFilterPosition", distortionFilterPosition, nullptr);
        else if (paramName == "distortionFilterType") params.setProperty("distortionFilterType", distortionFilterType, nullptr);
        else if (paramName == "distortionFilterFreq") params.setProperty("distortionFilterFreq", distortionFilterFreq, nullptr);
        else if (paramName == "distortionFilterQ") params.setProperty("distortionFilterQ", distortionFilterQ, nullptr);
        
        else if (paramName == "delayEnabled") params.setProperty("delayEnabled", delayEnabled, nullptr);
        else if (paramName == "delayFeedback") params.setProperty("delayFeedback", delayFeedback, nullptr);
        else if (paramName == "delayMix") params.setProperty("delayMix", delayMix, nullptr);
        else if (paramName == "delayPingPong") params.setProperty("delayPingPong", delayPingPong, nullptr);
        else if (paramName == "delayLeftTime") params.setProperty("delayLeftTime", delayLeftTime, nullptr);
        else if (paramName == "delayRightTime") params.setProperty("delayRightTime", delayRightTime, nullptr);
        else if (paramName == "delaySync") params.setProperty("delaySync", delaySync, nullptr);
        else if (paramName == "delayTriplet") params.setProperty("delayTriplet", delayTriplet, nullptr);
        else if (paramName == "delayDotted") params.setProperty("delayDotted", delayDotted, nullptr);
        else if (paramName == "delayRTriplet") params.setProperty("delayRTriplet", delayRTriplet, nullptr);
        else if (paramName == "delayRDotted") params.setProperty("delayRDotted", delayRDotted, nullptr);
        else if (paramName == "delayFilterFreq") params.setProperty("delayFilterFreq", delayFilterFreq, nullptr);
        else if (paramName == "delayFilterQ") params.setProperty("delayFilterQ", delayFilterQ, nullptr);
        
        else if (paramName == "reverbEnabled") params.setProperty("reverbEnabled", reverbEnabled, nullptr);
        else if (paramName == "reverbMix") params.setProperty("reverbMix", reverbMix, nullptr);
        else if (paramName == "reverbType") params.setProperty("reverbType", reverbType, nullptr);
        else if (paramName == "reverbLowCut") params.setProperty("reverbLowCut", reverbLowCut, nullptr);
        else if (paramName == "reverbHighCut") params.setProperty("reverbHighCut", reverbHighCut, nullptr);
        else if (paramName == "reverbSize") params.setProperty("reverbSize", reverbSize, nullptr);
        else if (paramName == "reverbPreDelay") params.setProperty("reverbPreDelay", reverbPreDelay, nullptr);
        else if (paramName == "reverbDamping") params.setProperty("reverbDamping", reverbDamping, nullptr);
        else if (paramName == "reverbWidth") params.setProperty("reverbWidth", reverbWidth, nullptr);
        
        else if (paramName == "eqEnabled") params.setProperty("eqEnabled", eqEnabled, nullptr);
        else if (paramName == "eq1Enabled") params.setProperty("eq1Enabled", eq1Enabled, nullptr);
        else if (paramName == "eq1Frequency") params.setProperty("eq1Frequency", eq1Frequency, nullptr);
        else if (paramName == "eq1Q") params.setProperty("eq1Q", eq1Q, nullptr);
        else if (paramName == "eq1Gain") params.setProperty("eq1Gain", eq1Gain, nullptr);
        else if (paramName == "eq1Type") params.setProperty("eq1Type", eq1Type, nullptr);
        else if (paramName == "eq2Enabled") params.setProperty("eq2Enabled", eq2Enabled, nullptr);
        else if (paramName == "eq2Frequency") params.setProperty("eq2Frequency", eq2Frequency, nullptr);
        else if (paramName == "eq2Q") params.setProperty("eq2Q", eq2Q, nullptr);
        else if (paramName == "eq2Gain") params.setProperty("eq2Gain", eq2Gain, nullptr);
        else if (paramName == "eq2Type") params.setProperty("eq2Type", eq2Type, nullptr);
        else
        {
            // Debug output for any missed parameters
            DBG("Warning: Parameter not saved in preset: " << paramName);
        }
    }
    
    // Main oscillator 1 parameters
    params.setProperty("masterVolume", masterVolume, nullptr);
    params.setProperty("osc1Detune", osc1Detune, nullptr);
    params.setProperty("osc1StereoWidth", osc1StereoWidth, nullptr);
    params.setProperty("osc1Pan", osc1Pan, nullptr);
    params.setProperty("osc1Phase", osc1Phase, nullptr);
    params.setProperty("osc1Attack", osc1Attack, nullptr);
    params.setProperty("osc1Decay", osc1Decay, nullptr);
    params.setProperty("osc1Sustain", osc1Sustain, nullptr);
    params.setProperty("osc1Release", osc1Release, nullptr);
    
    // Oscillator 1 parameters
    params.setProperty("osc1Type", osc1Type, nullptr);
    params.setProperty("osc1PulseWidth", osc1PulseWidth, nullptr);
    params.setProperty("osc1Octave", osc1Octave, nullptr);
    params.setProperty("osc1Semitone", osc1Semitone, nullptr);
    params.setProperty("osc1FineTune", osc1FineTune, nullptr);
    params.setProperty("osc1RandomPhase", osc1RandomPhase, nullptr);
    params.setProperty("osc1VoiceCount", osc1VoiceCount, nullptr);
    params.setProperty("osc1Volume", osc1Volume, nullptr);
    
    // Oscillator 2 parameters
    params.setProperty("osc2Enabled", osc2Enabled, nullptr);
    params.setProperty("osc2Type", osc2Type, nullptr);
    params.setProperty("osc2Volume", osc2Volume, nullptr);
    params.setProperty("osc2Detune", osc2Detune, nullptr);
    params.setProperty("osc2Stereo", osc2Stereo, nullptr);
    params.setProperty("osc2Pan", osc2Pan, nullptr);
    params.setProperty("osc2Octave", osc2Octave, nullptr);
    params.setProperty("osc2Semitone", osc2Semitone, nullptr);
    params.setProperty("osc2FineTune", osc2FineTune, nullptr);
    params.setProperty("osc2RandomPhase", osc2RandomPhase, nullptr);
    params.setProperty("osc2Phase", osc2Phase, nullptr);
    params.setProperty("osc2Attack", osc2Attack, nullptr);
    params.setProperty("osc2Decay", osc2Decay, nullptr);
    params.setProperty("osc2Sustain", osc2Sustain, nullptr);
    params.setProperty("osc2Release", osc2Release, nullptr);
    params.setProperty("osc2VoiceCount", osc2VoiceCount, nullptr);
    
    // Filter parameters
    params.setProperty("filterCutoff", filterCutoff, nullptr);
    params.setProperty("filterResonance", filterResonance, nullptr);
    params.setProperty("osc1FilterEnabled", osc1FilterEnabled, nullptr);
    params.setProperty("osc2FilterEnabled", osc2FilterEnabled, nullptr);
    params.setProperty("filterLPEnabled", filterLPEnabled, nullptr);
    params.setProperty("filterHPEnabled", filterHPEnabled, nullptr);
    params.setProperty("filterBPEnabled", filterBPEnabled, nullptr);
    params.setProperty("filterNotchEnabled", filterNotchEnabled, nullptr);
    params.setProperty("filterCombEnabled", filterCombEnabled, nullptr);
    params.setProperty("filterFormantEnabled", filterFormantEnabled, nullptr);
    params.setProperty("filter12dBEnabled", filter12dBEnabled, nullptr);
    params.setProperty("filter24dBEnabled", filter24dBEnabled, nullptr);
    
    // Effects parameters - Chorus
    params.setProperty("chorusEnabled", chorusEnabled, nullptr);
    params.setProperty("chorusRate", chorusRate, nullptr);
    params.setProperty("chorusDelay1", chorusDelay1, nullptr);
    params.setProperty("chorusDelay2", chorusDelay2, nullptr);
    params.setProperty("chorusDepth", chorusDepth, nullptr);
    params.setProperty("chorusFeedback", chorusFeedback, nullptr);
    params.setProperty("chorusLPF", chorusLPF, nullptr);
    params.setProperty("chorusMix", chorusMix, nullptr);
    
    // Effects parameters - Flanger
    params.setProperty("flangerEnabled", flangerEnabled, nullptr);
    params.setProperty("flangerRate", flangerRate, nullptr);
    params.setProperty("flangerDepth", flangerDepth, nullptr);
    params.setProperty("flangerFeedback", flangerFeedback, nullptr);
    params.setProperty("flangerMix", flangerMix, nullptr);
    params.setProperty("flangerPhase", flangerPhase, nullptr);
    
    // Effects parameters - Phaser
    params.setProperty("phaserEnabled", phaserEnabled, nullptr);
    params.setProperty("phaserRate", phaserRate, nullptr);
    params.setProperty("phaserDepth1", phaserDepth1, nullptr);
    params.setProperty("phaserDepth2", phaserDepth2, nullptr);
    params.setProperty("phaserFeedback", phaserFeedback, nullptr);
    params.setProperty("phaserMix", phaserMix, nullptr);
    params.setProperty("phaserPhase", phaserPhase, nullptr);
    params.setProperty("phaserFrequency", phaserFrequency, nullptr);
    params.setProperty("phaserPoles", phaserPoles, nullptr);
    
    // Effects parameters - Compressor
    params.setProperty("compressorEnabled", compressorEnabled, nullptr);
    params.setProperty("compressorThreshold", compressorThreshold, nullptr);
    params.setProperty("compressorRatio", compressorRatio, nullptr);
    params.setProperty("compressorAttack", compressorAttack, nullptr);
    params.setProperty("compressorRelease", compressorRelease, nullptr);
    params.setProperty("compressorGain", compressorGain, nullptr);
    params.setProperty("compressorMix", compressorMix, nullptr);
    params.setProperty("compressorMultiband", compressorMultiband, nullptr);
    
    // Effects parameters - Distortion
    params.setProperty("distortionEnabled", distortionEnabled, nullptr);
    params.setProperty("distortionType", distortionType, nullptr);
    params.setProperty("distortionDrive", distortionDrive, nullptr);
    params.setProperty("distortionMix", distortionMix, nullptr);
    params.setProperty("distortionFilterPosition", distortionFilterPosition, nullptr);
    params.setProperty("distortionFilterType", distortionFilterType, nullptr);
    params.setProperty("distortionFilterFreq", distortionFilterFreq, nullptr);
    params.setProperty("distortionFilterQ", distortionFilterQ, nullptr);
    
    // Effects parameters - Delay
    params.setProperty("delayEnabled", delayEnabled, nullptr);
    params.setProperty("delayFeedback", delayFeedback, nullptr);
    params.setProperty("delayMix", delayMix, nullptr);
    params.setProperty("delayPingPong", delayPingPong, nullptr);
    params.setProperty("delayLeftTime", delayLeftTime, nullptr);
    params.setProperty("delayRightTime", delayRightTime, nullptr);
    params.setProperty("delaySync", delaySync, nullptr);
    params.setProperty("delayTriplet", delayTriplet, nullptr);
    params.setProperty("delayDotted", delayDotted, nullptr);
    params.setProperty("delayRTriplet", delayRTriplet, nullptr);
    params.setProperty("delayRDotted", delayRDotted, nullptr);
    params.setProperty("delayFilterFreq", delayFilterFreq, nullptr);
    params.setProperty("delayFilterQ", delayFilterQ, nullptr);
    
    // Effects parameters - Reverb
    params.setProperty("reverbEnabled", reverbEnabled, nullptr);
    params.setProperty("reverbMix", reverbMix, nullptr);
    params.setProperty("reverbType", reverbType, nullptr);
    params.setProperty("reverbLowCut", reverbLowCut, nullptr);
    params.setProperty("reverbHighCut", reverbHighCut, nullptr);
    params.setProperty("reverbSize", reverbSize, nullptr);
    params.setProperty("reverbPreDelay", reverbPreDelay, nullptr);
    params.setProperty("reverbDamping", reverbDamping, nullptr);
    params.setProperty("reverbWidth", reverbWidth, nullptr);
    
    // Effects parameters - EQ
    params.setProperty("eqEnabled", eqEnabled, nullptr);
    params.setProperty("eq1Enabled", eq1Enabled, nullptr);
    params.setProperty("eq1Frequency", eq1Frequency, nullptr);
    params.setProperty("eq1Q", eq1Q, nullptr);
    params.setProperty("eq1Gain", eq1Gain, nullptr);
    params.setProperty("eq1Type", eq1Type, nullptr);
    params.setProperty("eq2Enabled", eq2Enabled, nullptr);
    params.setProperty("eq2Frequency", eq2Frequency, nullptr);
    params.setProperty("eq2Q", eq2Q, nullptr);
    params.setProperty("eq2Gain", eq2Gain, nullptr);
    params.setProperty("eq2Type", eq2Type, nullptr);
    
    preset.appendChild(params, nullptr);
    return preset;
}

bool SummonerXSerum2AudioProcessor::applyPresetData(const juce::ValueTree& presetData)
{
    if (!presetData.hasType("PRESET"))
        return false;
        
    auto params = presetData.getChildWithName("PARAMETERS");
    if (!params.isValid())
        return false;
    
    // Apply all parameters - using the property accessors to ensure proper updating
    // Support both new and old parameter names for backward compatibility
    // NOTE: masterVolume is intentionally preserved during preset loading
    // if (params.hasProperty("masterVolume")) setMasterVolume(params.getProperty("masterVolume"));
    // else if (params.hasProperty("osc1MainVolume")) setMasterVolume(params.getProperty("osc1MainVolume"));
    // else if (params.hasProperty("synthVolume")) setMasterVolume(params.getProperty("synthVolume"));
    
    if (params.hasProperty("osc1Detune")) setOsc1Detune(params.getProperty("osc1Detune"));
    else if (params.hasProperty("synthDetune")) setOsc1Detune(params.getProperty("synthDetune"));
    
    if (params.hasProperty("osc1StereoWidth")) setOsc1StereoWidth(params.getProperty("osc1StereoWidth"));
    else if (params.hasProperty("synthStereoWidth")) setOsc1StereoWidth(params.getProperty("synthStereoWidth"));
    
    if (params.hasProperty("osc1Pan")) setOsc1Pan(params.getProperty("osc1Pan"));
    else if (params.hasProperty("synthPan")) setOsc1Pan(params.getProperty("synthPan"));
    
    if (params.hasProperty("osc1Phase")) setOsc1Phase(params.getProperty("osc1Phase"));
    else if (params.hasProperty("synthPhase")) setOsc1Phase(params.getProperty("synthPhase"));
    
    if (params.hasProperty("osc1Attack")) setOsc1Attack(params.getProperty("osc1Attack"));
    else if (params.hasProperty("synthAttack")) setOsc1Attack(params.getProperty("synthAttack"));
    
    if (params.hasProperty("osc1Decay")) setOsc1Decay(params.getProperty("osc1Decay"));
    else if (params.hasProperty("synthDecay")) setOsc1Decay(params.getProperty("synthDecay"));
    
    if (params.hasProperty("osc1Sustain")) setOsc1Sustain(params.getProperty("osc1Sustain"));
    else if (params.hasProperty("synthSustain")) setOsc1Sustain(params.getProperty("synthSustain"));
    
    if (params.hasProperty("osc1Release")) setOsc1Release(params.getProperty("osc1Release"));
    else if (params.hasProperty("synthRelease")) setOsc1Release(params.getProperty("synthRelease"));
    
    // Oscillator 1 parameters
    if (params.hasProperty("osc1Type")) setOsc1Type(params.getProperty("osc1Type"));
    if (params.hasProperty("osc1PulseWidth")) setOsc1PulseWidth(params.getProperty("osc1PulseWidth"));
    if (params.hasProperty("osc1Octave")) setOsc1Octave(params.getProperty("osc1Octave"));
    if (params.hasProperty("osc1Semitone")) setOsc1Semitone(params.getProperty("osc1Semitone"));
    if (params.hasProperty("osc1FineTune")) setOsc1FineTune(params.getProperty("osc1FineTune"));
    if (params.hasProperty("osc1RandomPhase")) setOsc1RandomPhase(params.getProperty("osc1RandomPhase"));
    if (params.hasProperty("osc1VoiceCount")) setOsc1VoiceCount(params.getProperty("osc1VoiceCount"));
    if (params.hasProperty("osc1Volume")) setOsc1Volume(params.getProperty("osc1Volume"));
    
    // Oscillator 2 parameters  
    if (params.hasProperty("osc2Enabled")) setOsc2Enabled(params.getProperty("osc2Enabled"));
    if (params.hasProperty("osc2Type")) setOsc2Type(params.getProperty("osc2Type"));
    if (params.hasProperty("osc2Volume")) setOsc2Volume(params.getProperty("osc2Volume"));
    if (params.hasProperty("osc2Detune")) setOsc2Detune(params.getProperty("osc2Detune"));
    if (params.hasProperty("osc2Stereo")) setOsc2Stereo(params.getProperty("osc2Stereo"));
    if (params.hasProperty("osc2Pan")) setOsc2Pan(params.getProperty("osc2Pan"));
    if (params.hasProperty("osc2Octave")) setOsc2Octave(params.getProperty("osc2Octave"));
    if (params.hasProperty("osc2Semitone")) setOsc2Semitone(params.getProperty("osc2Semitone"));
    if (params.hasProperty("osc2FineTune")) setOsc2FineTune(params.getProperty("osc2FineTune"));
    if (params.hasProperty("osc2RandomPhase")) setOsc2RandomPhase(params.getProperty("osc2RandomPhase"));
    if (params.hasProperty("osc2Phase")) setOsc2Phase(params.getProperty("osc2Phase"));
    if (params.hasProperty("osc2Attack")) setOsc2Attack(params.getProperty("osc2Attack"));
    if (params.hasProperty("osc2Decay")) setOsc2Decay(params.getProperty("osc2Decay"));
    if (params.hasProperty("osc2Sustain")) setOsc2Sustain(params.getProperty("osc2Sustain"));
    if (params.hasProperty("osc2Release")) setOsc2Release(params.getProperty("osc2Release"));
    if (params.hasProperty("osc2VoiceCount")) setOsc2VoiceCount(params.getProperty("osc2VoiceCount"));
    
    // Filter parameters
    if (params.hasProperty("filterCutoff")) setFilterCutoff(params.getProperty("filterCutoff"));
    if (params.hasProperty("filterResonance")) setFilterResonance(params.getProperty("filterResonance"));
    if (params.hasProperty("osc1FilterEnabled")) setOsc1FilterEnabled(params.getProperty("osc1FilterEnabled"));
    if (params.hasProperty("osc2FilterEnabled")) setOsc2FilterEnabled(params.getProperty("osc2FilterEnabled"));
    if (params.hasProperty("filterLPEnabled")) setFilterLPEnabled(params.getProperty("filterLPEnabled"));
    if (params.hasProperty("filterHPEnabled")) setFilterHPEnabled(params.getProperty("filterHPEnabled"));
    if (params.hasProperty("filterBPEnabled")) setFilterBPEnabled(params.getProperty("filterBPEnabled"));
    if (params.hasProperty("filterNotchEnabled")) setFilterNotchEnabled(params.getProperty("filterNotchEnabled"));
    if (params.hasProperty("filterCombEnabled")) setFilterCombEnabled(params.getProperty("filterCombEnabled"));
    if (params.hasProperty("filterFormantEnabled")) setFilterFormantEnabled(params.getProperty("filterFormantEnabled"));
    if (params.hasProperty("filter12dBEnabled")) setFilter12dBEnabled(params.getProperty("filter12dBEnabled"));
    if (params.hasProperty("filter24dBEnabled")) setFilter24dBEnabled(params.getProperty("filter24dBEnabled"));
    
    // Effects parameters - Chorus
    if (params.hasProperty("chorusEnabled")) setChorusEnabled(params.getProperty("chorusEnabled"));
    if (params.hasProperty("chorusRate")) setChorusRate(params.getProperty("chorusRate"));
    if (params.hasProperty("chorusDelay1")) setChorusDelay1(params.getProperty("chorusDelay1"));
    if (params.hasProperty("chorusDelay2")) setChorusDelay2(params.getProperty("chorusDelay2"));
    if (params.hasProperty("chorusDepth")) setChorusDepth(params.getProperty("chorusDepth"));
    if (params.hasProperty("chorusFeedback")) setChorusFeedback(params.getProperty("chorusFeedback"));
    if (params.hasProperty("chorusLPF")) setChorusLPF(params.getProperty("chorusLPF"));
    if (params.hasProperty("chorusMix")) setChorusMix(params.getProperty("chorusMix"));
    
    // Effects parameters - Flanger
    if (params.hasProperty("flangerEnabled")) setFlangerEnabled(params.getProperty("flangerEnabled"));
    if (params.hasProperty("flangerRate")) setFlangerRate(params.getProperty("flangerRate"));
    if (params.hasProperty("flangerDepth")) setFlangerDepth(params.getProperty("flangerDepth"));
    if (params.hasProperty("flangerFeedback")) setFlangerFeedback(params.getProperty("flangerFeedback"));
    if (params.hasProperty("flangerMix")) setFlangerMix(params.getProperty("flangerMix"));
    if (params.hasProperty("flangerPhase")) setFlangerPhase(params.getProperty("flangerPhase"));
    
    // Effects parameters - Phaser
    if (params.hasProperty("phaserEnabled")) setPhaserEnabled(params.getProperty("phaserEnabled"));
    if (params.hasProperty("phaserRate")) setPhaserRate(params.getProperty("phaserRate"));
    if (params.hasProperty("phaserDepth1")) setPhaserDepth1(params.getProperty("phaserDepth1"));
    if (params.hasProperty("phaserDepth2")) setPhaserDepth2(params.getProperty("phaserDepth2"));
    if (params.hasProperty("phaserFeedback")) setPhaserFeedback(params.getProperty("phaserFeedback"));
    if (params.hasProperty("phaserMix")) setPhaserMix(params.getProperty("phaserMix"));
    if (params.hasProperty("phaserPhase")) setPhaserPhase(params.getProperty("phaserPhase"));
    if (params.hasProperty("phaserFrequency")) setPhaserFrequency(params.getProperty("phaserFrequency"));
    if (params.hasProperty("phaserPoles")) setPhaserPoles(params.getProperty("phaserPoles"));
    
    // Effects parameters - Compressor
    if (params.hasProperty("compressorEnabled")) setCompressorEnabled(params.getProperty("compressorEnabled"));
    if (params.hasProperty("compressorThreshold")) setCompressorThreshold(params.getProperty("compressorThreshold"));
    if (params.hasProperty("compressorRatio")) setCompressorRatio(params.getProperty("compressorRatio"));
    if (params.hasProperty("compressorAttack")) setCompressorAttack(params.getProperty("compressorAttack"));
    if (params.hasProperty("compressorRelease")) setCompressorRelease(params.getProperty("compressorRelease"));
    if (params.hasProperty("compressorGain")) setCompressorGain(params.getProperty("compressorGain"));
    if (params.hasProperty("compressorMix")) setCompressorMix(params.getProperty("compressorMix"));
    if (params.hasProperty("compressorMultiband")) setCompressorMultiband(params.getProperty("compressorMultiband"));
    
    // Effects parameters - Distortion
    if (params.hasProperty("distortionEnabled")) setDistortionEnabled(params.getProperty("distortionEnabled"));
    if (params.hasProperty("distortionType")) setDistortionType(params.getProperty("distortionType"));
    if (params.hasProperty("distortionDrive")) setDistortionDrive(params.getProperty("distortionDrive"));
    if (params.hasProperty("distortionMix")) setDistortionMix(params.getProperty("distortionMix"));
    if (params.hasProperty("distortionFilterPosition")) setDistortionFilterPosition(params.getProperty("distortionFilterPosition"));
    if (params.hasProperty("distortionFilterType")) setDistortionFilterType(params.getProperty("distortionFilterType"));
    if (params.hasProperty("distortionFilterFreq")) setDistortionFilterFreq(params.getProperty("distortionFilterFreq"));
    if (params.hasProperty("distortionFilterQ")) setDistortionFilterQ(params.getProperty("distortionFilterQ"));
    
    // Effects parameters - Delay
    if (params.hasProperty("delayEnabled")) setDelayEnabled(params.getProperty("delayEnabled"));
    if (params.hasProperty("delayFeedback")) setDelayFeedback(params.getProperty("delayFeedback"));
    if (params.hasProperty("delayMix")) setDelayMix(params.getProperty("delayMix"));
    if (params.hasProperty("delayPingPong")) setDelayPingPong(params.getProperty("delayPingPong"));
    if (params.hasProperty("delayLeftTime")) setDelayLeftTime(params.getProperty("delayLeftTime"));
    if (params.hasProperty("delayRightTime")) setDelayRightTime(params.getProperty("delayRightTime"));
    if (params.hasProperty("delaySync")) setDelaySync(params.getProperty("delaySync"));
    if (params.hasProperty("delayTriplet")) setDelayTriplet(params.getProperty("delayTriplet"));
    if (params.hasProperty("delayDotted")) setDelayDotted(params.getProperty("delayDotted"));
    if (params.hasProperty("delayRTriplet")) setDelayRTriplet(params.getProperty("delayRTriplet"));
    if (params.hasProperty("delayRDotted")) setDelayRDotted(params.getProperty("delayRDotted"));
    if (params.hasProperty("delayFilterFreq")) setDelayFilterFreq(params.getProperty("delayFilterFreq"));
    if (params.hasProperty("delayFilterQ")) setDelayFilterQ(params.getProperty("delayFilterQ"));
    
    // Effects parameters - Reverb
    if (params.hasProperty("reverbEnabled")) setReverbEnabled(params.getProperty("reverbEnabled"));
    if (params.hasProperty("reverbMix")) setReverbMix(params.getProperty("reverbMix"));
    if (params.hasProperty("reverbType")) setReverbType(params.getProperty("reverbType"));
    if (params.hasProperty("reverbLowCut")) setReverbLowCut(params.getProperty("reverbLowCut"));
    if (params.hasProperty("reverbHighCut")) setReverbHighCut(params.getProperty("reverbHighCut"));
    if (params.hasProperty("reverbSize")) setReverbSize(params.getProperty("reverbSize"));
    if (params.hasProperty("reverbPreDelay")) setReverbPreDelay(params.getProperty("reverbPreDelay"));
    if (params.hasProperty("reverbDamping")) setReverbDamping(params.getProperty("reverbDamping"));
    if (params.hasProperty("reverbWidth")) setReverbWidth(params.getProperty("reverbWidth"));
    
    // Effects parameters - EQ
    if (params.hasProperty("eqEnabled")) setEQEnabled(params.getProperty("eqEnabled"));
    if (params.hasProperty("eq1Enabled")) setEQ1Enabled(params.getProperty("eq1Enabled"));
    if (params.hasProperty("eq1Frequency")) setEQ1Frequency(params.getProperty("eq1Frequency"));
    if (params.hasProperty("eq1Q")) setEQ1Q(params.getProperty("eq1Q"));
    if (params.hasProperty("eq1Gain")) setEQ1Gain(params.getProperty("eq1Gain"));
    if (params.hasProperty("eq1Type")) setEQ1Type(params.getProperty("eq1Type"));
    if (params.hasProperty("eq2Enabled")) setEQ2Enabled(params.getProperty("eq2Enabled"));
    if (params.hasProperty("eq2Frequency")) setEQ2Frequency(params.getProperty("eq2Frequency"));
    if (params.hasProperty("eq2Q")) setEQ2Q(params.getProperty("eq2Q"));
    if (params.hasProperty("eq2Gain")) setEQ2Gain(params.getProperty("eq2Gain"));
    if (params.hasProperty("eq2Type")) setEQ2Type(params.getProperty("eq2Type"));
    
    // Trigger UI update
    updateHostDisplay();
    if (onPresetApplied)
        juce::MessageManager::callAsync([this]() { onPresetApplied(); });
    if (onPresetChanged)
        juce::MessageManager::callAsync([this]() { onPresetChanged(); });
        
    return true;
}

bool SummonerXSerum2AudioProcessor::savePreset(const juce::String& name, const juce::String& description)
{
    if (name.isEmpty())
        return false;
        
    currentPresetName = name;
    
    juce::ValueTree presetData = createPresetData();
    presetData.setProperty("description", description, nullptr);
    
    juce::File presetFile = presetDirectory.getChildFile(name + ".sxs2");
    
    std::unique_ptr<juce::XmlElement> xml(presetData.createXml());
    if (xml && xml->writeTo(presetFile))
    {
        refreshPresetList();
        updatePresetDisplay();
        return true;
    }
    
    return false;
}

bool SummonerXSerum2AudioProcessor::savePresetToFile(const juce::File& file, const juce::String& description)
{
    if (!file.hasFileExtension(".sxs2"))
        return false;
        
    currentPresetName = file.getFileNameWithoutExtension();
    
    juce::ValueTree presetData = createPresetData();
    presetData.setProperty("description", description, nullptr);
    
    // Ensure the directory exists
    file.getParentDirectory().createDirectory();
    
    std::unique_ptr<juce::XmlElement> xml(presetData.createXml());
    if (xml && xml->writeTo(file))
    {
        refreshPresetList();
        updatePresetDisplay();
        return true;
    }
    
    return false;
}

bool SummonerXSerum2AudioProcessor::loadPreset(const juce::String& presetPath)
{
    juce::File presetFile(presetPath);
    if (!presetFile.exists())
        return false;
        
    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(presetFile);
    if (!xml)
        return false;
        
    juce::ValueTree presetData = juce::ValueTree::fromXml(*xml);
    if (applyPresetData(presetData))
    {
        currentPresetName = presetData.getProperty("name", "Unnamed");
        refreshPresetList();
        updatePresetDisplay();
        return true;
    }
    
    return false;
}

bool SummonerXSerum2AudioProcessor::loadPresetByIndex(int index)
{
    if (index >= 0 && index < (int)availablePresets.size())
    {
        currentPresetIndex = index;
        return loadPreset(availablePresets[index].getFullPathName());
    }
    return false;
}

std::vector<juce::File> SummonerXSerum2AudioProcessor::getAvailablePresets()
{
    return availablePresets;
}

bool SummonerXSerum2AudioProcessor::nextPreset()
{
    if (hasNextPreset())
    {
        return loadPresetByIndex(currentPresetIndex + 1);
    }
    return false;
}

bool SummonerXSerum2AudioProcessor::previousPreset()
{
    if (hasPreviousPreset())
    {
        return loadPresetByIndex(currentPresetIndex - 1);
    }
    return false;
}

bool SummonerXSerum2AudioProcessor::initializeAllParameters()
{
    // Reset ALL parameters to their actual startup default values
    std::map<std::string, float> startupDefaults = {
        // Main Oscillator 1 Parameters
        {"masterVolume", 3.0f},
        {"osc1Detune", 0.0f},
        {"osc1StereoWidth", 0.5f},
        {"osc1Pan", 0.0f},
        {"osc1Phase", 0.0f},
        {"osc1Attack", 0.1f},
        {"osc1Decay", 0.2f},
        {"osc1Sustain", 0.7f},
        {"osc1Release", 0.3f},
        
        // Oscillator 1 Parameters
        {"osc1Type", 1.0f},  // Saw wave
        {"osc1PulseWidth", 0.5f},
        {"osc1Octave", 0.0f},
        {"osc1Semitone", 0.0f},
        {"osc1FineTune", 0.0f},
        {"osc1RandomPhase", 1.0f},  // true
        {"osc1VoiceCount", 1.0f},
        {"osc1Volume", 0.5f},
        
        // Oscillator 2 Parameters
        {"osc2Enabled", 1.0f},  // true
        {"osc2Type", 1.0f},  // Saw wave
        {"osc2Volume", 0.0f},  // starts silent
        {"osc2Detune", 0.0f},
        {"osc2Stereo", 0.5f},
        {"osc2Pan", 0.0f},
        {"osc2Octave", 0.0f},
        {"osc2Semitone", 0.0f},
        {"osc2FineTune", 0.0f},
        {"osc2RandomPhase", 1.0f},  // true
        {"osc2Phase", 0.0f},
        {"osc2Attack", 0.1f},
        {"osc2Decay", 0.2f},
        {"osc2Sustain", 0.7f},
        {"osc2Release", 0.3f},
        {"osc2VoiceCount", 1.0f},
        
        // Filter Parameters
        {"filterCutoff", 1000.0f},
        {"filterResonance", 0.0f},
        {"osc1FilterEnabled", 0.0f},  // false
        {"osc2FilterEnabled", 0.0f},  // false
        {"filterLPEnabled", 1.0f},  // true
        {"filterHPEnabled", 0.0f},
        {"filterBPEnabled", 0.0f},
        {"filterNotchEnabled", 0.0f},
        {"filterCombEnabled", 0.0f},
        {"filterFormantEnabled", 0.0f},
        {"filter12dBEnabled", 1.0f},  // true
        {"filter24dBEnabled", 0.0f}
    };
    
    // Apply startup defaults for all parameters
    for (const auto& param : parameterMap)
    {
        const std::string& paramName = param.first;
        const ParameterInfo& info = param.second;
        float defaultValue = 0.0f;
        
        // Use startup default if available
        if (startupDefaults.find(paramName) != startupDefaults.end())
        {
            defaultValue = startupDefaults[paramName];
        }
        else
        {
            // For parameters not explicitly set, use safe defaults
            if (paramName.find("Enabled") != std::string::npos)
                defaultValue = 0.0f; // Disable all effects by default
            else if (info.type == ParameterInfo::BOOL)
                defaultValue = 0.0f;
            else if (info.type == ParameterInfo::INT)
                defaultValue = info.minValue;
            else if (info.type == ParameterInfo::FLOAT)
                defaultValue = info.minValue;
        }
        
        // Clamp to valid range
        defaultValue = juce::jlimit(info.minValue, info.maxValue, defaultValue);
        
        // Apply the default value using the setter
        info.setter(defaultValue);
    }
    
    // Update preset name and trigger callbacks
    currentPresetName = "Init";
    updateHostDisplay();
    
    if (onPresetChanged)
        juce::MessageManager::callAsync([this]() { onPresetChanged(); });
    
    return true;
}

void SummonerXSerum2AudioProcessor::updatePresetDisplay()
{
    // This function can be called by UI components to update preset display
    // The actual UI update will be handled by the SynthesizerComponent
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SummonerXSerum2AudioProcessor();
}
