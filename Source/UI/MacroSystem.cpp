#include "MacroSystem.h"

MacroSystem::MacroSystem()
{
}

void MacroSystem::createMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    if (!targetSlider) return;
    
    // Remove any existing mapping for this specific macro/slider combination
    // This allows the same macro to control multiple different sliders
    macroMappings.erase(
        std::remove_if(macroMappings.begin(), macroMappings.end(),
            [macroIndex, targetSlider](const MacroMapping& mapping) {
                return mapping.macroIndex == macroIndex && mapping.targetSlider == targetSlider;
            }),
        macroMappings.end());
    
    // Create new mapping with current slider value as base
    double currentValue = targetSlider->getValue();
    double minRange = targetSlider->getMinimum();
    double maxRange = targetSlider->getMaximum();
    
    macroMappings.emplace_back(macroIndex, targetSlider, currentValue, minRange, maxRange);
}

void MacroSystem::updateMacroMappings(int macroIndex, double macroValue)
{
    // macroValue should be 0.0 to 1.0 (macro knob range)
    for (auto& mapping : macroMappings)
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
        }
    }
}

void MacroSystem::removeMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    macroMappings.erase(
        std::remove_if(macroMappings.begin(), macroMappings.end(),
            [macroIndex, targetSlider](const MacroMapping& mapping) {
                return mapping.macroIndex == macroIndex && mapping.targetSlider == targetSlider;
            }),
        macroMappings.end());
}

juce::Slider* MacroSystem::findSliderAt(juce::Point<int> position, const std::vector<juce::Slider*>& allSliders)
{
    // Check each slider to see if the position is within its bounds
    // Return the slider with the smallest bounds that contains the position (most specific match)
    juce::Slider* bestMatch = nullptr;
    int smallestArea = INT_MAX;
    
    for (auto* slider : allSliders)
    {
        if (slider && slider->isVisible())
        {
            // Get slider bounds in its parent's coordinate system
            auto sliderBounds = slider->getBounds();
            if (sliderBounds.contains(position))
            {
                int area = sliderBounds.getWidth() * sliderBounds.getHeight();
                if (area < smallestArea)
                {
                    smallestArea = area;
                    bestMatch = slider;
                }
            }
        }
    }
    
    return bestMatch;
}

MacroMapping* MacroSystem::findMacroMappingAtPosition(juce::Point<int> position)
{
    for (auto& mapping : macroMappings)
    {
        if (mapping.targetSlider && mapping.targetSlider->isVisible())
        {
            auto center = getSliderKnobCenter(mapping.targetSlider);
            float radius = getKnobRadius(mapping.targetSlider);
            
            // Check if position is near the arc (handle areas)
            float distanceFromCenter = center.getDistanceFrom(position.toFloat());
            
            // Allow some tolerance for clicking on arc handles
            if (std::abs(distanceFromCenter - radius) < 10.0f)
            {
                return &mapping;
            }
        }
    }
    return nullptr;
}

void MacroSystem::updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition)
{
    if (!mapping || !mapping->targetSlider)
        return;
    
    auto center = getSliderKnobCenter(mapping->targetSlider);
    float radius = getKnobRadius(mapping->targetSlider);
    
    // Calculate angle from center to drag position
    float dx = dragPosition.x - center.x;
    float dy = dragPosition.y - center.y;
    float angle = std::atan2(dy, dx) + juce::MathConstants<float>::halfPi;
    
    // Normalize angle to 0-2π range
    while (angle < 0) angle += juce::MathConstants<float>::twoPi;
    while (angle >= juce::MathConstants<float>::twoPi) angle -= juce::MathConstants<float>::twoPi;
    
    // Map to knob range (225° to 495°)
    float knobStartAngle = juce::MathConstants<float>::pi * 1.25f;
    float knobEndAngle = juce::MathConstants<float>::pi * 2.75f;
    float knobRange = knobEndAngle - knobStartAngle;
    
    // Convert angle to normalized position (0-1)
    float normalizedPosition = (angle - knobStartAngle) / knobRange;
    normalizedPosition = juce::jlimit(0.0f, 1.0f, normalizedPosition);
    
    // Convert to actual slider value
    double sliderRange = mapping->maxRange - mapping->minRange;
    double newValue = mapping->minRange + (normalizedPosition * sliderRange);
    
    // Update the user range (this is a simplified version - in practice you'd determine which handle is being dragged)
    mapping->userMinRange = std::min(mapping->userMinRange, newValue);
    mapping->userMaxRange = std::max(mapping->userMaxRange, newValue);
}

void MacroSystem::drawMacroIndicators(juce::Graphics& g)
{
    for (const auto& mapping : macroMappings)
    {
        if (mapping.targetSlider && mapping.targetSlider->isVisible())
        {
            drawCircularIndicator(g, mapping.targetSlider, mapping);
        }
    }
}

