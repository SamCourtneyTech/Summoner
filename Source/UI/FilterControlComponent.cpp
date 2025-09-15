#include "FilterControlComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

FilterControlComponent::FilterControlComponent()
{
    // Initialize filter controls
    
    // Cutoff knob and label
    filterCutoffLabel.setText("CUTOFF", juce::dontSendNotification);
    filterCutoffLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    filterCutoffLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterCutoffLabel);
    
    filterCutoffKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffKnob.setRange(20.0, 20000.0, 1.0);
    filterCutoffKnob.setValue(1000.0);
    filterCutoffKnob.setSkewFactorFromMidPoint(1000.0);
    filterCutoffKnob.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterCutoffKnob.addListener(this);
    addAndMakeVisible(filterCutoffKnob);
    
    // Resonance knob and label
    filterResonanceKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterResonanceKnob.setRange(0.0, 1.0, 0.01);
    filterResonanceKnob.setValue(0.0);
    filterResonanceKnob.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterResonanceKnob.addListener(this);
    addAndMakeVisible(filterResonanceKnob);
    
    filterResonanceLabel.setText("RES", juce::dontSendNotification);
    filterResonanceLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    filterResonanceLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterResonanceLabel);
    
    // OSC filter enable buttons
    osc1FilterEnableButton.setButtonText("OSC 1");
    osc1FilterEnableButton.setClickingTogglesState(true);
    osc1FilterEnableButton.setToggleState(false, juce::dontSendNotification);
    osc1FilterEnableButton.addListener(this);
    addAndMakeVisible(osc1FilterEnableButton);
    
    osc2FilterEnableButton.setButtonText("OSC 2");
    osc2FilterEnableButton.setClickingTogglesState(true);
    osc2FilterEnableButton.setToggleState(false, juce::dontSendNotification);
    osc2FilterEnableButton.addListener(this);
    addAndMakeVisible(osc2FilterEnableButton);
    
    // Filter type buttons
    filterLPButton.setButtonText("LP");
    filterLPButton.setClickingTogglesState(true);
    filterLPButton.setToggleState(true, juce::dontSendNotification); // LP selected by default
    filterLPButton.addListener(this);
    addAndMakeVisible(filterLPButton);
    
    filterHPButton.setButtonText("HP");
    filterHPButton.setClickingTogglesState(true);
    filterHPButton.setToggleState(false, juce::dontSendNotification);
    filterHPButton.addListener(this);
    addAndMakeVisible(filterHPButton);
    
    filterBPButton.setButtonText("BP");
    filterBPButton.setClickingTogglesState(true);
    filterBPButton.setToggleState(false, juce::dontSendNotification);
    filterBPButton.addListener(this);
    addAndMakeVisible(filterBPButton);
    
    filterNotchButton.setButtonText("NTCH");
    filterNotchButton.setClickingTogglesState(true);
    filterNotchButton.setToggleState(false, juce::dontSendNotification);
    filterNotchButton.addListener(this);
    addAndMakeVisible(filterNotchButton);
    
    filterCombButton.setButtonText("COMB");
    filterCombButton.setClickingTogglesState(true);
    filterCombButton.setToggleState(false, juce::dontSendNotification);
    filterCombButton.addListener(this);
    addAndMakeVisible(filterCombButton);
    
    filterFormantButton.setButtonText("FMNT");
    filterFormantButton.setClickingTogglesState(true);
    filterFormantButton.setToggleState(false, juce::dontSendNotification);
    filterFormantButton.addListener(this);
    addAndMakeVisible(filterFormantButton);
    
    // Filter slope buttons
    filter12dBButton.setButtonText("12dB");
    filter12dBButton.setClickingTogglesState(true);
    filter12dBButton.setToggleState(true, juce::dontSendNotification); // 12dB selected by default
    filter12dBButton.addListener(this);
    addAndMakeVisible(filter12dBButton);
    
    filter24dBButton.setButtonText("24dB");
    filter24dBButton.setClickingTogglesState(true);
    filter24dBButton.setToggleState(false, juce::dontSendNotification);
    filter24dBButton.addListener(this);
    addAndMakeVisible(filter24dBButton);
}

void FilterControlComponent::paint(juce::Graphics& g)
{
    // Component background is handled by parent
}

void FilterControlComponent::resized()
{
    // Layout controls when component is resized
    layoutControls(getLocalBounds());
}

void FilterControlComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parentSynthesizer == nullptr) return;
    
    if (slider == &filterCutoffKnob)
    {
        parentSynthesizer->audioProcessor.setFilterCutoff(static_cast<float>(filterCutoffKnob.getValue()));
    }
    else if (slider == &filterResonanceKnob)
    {
        parentSynthesizer->audioProcessor.setFilterResonance(static_cast<float>(filterResonanceKnob.getValue()));
    }
}

