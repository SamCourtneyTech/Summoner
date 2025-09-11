#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class DelayComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    DelayComponent();
    
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
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Delay effect controls
    juce::Slider delayFeedbackKnob;
    juce::Label delayFeedbackLabel;
    juce::TextButton delayBpmButton;
    juce::TextButton delayLinkButton;
    juce::Slider delayLeftTimeSlider;
    juce::Label delayLeftTimeLabel;
    juce::Slider delayRightTimeSlider;
    juce::Label delayRightTimeLabel;
    juce::TextButton delayLeftTripletButton;
    juce::TextButton delayLeftDotButton;
    juce::TextButton delayRightTripletButton;
    juce::TextButton delayRightDotButton;
    juce::Slider delayFilterFreqKnob;
    juce::Label delayFilterFreqLabel;
    juce::Slider delayFilterQKnob;
    juce::Label delayFilterQLabel;
    juce::TextButton delayNormalButton;
    juce::TextButton delayPingPongButton;
    juce::Slider delayMixKnob;
    juce::Label delayMixLabel;
    juce::TextButton delayPowerButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayComponent)
};