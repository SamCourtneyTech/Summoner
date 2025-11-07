#include "MacroMappingManager.h"
#include "SynthesizerComponent.h"
#include "PluginProcessor.h"
#include "UI/VolumeControlsComponent.h"
#include "UI/PitchControlsComponent.h"
#include "UI/ADSRKnobsComponent.h"
#include "UI/WaveTypeSelectorComponent.h"
#include "UI/SecondOscillatorComponent.h"
#include "UI/FilterControlComponent.h"
#include "UI/EQControlsComponent.h"
#include "UI/ParametricEQComponent.h"
#include "UI/ChorusComponent.h"
#include "UI/CompressorComponent.h"
#include "UI/DelayComponent.h"
#include "UI/FlangerComponent.h"
#include "UI/PhaserComponent.h"
#include "UI/ReverbComponent.h"
#include "UI/PresetManagementComponent.h"
#include "UI/ADSREnvelopeComponent.h"

MacroMappingManager::MacroMappingManager(MacroControlsComponent& macroControls,
                                         SummonerXSerum2AudioProcessor& processor)
    : macroControls(macroControls)
    , audioProcessor(processor)
{
}

MacroMappingManager::~MacroMappingManager()
{
}

void MacroMappingManager::createMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    macroControls.createMacroMapping(macroIndex, targetSlider);
}

juce::Slider* MacroMappingManager::findSliderAt(juce::Point<int> position,
                                                const std::vector<juce::Slider*>& allSliders)
{
    // Use MacroSystem to find slider at position with coordinate conversion
    return macroControls.getMacroSystem().findSliderAt(position, allSliders);
}

MacroMapping* MacroMappingManager::findMacroMappingAtPosition(juce::Point<int> position)
{
    return macroControls.getMacroSystem().findMacroMappingAtPosition(position);
}

void MacroMappingManager::updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition)
{
    macroControls.getMacroSystem().updateMappingRange(mapping, dragPosition);
    if (parentComponent != nullptr)
    {
        parentComponent->repaint();
    }
}

