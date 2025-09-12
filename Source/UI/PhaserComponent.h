#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class PhaserComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    PhaserComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener  
    void buttonClicked(juce::Button* button) override;
    
    // Mouse listener for phaser poles cycling
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
    juce::Slider phaserMixKnob;
    juce::Slider phaserRateKnob;
    juce::Slider phaserDepth1Knob;
    juce::Slider phaserDepth2Knob;
    juce::Slider phaserFrequencyKnob;
    juce::Slider phaserFeedbackKnob;
    juce::Slider phaserPhaseKnob;
    juce::TextButton phaserPowerButton;
    juce::TextButton phaserBpmButton;
    
private:
    SynthesizerComponent* parentSynthesizer = nullptr;
    
    // Phaser poles cycling
    int currentPhaserPoles = 4; // Default 4 poles, range 2-8
    void cyclePhaserPoles();
    
    // Phaser effect labels
    juce::Label phaserMixLabel;
    juce::Label phaserRateLabel;
    juce::Label phaserPolesValueLabel;
    juce::Label phaserPolesLabel;
    juce::Label phaserDepth1Label;
    juce::Label phaserDepth2Label;
    juce::Label phaserFrequencyLabel;
    juce::Label phaserFeedbackLabel;
    juce::Label phaserPhaseLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaserComponent)
};