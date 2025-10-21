#include "MacroControlsComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

MacroControlsComponent::MacroControlsComponent(SynthesizerComponent& parent,
                                             SummonerXSerum2AudioProcessor& processor,
                                             juce::LookAndFeel* simpleKnobLF,
                                             juce::LookAndFeel* engravedLabelLF)
    : parentComponent(parent),
      audioProcessor(processor),
      simpleKnobLookAndFeel(simpleKnobLF),
      engravedLabelLookAndFeel(engravedLabelLF)
{
    initializeMacroControls();
}

MacroControlsComponent::~MacroControlsComponent()
{
    // Clean up look and feel references
    macro1Knob.setLookAndFeel(nullptr);
    macro1Label.setLookAndFeel(nullptr);
    macro2Knob.setLookAndFeel(nullptr);
    macro2Label.setLookAndFeel(nullptr);
    macro3Knob.setLookAndFeel(nullptr);
    macro3Label.setLookAndFeel(nullptr);
    macro4Knob.setLookAndFeel(nullptr);
    macro4Label.setLookAndFeel(nullptr);
    macro5Knob.setLookAndFeel(nullptr);
    macro5Label.setLookAndFeel(nullptr);
    macro6Knob.setLookAndFeel(nullptr);
    macro6Label.setLookAndFeel(nullptr);
    macro7Knob.setLookAndFeel(nullptr);
    macro7Label.setLookAndFeel(nullptr);
    macro8Knob.setLookAndFeel(nullptr);
    macro8Label.setLookAndFeel(nullptr);
}

void MacroControlsComponent::initializeMacroControls()
{
    // Macro 1
    macro1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro1Knob.setRange(0.0, 1.0, 0.01);
    macro1Knob.setValue(0.5);
    macro1Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro1Knob.addListener(this);
    addAndMakeVisible(macro1Knob);

    macro1Label.setText("MACRO 1", juce::dontSendNotification);
    macro1Label.setJustificationType(juce::Justification::centred);
    macro1Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro1Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro1Label);

    // Macro 2
    macro2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro2Knob.setRange(0.0, 1.0, 0.01);
    macro2Knob.setValue(0.5);
    macro2Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro2Knob.addListener(this);
    addAndMakeVisible(macro2Knob);

    macro2Label.setText("MACRO 2", juce::dontSendNotification);
    macro2Label.setJustificationType(juce::Justification::centred);
    macro2Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro2Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro2Label);

    // Macro 3
    macro3Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro3Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro3Knob.setRange(0.0, 1.0, 0.01);
    macro3Knob.setValue(0.5);
    macro3Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro3Knob.addListener(this);
    addAndMakeVisible(macro3Knob);

    macro3Label.setText("MACRO 3", juce::dontSendNotification);
    macro3Label.setJustificationType(juce::Justification::centred);
    macro3Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro3Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro3Label);

    // Macro 4
    macro4Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro4Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro4Knob.setRange(0.0, 1.0, 0.01);
    macro4Knob.setValue(0.5);
    macro4Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro4Knob.addListener(this);
    addAndMakeVisible(macro4Knob);

    macro4Label.setText("MACRO 4", juce::dontSendNotification);
    macro4Label.setJustificationType(juce::Justification::centred);
    macro4Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro4Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro4Label);

    // Macro 5
    macro5Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro5Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro5Knob.setRange(0.0, 1.0, 0.01);
    macro5Knob.setValue(0.5);
    macro5Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro5Knob.addListener(this);
    addAndMakeVisible(macro5Knob);

    macro5Label.setText("MACRO 5", juce::dontSendNotification);
    macro5Label.setJustificationType(juce::Justification::centred);
    macro5Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro5Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro5Label);

    // Macro 6
    macro6Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro6Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro6Knob.setRange(0.0, 1.0, 0.01);
    macro6Knob.setValue(0.5);
    macro6Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro6Knob.addListener(this);
    addAndMakeVisible(macro6Knob);

    macro6Label.setText("MACRO 6", juce::dontSendNotification);
    macro6Label.setJustificationType(juce::Justification::centred);
    macro6Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro6Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro6Label);

    // Macro 7
    macro7Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro7Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro7Knob.setRange(0.0, 1.0, 0.01);
    macro7Knob.setValue(0.5);
    macro7Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro7Knob.addListener(this);
    addAndMakeVisible(macro7Knob);

    macro7Label.setText("MACRO 7", juce::dontSendNotification);
    macro7Label.setJustificationType(juce::Justification::centred);
    macro7Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro7Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro7Label);

    // Macro 8
    macro8Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro8Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro8Knob.setRange(0.0, 1.0, 0.01);
    macro8Knob.setValue(0.5);
    macro8Knob.setLookAndFeel(simpleKnobLookAndFeel);
    macro8Knob.addListener(this);
    addAndMakeVisible(macro8Knob);

    macro8Label.setText("MACRO 8", juce::dontSendNotification);
    macro8Label.setJustificationType(juce::Justification::centred);
    macro8Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro8Label.setLookAndFeel(engravedLabelLookAndFeel);
    addAndMakeVisible(macro8Label);

    // Initialize draggable macro symbols
    macroSymbol1 = std::make_unique<DraggableMacroSymbol>(1, &parentComponent);
    macroSymbol2 = std::make_unique<DraggableMacroSymbol>(2, &parentComponent);
    macroSymbol3 = std::make_unique<DraggableMacroSymbol>(3, &parentComponent);
    macroSymbol4 = std::make_unique<DraggableMacroSymbol>(4, &parentComponent);
    macroSymbol5 = std::make_unique<DraggableMacroSymbol>(5, &parentComponent);
    macroSymbol6 = std::make_unique<DraggableMacroSymbol>(6, &parentComponent);
    macroSymbol7 = std::make_unique<DraggableMacroSymbol>(7, &parentComponent);
    macroSymbol8 = std::make_unique<DraggableMacroSymbol>(8, &parentComponent);

    addAndMakeVisible(*macroSymbol1);
    addAndMakeVisible(*macroSymbol2);
    addAndMakeVisible(*macroSymbol3);
    addAndMakeVisible(*macroSymbol4);
    addAndMakeVisible(*macroSymbol5);
    addAndMakeVisible(*macroSymbol6);
    addAndMakeVisible(*macroSymbol7);
    addAndMakeVisible(*macroSymbol8);
}

