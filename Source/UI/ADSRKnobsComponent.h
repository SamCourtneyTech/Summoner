#pragma once
#include <JuceHeader.h>
#include "../LookAndFeel.h"

class SummonerXSerum2AudioProcessor;

class ADSRKnobsComponent : public juce::Component, private juce::Slider::Listener
{
public:
    explicit ADSRKnobsComponent(SummonerXSerum2AudioProcessor& processor);
    ~ADSRKnobsComponent() override;

    void resized() override;

    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;

    // Getters for the knobs (needed for external access)
    RestrictedHitSlider& getAttackKnob() { return adsrAttackKnob; }
    RestrictedHitSlider& getDecayKnob() { return adsrDecayKnob; }
    RestrictedHitSlider& getSustainKnob() { return adsrSustainKnob; }
    RestrictedHitSlider& getReleaseKnob() { return adsrReleaseKnob; }

    // Method to update envelope display (callback to parent)
    std::function<void()> onEnvelopeChanged;

private:
    SummonerXSerum2AudioProcessor& audioProcessor;

    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;

    // LED label look and feel
    LEDLabelLookAndFeel ledLabelLookAndFeel;

    // ADSR KNOBS
    RestrictedHitSlider adsrAttackKnob;
    juce::Label adsrAttackLabel;
    RestrictedHitSlider adsrDecayKnob;
    juce::Label adsrDecayLabel;
    RestrictedHitSlider adsrSustainKnob;
    juce::Label adsrSustainLabel;
    RestrictedHitSlider adsrReleaseKnob;
    juce::Label adsrReleaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRKnobsComponent)
};
