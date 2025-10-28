#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel.h"

class SummonerXSerum2AudioProcessor;

class VolumeControlsComponent : public juce::Component, private juce::Slider::Listener
{
public:
    explicit VolumeControlsComponent(SummonerXSerum2AudioProcessor& processor,
                                     CustomKnobLookAndFeel* knobLookAndFeel,
                                     LEDLabelLookAndFeel* labelLookAndFeel);
    ~VolumeControlsComponent() override;

    void resized() override;
    void updateAllGuiControls();

    // Public accessors for macro system
    RestrictedHitSlider& getVolumeKnob() { return volumeControlsVolumeKnob; }
    RestrictedHitSlider& getDetuneKnob() { return volumeControlsDetuneKnob; }
    RestrictedHitSlider& getStereoWidthKnob() { return volumeControlsStereoWidthKnob; }
    RestrictedHitSlider& getPanKnob() { return volumeControlsPanKnob; }

private:
    void sliderValueChanged(juce::Slider* slider) override;

    SummonerXSerum2AudioProcessor& audioProcessor;

    // Look and feel (not owned, just references)
    CustomKnobLookAndFeel* customKnobLookAndFeel;
    LEDLabelLookAndFeel* ledLabelLookAndFeel;

    // Volume controls
    RestrictedHitSlider volumeControlsVolumeKnob;
    juce::Label volumeControlsVolumeLabel;
    RestrictedHitSlider volumeControlsDetuneKnob;
    juce::Label volumeControlsDetuneLabel;
    RestrictedHitSlider volumeControlsStereoWidthKnob;
    juce::Label volumeControlsStereoWidthLabel;
    RestrictedHitSlider volumeControlsPanKnob;
    juce::Label volumeControlsPanLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeControlsComponent)
};
