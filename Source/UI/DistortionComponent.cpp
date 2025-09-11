#include "DistortionComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

DistortionComponent::DistortionComponent()
{
    // Type label
    distortionTypeLabel.setText("TYPE", juce::dontSendNotification);
    distortionTypeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionTypeLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionTypeLabel);
    
    // Type value label (interactive)
    distortionTypeValueLabel.setText("TUBE", juce::dontSendNotification);
    distortionTypeValueLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    distortionTypeValueLabel.setInterceptsMouseClicks(true, true); // Enable mouse interaction for self and children
    distortionTypeValueLabel.setEditable(false, false, false); // Make it respond to mouse but not editable
    distortionTypeValueLabel.addMouseListener(this, true); // Add mouse listener with childEvents = true
    addAndMakeVisible(distortionTypeValueLabel);
    
    // Drive knob
    distortionDriveKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionDriveKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionDriveKnob.setRange(0.0, 100.0, 0.1);
    distortionDriveKnob.setValue(50.0);
    // Note: LookAndFeel will be set by parent
    distortionDriveKnob.addListener(this);
    addAndMakeVisible(distortionDriveKnob);
    
    distortionDriveLabel.setText("DRIVE", juce::dontSendNotification);
    distortionDriveLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionDriveLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionDriveLabel);
    
    // Mix knob
    distortionMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionMixKnob.setRange(0.0, 1.0, 0.01);
    distortionMixKnob.setValue(1.0);
    // Note: LookAndFeel will be set by parent
    distortionMixKnob.addListener(this);
    addAndMakeVisible(distortionMixKnob);
    
    distortionMixLabel.setText("MIX", juce::dontSendNotification);
    distortionMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionMixLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionMixLabel);
    
    // Power button
    distortionPowerButton.setButtonText("DIST ON");
    distortionPowerButton.setClickingTogglesState(true);
    distortionPowerButton.setToggleState(false, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionPowerButton.addListener(this);
    addAndMakeVisible(distortionPowerButton);
    
    // Filter section label
    distortionFilterSectionLabel.setText("FILTER", juce::dontSendNotification);
    distortionFilterSectionLabel.setFont(juce::Font("Times New Roman", 10.0f, juce::Font::bold));
    distortionFilterSectionLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionFilterSectionLabel);
    
    // DISTORTION FILTER CONTROLS
    // Filter mode buttons (Off/Pre/Post)
    distortionFilterOffButton.setButtonText("OFF");
    distortionFilterOffButton.setClickingTogglesState(true);
    distortionFilterOffButton.setToggleState(true, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterOffButton.addListener(this);
    addAndMakeVisible(distortionFilterOffButton);
    
    distortionFilterPreButton.setButtonText("PRE");
    distortionFilterPreButton.setClickingTogglesState(true);
    distortionFilterPreButton.setToggleState(false, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterPreButton.addListener(this);
    addAndMakeVisible(distortionFilterPreButton);
    
    distortionFilterPostButton.setButtonText("POST");
    distortionFilterPostButton.setClickingTogglesState(true);
    distortionFilterPostButton.setToggleState(false, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterPostButton.addListener(this);
    addAndMakeVisible(distortionFilterPostButton);
    
    // Filter type buttons (LP/BP/HP)
    distortionFilterLPButton.setButtonText("LP");
    distortionFilterLPButton.setClickingTogglesState(true);
    distortionFilterLPButton.setToggleState(true, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterLPButton.addListener(this);
    addAndMakeVisible(distortionFilterLPButton);
    
    distortionFilterBPButton.setButtonText("BP");
    distortionFilterBPButton.setClickingTogglesState(true);
    distortionFilterBPButton.setToggleState(false, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterBPButton.addListener(this);
    addAndMakeVisible(distortionFilterBPButton);
    
    distortionFilterHPButton.setButtonText("HP");
    distortionFilterHPButton.setClickingTogglesState(true);
    distortionFilterHPButton.setToggleState(false, juce::dontSendNotification);
    // Note: LookAndFeel will be set by parent
    distortionFilterHPButton.addListener(this);
    addAndMakeVisible(distortionFilterHPButton);
    
    // Filter frequency knob
    distortionFilterFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionFilterFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionFilterFreqKnob.setRange(20.0, 20000.0, 1.0);
    distortionFilterFreqKnob.setValue(800.0);
    // Note: LookAndFeel will be set by parent
    distortionFilterFreqKnob.addListener(this);
    addAndMakeVisible(distortionFilterFreqKnob);
    
    distortionFilterFreqLabel.setText("FREQ", juce::dontSendNotification);
    distortionFilterFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionFilterFreqLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionFilterFreqLabel);
    
    // Filter Q knob
    distortionFilterQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionFilterQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionFilterQKnob.setRange(0.1, 10.0, 0.1);
    distortionFilterQKnob.setValue(2.0);
    // Note: LookAndFeel will be set by parent
    distortionFilterQKnob.addListener(this);
    addAndMakeVisible(distortionFilterQKnob);
    
    distortionFilterQLabel.setText("Q", juce::dontSendNotification);
    distortionFilterQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionFilterQLabel.setJustificationType(juce::Justification::centred);
    // Note: LookAndFeel will be set by parent
    addAndMakeVisible(distortionFilterQLabel);
}

void DistortionComponent::paint(juce::Graphics& g)
{
    // Placeholder - component handles its own painting through child components
}

void DistortionComponent::resized()
{
    layoutControls(getLocalBounds());
}

void DistortionComponent::mouseDown(const juce::MouseEvent& event)
{
    if (!parentSynthesizer) return;
    
    if (event.eventComponent == &distortionTypeValueLabel)
    {
        // Handle click to cycle through distortion types
        distortionTypeValue++;
        if (distortionTypeValue > 16)
            distortionTypeValue = 1;
            
        // Update text based on type value
        juce::StringArray typeNames = {
            "TUBE", "SOFTCLIP", "HARDCLIP", "DIODE 1", "DIODE 2", 
            "LINEAR FOLD", "SINE FOLD", "ZERO-SQUARE", "DOWNSAMPLE", 
            "ASYMMETRIC", "RECTIFY", "SINE SHAPER", "STOMP BOX", 
            "TAPE SAT", "OVERDRIVE", "SOFT SAT"
        };
        
        distortionTypeValueLabel.setText(typeNames[distortionTypeValue - 1], juce::dontSendNotification);
        
        // Update the processor with the new distortion type
        parentSynthesizer->audioProcessor.setDistortionType(distortionTypeValue);
        
        // Still set up drag state in case drag events work
        isDraggingDistortionType = true;
        dragStartY = event.getScreenPosition().y;
        dragStartDistortionType = distortionTypeValue;
    }
}

void DistortionComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!parentSynthesizer) return;
    
    if (isDraggingDistortionType)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newType = dragStartDistortionType + (deltaY / 10); // 10 pixels per type
        
        // Clamp to valid range (1 to 16 types)
        newType = juce::jlimit(1, 16, newType);
        
        if (newType != distortionTypeValue)
        {
            distortionTypeValue = newType;
            
            // Update text based on type value
            juce::StringArray typeNames = {
                "TUBE", "SOFTCLIP", "HARDCLIP", "DIODE 1", "DIODE 2", 
                "LINEAR FOLD", "SINE FOLD", "ZERO-SQUARE", "DOWNSAMPLE", 
                "ASYMMETRIC", "RECTIFY", "SINE SHAPER", "STOMP BOX", 
                "TAPE SAT", "OVERDRIVE", "SOFT SAT"
            };
            
            distortionTypeValueLabel.setText(typeNames[distortionTypeValue - 1], juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionType(distortionTypeValue);
        }
    }
}

void DistortionComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingDistortionType = false;
}