void MacroSystem::drawCircularIndicator(juce::Graphics& g, juce::Slider* slider, const MacroMapping& mapping)
{
    if (!slider) return;
    
    auto center = getSliderKnobCenter(slider);
    float radius = getKnobRadius(slider);
    
    // Use a default macro value of 0.5 since we don't have access to macro knobs here
    // This will need to be passed in from the parent component
    double macroValue = 0.5;
    double sliderRange = mapping.maxRange - mapping.minRange;
    
    // Use the user-defined range for the arc
    double potentialMin = mapping.userMinRange;
    double potentialMax = mapping.userMaxRange;
    
    // Convert to normalized positions (0 to 1)
    double normalizedPotentialMin = (potentialMin - mapping.minRange) / sliderRange;
    double normalizedPotentialMax = (potentialMax - mapping.minRange) / sliderRange;
    double normalizedBase = (mapping.baseValue - mapping.minRange) / sliderRange;
    
    // Map to typical rotary knob range: 225° to 495° (7 o'clock to 5 o'clock)
    float knobStartAngle = juce::MathConstants<float>::pi * 1.25f; // 225°
    float knobEndAngle = juce::MathConstants<float>::pi * 2.75f;   // 495° 
    float knobRange = knobEndAngle - knobStartAngle;
    
    float potentialStartAngle = knobStartAngle + (float)(normalizedPotentialMin * knobRange);
    float potentialEndAngle = knobStartAngle + (float)(normalizedPotentialMax * knobRange);
    float baseAngle = knobStartAngle + (float)(normalizedBase * knobRange);
    
    // 1. Draw the static potential range arc (thin line)
    g.setColour(mapping.indicatorColor.withAlpha(0.4f));
    juce::Path potentialPath;
    potentialPath.addCentredArc(center.x, center.y, radius, radius, 0.0f, potentialStartAngle, potentialEndAngle, true);
    g.strokePath(potentialPath, juce::PathStrokeType(2.0f));
    
    // 2. Calculate current macro target position for the moving dot
    double offset = (macroValue - 0.5) * sliderRange;
    double currentTarget = juce::jlimit(mapping.minRange, mapping.maxRange, mapping.baseValue + offset);
    double normalizedCurrent = (currentTarget - mapping.minRange) / sliderRange;
    float currentAngle = knobStartAngle + (float)(normalizedCurrent * knobRange);
    
    // 3. Draw moving dot at current macro position
    g.setColour(mapping.indicatorColor);
    float dotRadius = 4.0f;
    float dotX = center.x + radius * std::cos(currentAngle - juce::MathConstants<float>::halfPi);
    float dotY = center.y + radius * std::sin(currentAngle - juce::MathConstants<float>::halfPi);
    g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2, dotRadius * 2);
    
    // 4. Draw a subtle base position indicator
    g.setColour(mapping.indicatorColor.withAlpha(0.6f));
    float baseDotRadius = 2.0f;
    float baseX = center.x + radius * std::cos(baseAngle - juce::MathConstants<float>::halfPi);
    float baseY = center.y + radius * std::sin(baseAngle - juce::MathConstants<float>::halfPi);
    g.fillEllipse(baseX - baseDotRadius, baseY - baseDotRadius, baseDotRadius * 2, baseDotRadius * 2);
    
    // 5. Draw drag handles at arc endpoints
    float handleRadius = 5.0f;
    
    // Min range handle
    float minX = center.x + radius * std::cos(potentialStartAngle - juce::MathConstants<float>::halfPi);
    float minY = center.y + radius * std::sin(potentialStartAngle - juce::MathConstants<float>::halfPi);
    g.setColour(mapping.indicatorColor.withAlpha(0.9f));
    g.fillEllipse(minX - handleRadius, minY - handleRadius, handleRadius * 2, handleRadius * 2);
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawEllipse(minX - handleRadius, minY - handleRadius, handleRadius * 2, handleRadius * 2, 1.5f);
    
    // Max range handle
    float maxX = center.x + radius * std::cos(potentialEndAngle - juce::MathConstants<float>::halfPi);
    float maxY = center.y + radius * std::sin(potentialEndAngle - juce::MathConstants<float>::halfPi);
    g.setColour(mapping.indicatorColor.withAlpha(0.9f));
    g.fillEllipse(maxX - handleRadius, maxY - handleRadius, handleRadius * 2, handleRadius * 2);
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawEllipse(maxX - handleRadius, maxY - handleRadius, handleRadius * 2, handleRadius * 2, 1.5f);
}

juce::Point<float> MacroSystem::getSliderKnobCenter(juce::Slider* slider)
{
    auto bounds = slider->getBounds();
    return bounds.getCentre().toFloat();
}

float MacroSystem::getKnobRadius(juce::Slider* slider)
{
    auto bounds = slider->getBounds();
    return std::max(bounds.getWidth(), bounds.getHeight()) * 0.6f;
}