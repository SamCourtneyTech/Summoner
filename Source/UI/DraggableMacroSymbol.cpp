#include "DraggableMacroSymbol.h"
#include "../SynthesizerComponent.h"

DraggableMacroSymbol::DraggableMacroSymbol(int index, SynthesizerComponent* parent) 
    : macroIndex(index), parentComponent(parent)
{
    setSize(35, 35); // Even larger size for better visibility
}

void DraggableMacroSymbol::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Set color based on drag state
    if (isDragging)
        g.setColour(juce::Colour(0xff00ff00)); // Bright green when dragging
    else
        g.setColour(juce::Colour(0xff888888)); // Gray when idle
    
    // Draw the * symbol - even larger font for bigger symbol
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("*", bounds, juce::Justification::centred);
}

void DraggableMacroSymbol::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    dragOffset = event.getPosition();
    
    // Bring this component to the front so it stays visible over FX modules during drag
    toFront(true);
    
    repaint();
}

void DraggableMacroSymbol::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        auto newPosition = getPosition() + event.getPosition() - dragOffset;
        setTopLeftPosition(newPosition);
    }
}

void DraggableMacroSymbol::mouseUp(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        isDragging = false;
        repaint();
        
        // Check if we dropped on a slider/knob
        // Use more robust coordinate conversion that works with nested components
        auto globalPosition = getScreenPosition() + event.getPosition();
        auto relativePosition = parentComponent->getLocalPoint(nullptr, globalPosition);
        
        // Find if there's a slider at the drop position
        if (auto* targetSlider = parentComponent->findSliderAt(relativePosition))
        {
            // Create macro mapping
            parentComponent->createMacroMapping(macroIndex, targetSlider);
        }
        
        // Auto-return to original position after drop
        returnToOriginalPosition();
    }
}

void DraggableMacroSymbol::returnToOriginalPosition()
{
    setTopLeftPosition(originalPosition);
}

void DraggableMacroSymbol::setOriginalPosition(juce::Point<int> position)
{
    originalPosition = position;
    setTopLeftPosition(position);
}