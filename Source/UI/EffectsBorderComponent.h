#pragma once
#include <JuceHeader.h>

class EffectsBorderComponent : public juce::Component
{
public:
    EffectsBorderComponent();
    void paint(juce::Graphics& g) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsBorderComponent)
};