void FilterControlComponent::buttonClicked(juce::Button* button)
{
    if (parentSynthesizer == nullptr) return;
    
    // Handle filter type buttons with radio behavior
    if (button == &filterLPButton)
    {
        if (filterLPButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterHPButton.setToggleState(false, juce::dontSendNotification);
            filterBPButton.setToggleState(false, juce::dontSendNotification);
            filterNotchButton.setToggleState(false, juce::dontSendNotification);
            filterCombButton.setToggleState(false, juce::dontSendNotification);
            filterFormantButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(true);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(false);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(false);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(false);
        }
    }
    else if (button == &filterHPButton)
    {
        if (filterHPButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterLPButton.setToggleState(false, juce::dontSendNotification);
            filterBPButton.setToggleState(false, juce::dontSendNotification);
            filterNotchButton.setToggleState(false, juce::dontSendNotification);
            filterCombButton.setToggleState(false, juce::dontSendNotification);
            filterFormantButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(true);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(false);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(false);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(false);
        }
    }
    else if (button == &filterBPButton)
    {
        if (filterBPButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterLPButton.setToggleState(false, juce::dontSendNotification);
            filterHPButton.setToggleState(false, juce::dontSendNotification);
            filterNotchButton.setToggleState(false, juce::dontSendNotification);
            filterCombButton.setToggleState(false, juce::dontSendNotification);
            filterFormantButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(true);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(false);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(false);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(false);
        }
    }
    else if (button == &filterNotchButton)
    {
        if (filterNotchButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterLPButton.setToggleState(false, juce::dontSendNotification);
            filterHPButton.setToggleState(false, juce::dontSendNotification);
            filterBPButton.setToggleState(false, juce::dontSendNotification);
            filterCombButton.setToggleState(false, juce::dontSendNotification);
            filterFormantButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(true);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(false);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(false);
        }
    }
    else if (button == &filterCombButton)
    {
        if (filterCombButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterLPButton.setToggleState(false, juce::dontSendNotification);
            filterHPButton.setToggleState(false, juce::dontSendNotification);
            filterBPButton.setToggleState(false, juce::dontSendNotification);
            filterNotchButton.setToggleState(false, juce::dontSendNotification);
            filterFormantButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(false);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(true);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(false);
        }
    }
    else if (button == &filterFormantButton)
    {
        if (filterFormantButton.getToggleState())
        {
            // Turn off other filter type buttons
            filterLPButton.setToggleState(false, juce::dontSendNotification);
            filterHPButton.setToggleState(false, juce::dontSendNotification);
            filterBPButton.setToggleState(false, juce::dontSendNotification);
            filterNotchButton.setToggleState(false, juce::dontSendNotification);
            filterCombButton.setToggleState(false, juce::dontSendNotification);
            
            parentSynthesizer->audioProcessor.setFilterLPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterHPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterBPEnabled(false);
            parentSynthesizer->audioProcessor.setFilterNotchEnabled(false);
            parentSynthesizer->audioProcessor.setFilterCombEnabled(false);
            parentSynthesizer->audioProcessor.setFilterFormantEnabled(true);
        }
    }
    // Handle filter slope buttons with radio behavior
    else if (button == &filter12dBButton)
    {
        if (filter12dBButton.getToggleState())
        {
            filter24dBButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setFilter12dBEnabled(true);
            parentSynthesizer->audioProcessor.setFilter24dBEnabled(false);
        }
    }
    else if (button == &filter24dBButton)
    {
        if (filter24dBButton.getToggleState())
        {
            filter12dBButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setFilter12dBEnabled(false);
            parentSynthesizer->audioProcessor.setFilter24dBEnabled(true);
        }
    }
    // Handle OSC filter enable buttons
    else if (button == &osc1FilterEnableButton)
    {
        parentSynthesizer->audioProcessor.setOsc1FilterEnabled(osc1FilterEnableButton.getToggleState());
    }
    else if (button == &osc2FilterEnableButton)
    {
        parentSynthesizer->audioProcessor.setOsc2FilterEnabled(osc2FilterEnableButton.getToggleState());
    }
}

void FilterControlComponent::syncWithDSPState()
{
    if (parentSynthesizer == nullptr) return;
    
    filterCutoffKnob.setValue(parentSynthesizer->audioProcessor.getFilterCutoff(), juce::dontSendNotification);
    filterResonanceKnob.setValue(parentSynthesizer->audioProcessor.getFilterResonance(), juce::dontSendNotification);
    
    // Sync filter type buttons
    filterLPButton.setToggleState(parentSynthesizer->audioProcessor.getFilterLPEnabled(), juce::dontSendNotification);
    filterHPButton.setToggleState(parentSynthesizer->audioProcessor.getFilterHPEnabled(), juce::dontSendNotification);
    filterBPButton.setToggleState(parentSynthesizer->audioProcessor.getFilterBPEnabled(), juce::dontSendNotification);
    filterNotchButton.setToggleState(parentSynthesizer->audioProcessor.getFilterNotchEnabled(), juce::dontSendNotification);
    filterCombButton.setToggleState(parentSynthesizer->audioProcessor.getFilterCombEnabled(), juce::dontSendNotification);
    filterFormantButton.setToggleState(parentSynthesizer->audioProcessor.getFilterFormantEnabled(), juce::dontSendNotification);
    
    // Sync filter slope buttons
    filter12dBButton.setToggleState(parentSynthesizer->audioProcessor.getFilter12dBEnabled(), juce::dontSendNotification);
    filter24dBButton.setToggleState(parentSynthesizer->audioProcessor.getFilter24dBEnabled(), juce::dontSendNotification);
    
    // Sync OSC filter enable buttons
    osc1FilterEnableButton.setToggleState(parentSynthesizer->audioProcessor.getOsc1FilterEnabled(), juce::dontSendNotification);
    osc2FilterEnableButton.setToggleState(parentSynthesizer->audioProcessor.getOsc2FilterEnabled(), juce::dontSendNotification);
}

void FilterControlComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    // Filter knobs
    filterCutoffKnob.setLookAndFeel(digitalKnobLAF);
    filterResonanceKnob.setLookAndFeel(digitalKnobLAF);
    
    // Filter labels
    filterCutoffLabel.setLookAndFeel(digitalKnobLAF);
    filterResonanceLabel.setLookAndFeel(digitalKnobLAF);
    
    // OSC filter enable buttons
    osc1FilterEnableButton.setLookAndFeel(digitalButtonLAF);
    osc2FilterEnableButton.setLookAndFeel(digitalButtonLAF);
    
    // Filter type buttons
    filterLPButton.setLookAndFeel(digitalButtonLAF);
    filterHPButton.setLookAndFeel(digitalButtonLAF);
    filterBPButton.setLookAndFeel(digitalButtonLAF);
    filterNotchButton.setLookAndFeel(digitalButtonLAF);
    filterCombButton.setLookAndFeel(digitalButtonLAF);
    filterFormantButton.setLookAndFeel(digitalButtonLAF);
    
    // Filter slope buttons
    filter12dBButton.setLookAndFeel(digitalButtonLAF);
    filter24dBButton.setLookAndFeel(digitalButtonLAF);
}

void FilterControlComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Use the exact original layout logic - this will be called with local bounds from resized()
    auto localBounds = getLocalBounds();
    
    // Single row of filter type and slope buttons in landscape format  
    auto filterButtonHeight = 25;
    auto filterButtonSpacing = 3; // Even more reduced spacing for narrower row
    
    // Calculate button widths and positions for landscape layout
    auto typeButtonWidth = 45;
    auto slopeButtonWidth = 50;
    auto totalButtonsWidth = (6 * typeButtonWidth) + (2 * slopeButtonWidth) + (7 * filterButtonSpacing);
    auto buttonsStartX = (localBounds.getWidth() - totalButtonsWidth) / 2;
    
    auto currentX = buttonsStartX;
    
    // Filter type buttons in single row: LP, HP, BP, NOTCH, COMB, FORMANT (lowered by 40px)
    auto filterButtonsY = 43;
    filterLPButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    filterHPButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    filterBPButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    filterNotchButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    filterCombButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    filterFormantButton.setBounds(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    // Filter slope buttons continue in same row
    filter12dBButton.setBounds(currentX, filterButtonsY, slopeButtonWidth, filterButtonHeight);
    currentX += slopeButtonWidth + filterButtonSpacing;
    
    filter24dBButton.setBounds(currentX, filterButtonsY, slopeButtonWidth, filterButtonHeight);
    
    // Knobs row below buttons - landscape layout with OSC enable buttons
    auto filterKnobRowY = filterButtonsY + filterButtonHeight - 2;
    auto filterKnobWidth = 80;
    auto filterKnobHeight = 60;
    auto filterKnobSpacing = 20;
    auto oscButtonWidth = 60;
    auto oscButtonHeight = 30;
    
    // Calculate total width for knobs and osc buttons layout
    auto filterTotalKnobsWidth = (2 * filterKnobWidth) + (2 * oscButtonWidth) + (3 * filterKnobSpacing);
    auto filterKnobsStartX = (localBounds.getWidth() - filterTotalKnobsWidth) / 2;
    
    currentX = filterKnobsStartX;
    
    // OSC1 Filter Enable button
    osc1FilterEnableButton.setBounds(currentX, filterKnobRowY + (filterKnobHeight - oscButtonHeight) / 2, oscButtonWidth, oscButtonHeight);
    currentX += oscButtonWidth + filterKnobSpacing;
    
    // Filter cutoff knob
    filterCutoffKnob.setBounds(currentX, filterKnobRowY, filterKnobWidth, filterKnobHeight);
    filterCutoffLabel.setBounds(currentX - 15, filterKnobRowY + 2, filterKnobWidth, 20);
    currentX += filterKnobWidth + filterKnobSpacing;
    
    // Filter Resonance knob
    filterResonanceKnob.setBounds(currentX, filterKnobRowY, filterKnobWidth, filterKnobHeight);
    filterResonanceLabel.setBounds(currentX - 21, filterKnobRowY + 2, filterKnobWidth, 20);
    currentX += filterKnobWidth + filterKnobSpacing;
    
    // OSC2 Filter Enable button
    osc2FilterEnableButton.setBounds(currentX, filterKnobRowY + (filterKnobHeight - oscButtonHeight) / 2, oscButtonWidth, oscButtonHeight);
}