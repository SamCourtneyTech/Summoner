#pragma once
#include <JuceHeader.h>
#include "MacroSystem.h"
#include "DraggableMacroSymbol.h"

class SynthesizerComponent;
class SummonerXSerum2AudioProcessor;

class MacroControlsComponent : public juce::Component, private juce::Slider::Listener
{
public:
    explicit MacroControlsComponent(SynthesizerComponent& parent,
                                   SummonerXSerum2AudioProcessor& processor,
                                   juce::LookAndFeel* simpleKnobLF,
                                   juce::LookAndFeel* engravedLabelLF);
    ~MacroControlsComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

    // Public interface for parent component
    void createMacroMapping(int macroIndex, juce::Slider* targetSlider);
    void removeMacroMapping(int macroIndex, juce::Slider* targetSlider);
    void drawMacroIndicators(juce::Graphics& g);
    double getMacroKnobValue(int macroIndex);

    // Access to macro system for DraggableMacroSymbol
    MacroSystem& getMacroSystem() { return macroSystem; }

private:
    SynthesizerComponent& parentComponent;
    SummonerXSerum2AudioProcessor& audioProcessor;

    // Look and feel references (owned by parent)
    juce::LookAndFeel* simpleKnobLookAndFeel;
    juce::LookAndFeel* engravedLabelLookAndFeel;

    // Macro controls
    juce::Slider macro1Knob;
    juce::Label macro1Label;
    juce::Slider macro2Knob;
    juce::Label macro2Label;
    juce::Slider macro3Knob;
    juce::Label macro3Label;
    juce::Slider macro4Knob;
    juce::Label macro4Label;
    juce::Slider macro5Knob;
    juce::Label macro5Label;
    juce::Slider macro6Knob;
    juce::Label macro6Label;
    juce::Slider macro7Knob;
    juce::Label macro7Label;
    juce::Slider macro8Knob;
    juce::Label macro8Label;

    // Draggable macro mapping symbols
    std::unique_ptr<DraggableMacroSymbol> macroSymbol1;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol2;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol3;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol4;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol5;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol6;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol7;
    std::unique_ptr<DraggableMacroSymbol> macroSymbol8;

    // Macro mapping system
    MacroSystem macroSystem;

    // Helper methods
    void initializeMacroControls();
    void updateMacroMappings(int macroIndex, double macroValue);
    void triggerParameterUpdate(juce::Slider* slider, double newValue);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroControlsComponent)
};
