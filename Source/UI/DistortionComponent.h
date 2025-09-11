#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class DistortionComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    DistortionComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
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
    
    // Distortion type handling
    int distortionTypeValue = 1;
    bool isDraggingDistortionType = false;
    int dragStartDistortionType = 1;
    int dragStartY = 0;
    
    // Main distortion controls
    juce::Label distortionTypeLabel;
    juce::Label distortionTypeValueLabel;
    juce::Slider distortionDriveKnob;
    juce::Label distortionDriveLabel;
    juce::Slider distortionMixKnob;
    juce::Label distortionMixLabel;
    juce::TextButton distortionPowerButton;
    
    // Distortion filter controls
    juce::Label distortionFilterSectionLabel;
    juce::TextButton distortionFilterOffButton;
    juce::TextButton distortionFilterPreButton;
    juce::TextButton distortionFilterPostButton;
    juce::TextButton distortionFilterLPButton;
    juce::TextButton distortionFilterBPButton;
    juce::TextButton distortionFilterHPButton;
    juce::Slider distortionFilterFreqKnob;
    juce::Label distortionFilterFreqLabel;
    juce::Slider distortionFilterQKnob;
    juce::Label distortionFilterQLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionComponent)
};