void DistortionComponent::sliderValueChanged(juce::Slider* slider)
{
    if (!parentSynthesizer) return;
    
    if (slider == &distortionDriveKnob)
    {
        parentSynthesizer->audioProcessor.setDistortionDrive(static_cast<float>(distortionDriveKnob.getValue()));
    }
    else if (slider == &distortionMixKnob)
    {
        parentSynthesizer->audioProcessor.setDistortionMix(static_cast<float>(distortionMixKnob.getValue()));
    }
    else if (slider == &distortionFilterFreqKnob)
    {
        parentSynthesizer->audioProcessor.setDistortionFilterFreq(static_cast<float>(distortionFilterFreqKnob.getValue()));
    }
    else if (slider == &distortionFilterQKnob)
    {
        parentSynthesizer->audioProcessor.setDistortionFilterQ(static_cast<float>(distortionFilterQKnob.getValue()));
    }
}

void DistortionComponent::buttonClicked(juce::Button* button)
{
    if (!parentSynthesizer) return;
    
    if (button == &distortionPowerButton)
    {
        parentSynthesizer->audioProcessor.setDistortionEnabled(distortionPowerButton.getToggleState());
    }
    // Distortion filter position buttons (exclusive)
    else if (button == &distortionFilterOffButton)
    {
        if (distortionFilterOffButton.getToggleState())
        {
            distortionFilterPreButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterPostButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterPosition(0); // OFF
        }
        else
        {
            distortionFilterOffButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    else if (button == &distortionFilterPreButton)
    {
        if (distortionFilterPreButton.getToggleState())
        {
            distortionFilterOffButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterPostButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterPosition(1); // PRE
        }
        else
        {
            distortionFilterPreButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    else if (button == &distortionFilterPostButton)
    {
        if (distortionFilterPostButton.getToggleState())
        {
            distortionFilterOffButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterPreButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterPosition(2); // POST
        }
        else
        {
            distortionFilterPostButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    
    // Distortion filter type buttons (exclusive)
    else if (button == &distortionFilterLPButton)
    {
        if (distortionFilterLPButton.getToggleState())
        {
            distortionFilterBPButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterHPButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterType(1); // LP
        }
        else
        {
            distortionFilterLPButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    else if (button == &distortionFilterBPButton)
    {
        if (distortionFilterBPButton.getToggleState())
        {
            distortionFilterLPButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterHPButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterType(2); // BP
        }
        else
        {
            distortionFilterBPButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    else if (button == &distortionFilterHPButton)
    {
        if (distortionFilterHPButton.getToggleState())
        {
            distortionFilterLPButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterBPButton.setToggleState(false, juce::dontSendNotification);
            parentSynthesizer->audioProcessor.setDistortionFilterType(3); // HP
        }
        else
        {
            distortionFilterHPButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
}

void DistortionComponent::syncWithDSPState()
{
    if (!parentSynthesizer) return;
    
    // Distortion controls
    distortionDriveKnob.setValue(parentSynthesizer->audioProcessor.getDistortionDrive(), juce::dontSendNotification);
    distortionMixKnob.setValue(parentSynthesizer->audioProcessor.getDistortionMix(), juce::dontSendNotification);
    distortionFilterFreqKnob.setValue(parentSynthesizer->audioProcessor.getDistortionFilterFreq(), juce::dontSendNotification);
    distortionFilterQKnob.setValue(parentSynthesizer->audioProcessor.getDistortionFilterQ(), juce::dontSendNotification);
    distortionPowerButton.setToggleState(parentSynthesizer->audioProcessor.getDistortionEnabled(), juce::dontSendNotification);
}

void DistortionComponent::setLookAndFeels(juce::LookAndFeel* digitalKnobLAF, juce::LookAndFeel* digitalButtonLAF, juce::LookAndFeel* ledNumberLAF)
{
    // Set look and feel for all controls
    distortionTypeLabel.setLookAndFeel(digitalKnobLAF);
    distortionTypeValueLabel.setLookAndFeel(ledNumberLAF);
    distortionDriveKnob.setLookAndFeel(digitalKnobLAF);
    distortionDriveLabel.setLookAndFeel(digitalKnobLAF);
    distortionMixKnob.setLookAndFeel(digitalKnobLAF);
    distortionMixLabel.setLookAndFeel(digitalKnobLAF);
    distortionPowerButton.setLookAndFeel(digitalButtonLAF);
    distortionFilterSectionLabel.setLookAndFeel(digitalKnobLAF);
    
    // Filter position buttons
    distortionFilterOffButton.setLookAndFeel(digitalButtonLAF);
    distortionFilterPreButton.setLookAndFeel(digitalButtonLAF);
    distortionFilterPostButton.setLookAndFeel(digitalButtonLAF);
    
    // Filter type buttons
    distortionFilterLPButton.setLookAndFeel(digitalButtonLAF);
    distortionFilterBPButton.setLookAndFeel(digitalButtonLAF);
    distortionFilterHPButton.setLookAndFeel(digitalButtonLAF);
    
    // Filter knobs
    distortionFilterFreqKnob.setLookAndFeel(digitalKnobLAF);
    distortionFilterFreqLabel.setLookAndFeel(digitalKnobLAF);
    distortionFilterQKnob.setLookAndFeel(digitalKnobLAF);
    distortionFilterQLabel.setLookAndFeel(digitalKnobLAF);
}

void DistortionComponent::layoutControls(juce::Rectangle<int> bounds)
{
    // Layout with same large knobs and green styling as other effects
    auto knobSize = 62; // Same size as other effects for consistency
    auto labelHeight = 22; // Same as other effects
    auto knobSpacing = 25; // Tighter spacing due to more controls
    auto buttonWidth = 65; // Smaller buttons to fit more
    auto buttonHeight = 28; // Standard button height
    auto typeBoxWidth = 120; // Width for distortion type selector
    auto typeBoxHeight = 35; // Height for distortion type selector
    auto smallButtonWidth = 45; // For LP/BP/HP buttons
    
    auto startY = 25; // Higher positioning for more rows
    
    // Row 1: Power button and Mix knob (moved down 65 pixels)
    auto row1Y = startY + 65;
    auto powerButtonX = (bounds.getWidth() - buttonWidth - knobSpacing - knobSize) / 2 + 5;
    distortionPowerButton.setBounds(powerButtonX, row1Y - 4, buttonWidth, buttonHeight);
    
    // Mix knob in same row as power button
    auto mixRow1X = powerButtonX + buttonWidth + knobSpacing;
    distortionMixKnob.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2 - 4, knobSize, knobSize);
    distortionMixLabel.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2 + knobSize - 12 - 4, knobSize, labelHeight);
    
    // Row 2: Type selector, Drive knob (moved up 45 pixels from previous position)
    auto row2Y = row1Y + buttonHeight + 20; // Moved up 45 pixels total
    auto totalMainWidth = typeBoxWidth + knobSpacing + knobSize; // Only type + drive now
    auto mainStartX = (bounds.getWidth() - totalMainWidth) / 2 - 6; // Moved left 6 pixels from center (10 - 4)
    
    // Type selector area
    auto typeY = row2Y + (knobSize - typeBoxHeight) / 2;
    distortionTypeValueLabel.setBounds(mainStartX, typeY, typeBoxWidth, typeBoxHeight);
    distortionTypeLabel.setBounds(mainStartX, typeY + typeBoxHeight + 3, typeBoxWidth, labelHeight);
    
    // Drive knob and label
    auto drive_x = mainStartX + typeBoxWidth + knobSpacing;
    distortionDriveKnob.setBounds(drive_x, row2Y, knobSize, knobSize);
    distortionDriveLabel.setBounds(drive_x, row2Y + knobSize - 12, knobSize, labelHeight);
    
    // Mix knob moved to row 1 - no longer here
    
    // Filter section label (moved down 30 pixels from current position)
    auto filterLabelY = row2Y + knobSize + labelHeight;
    distortionFilterSectionLabel.setBounds((bounds.getWidth() - 60) / 2 + 6, filterLabelY, 60, 15);
    
    // Row 3: Filter mode buttons (Off/Pre/Post) - moved down 40 pixels to avoid clashing
    auto row3Y = filterLabelY + 25;
    
    // Filter mode buttons (Off/Pre/Post) - centered (moved left 20 pixels)
    auto filterModeWidth = 3 * smallButtonWidth + 2 * 5; // Total width of 3 buttons + spacing
    auto filterModeStartX = (bounds.getWidth() - filterModeWidth) / 2 + 6;
    distortionFilterOffButton.setBounds(filterModeStartX, row3Y, smallButtonWidth, buttonHeight);
    distortionFilterPreButton.setBounds(filterModeStartX + smallButtonWidth + 5, row3Y, smallButtonWidth, buttonHeight);
    distortionFilterPostButton.setBounds(filterModeStartX + 2 * (smallButtonWidth + 5), row3Y, smallButtonWidth, buttonHeight);
    
    // Row 4: Filter type buttons (LP/BP/HP) - separate row
    auto row4Y = row3Y + buttonHeight + 15;
    auto filterTypeWidth = 3 * smallButtonWidth + 2 * 5; // Total width of 3 buttons + spacing
    auto filterTypeStartX = (bounds.getWidth() - filterTypeWidth) / 2 + 6;
    distortionFilterLPButton.setBounds(filterTypeStartX, row4Y, smallButtonWidth, buttonHeight);
    distortionFilterBPButton.setBounds(filterTypeStartX + smallButtonWidth + 5, row4Y, smallButtonWidth, buttonHeight);
    distortionFilterHPButton.setBounds(filterTypeStartX + 2 * (smallButtonWidth + 5), row4Y, smallButtonWidth, buttonHeight);
    
    // Row 5: Filter knobs (Freq, Q) - separate row below buttons (moved up 20 pixels total)
    auto row5Y = row4Y + buttonHeight;
    auto filterKnobStartX = (bounds.getWidth() - (2 * knobSize) - knobSpacing) / 2 + 8; // Moved right 8 pixels (11 - 3)
    
    distortionFilterFreqKnob.setBounds(filterKnobStartX, row5Y, knobSize, knobSize);
    distortionFilterFreqLabel.setBounds(filterKnobStartX, row5Y + knobSize - 12, knobSize, labelHeight);
    
    distortionFilterQKnob.setBounds(filterKnobStartX + knobSize + knobSpacing, row5Y, knobSize, knobSize);
    distortionFilterQLabel.setBounds(filterKnobStartX + knobSize + knobSpacing, row5Y + knobSize - 12, knobSize, labelHeight);
}