#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class CompressorComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    CompressorComponent();
    
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
    juce::Slider compressorThresholdKnob;
    juce::Slider compressorRatioKnob;
    juce::Slider compressorAttackKnob;
    juce::Slider compressorReleaseKnob;
    juce::Slider compressorGainKnob;
    juce::Slider compressorMixKnob;
    juce::TextButton compressorPowerButton;
    juce::TextButton compressorMultibandButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Compressor effect labels
    juce::Label compressorThresholdLabel;
    juce::Label compressorRatioLabel;
    juce::Label compressorAttackLabel;
    juce::Label compressorReleaseLabel;
    juce::Label compressorGainLabel;
    juce::Label compressorMixLabel;
    
    // Compressor value displays
    juce::Label compressorRatioValueLabel;
    juce::Label compressorAttackValueLabel;
    juce::Label compressorReleaseValueLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
};