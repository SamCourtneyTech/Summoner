#pragma once
#include <JuceHeader.h>
#include "UI/MacroSystem.h"
#include "UI/MacroControlsComponent.h"

class SummonerXSerum2AudioProcessor;

/**
 * MacroMappingManager handles all macro mapping-related functionality
 * that was previously in SynthesizerComponent.
 *
 * This includes:
 * - Creating macro mappings between macro knobs and target sliders
 * - Finding sliders at specific positions for drag-and-drop mapping
 * - Finding macro mappings at positions for arc editing
 * - Updating mapping ranges during arc dragging
 * - Triggering parameter updates for mapped sliders
 * - Updating all GUI controls from audio processor state
 */
class MacroMappingManager
{
public:
    /**
     * Constructor
     * @param macroControls Reference to the macro controls component
     * @param processor Reference to the audio processor
     */
    MacroMappingManager(MacroControlsComponent& macroControls,
                       SummonerXSerum2AudioProcessor& processor);

    ~MacroMappingManager();

    /**
     * Create a macro mapping between a macro knob and a target slider
     * @param macroIndex The macro index (1-8)
     * @param targetSlider The slider to map to
     */
    void createMacroMapping(int macroIndex, juce::Slider* targetSlider);

    /**
     * Find a slider at a specific position for drag-and-drop mapping
     * @param position The position to check
     * @param allSliders Vector of all available sliders to check
     * @return Pointer to the slider at the position, or nullptr if none found
     */
    juce::Slider* findSliderAt(juce::Point<int> position,
                               const std::vector<juce::Slider*>& allSliders);

    /**
     * Find a macro mapping at a specific position for arc editing
     * @param position The position to check
     * @return Pointer to the macro mapping at the position, or nullptr if none found
     */
    MacroMapping* findMacroMappingAtPosition(juce::Point<int> position);

    /**
     * Update the range of a macro mapping during arc dragging
     * @param mapping The mapping to update
     * @param dragPosition The current drag position
     */
    void updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition);

    /**
     * Trigger a parameter update for a slider without sending notifications
     * This is used when macro mappings update slider values
     * @param slider The slider to update
     * @param newValue The new value for the slider
     */
    void triggerParameterUpdate(juce::Slider* slider, double newValue);

    /**
     * Update all GUI controls from the audio processor state
     * This is called when loading presets or initializing the UI
     * @param allComponents Map of component names to component pointers for updating
     */
    void updateAllGuiControls(const std::map<juce::String, juce::Component*>& allComponents);

    /**
     * Set the parent component for repaint requests
     * @param parent The parent component (typically SynthesizerComponent)
     */
    void setParentComponent(juce::Component* parent) { parentComponent = parent; }

private:
    MacroControlsComponent& macroControls;
    SummonerXSerum2AudioProcessor& audioProcessor;
    juce::Component* parentComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroMappingManager)
};
