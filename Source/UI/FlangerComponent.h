#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class FlangerComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    FlangerComponent();
    
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
    void setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF);
    
    // Layout
    void layoutControls(juce::Rectangle<int> bounds);
    
    // Public access to controls for macro system
    juce::Slider flangerMixKnob;
    juce::Slider flangerRateKnob;
    juce::Slider flangerDepthKnob;
    juce::Slider flangerFeedbackKnob;
    juce::Slider flangerPhaseKnob;
    juce::TextButton flangerPowerButton;
    juce::TextButton flangerBpmButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Flanger effect labels
    juce::Label flangerMixLabel;
    juce::Label flangerRateLabel;
    juce::Label flangerDepthLabel;
    juce::Label flangerFeedbackLabel;
    juce::Label flangerPhaseLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerComponent)
};