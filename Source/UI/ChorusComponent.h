#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class ChorusComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    ChorusComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener  
    void buttonClicked(juce::Button* button) override;
    
    // Parent communication
    void setParentSynthesizer(SynthesizerComponent* parent) { parentSynthesizer = parent; }
    
    // Parameter synchronization
    void syncWithDSPState();
    
    // Look and Feel setup
    void setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF);
    
    // Layout
    void layoutControls(juce::Rectangle<int> bounds);
    
    // Public access to controls for macro system
    juce::Slider chorusRateKnob;
    juce::Slider chorusDelay1Knob;
    juce::Slider chorusDelay2Knob;
    juce::Slider chorusDepthKnob;
    juce::Slider chorusFeedKnob;
    juce::Slider chorusLpfKnob;
    juce::Slider chorusMixKnob;
    juce::TextButton chorusPowerButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Chorus effect labels
    juce::Label chorusRateLabel;
    juce::Label chorusDelay1Label;
    juce::Label chorusDelay2Label;
    juce::Label chorusDepthLabel;
    juce::Label chorusFeedLabel;
    juce::Label chorusLpfLabel;
    juce::Label chorusMixLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusComponent)
};