void MacroControlsComponent::paint(juce::Graphics& g)
{
    // Macro indicators are drawn by parent via drawMacroIndicators
}

void MacroControlsComponent::resized()
{
    auto bounds = getLocalBounds();

    // Macro controls layout - use same constants as parent
    const int macroKnobSize = 45;       // Match parent: 45px (10% smaller than original 50)
    const int macroLabelHeight = 15;    // Match parent: 15px
    const int macroKnobSpacing = 18;    // Match parent: 18px spacing
    const int macroRowSpacing = 10;     // Match parent: 10px row spacing

    // Start from (0, 0) since parent already positions this component
    // Small negative offset to shift rows left slightly
    const int macroStartX = -8;
    const int macroY = 0;

    // Top row (Macro 1, 2, 3, 4)
    auto topRowY = macroY;

    // Macro 1
    auto macro1Area = juce::Rectangle<int>(macroStartX, topRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro1LabelArea = juce::Rectangle<int>(macroStartX - 7, topRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro1Label.setBounds(macro1LabelArea);
    macro1Knob.setBounds(macro1Area.removeFromTop(macroKnobSize));

    // Macro 2
    auto macro2Area = juce::Rectangle<int>(macroStartX + macroKnobSize + macroKnobSpacing, topRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro2LabelArea = juce::Rectangle<int>(macroStartX + macroKnobSize + macroKnobSpacing - 7, topRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro2Label.setBounds(macro2LabelArea);
    macro2Knob.setBounds(macro2Area.removeFromTop(macroKnobSize));

    // Macro 3
    auto macro3Area = juce::Rectangle<int>(macroStartX + 2 * (macroKnobSize + macroKnobSpacing), topRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro3LabelArea = juce::Rectangle<int>(macroStartX + 2 * (macroKnobSize + macroKnobSpacing) - 7, topRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro3Label.setBounds(macro3LabelArea);
    macro3Knob.setBounds(macro3Area.removeFromTop(macroKnobSize));

    // Macro 4
    auto macro4Area = juce::Rectangle<int>(macroStartX + 3 * (macroKnobSize + macroKnobSpacing), topRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro4LabelArea = juce::Rectangle<int>(macroStartX + 3 * (macroKnobSize + macroKnobSpacing) - 7, topRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro4Label.setBounds(macro4LabelArea);
    macro4Knob.setBounds(macro4Area.removeFromTop(macroKnobSize));

    // Bottom row (Macro 5, 6, 7, 8)
    auto bottomRowY = topRowY + macroKnobSize + macroLabelHeight + macroRowSpacing - 20;

    // Macro 5
    auto macro5Area = juce::Rectangle<int>(macroStartX, bottomRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro5LabelArea = juce::Rectangle<int>(macroStartX - 7, bottomRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro5Label.setBounds(macro5LabelArea);
    macro5Knob.setBounds(macro5Area.removeFromTop(macroKnobSize));

    // Macro 6
    auto macro6Area = juce::Rectangle<int>(macroStartX + macroKnobSize + macroKnobSpacing, bottomRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro6LabelArea = juce::Rectangle<int>(macroStartX + macroKnobSize + macroKnobSpacing - 7, bottomRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro6Label.setBounds(macro6LabelArea);
    macro6Knob.setBounds(macro6Area.removeFromTop(macroKnobSize));

    // Macro 7
    auto macro7Area = juce::Rectangle<int>(macroStartX + 2 * (macroKnobSize + macroKnobSpacing), bottomRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro7LabelArea = juce::Rectangle<int>(macroStartX + 2 * (macroKnobSize + macroKnobSpacing) - 7, bottomRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro7Label.setBounds(macro7LabelArea);
    macro7Knob.setBounds(macro7Area.removeFromTop(macroKnobSize));

    // Macro 8
    auto macro8Area = juce::Rectangle<int>(macroStartX + 3 * (macroKnobSize + macroKnobSpacing), bottomRowY, macroKnobSize, macroKnobSize + macroLabelHeight);
    auto macro8LabelArea = juce::Rectangle<int>(macroStartX + 3 * (macroKnobSize + macroKnobSpacing) - 7, bottomRowY + macroKnobSize - 6, macroKnobSize + 15, macroLabelHeight);
    macro8Label.setBounds(macro8LabelArea);
    macro8Knob.setBounds(macro8Area.removeFromTop(macroKnobSize));

    // Position draggable macro symbols to the right of each macro knob
    auto symbolOffsetX = macroKnobSize + 8 - 8 - 10 - 3 - 3 - 6 + 3; // 8px to the right of each knob, then 8px left, then 10px more left, then 3px more left, then 3px more left, then 6px more left, then 3px right
    auto symbolOffsetY = (macroKnobSize - 35) / 2 + 4 + 5 - 1 - 1 - 1; // Center vertically with knob (updated for 35px symbol), then 4px down, then 5px more down, then 1px up, then 1px more up, then 1px more up

    // Top row symbols (Macro 1-4)
    macroSymbol1->setOriginalPosition({macroStartX + symbolOffsetX, topRowY + symbolOffsetY});
    macroSymbol2->setOriginalPosition({macroStartX + macroKnobSize + macroKnobSpacing + symbolOffsetX, topRowY + symbolOffsetY});
    macroSymbol3->setOriginalPosition({macroStartX + 2 * (macroKnobSize + macroKnobSpacing) + symbolOffsetX, topRowY + symbolOffsetY});
    macroSymbol4->setOriginalPosition({macroStartX + 3 * (macroKnobSize + macroKnobSpacing) + symbolOffsetX, topRowY + symbolOffsetY});

    // Bottom row symbols (Macro 5-8)
    macroSymbol5->setOriginalPosition({macroStartX + symbolOffsetX, bottomRowY + symbolOffsetY});
    macroSymbol6->setOriginalPosition({macroStartX + macroKnobSize + macroKnobSpacing + symbolOffsetX, bottomRowY + symbolOffsetY});
    macroSymbol7->setOriginalPosition({macroStartX + 2 * (macroKnobSize + macroKnobSpacing) + symbolOffsetX, bottomRowY + symbolOffsetY});
    macroSymbol8->setOriginalPosition({macroStartX + 3 * (macroKnobSize + macroKnobSpacing) + symbolOffsetX, bottomRowY + symbolOffsetY});
}

void MacroControlsComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &macro1Knob)
    {
        updateMacroMappings(1, macro1Knob.getValue());
    }
    else if (slider == &macro2Knob)
    {
        updateMacroMappings(2, macro2Knob.getValue());
    }
    else if (slider == &macro3Knob)
    {
        updateMacroMappings(3, macro3Knob.getValue());
    }
    else if (slider == &macro4Knob)
    {
        updateMacroMappings(4, macro4Knob.getValue());
    }
    else if (slider == &macro5Knob)
    {
        updateMacroMappings(5, macro5Knob.getValue());
    }
    else if (slider == &macro6Knob)
    {
        updateMacroMappings(6, macro6Knob.getValue());
    }
    else if (slider == &macro7Knob)
    {
        updateMacroMappings(7, macro7Knob.getValue());
    }
    else if (slider == &macro8Knob)
    {
        updateMacroMappings(8, macro8Knob.getValue());
    }
}

void MacroControlsComponent::createMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    if (!targetSlider) return;

    // Use the MacroSystem to create the mapping
    macroSystem.createMacroMapping(macroIndex, targetSlider);

    // Trigger repaint to show new indicator
    parentComponent.repaint();
}

void MacroControlsComponent::updateMacroMappings(int macroIndex, double macroValue)
{
    // Use MacroSystem to update mappings
    macroSystem.updateMacroMappings(macroIndex, macroValue);

    // Handle any additional parameter updates for specific sliders
    for (auto& mapping : macroSystem.getMacroMappings())
    {
        if (mapping.macroIndex == macroIndex && mapping.targetSlider)
        {
            // Calculate proportional offset from base value
            // When macro = 0.0, target stays at baseValue
            // When macro = 1.0, target goes to baseValue + (range * direction)

            // Use user-defined range instead of full slider range
            double userRange = mapping.userMaxRange - mapping.userMinRange;
            double offset = (macroValue - 0.5) * userRange; // -0.5 to +0.5 range for bidirectional control
            double newValue = mapping.baseValue + offset;

            // Clamp to user-defined range (which is within slider's valid range)
            newValue = juce::jlimit(mapping.userMinRange, mapping.userMaxRange, newValue);

            // Update the target slider value but don't send notification to avoid visual movement
            // We want the parameter to change but the knob to stay visually at its base position
            mapping.targetSlider->setValue(newValue, juce::dontSendNotification);

            // Manually trigger the parameter update to the audio processor
            triggerParameterUpdate(mapping.targetSlider, newValue);
        }
    }

    // Trigger visual update for the circular indicators
    parentComponent.repaint();
}

void MacroControlsComponent::removeMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    macroSystem.removeMacroMapping(macroIndex, targetSlider);
    parentComponent.repaint();
}

void MacroControlsComponent::drawMacroIndicators(juce::Graphics& g)
{
    // Use MacroSystem to draw macro indicators
    macroSystem.drawMacroIndicators(g);
}

double MacroControlsComponent::getMacroKnobValue(int macroIndex)
{
    switch (macroIndex) {
        case 1: return macro1Knob.getValue();
        case 2: return macro2Knob.getValue();
        case 3: return macro3Knob.getValue();
        case 4: return macro4Knob.getValue();
        case 5: return macro5Knob.getValue();
        case 6: return macro6Knob.getValue();
        case 7: return macro7Knob.getValue();
        case 8: return macro8Knob.getValue();
        default: return 0.5; // Default middle value
    }
}

void MacroControlsComponent::triggerParameterUpdate(juce::Slider* slider, double newValue)
{
    // Forward to parent component which has access to all sliders and their parameter mappings
    parentComponent.triggerParameterUpdate(slider, newValue);
}
