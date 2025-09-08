#pragma once
#include <JuceHeader.h>

class ADSREnvelopeComponent : public juce::Component
{
public:
    ADSREnvelopeComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void updateEnvelope(float attack, float decay, float sustain, float release);
    
private:
    float attackTime = 0.1f;
    float decayTime = 0.2f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.3f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSREnvelopeComponent)
};