#include "ParametricEQComponent.h"
#include "../SynthesizerComponent.h"
#include "../PluginProcessor.h"

ParametricEQComponent::ParametricEQComponent()
{
    // Initialize bands
    bands[0].frequency = 400.0f;  // Left band (low freq)
    bands[0].gain = 0.0f;
    bands[0].q = 1.0f;
    bands[0].filterType = Peak;
    bands[0].isLeftBand = true;
    
    bands[1].frequency = 4000.0f; // Right band (high freq)
    bands[1].gain = 0.0f;
    bands[1].q = 1.0f;
    bands[1].filterType = Peak;
    bands[1].isLeftBand = false;
}

void ParametricEQComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw debug background first to ensure visibility
    g.setColour(juce::Colours::red);
    g.fillRect(bounds);
    
    // Draw digital screen background
    g.setColour(juce::Colour(0xff000008)); // Very dark blue-black
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Draw green outline
    g.setColour(juce::Colour(0xff00ff00).withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, 4.0f, 2.0f);
    
    // Set graph area (leave margin for borders and labels)
    graphArea = bounds.reduced(20.0f, 20.0f);
    
    // Draw frequency grid
    g.setColour(juce::Colour(0xff003300).withAlpha(0.3f));
    
    // Vertical frequency lines (20Hz, 200Hz, 2kHz, 20kHz)
    std::vector<float> frequencies = {20.0f, 200.0f, 2000.0f, 20000.0f};
    for (auto freq : frequencies)
    {
        float x = frequencyToX(freq);
        g.drawVerticalLine(int(x), graphArea.getY(), graphArea.getBottom());
    }
    
    // Horizontal gain lines (-12dB, -6dB, 0dB, +6dB, +12dB)
    std::vector<float> gains = {-12.0f, -6.0f, 0.0f, 6.0f, 12.0f};
    for (auto gain : gains)
    {
        float y = gainToY(gain);
        g.drawHorizontalLine(int(y), graphArea.getX(), graphArea.getRight());
        
        // Draw 0dB line thicker
        if (gain == 0.0f)
        {
            g.setColour(juce::Colour(0xff00ff00).withAlpha(0.8f));
            g.drawHorizontalLine(int(y), graphArea.getX(), graphArea.getRight());
            g.drawHorizontalLine(int(y) + 1, graphArea.getX(), graphArea.getRight());
            g.setColour(juce::Colour(0xff003300).withAlpha(0.3f));
        }
    }
    
    // Draw frequency response curve
    drawFrequencyResponse(g);
    
    // Draw band handles
    for (int i = 0; i < 2; ++i)
    {
        drawBandHandle(g, bands[i], i);
    }
    
    // Draw frequency labels
    g.setColour(juce::Colour(0xff00cc00));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::bold));
    
    for (auto freq : frequencies)
    {
        float x = frequencyToX(freq);
        juce::String label = freq < 1000.0f ? juce::String(int(freq)) + "Hz" 
                                            : juce::String(freq / 1000.0f, 1) + "k";
        g.drawText(label, x - 15, graphArea.getBottom() + 2, 30, 12, juce::Justification::centred);
    }
    
    // Draw gain labels
    for (auto gain : gains)
    {
        float y = gainToY(gain);
        juce::String label = (gain > 0 ? "+" : "") + juce::String(int(gain)) + "dB";
        g.drawText(label, 2, y - 6, 16, 12, juce::Justification::centred);
    }
}

void ParametricEQComponent::resized()
{
    auto bounds = getLocalBounds().toFloat();
    graphArea = bounds.reduced(20.0f, 20.0f);
    
    // Update band positions based on current frequency/gain
    for (int i = 0; i < 2; ++i)
    {
        bands[i].graphPosition.x = frequencyToX(bands[i].frequency);
        bands[i].graphPosition.y = gainToY(bands[i].gain);
    }
    
    repaint();
}

void ParametricEQComponent::mouseDown(const juce::MouseEvent& event)
{
    draggedBand = getBandAtPoint(event.position);
}

void ParametricEQComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (draggedBand >= 0 && draggedBand < 2)
    {
        auto& band = bands[draggedBand];
        
        // Update frequency and gain from mouse position
        float clampedX = juce::jlimit(graphArea.getX(), graphArea.getRight(), event.position.x);
        float clampedY = juce::jlimit(graphArea.getY(), graphArea.getBottom(), event.position.y);
        
        band.frequency = xToFrequency(clampedX);
        band.gain = yToGain(clampedY);
        band.graphPosition = juce::Point<float>(clampedX, clampedY);
        
        // Update corresponding knobs
        updateKnobsFromBand(draggedBand);
        
        repaint();
    }
}

void ParametricEQComponent::mouseUp(const juce::MouseEvent& event)
{
    draggedBand = -1;
}

void ParametricEQComponent::updateBandFromKnobs(int bandIndex)
{
    // This will be called when knobs are changed
    if (bandIndex >= 0 && bandIndex < 2)
    {
        Component::repaint();
    }
}

void ParametricEQComponent::updateKnobsFromBand(int bandIndex)
{
    // This will be called when graph is dragged to update the knobs
    if (bandIndex < 0 || bandIndex >= 2 || !parentSynthesizer) return;
    
    if (bandIndex == 0)
    {
        // Update Band 1 knobs (both sets) - now accessed through eqControls
        parentSynthesizer->eqControls.getEQ1FreqKnob().setValue(bands[0].frequency, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ1QKnob().setValue(bands[0].q, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ1GainKnob().setValue(bands[0].gain, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ1NewFreqKnob().setValue(bands[0].frequency, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ1NewQKnob().setValue(bands[0].q, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ1NewGainKnob().setValue(bands[0].gain, juce::dontSendNotification);

        // Update audio processor DSP
        parentSynthesizer->audioProcessor.setEQ1Frequency(bands[0].frequency);
        parentSynthesizer->audioProcessor.setEQ1Q(bands[0].q);
        parentSynthesizer->audioProcessor.setEQ1Gain(bands[0].gain);
    }
    else if (bandIndex == 1)
    {
        // Update Band 2 knobs (both sets) - now accessed through eqControls
        parentSynthesizer->eqControls.getEQ2FreqKnob().setValue(bands[1].frequency, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ2QKnob().setValue(bands[1].q, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ2GainKnob().setValue(bands[1].gain, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ2NewFreqKnob().setValue(bands[1].frequency, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ2NewQKnob().setValue(bands[1].q, juce::dontSendNotification);
        parentSynthesizer->eqControls.getEQ2NewGainKnob().setValue(bands[1].gain, juce::dontSendNotification);

        // Update audio processor DSP
        parentSynthesizer->audioProcessor.setEQ2Frequency(bands[1].frequency);
        parentSynthesizer->audioProcessor.setEQ2Q(bands[1].q);
        parentSynthesizer->audioProcessor.setEQ2Gain(bands[1].gain);
    }
}

void ParametricEQComponent::setBandFilterType(int bandIndex, FilterType type)
{
    if (bandIndex >= 0 && bandIndex < 2)
    {
        bands[bandIndex].filterType = type;
        repaint();
    }
}

void ParametricEQComponent::syncWithDSPState()
{
    if (!parentSynthesizer) return;
    
    // Sync Band 1 (left) parameters
    bands[0].frequency = parentSynthesizer->audioProcessor.getEQ1Frequency();
    bands[0].q = parentSynthesizer->audioProcessor.getEQ1Q();
    bands[0].gain = parentSynthesizer->audioProcessor.getEQ1Gain();
    
    // Map DSP filter type to visual filter type
    int eq1Type = parentSynthesizer->audioProcessor.getEQ1Type();
    if (eq1Type == 0) bands[0].filterType = Peak;
    else if (eq1Type == 1) bands[0].filterType = Shelf;
    else if (eq1Type == 2) bands[0].filterType = Pass;
    
    // Sync Band 2 (right) parameters  
    bands[1].frequency = parentSynthesizer->audioProcessor.getEQ2Frequency();
    bands[1].q = parentSynthesizer->audioProcessor.getEQ2Q();
    bands[1].gain = parentSynthesizer->audioProcessor.getEQ2Gain();
    
    // Map DSP filter type to visual filter type
    int eq2Type = parentSynthesizer->audioProcessor.getEQ2Type();
    if (eq2Type == 0) bands[1].filterType = Peak;
    else if (eq2Type == 1) bands[1].filterType = Shelf;
    else if (eq2Type == 2) bands[1].filterType = Pass;
    
    // Sync band enable states with UI buttons - now accessed through eqControls
    parentSynthesizer->eqControls.getEQ1OnOffButton().setToggleState(parentSynthesizer->audioProcessor.getEQ1Enabled(), juce::dontSendNotification);
    parentSynthesizer->eqControls.getEQ2OnOffButton().setToggleState(parentSynthesizer->audioProcessor.getEQ2Enabled(), juce::dontSendNotification);
    
    // Update visual positions based on frequency/gain
    resized(); // This will update graphPosition for each band
    repaint();
}

float ParametricEQComponent::frequencyToX(float frequency) const
{
    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    float logFreq = std::log10(frequency);
    
    float normalized = (logFreq - logMin) / (logMax - logMin);
    return graphArea.getX() + normalized * graphArea.getWidth();
}

float ParametricEQComponent::xToFrequency(float x) const
{
    float normalized = (x - graphArea.getX()) / graphArea.getWidth();
    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    float logFreq = logMin + normalized * (logMax - logMin);
    
    return std::pow(10.0f, logFreq);
}

float ParametricEQComponent::gainToY(float gain) const
{
    float normalized = (gain + 15.0f) / 30.0f; // -15dB to +15dB range
    return graphArea.getBottom() - normalized * graphArea.getHeight();
}

float ParametricEQComponent::yToGain(float y) const
{
    float normalized = (graphArea.getBottom() - y) / graphArea.getHeight();
    return normalized * 30.0f - 15.0f; // -15dB to +15dB range
}

void ParametricEQComponent::drawFrequencyResponse(juce::Graphics& g)
{
    juce::Path responseCurve;
    
    int numPoints = 200;
    float startX = graphArea.getX();
    float width = graphArea.getWidth();
    
    for (int i = 0; i < numPoints; ++i)
    {
        float x = startX + (i / float(numPoints - 1)) * width;
        float freq = xToFrequency(x);
        
        // Calculate combined response from both bands
        float totalGain = 0.0f;
        
        for (int band = 0; band < 2; ++band)
        {
            // Check if band is enabled via parent synthesizer
            bool bandEnabled = true;
            if (parentSynthesizer)
            {
                bandEnabled = band == 0 ? parentSynthesizer->audioProcessor.getEQ1Enabled() : 
                                        parentSynthesizer->audioProcessor.getEQ2Enabled();
            }
            
            // Skip disabled bands
            if (!bandEnabled)
                continue;
                
            float bandGain = 0.0f;
            float centerFreq = bands[band].frequency;
            float q = bands[band].q;
            float gain = bands[band].gain;
            
            // Calculate filter response based on type
            if (bands[band].filterType == Peak)
            {
                // Peak/notch filter response
                float ratio = freq / centerFreq;
                float logRatio = std::log10(ratio);
                float bandwidth = 1.0f / q;
                float response = 1.0f / (1.0f + std::pow(logRatio / bandwidth, 2.0f) * 4.0f);
                bandGain = gain * response;
            }
            else if (bands[band].filterType == Shelf)
            {
                // Shelf filter response
                bool isHighShelf = bands[band].isLeftBand ? false : true; // Left=low shelf, Right=high shelf
                float ratio = freq / centerFreq;
                
                if (isHighShelf)
                {
                    // High shelf - boost/cut frequencies above the center frequency
                    float s = q; // Use Q parameter to control shelf slope (higher Q = steeper transition)
                    float A = std::pow(10.0f, gain / 40.0f); // Convert dB to linear scale
                    float w = ratio;
                    float response = ((A + 1.0f) + (A - 1.0f) * std::tanh((std::log(w)) * s)) / 2.0f;
                    bandGain = 20.0f * std::log10(std::max(0.001f, response)); // Convert back to dB, avoid log(0)
                }
                else
                {
                    // Low shelf - boost/cut frequencies below the center frequency  
                    float s = q; // Use Q parameter to control shelf slope
                    float A = std::pow(10.0f, gain / 40.0f); // Convert dB to linear scale
                    float w = ratio;
                    float response = ((A + 1.0f) - (A - 1.0f) * std::tanh((std::log(w)) * s)) / 2.0f;
                    bandGain = 20.0f * std::log10(std::max(0.001f, response)); // Convert back to dB, avoid log(0)
                }
            }
            else if (bands[band].filterType == Pass)
            {
                // Pass filter response (highpass for left, lowpass for right)
                bool isHighPass = bands[band].isLeftBand; // Left=highpass, Right=lowpass
                float ratio = freq / centerFreq;
                
                if (isHighPass)
                {
                    // High pass filter - steepness controlled by Q
                    float rolloffRate = 6.0f + (q * 6.0f); // 6-60 dB/octave based on Q (0.1-10)
                    if (ratio < 1.0f)
                    {
                        // Below cutoff - calculate rolloff
                        float octaves = std::log2(ratio);
                        bandGain = octaves * rolloffRate; // Negative gain below cutoff
                        bandGain = std::max(bandGain, -60.0f); // Limit minimum to -60dB
                    }
                    else
                    {
                        // Above cutoff - flat response
                        bandGain = 0.0f;
                    }
                }
                else
                {
                    // Low pass filter - steepness controlled by Q
                    float rolloffRate = 6.0f + (q * 6.0f); // 6-60 dB/octave based on Q (0.1-10)
                    if (ratio > 1.0f)
                    {
                        // Above cutoff - calculate rolloff
                        float octaves = std::log2(ratio);
                        bandGain = -octaves * rolloffRate; // Negative gain above cutoff
                        bandGain = std::max(bandGain, -60.0f); // Limit minimum to -60dB
                    }
                    else
                    {
                        // Below cutoff - flat response
                        bandGain = 0.0f;
                    }
                }
            }
            
            totalGain += bandGain;
        }
        
        float y = gainToY(totalGain);
        
        if (i == 0)
            responseCurve.startNewSubPath(x, y);
        else
            responseCurve.lineTo(x, y);
    }
    
    // Draw the response curve
    g.setColour(juce::Colour(0xff00ff00).withAlpha(0.8f));
    g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
}

void ParametricEQComponent::drawBandHandle(juce::Graphics& g, const EQBand& band, int bandIndex)
{
    // Check if band is enabled via parent synthesizer
    bool bandEnabled = true;
    if (parentSynthesizer)
    {
        bandEnabled = bandIndex == 0 ? parentSynthesizer->audioProcessor.getEQ1Enabled() : 
                                      parentSynthesizer->audioProcessor.getEQ2Enabled();
    }
    
    // Don't draw anything if band is disabled
    if (!bandEnabled)
        return;
    
    float x = frequencyToX(band.frequency);
    float y = gainToY(band.gain);
    
    // Different colors for left and right bands
    juce::Colour handleColor = band.isLeftBand ? juce::Colour(0xff00ccff) : juce::Colour(0xffff6600);
    
    // Draw handle circle
    float radius = 8.0f;
    g.setColour(handleColor.withAlpha(0.8f));
    g.fillEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f);
    
    // Draw handle border
    g.setColour(handleColor);
    g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
    
    // Draw center dot
    g.setColour(juce::Colours::white);
    g.fillEllipse(x - 2.0f, y - 2.0f, 4.0f, 4.0f);
}

int ParametricEQComponent::getBandAtPoint(juce::Point<float> point)
{
    for (int i = 0; i < 2; ++i)
    {
        float x = frequencyToX(bands[i].frequency);
        float y = gainToY(bands[i].gain);
        
        float distance = std::sqrt(std::pow(point.x - x, 2.0f) + std::pow(point.y - y, 2.0f));
        
        if (distance <= 12.0f) // Hit tolerance
            return i;
    }
    
    return -1;
}