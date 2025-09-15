#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class FilterControlComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    FilterControlComponent();
    
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
    juce::Slider filterCutoffKnob;
    juce::Slider filterResonanceKnob;
    juce::TextButton osc1FilterEnableButton;
    juce::TextButton osc2FilterEnableButton;
    juce::TextButton filterLPButton;
    juce::TextButton filterHPButton;
    juce::TextButton filterBPButton;
    juce::TextButton filterNotchButton;
    juce::TextButton filterCombButton;
    juce::TextButton filterFormantButton;
    juce::TextButton filter12dBButton;
    juce::TextButton filter24dBButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Filter control labels
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterControlComponent)
};