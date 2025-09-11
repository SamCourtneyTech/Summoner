#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class ReverbComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    ReverbComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener  
    void buttonClicked(juce::Button* button) override;
    
    // Mouse listener for reverb type cycling
    void mouseDown(const juce::MouseEvent& event) override;
    
    // Parent communication
    void setParentSynthesizer(SynthesizerComponent* parent) { parentSynthesizer = parent; }
    
    // Parameter synchronization
    void syncWithDSPState();
    
    // Look and Feel setup
    void setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF);
    
    // Layout
    void layoutControls(juce::Rectangle<int> bounds);
    
    // Public access to controls for macro system
    juce::Slider reverbMixKnob;
    juce::Slider reverbLowCutKnob;
    juce::Slider reverbHighCutKnob;
    juce::Slider reverbSizeKnob;
    juce::Slider reverbPreDelayKnob;
    juce::Slider reverbDampKnob;
    juce::Slider reverbWidthKnob;
    juce::TextButton reverbPowerButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Reverb type cycling  
    int currentReverbType = 1; // 1=PLATE, 2=HALL, 3=VINTAGE, 4=ROOM, 5=AMBIENCE
    std::vector<juce::String> reverbTypes = {"PLATE", "HALL", "VINTAGE", "ROOM", "AMBIENCE"};
    void cycleReverbType();
    
    // Reverb effect labels
    juce::Label reverbMixLabel;
    juce::Label reverbTypeLabel;
    juce::Label reverbTypeValueLabel;
    juce::Label reverbLowCutLabel;
    juce::Label reverbHighCutLabel;
    juce::Label reverbSizeLabel;
    juce::Label reverbPreDelayLabel;
    juce::Label reverbDampLabel;
    juce::Label reverbWidthLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbComponent)
};