void MacroMappingManager::triggerParameterUpdate(juce::Slider* slider, double newValue)
{
    // This method needs access to all the component references from SynthesizerComponent
    // We'll need to cast parentComponent to SynthesizerComponent to access them
    if (parentComponent == nullptr)
        return;

    auto* synthComponent = dynamic_cast<SynthesizerComponent*>(parentComponent);
    if (synthComponent == nullptr)
        return;

    // Mirror the logic from sliderValueChanged to update audio processor parameters
    // without sending slider notifications (to avoid visual movement)

    // Volume and oscillator controls
    if (slider == &synthComponent->volumeControls.getVolumeKnob())
    {
        audioProcessor.setOsc1Volume(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->volumeControls.getDetuneKnob())
    {
        audioProcessor.setOsc1Detune(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->volumeControls.getStereoWidthKnob())
    {
        audioProcessor.setOsc1StereoWidth(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->volumeControls.getPanKnob())
    {
        audioProcessor.setOsc1Pan(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->phaseControlsPhaseKnob)
    {
        audioProcessor.setOsc1Phase(static_cast<float>(newValue));
    }
    // ADSR controls
    else if (slider == &synthComponent->adsrKnobs.getAttackKnob())
    {
        audioProcessor.setOsc1Attack(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (synthComponent->secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Attack(static_cast<float>(newValue));
        }

        synthComponent->updateEnvelopeDisplay();
    }
    else if (slider == &synthComponent->adsrKnobs.getDecayKnob())
    {
        audioProcessor.setOsc1Decay(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (synthComponent->secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Decay(static_cast<float>(newValue));
        }

        synthComponent->updateEnvelopeDisplay();
    }
    else if (slider == &synthComponent->adsrKnobs.getSustainKnob())
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (synthComponent->secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Sustain(static_cast<float>(newValue));
        }

        synthComponent->updateEnvelopeDisplay();
    }
    else if (slider == &synthComponent->adsrKnobs.getReleaseKnob())
    {
        audioProcessor.setOsc1Release(static_cast<float>(newValue));

        // If ADSR is linked, also update oscillator 2 via secondOscillator component
        if (synthComponent->secondOscillator.getAdsrLinkButton().getToggleState())
        {
            audioProcessor.setOsc2Release(static_cast<float>(newValue));
        }

        synthComponent->updateEnvelopeDisplay();
    }
    // EQ parameters - now handled by EQControlsComponent
    else if (slider == &synthComponent->eqControls.getEQ1FreqKnob())
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ1QKnob())
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ1GainKnob())
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2FreqKnob())
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2QKnob())
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2GainKnob())
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    // New EQ knobs
    else if (slider == &synthComponent->eqControls.getEQ1NewFreqKnob())
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ1NewQKnob())
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ1NewGainKnob())
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2NewFreqKnob())
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2NewQKnob())
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    else if (slider == &synthComponent->eqControls.getEQ2NewGainKnob())
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        synthComponent->parametricEQ.syncWithDSPState();
    }
    // Chorus FX - handled by ChorusComponent
    else if (slider == &synthComponent->chorusModule.chorusRateKnob)
    {
        audioProcessor.setChorusRate(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->chorusModule.chorusDelay1Knob)
    {
        audioProcessor.setChorusDelay1(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->chorusModule.chorusDelay2Knob)
    {
        audioProcessor.setChorusDelay2(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->chorusModule.chorusDepthKnob)
    {
        audioProcessor.setChorusDepth(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->chorusModule.chorusFeedKnob)
    {
        audioProcessor.setChorusFeedback(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->chorusModule.chorusLpfKnob)
    {
        float filterFreq = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 20.0f, 20000.0f);
        audioProcessor.setChorusLPF(filterFreq);
    }
    else if (slider == &synthComponent->chorusModule.chorusMixKnob)
    {
        audioProcessor.setChorusMix(static_cast<float>(newValue));
    }
    // Compressor FX - handled by CompressorComponent
    else if (slider == &synthComponent->compressorModule.compressorThresholdKnob)
    {
        audioProcessor.setCompressorThreshold(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->compressorModule.compressorRatioKnob)
    {
        float ratio = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 20.0f);
        audioProcessor.setCompressorRatio(ratio);
    }
    else if (slider == &synthComponent->compressorModule.compressorAttackKnob)
    {
        float attack = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 0.1f, 100.0f);
        audioProcessor.setCompressorAttack(attack);
    }
    else if (slider == &synthComponent->compressorModule.compressorReleaseKnob)
    {
        float release = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 1000.0f);
        audioProcessor.setCompressorRelease(release);
    }
    else if (slider == &synthComponent->compressorModule.compressorGainKnob)
    {
        audioProcessor.setCompressorGain(static_cast<float>(newValue));
    }
    else if (slider == &synthComponent->compressorModule.compressorMixKnob)
    {
        audioProcessor.setCompressorMix(static_cast<float>(newValue));
    }
    // Filter controls, Flanger, Phaser, Delay, Reverb, and Oscillator 2 controls
    // are handled by their respective components
}

void MacroMappingManager::updateAllGuiControls(const std::map<juce::String, juce::Component*>& allComponents)
{
    // This method needs access to all the component references from SynthesizerComponent
    if (parentComponent == nullptr)
        return;

    auto* synthComponent = dynamic_cast<SynthesizerComponent*>(parentComponent);
    if (synthComponent == nullptr)
        return;

    // Main synthesizer controls (using correct control names)
    synthComponent->volumeControls.updateAllGuiControls();
    synthComponent->pitchControls.updateAllGuiControls();
    synthComponent->phaseControlsPhaseKnob.setValue(audioProcessor.getOsc1Phase(), juce::dontSendNotification);

    // Main ADSR envelope
    synthComponent->adsrKnobs.getAttackKnob().setValue(audioProcessor.getOsc1Attack(), juce::dontSendNotification);
    synthComponent->adsrKnobs.getDecayKnob().setValue(audioProcessor.getOsc1Decay(), juce::dontSendNotification);
    synthComponent->adsrKnobs.getSustainKnob().setValue(audioProcessor.getOsc1Sustain(), juce::dontSendNotification);
    synthComponent->adsrKnobs.getReleaseKnob().setValue(audioProcessor.getOsc1Release(), juce::dontSendNotification);

    // Oscillator 1 controls
    synthComponent->pulseWidthSlider.setValue(audioProcessor.getOsc1PulseWidth(), juce::dontSendNotification);

    // Update oscillator 1 wave type buttons
    int osc1Type = audioProcessor.getOsc1Type();
    synthComponent->waveTypeSelector.getSineButton().setToggleState(osc1Type == 0, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getSawButton().setToggleState(osc1Type == 1, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getSquareButton().setToggleState(osc1Type == 2, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getTriangleButton().setToggleState(osc1Type == 3, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getWhiteNoiseButton().setToggleState(osc1Type == 4, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getPinkNoiseButton().setToggleState(osc1Type == 5, juce::dontSendNotification);
    synthComponent->waveTypeSelector.getRandomPhaseButton().setToggleState(osc1Type == 6, juce::dontSendNotification);

    // Oscillator 2 controls now handled by SecondOscillatorComponent
    synthComponent->secondOscillator.updateAllGuiControls();

    // Filter controls - handled by FilterControlComponent
    synthComponent->filterModule.syncWithDSPState();

    // Effects controls - update the knobs with loaded preset values

    // Chorus controls - handled by ChorusComponent
    synthComponent->chorusModule.syncWithDSPState();

    // Compressor controls - handled by CompressorComponent
    synthComponent->compressorModule.syncWithDSPState();

    // Delay controls - handled by DelayComponent
    synthComponent->delayModule.syncWithDSPState();

    // EQ controls - now handled by EQControlsComponent
    synthComponent->eqControls.syncWithDSPState();

    // Flanger controls - now handled by FlangerComponent
    synthComponent->flangerModule.syncWithDSPState();

    // Phaser controls - now handled by PhaserComponent
    synthComponent->phaserModule.syncWithDSPState();

    // Reverb controls - now handled by ReverbComponent
    synthComponent->reverbModule.syncWithDSPState();

    // Update envelope display
    synthComponent->updateEnvelopeDisplay();

    // Update preset display - now handled by PresetManagementComponent
    synthComponent->presetManagement.updatePresetDisplay();

    // Trigger a repaint to update the visuals
    if (parentComponent != nullptr)
    {
        parentComponent->repaint();
    }
}
