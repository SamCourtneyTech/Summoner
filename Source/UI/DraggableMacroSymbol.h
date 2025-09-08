#pragma once
#include <JuceHeader.h>

// Forward declaration to avoid circular dependency
class SynthesizerComponent;

class DraggableMacroSymbol : public juce::Component
{
public:
    DraggableMacroSymbol(int macroIndex, SynthesizerComponent* parent);
    
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    void returnToOriginalPosition();
    void setOriginalPosition(juce::Point<int> position);
    
private:
    int macroIndex;
    juce::Point<int> originalPosition;
    juce::Point<int> dragOffset;
    bool isDragging = false;
    SynthesizerComponent* parentComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableMacroSymbol)
};