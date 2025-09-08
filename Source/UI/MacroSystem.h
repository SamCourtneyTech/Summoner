#pragma once
#include <JuceHeader.h>

// Macro mapping system
struct MacroMapping
{
    int macroIndex;                    // Which macro (1-8)
    juce::Slider* targetSlider;        // The linked slider/knob
    double baseValue;                  // Value when link was created
    double minRange;                   // Target slider's minimum value
    double maxRange;                   // Target slider's maximum value
    double userMinRange;               // User-adjustable minimum range (for arc sizing)
    double userMaxRange;               // User-adjustable maximum range (for arc sizing)
    juce::Colour indicatorColor;       // Color for the visual indicator
    
    MacroMapping(int macro, juce::Slider* slider, double base, double min, double max)
        : macroIndex(macro), targetSlider(slider), baseValue(base), minRange(min), maxRange(max)
    {
        // Initialize user ranges to full slider range (can be adjusted by dragging arc)
        userMinRange = min;
        userMaxRange = max;
        // Assign different colors for each macro
        const juce::Colour macroColors[] = {
            juce::Colour(0xff00ff00), // Green
            juce::Colour(0xff0080ff), // Blue
            juce::Colour(0xffff8000), // Orange
            juce::Colour(0xffff00ff), // Magenta
            juce::Colour(0xff00ffff), // Cyan
            juce::Colour(0xffffff00), // Yellow
            juce::Colour(0xffff0080), // Pink
            juce::Colour(0xff8000ff)  // Purple
        };
        indicatorColor = macroColors[(macro - 1) % 8];
    }
};

class MacroSystem
{
public:
    MacroSystem();
    
    // Macro management
    void createMacroMapping(int macroIndex, juce::Slider* targetSlider);
    void updateMacroMappings(int macroIndex, double macroValue);
    void removeMacroMapping(int macroIndex, juce::Slider* targetSlider);
    
    // Utility functions
    juce::Slider* findSliderAt(juce::Point<int> position, const std::vector<juce::Slider*>& allSliders);
    MacroMapping* findMacroMappingAtPosition(juce::Point<int> position);
    void updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition);
    
    // Drawing functions
    void drawMacroIndicators(juce::Graphics& g);
    void drawCircularIndicator(juce::Graphics& g, juce::Slider* slider, const MacroMapping& mapping);
    
    // Data access
    std::vector<MacroMapping>& getMacroMappings() { return macroMappings; }
    const std::vector<MacroMapping>& getMacroMappings() const { return macroMappings; }
    
private:
    std::vector<MacroMapping> macroMappings;
    
    // Helper functions for arc drawing
    juce::Point<float> getSliderKnobCenter(juce::Slider* slider);
    float getKnobRadius(juce::Slider* slider);
};