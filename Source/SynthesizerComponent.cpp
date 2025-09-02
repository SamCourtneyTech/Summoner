#include "SynthesizerComponent.h"
#include "PluginProcessor.h"
#include <set>

// DraggableMacroSymbol implementation
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

ADSREnvelopeComponent::ADSREnvelopeComponent()
{
}

void ADSREnvelopeComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Draw metallic outline (like knobs) - outer dark frame
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    
    // Inner metallic frame
    auto metalBounds = bounds.toFloat().reduced(3.0f);
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(metalBounds, 6.0f);
    
    // Metallic highlight
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(metalBounds.reduced(1.0f), 5.0f, 1.0f);
    
    
    // Draw background inside the frames
    auto innerBounds = bounds.toFloat().reduced(8.0f);
    g.setColour(juce::Colour(0xff0f0f1f).withAlpha(0.7f));
    g.fillRoundedRectangle(innerBounds, 4.0f);
    
    // Calculate envelope curve (adjust for the additional frames)
    bounds.reduce(16, 16); // More reduction to account for larger metallic + white frames
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();
    
    if (width <= 0 || height <= 0) return;
    
    // Calculate time divisions
    auto totalTime = attackTime + decayTime + 0.5f + releaseTime; // 0.5s sustain for visualization
    auto attackWidth = (attackTime / totalTime) * width;
    auto decayWidth = (decayTime / totalTime) * width;
    auto sustainWidth = (0.5f / totalTime) * width;
    auto releaseWidth = (releaseTime / totalTime) * width;
    
    // Create envelope path
    juce::Path envelopePath;
    
    // Start at bottom left
    envelopePath.startNewSubPath(bounds.getX(), bounds.getBottom());
    
    // Attack phase - rise to top
    envelopePath.lineTo(bounds.getX() + attackWidth, bounds.getY());
    
    // Decay phase - fall to sustain level
    auto sustainY = bounds.getY() + height * (1.0f - sustainLevel);
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth, sustainY);
    
    // Sustain phase - hold level
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth, sustainY);
    
    // Release phase - fall to bottom
    envelopePath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth + releaseWidth, bounds.getBottom());
    
    // Draw the envelope curve
    g.setColour(juce::Colours::white);
    g.strokePath(envelopePath, juce::PathStrokeType(2.0f));
    
    // Draw fill under curve
    juce::Path fillPath = envelopePath;
    fillPath.lineTo(bounds.getX() + attackWidth + decayWidth + sustainWidth + releaseWidth, bounds.getBottom());
    fillPath.lineTo(bounds.getX(), bounds.getBottom());
    fillPath.closeSubPath();
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.fillPath(fillPath);
    
    // Draw grid lines
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    
    // Horizontal lines
    for (int i = 1; i < 4; ++i)
    {
        auto y = bounds.getY() + (height * i / 4.0f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
    }
    
    // Vertical phase dividers
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    auto x1 = bounds.getX() + attackWidth;
    auto x2 = bounds.getX() + attackWidth + decayWidth;
    auto x3 = bounds.getX() + attackWidth + decayWidth + sustainWidth;
    
    g.drawLine(x1, bounds.getY(), x1, bounds.getBottom(), 0.5f);
    g.drawLine(x2, bounds.getY(), x2, bounds.getBottom(), 0.5f);
    g.drawLine(x3, bounds.getY(), x3, bounds.getBottom(), 0.5f);
    
    // Draw phase labels
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font("Press Start 2P", 8.0f, juce::Font::plain));
    
    // Only draw labels if there's enough space
    if (attackWidth > 20)
        g.drawText("A", juce::Rectangle<int>(bounds.getX(), bounds.getY(), attackWidth, 12), juce::Justification::centred);
    if (decayWidth > 20)
        g.drawText("D", juce::Rectangle<int>(bounds.getX() + attackWidth, bounds.getY(), decayWidth, 12), juce::Justification::centred);
    if (sustainWidth > 20)
        g.drawText("S", juce::Rectangle<int>(bounds.getX() + attackWidth + decayWidth, bounds.getY(), sustainWidth, 12), juce::Justification::centred);
    if (releaseWidth > 20)
        g.drawText("R", juce::Rectangle<int>(bounds.getX() + attackWidth + decayWidth + sustainWidth, bounds.getY(), releaseWidth, 12), juce::Justification::centred);
}

void ADSREnvelopeComponent::resized()
{
    repaint();
}

void ADSREnvelopeComponent::updateEnvelope(float attack, float decay, float sustain, float release)
{
    attackTime = attack;
    decayTime = decay;
    sustainLevel = sustain;
    releaseTime = release;
    repaint();
}

// ParametricEQComponent Implementation
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
        // Update Band 1 knobs (both sets)
        parentSynthesizer->eq1FreqKnob.setValue(bands[0].frequency, juce::dontSendNotification);
        parentSynthesizer->eq1QKnob.setValue(bands[0].q, juce::dontSendNotification);
        parentSynthesizer->eq1GainKnob.setValue(bands[0].gain, juce::dontSendNotification);
        parentSynthesizer->eq1NewFreqKnob.setValue(bands[0].frequency, juce::dontSendNotification);
        parentSynthesizer->eq1NewQKnob.setValue(bands[0].q, juce::dontSendNotification);
        parentSynthesizer->eq1NewGainKnob.setValue(bands[0].gain, juce::dontSendNotification);
        
        // Update audio processor DSP
        parentSynthesizer->audioProcessor.setEQ1Frequency(bands[0].frequency);
        parentSynthesizer->audioProcessor.setEQ1Q(bands[0].q);
        parentSynthesizer->audioProcessor.setEQ1Gain(bands[0].gain);
    }
    else if (bandIndex == 1)
    {
        // Update Band 2 knobs (both sets)
        parentSynthesizer->eq2FreqKnob.setValue(bands[1].frequency, juce::dontSendNotification);
        parentSynthesizer->eq2QKnob.setValue(bands[1].q, juce::dontSendNotification);
        parentSynthesizer->eq2GainKnob.setValue(bands[1].gain, juce::dontSendNotification);
        parentSynthesizer->eq2NewFreqKnob.setValue(bands[1].frequency, juce::dontSendNotification);
        parentSynthesizer->eq2NewQKnob.setValue(bands[1].q, juce::dontSendNotification);
        parentSynthesizer->eq2NewGainKnob.setValue(bands[1].gain, juce::dontSendNotification);
        
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
        Component::repaint();
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
    
    // Sync band enable states with UI buttons
    parentSynthesizer->eq1OnOffButton.setToggleState(parentSynthesizer->audioProcessor.getEQ1Enabled(), juce::dontSendNotification);
    parentSynthesizer->eq2OnOffButton.setToggleState(parentSynthesizer->audioProcessor.getEQ2Enabled(), juce::dontSendNotification);
    
    // Update visual positions based on frequency/gain
    resized(); // This will update graphPosition for each band
    repaint();
}

SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor), effectsModule(juce::TabbedButtonBar::TabsAtTop)
{
    // VOLUME CONTROLS GROUP - Row 4 (MOVEABLE)
    volumeControlsVolumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeControlsVolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsVolumeLabel.setJustificationType(juce::Justification::centred);
    volumeControlsVolumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsVolumeLabel);
    
    volumeControlsVolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsVolumeKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsVolumeKnob.setValue(0.75); // Start oscillator 1 at 75% volume
    volumeControlsVolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsVolumeKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsVolumeKnob.addListener(this);
    addAndMakeVisible(volumeControlsVolumeKnob);
    
    volumeControlsDetuneLabel.setText("DETUNE", juce::dontSendNotification);
    volumeControlsDetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsDetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsDetuneLabel.setJustificationType(juce::Justification::centred);
    volumeControlsDetuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsDetuneLabel);

    volumeControlsDetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsDetuneKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsDetuneKnob.setValue(0.0);
    volumeControlsDetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsDetuneKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsDetuneKnob.addListener(this);
    addAndMakeVisible(volumeControlsDetuneKnob);
    
    volumeControlsStereoWidthLabel.setText("STEREO", juce::dontSendNotification);
    volumeControlsStereoWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsStereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsStereoWidthLabel.setJustificationType(juce::Justification::centred);
    volumeControlsStereoWidthLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsStereoWidthLabel);

    volumeControlsStereoWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsStereoWidthKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsStereoWidthKnob.setValue(0.5);
    volumeControlsStereoWidthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsStereoWidthKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsStereoWidthKnob.addListener(this);
    addAndMakeVisible(volumeControlsStereoWidthKnob);
    
    volumeControlsPanLabel.setText("PAN", juce::dontSendNotification);
    volumeControlsPanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsPanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsPanLabel.setJustificationType(juce::Justification::centred);
    volumeControlsPanLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsPanLabel);

    volumeControlsPanKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsPanKnob.setRange(-50.0, 50.0, 1.0);
    volumeControlsPanKnob.setValue(0.0);
    volumeControlsPanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsPanKnob.setLookAndFeel(&customKnobLookAndFeel);
    volumeControlsPanKnob.addListener(this);
    addAndMakeVisible(volumeControlsPanKnob);
    
    // PHASE CONTROLS GROUP - (STATIONARY)
    phaseControlsPhaseLabel.setText("PHASE", juce::dontSendNotification);
    phaseControlsPhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    phaseControlsPhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    phaseControlsPhaseLabel.setJustificationType(juce::Justification::centred);
    phaseControlsPhaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(phaseControlsPhaseLabel);

    phaseControlsPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaseControlsPhaseKnob.setRange(0.0, 360.0, 1.0);
    phaseControlsPhaseKnob.setValue(0.0);
    phaseControlsPhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    phaseControlsPhaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    phaseControlsPhaseKnob.addListener(this);
    addAndMakeVisible(phaseControlsPhaseKnob);
    
    // ADSR KNOBS GROUP - Row 3 (MOVEABLE)
    adsrAttackLabel.setText("ATTACK", juce::dontSendNotification);
    adsrAttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrAttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrAttackLabel.setJustificationType(juce::Justification::centred);
    adsrAttackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrAttackLabel);
    
    adsrAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrAttackKnob.setRange(0.01, 2.0, 0.01);
    adsrAttackKnob.setValue(0.1);
    adsrAttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrAttackKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrAttackKnob.addListener(this);
    addAndMakeVisible(adsrAttackKnob);
    
    adsrDecayLabel.setText("DECAY", juce::dontSendNotification);
    adsrDecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrDecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrDecayLabel.setJustificationType(juce::Justification::centred);
    adsrDecayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrDecayLabel);
    
    adsrDecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrDecayKnob.setRange(0.01, 2.0, 0.01);
    adsrDecayKnob.setValue(0.2);
    adsrDecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrDecayKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrDecayKnob.addListener(this);
    addAndMakeVisible(adsrDecayKnob);
    
    adsrSustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    adsrSustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrSustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrSustainLabel.setJustificationType(juce::Justification::centred);
    adsrSustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrSustainLabel);
    
    adsrSustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrSustainKnob.setRange(0.0, 1.0, 0.01);
    adsrSustainKnob.setValue(0.7);
    adsrSustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrSustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrSustainKnob.addListener(this);
    addAndMakeVisible(adsrSustainKnob);
    
    adsrReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    adsrReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    adsrReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    adsrReleaseLabel.setJustificationType(juce::Justification::centred);
    adsrReleaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrReleaseLabel);
    
    adsrReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrReleaseKnob.setRange(0.01, 3.0, 0.01);
    adsrReleaseKnob.setValue(0.3);
    adsrReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrReleaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrReleaseKnob.addListener(this);
    addAndMakeVisible(adsrReleaseKnob);
    
    // Pulse width slider (commented out for now)
    /*
    pulseWidthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pulseWidthSlider.setRange(0.1, 0.9, 0.01);
    pulseWidthSlider.setValue(0.5);
    pulseWidthLabel.setText("Pulse Width", juce::dontSendNotification);
    pulseWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pulseWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pulseWidthLabel.setJustificationType(juce::Justification::centred);
    pulseWidthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    pulseWidthSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    pulseWidthSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    pulseWidthSlider.setColour(juce::Slider::trackColourId, juce::Colours::grey);
    pulseWidthSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    pulseWidthSlider.addListener(this);
    addAndMakeVisible(pulseWidthSlider);
    addAndMakeVisible(pulseWidthLabel);
    */
    
    // PITCH CONTROLS GROUP - Row 5 (MOVEABLE)
    pitchControlsOctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    pitchControlsOctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsOctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsOctaveLabel);
    
    pitchControlsOctaveValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsOctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsOctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsOctaveValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsOctaveValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsOctaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsOctaveValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsOctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsOctaveValueLabel);
    
    pitchControlsSemitoneLabel.setText("SEMI", juce::dontSendNotification);
    pitchControlsSemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsSemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsSemitoneLabel);
    
    pitchControlsSemitoneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsSemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsSemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsSemitoneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsSemitoneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsSemitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsSemitoneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsSemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsSemitoneValueLabel);
    
    pitchControlsFineTuneLabel.setText("FINE", juce::dontSendNotification);
    pitchControlsFineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsFineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsFineTuneLabel);
    
    pitchControlsFineTuneValueLabel.setText("0", juce::dontSendNotification);
    pitchControlsFineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsFineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsFineTuneValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsFineTuneValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsFineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsFineTuneValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsFineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsFineTuneValueLabel);
    
    pitchControlsVoiceCountLabel.setText("VOICES", juce::dontSendNotification);
    pitchControlsVoiceCountLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    pitchControlsVoiceCountLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(pitchControlsVoiceCountLabel);
    
    pitchControlsVoiceCountValueLabel.setText("1", juce::dontSendNotification);
    pitchControlsVoiceCountValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    pitchControlsVoiceCountValueLabel.setColour(juce::Label::outlineColourId, juce::Colours::white);
    pitchControlsVoiceCountValueLabel.setJustificationType(juce::Justification::centred);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    pitchControlsVoiceCountValueLabel.setInterceptsMouseClicks(true, false);
    pitchControlsVoiceCountValueLabel.addMouseListener(this, false);
    addAndMakeVisible(pitchControlsVoiceCountValueLabel);
    
    // WAVE TYPE BUTTONS GROUP - Row 1 (MOVEABLE)
    waveTypeSineButton.setButtonText("SIN");
    // Custom look and feel handles all colors
    waveTypeSineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSineButton.setClickingTogglesState(true);
    waveTypeSineButton.setToggleState(false, juce::dontSendNotification); // Sine not selected by default
    waveTypeSineButton.addListener(this);
    addAndMakeVisible(waveTypeSineButton);
    
    waveTypeSawButton.setButtonText("SAW");
    // Custom look and feel handles all colors
    waveTypeSawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSawButton.setClickingTogglesState(true);
    waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Start with saw selected
    waveTypeSawButton.addListener(this);
    addAndMakeVisible(waveTypeSawButton);
    
    // Set oscillator 1 to saw wave by default
    audioProcessor.setOsc1Type(1); // 1 = saw wave
    
    waveTypeSquareButton.setButtonText("SQR");
    // Custom look and feel handles all colors
    waveTypeSquareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSquareButton.setClickingTogglesState(true);
    waveTypeSquareButton.addListener(this);
    addAndMakeVisible(waveTypeSquareButton);
    
    waveTypeTriangleButton.setButtonText("TRI");
    // Custom look and feel handles all colors
    waveTypeTriangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeTriangleButton.setClickingTogglesState(true);
    waveTypeTriangleButton.addListener(this);
    addAndMakeVisible(waveTypeTriangleButton);
    
    waveTypeWhiteNoiseButton.setButtonText("WHT");
    // Custom look and feel handles all colors
    waveTypeWhiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeWhiteNoiseButton.setClickingTogglesState(true);
    waveTypeWhiteNoiseButton.addListener(this);
    addAndMakeVisible(waveTypeWhiteNoiseButton);
    
    waveTypePinkNoiseButton.setButtonText("PNK");
    // Custom look and feel handles all colors
    waveTypePinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypePinkNoiseButton.setClickingTogglesState(true);
    waveTypePinkNoiseButton.addListener(this);
    addAndMakeVisible(waveTypePinkNoiseButton);
    
    waveTypeRandomPhaseButton.setButtonText("RND PHASE");
    // Custom look and feel handles all colors
    waveTypeRandomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeRandomPhaseButton.setClickingTogglesState(true);
    waveTypeRandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Default to random phase ON
    waveTypeRandomPhaseButton.addListener(this);
    addAndMakeVisible(waveTypeRandomPhaseButton);
    
    // SECOND OSCILLATOR CONTROLS - Row 6 (MOVEABLE)
    // osc2TitleLabel removed
    
    osc2SineButton.setButtonText("SIN");
    osc2SineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SineButton.setClickingTogglesState(true);
    osc2SineButton.setToggleState(false, juce::dontSendNotification); // Sine not selected by default
    osc2SineButton.addListener(this);
    addAndMakeVisible(osc2SineButton);
    
    osc2VoicesLabel.setText("VOICES", juce::dontSendNotification);
    osc2VoicesLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VoicesLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2VoicesLabel);
    
    osc2VoicesValueLabel.setText("1", juce::dontSendNotification);
    osc2VoicesValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2VoicesValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VoicesValueLabel.setJustificationType(juce::Justification::centred);
    osc2VoicesValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2VoicesValueLabel.setInterceptsMouseClicks(true, false);
    osc2VoicesValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2VoicesValueLabel);
    
    osc2PanLabel.setText("PAN", juce::dontSendNotification);
    osc2PanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PanLabel.setJustificationType(juce::Justification::centred);
    osc2PanLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2PanLabel);
    
    osc2PanKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    osc2PanKnob.setRange(-1.0, 1.0, 0.01);
    osc2PanKnob.setValue(0.0);
    osc2PanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2PanKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2PanKnob.addListener(this);
    addAndMakeVisible(osc2PanKnob);
    
    osc2OctaveLabel.setText("OCTAVE", juce::dontSendNotification);
    osc2OctaveLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2OctaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2OctaveLabel);
    
    osc2OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc2OctaveValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2OctaveValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2OctaveValueLabel.setInterceptsMouseClicks(true, false);
    osc2OctaveValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2OctaveValueLabel);
    
    osc2SemitoneLabel.setText("SEMI", juce::dontSendNotification);
    osc2SemitoneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SemitoneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2SemitoneLabel);
    
    osc2SemitoneValueLabel.setText("0", juce::dontSendNotification);
    osc2SemitoneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2SemitoneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SemitoneValueLabel.setJustificationType(juce::Justification::centred);
    osc2SemitoneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2SemitoneValueLabel.setInterceptsMouseClicks(true, false);
    osc2SemitoneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2SemitoneValueLabel);
    
    osc2FineTuneLabel.setText("FINE", juce::dontSendNotification);
    osc2FineTuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2FineTuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2FineTuneLabel);
    
    osc2FineTuneValueLabel.setText("0", juce::dontSendNotification);
    osc2FineTuneValueLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    osc2FineTuneValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2FineTuneValueLabel.setJustificationType(juce::Justification::centred);
    osc2FineTuneValueLabel.setLookAndFeel(&ledNumberLookAndFeel);
    osc2FineTuneValueLabel.setInterceptsMouseClicks(true, false);
    osc2FineTuneValueLabel.addMouseListener(this, false);
    addAndMakeVisible(osc2FineTuneValueLabel);
    
    osc2RandomPhaseButton.setButtonText("RND PHASE");
    osc2RandomPhaseButton.setClickingTogglesState(true);
    osc2RandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    osc2RandomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2RandomPhaseButton.addListener(this);
    addAndMakeVisible(osc2RandomPhaseButton);
    
    osc2PhaseLabel.setText("PHASE", juce::dontSendNotification);
    osc2PhaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2PhaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2PhaseLabel.setJustificationType(juce::Justification::centred);
    osc2PhaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2PhaseLabel);

    osc2PhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2PhaseKnob.setRange(0.0, 360.0, 1.0);
    osc2PhaseKnob.setValue(0.0);
    osc2PhaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 20);
    osc2PhaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2PhaseKnob.addListener(this);
    addAndMakeVisible(osc2PhaseKnob);
    
    // Set oscillator 2 to saw wave and enable it by default
    audioProcessor.setOsc2Enabled(true);
    audioProcessor.setOsc2Type(1); // 1 = saw wave
    
    osc2SawButton.setButtonText("SAW");
    osc2SawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SawButton.setClickingTogglesState(true);
    osc2SawButton.setToggleState(true, juce::dontSendNotification); // Start selected
    osc2SawButton.addListener(this);
    addAndMakeVisible(osc2SawButton);
    
    osc2SquareButton.setButtonText("SQR");
    osc2SquareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2SquareButton.setClickingTogglesState(true);
    osc2SquareButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2SquareButton.addListener(this);
    addAndMakeVisible(osc2SquareButton);
    
    osc2TriangleButton.setButtonText("TRI");
    osc2TriangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2TriangleButton.setClickingTogglesState(true);
    osc2TriangleButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2TriangleButton.addListener(this);
    addAndMakeVisible(osc2TriangleButton);
    
    osc2WhiteNoiseButton.setButtonText("WHT");
    osc2WhiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2WhiteNoiseButton.setClickingTogglesState(true);
    osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2WhiteNoiseButton.addListener(this);
    addAndMakeVisible(osc2WhiteNoiseButton);
    
    osc2PinkNoiseButton.setButtonText("PNK");
    osc2PinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2PinkNoiseButton.setClickingTogglesState(true);
    osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification); // Start unselected
    osc2PinkNoiseButton.addListener(this);
    addAndMakeVisible(osc2PinkNoiseButton);
    
    osc2VolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2VolumeKnob.setRange(0.0, 1.0, 0.01); // Same range as oscillator 1
    osc2VolumeKnob.setValue(0.0); // Start oscillator 2 at 0 volume (off by default)
    osc2VolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2VolumeKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2VolumeKnob.addListener(this);
    addAndMakeVisible(osc2VolumeKnob);

    osc2DetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DetuneKnob.setRange(0.0, 1.0, 0.01);
    osc2DetuneKnob.setValue(0.0); // Start at 0 detune
    osc2DetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DetuneKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2DetuneKnob.addListener(this);
    addAndMakeVisible(osc2DetuneKnob);

    osc2StereoKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2StereoKnob.setRange(0.0, 1.0, 0.01);
    osc2StereoKnob.setValue(0.5); // Start at 50% stereo width
    osc2StereoKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2StereoKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2StereoKnob.addListener(this);
    addAndMakeVisible(osc2StereoKnob);
    
    osc2VolumeLabel.setText("VOLUME", juce::dontSendNotification);
    osc2VolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2VolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2VolumeLabel.setJustificationType(juce::Justification::centred);
    osc2VolumeLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2VolumeLabel);

    osc2DetuneLabel.setText("DETUNE", juce::dontSendNotification);
    osc2DetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DetuneLabel.setJustificationType(juce::Justification::centred);
    osc2DetuneLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2DetuneLabel);

    osc2StereoLabel.setText("STEREO", juce::dontSendNotification);
    osc2StereoLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2StereoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2StereoLabel.setJustificationType(juce::Justification::centred);
    osc2StereoLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2StereoLabel);
    
    // Oscillator 2 ADSR controls
    osc2AttackLabel.setText("ATTACK", juce::dontSendNotification);
    osc2AttackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2AttackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2AttackLabel.setJustificationType(juce::Justification::centred);
    osc2AttackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2AttackLabel);
    
    osc2AttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2AttackKnob.setRange(0.01, 2.0, 0.01);
    osc2AttackKnob.setValue(0.1);
    osc2AttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2AttackKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2AttackKnob.addListener(this);
    addAndMakeVisible(osc2AttackKnob);
    
    osc2DecayLabel.setText("DECAY", juce::dontSendNotification);
    osc2DecayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2DecayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2DecayLabel.setJustificationType(juce::Justification::centred);
    osc2DecayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2DecayLabel);
    
    osc2DecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DecayKnob.setRange(0.01, 2.0, 0.01);
    osc2DecayKnob.setValue(0.2);
    osc2DecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2DecayKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2DecayKnob.addListener(this);
    addAndMakeVisible(osc2DecayKnob);
    
    osc2SustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    osc2SustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2SustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2SustainLabel.setJustificationType(juce::Justification::centred);
    osc2SustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2SustainLabel);
    
    osc2SustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2SustainKnob.setRange(0.0, 1.0, 0.01);
    osc2SustainKnob.setValue(0.7);
    osc2SustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2SustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2SustainKnob.addListener(this);
    addAndMakeVisible(osc2SustainKnob);
    
    osc2ReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    osc2ReleaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    osc2ReleaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    osc2ReleaseLabel.setJustificationType(juce::Justification::centred);
    osc2ReleaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(osc2ReleaseLabel);
    
    osc2ReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2ReleaseKnob.setRange(0.01, 3.0, 0.01);
    osc2ReleaseKnob.setValue(0.3);
    osc2ReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    osc2ReleaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    osc2ReleaseKnob.addListener(this);
    addAndMakeVisible(osc2ReleaseKnob);
    
    osc2AdsrLinkButton.setButtonText("OSC1 ADSR");
    osc2AdsrLinkButton.setClickingTogglesState(true);
    osc2AdsrLinkButton.setToggleState(false, juce::dontSendNotification); // Off by default
    osc2AdsrLinkButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2AdsrLinkButton.addListener(this);
    addAndMakeVisible(osc2AdsrLinkButton);
    
    // FILTER CONTROLS
    filterCutoffLabel.setText("CUTOFF", juce::dontSendNotification);
    filterCutoffLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    filterCutoffLabel.setJustificationType(juce::Justification::centred);
    filterCutoffLabel.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(filterCutoffLabel);
    
    filterCutoffKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffKnob.setRange(20.0, 20000.0, 1.0);
    filterCutoffKnob.setValue(1000.0);
    filterCutoffKnob.setSkewFactorFromMidPoint(1000.0);
    filterCutoffKnob.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterCutoffKnob.setLookAndFeel(&digitalFilterKnobLookAndFeel);
    filterCutoffKnob.addListener(this);
    addAndMakeVisible(filterCutoffKnob);
    
    // Filter Resonance knob
    filterResonanceKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterResonanceKnob.setRange(0.0, 1.0, 0.01);
    filterResonanceKnob.setValue(0.0);
    filterResonanceKnob.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterResonanceKnob.setLookAndFeel(&digitalFilterKnobLookAndFeel);
    filterResonanceKnob.addListener(this);
    addAndMakeVisible(filterResonanceKnob);
    
    // Filter Resonance label
    filterResonanceLabel.setText("RES", juce::dontSendNotification);
    filterResonanceLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    filterResonanceLabel.setJustificationType(juce::Justification::centred);
    filterResonanceLabel.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(filterResonanceLabel);
    
    // OSC filter enable buttons
    osc1FilterEnableButton.setButtonText("OSC 1");
    osc1FilterEnableButton.setClickingTogglesState(true);
    osc1FilterEnableButton.setToggleState(false, juce::dontSendNotification); // Off by default
    osc1FilterEnableButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc1FilterEnableButton.addListener(this);
    addAndMakeVisible(osc1FilterEnableButton);
    
    osc2FilterEnableButton.setButtonText("OSC 2");
    osc2FilterEnableButton.setClickingTogglesState(true);
    osc2FilterEnableButton.setToggleState(false, juce::dontSendNotification); // Off by default
    osc2FilterEnableButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    osc2FilterEnableButton.addListener(this);
    addAndMakeVisible(osc2FilterEnableButton);
    
    // Filter type buttons (Low-pass and High-pass) - radio button behavior
    filterLPButton.setButtonText("LP");
    filterLPButton.setClickingTogglesState(true);
    filterLPButton.setToggleState(true, juce::dontSendNotification); // LP selected by default
    filterLPButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterLPButton.addListener(this);
    addAndMakeVisible(filterLPButton);
    
    filterHPButton.setButtonText("HP");
    filterHPButton.setClickingTogglesState(true);
    filterHPButton.setToggleState(false, juce::dontSendNotification); // HP off by default
    filterHPButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterHPButton.addListener(this);
    addAndMakeVisible(filterHPButton);
    
    filterBPButton.setButtonText("BP");
    filterBPButton.setClickingTogglesState(true);
    filterBPButton.setToggleState(false, juce::dontSendNotification); // BP off by default
    filterBPButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterBPButton.addListener(this);
    addAndMakeVisible(filterBPButton);
    
    filterNotchButton.setButtonText("NTCH");
    filterNotchButton.setClickingTogglesState(true);
    filterNotchButton.setToggleState(false, juce::dontSendNotification); // NOTCH off by default
    filterNotchButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterNotchButton.addListener(this);
    addAndMakeVisible(filterNotchButton);
    
    filterCombButton.setButtonText("COMB");
    filterCombButton.setClickingTogglesState(true);
    filterCombButton.setToggleState(false, juce::dontSendNotification); // COMB off by default
    filterCombButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterCombButton.addListener(this);
    addAndMakeVisible(filterCombButton);
    
    filterFormantButton.setButtonText("FMNT");
    filterFormantButton.setClickingTogglesState(true);
    filterFormantButton.setToggleState(false, juce::dontSendNotification); // FORMANT off by default
    filterFormantButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filterFormantButton.addListener(this);
    addAndMakeVisible(filterFormantButton);
    
    // Filter slope buttons (12dB and 24dB) - radio button behavior
    filter12dBButton.setButtonText("12dB");
    filter12dBButton.setClickingTogglesState(true);
    filter12dBButton.setToggleState(true, juce::dontSendNotification); // 12dB selected by default
    filter12dBButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filter12dBButton.addListener(this);
    addAndMakeVisible(filter12dBButton);
    
    filter24dBButton.setButtonText("24dB");
    filter24dBButton.setClickingTogglesState(true);
    filter24dBButton.setToggleState(false, juce::dontSendNotification); // 24dB off by default
    filter24dBButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    filter24dBButton.addListener(this);
    addAndMakeVisible(filter24dBButton);
    
    // MACRO CONTROLS - 4 placeholder knobs
    macro1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro1Knob.setRange(0.0, 1.0, 0.01);
    macro1Knob.setValue(0.5);
    macro1Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro1Knob.addListener(this);
    addAndMakeVisible(macro1Knob);
    
    macro1Label.setText("MACRO 1", juce::dontSendNotification);
    macro1Label.setJustificationType(juce::Justification::centred);
    macro1Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro1Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro1Label);
    
    macro2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro2Knob.setRange(0.0, 1.0, 0.01);
    macro2Knob.setValue(0.5);
    macro2Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro2Knob.addListener(this);
    addAndMakeVisible(macro2Knob);
    
    macro2Label.setText("MACRO 2", juce::dontSendNotification);
    macro2Label.setJustificationType(juce::Justification::centred);
    macro2Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro2Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro2Label);
    
    macro3Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro3Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro3Knob.setRange(0.0, 1.0, 0.01);
    macro3Knob.setValue(0.5);
    macro3Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro3Knob.addListener(this);
    addAndMakeVisible(macro3Knob);
    
    macro3Label.setText("MACRO 3", juce::dontSendNotification);
    macro3Label.setJustificationType(juce::Justification::centred);
    macro3Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro3Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro3Label);
    
    macro4Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro4Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro4Knob.setRange(0.0, 1.0, 0.01);
    macro4Knob.setValue(0.5);
    macro4Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro4Knob.addListener(this);
    addAndMakeVisible(macro4Knob);
    
    macro4Label.setText("MACRO 4", juce::dontSendNotification);
    macro4Label.setJustificationType(juce::Justification::centred);
    macro4Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro4Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro4Label);
    
    macro5Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro5Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro5Knob.setRange(0.0, 1.0, 0.01);
    macro5Knob.setValue(0.5);
    macro5Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro5Knob.addListener(this);
    addAndMakeVisible(macro5Knob);
    
    macro5Label.setText("MACRO 5", juce::dontSendNotification);
    macro5Label.setJustificationType(juce::Justification::centred);
    macro5Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro5Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro5Label);
    
    macro6Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro6Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro6Knob.setRange(0.0, 1.0, 0.01);
    macro6Knob.setValue(0.5);
    macro6Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro6Knob.addListener(this);
    addAndMakeVisible(macro6Knob);
    
    macro6Label.setText("MACRO 6", juce::dontSendNotification);
    macro6Label.setJustificationType(juce::Justification::centred);
    macro6Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro6Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro6Label);
    
    macro7Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro7Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro7Knob.setRange(0.0, 1.0, 0.01);
    macro7Knob.setValue(0.5);
    macro7Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro7Knob.addListener(this);
    addAndMakeVisible(macro7Knob);
    
    macro7Label.setText("MACRO 7", juce::dontSendNotification);
    macro7Label.setJustificationType(juce::Justification::centred);
    macro7Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro7Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro7Label);
    
    macro8Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    macro8Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    macro8Knob.setRange(0.0, 1.0, 0.01);
    macro8Knob.setValue(0.5);
    macro8Knob.setLookAndFeel(&simpleKnobLookAndFeel);
    macro8Knob.addListener(this);
    addAndMakeVisible(macro8Knob);
    
    macro8Label.setText("MACRO 8", juce::dontSendNotification);
    macro8Label.setJustificationType(juce::Justification::centred);
    macro8Label.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    macro8Label.setLookAndFeel(&engravedLabelLookAndFeel);
    addAndMakeVisible(macro8Label);
    
    // Initialize draggable macro symbols
    macroSymbol1 = std::make_unique<DraggableMacroSymbol>(1, this);
    macroSymbol2 = std::make_unique<DraggableMacroSymbol>(2, this);
    macroSymbol3 = std::make_unique<DraggableMacroSymbol>(3, this);
    macroSymbol4 = std::make_unique<DraggableMacroSymbol>(4, this);
    macroSymbol5 = std::make_unique<DraggableMacroSymbol>(5, this);
    macroSymbol6 = std::make_unique<DraggableMacroSymbol>(6, this);
    macroSymbol7 = std::make_unique<DraggableMacroSymbol>(7, this);
    macroSymbol8 = std::make_unique<DraggableMacroSymbol>(8, this);
    
    addAndMakeVisible(*macroSymbol1);
    addAndMakeVisible(*macroSymbol2);
    addAndMakeVisible(*macroSymbol3);
    addAndMakeVisible(*macroSymbol4);
    addAndMakeVisible(*macroSymbol5);
    addAndMakeVisible(*macroSymbol6);
    addAndMakeVisible(*macroSymbol7);
    addAndMakeVisible(*macroSymbol8);
    
    // ADSR ENVELOPE VISUALIZER GROUP - Row 2 (MOVEABLE)
    addAndMakeVisible(adsrEnvelopeVisualizer);
    
    // OSCILLATOR 2 ADSR ENVELOPE VISUALIZER
    addAndMakeVisible(osc2AdsrEnvelopeVisualizer);
    
    // LFO MODULE
    lfoModule.setLookAndFeel(&customKnobLookAndFeel, &customWaveButtonLookAndFeel, &ledLabelLookAndFeel);
    addAndMakeVisible(lfoModule);
    
    // EFFECTS MODULE - Digital Screen Style
    effectsModule.setTabBarDepth(29); // Back to single row height
    effectsModule.setCurrentTabIndex(4); // Set to EQ tab for testing
    
    // Enable multiple rows for tabs - force tabs to wrap by setting a much smaller minimum scale
    effectsModule.getTabbedButtonBar().setMinimumTabScaleFactor(0.3f); // Allow much smaller tabs to force wrapping
    
    // Set digital screen colors
    effectsModule.setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xff000008)); // Very dark blue-black
    effectsModule.setColour(juce::TabbedComponent::outlineColourId, juce::Colour(0xff00ff00).withAlpha(0.8f)); // Bright green outline
    
    // Create placeholder components for each effect with digital styling
    auto chorusTab = new juce::Component();
    auto compressorTab = new juce::Component();
    auto delayTab = new juce::Component();
    auto distortionTab = new juce::Component();
    auto equalizerTab = new juce::Component();
    equalizerTab->setVisible(true);
    
    // Initialize parametric EQ component
    parametricEQ.setParentSynthesizer(this);
    parametricEQ.syncWithDSPState(); // Sync with initial DSP state
    parametricEQ.setVisible(true);
    equalizerTab->addAndMakeVisible(parametricEQ);
    
    // Initialize EQ on/off button - positioned above the EQ graph
    eqOnOffButton.setButtonText("EQ ON");
    eqOnOffButton.setClickingTogglesState(true);
    eqOnOffButton.setToggleState(false, juce::dontSendNotification); // EQ disabled by default
    eqOnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eqOnOffButton.addListener(this);
    eqOnOffButton.setVisible(true);
    eqOnOffButton.setBounds(120, 50, 60, 25); // Centered above EQ graph, moved higher
    equalizerTab->addAndMakeVisible(eqOnOffButton);
    
    // Set initial bounds with space for button above (will be properly laid out in resized())
    parametricEQ.setBounds(0, 95, 300, 200);
    
    auto flangerTab = new juce::Component();
    
    // Initialize flanger controls
    // Row 1: Power and Mix controls
    flangerPowerButton.setButtonText("FLANGER ON");
    flangerPowerButton.setClickingTogglesState(true);
    flangerPowerButton.setToggleState(false, juce::dontSendNotification);
    flangerPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    flangerPowerButton.setVisible(true);
    flangerPowerButton.setBounds(68, 130, 80, 25);
    flangerTab->addAndMakeVisible(flangerPowerButton);
    
    flangerMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerMixKnob.setRange(0.0, 100.0, 1.0);
    flangerMixKnob.setValue(50.0);
    flangerMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerMixKnob.setVisible(true);
    flangerMixKnob.setBounds(168, 125, 40, 40);
    flangerTab->addAndMakeVisible(flangerMixKnob);
    
    flangerMixLabel.setText("MIX", juce::dontSendNotification);
    flangerMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerMixLabel.setJustificationType(juce::Justification::centred);
    flangerMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerMixLabel.setVisible(true);
    flangerMixLabel.setBounds(168, 167, 40, 12);
    flangerTab->addAndMakeVisible(flangerMixLabel);
    
    // Row 2: Rate, BPM Sync, and Depth
    flangerRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerRateKnob.setRange(0.1, 10.0, 0.1);
    flangerRateKnob.setValue(1.0);
    flangerRateKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerRateKnob.setVisible(true);
    flangerRateKnob.setBounds(86, 190, 50, 50);
    flangerTab->addAndMakeVisible(flangerRateKnob);
    
    flangerRateLabel.setText("RATE", juce::dontSendNotification);
    flangerRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerRateLabel.setJustificationType(juce::Justification::centred);
    flangerRateLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerRateLabel.setVisible(true);
    flangerRateLabel.setBounds(86, 245, 50, 15);
    flangerTab->addAndMakeVisible(flangerRateLabel);
    
    flangerBpmButton.setButtonText("BPM SYNC");
    flangerBpmButton.setClickingTogglesState(true);
    flangerBpmButton.setToggleState(false, juce::dontSendNotification);
    flangerBpmButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    flangerBpmButton.setVisible(true);
    flangerBpmButton.setBounds(146, 205, 70, 25);
    flangerTab->addAndMakeVisible(flangerBpmButton);
    
    flangerDepthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerDepthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerDepthKnob.setRange(0.0, 100.0, 1.0);
    flangerDepthKnob.setValue(50.0);
    flangerDepthKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerDepthKnob.setVisible(true);
    flangerDepthKnob.setBounds(195, 270, 50, 50);
    flangerTab->addAndMakeVisible(flangerDepthKnob);
    
    flangerDepthLabel.setText("DEPTH", juce::dontSendNotification);
    flangerDepthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerDepthLabel.setJustificationType(juce::Justification::centred);
    flangerDepthLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerDepthLabel.setVisible(true);
    flangerDepthLabel.setBounds(195, 325, 50, 15);
    flangerTab->addAndMakeVisible(flangerDepthLabel);
    
    // Row 3: Feedback and Phase
    flangerFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerFeedbackKnob.setRange(0.0, 100.0, 1.0);
    flangerFeedbackKnob.setValue(25.0);
    flangerFeedbackKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerFeedbackKnob.setVisible(true);
    flangerFeedbackKnob.setBounds(51, 270, 50, 50);
    flangerTab->addAndMakeVisible(flangerFeedbackKnob);
    
    flangerFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    flangerFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerFeedbackLabel.setJustificationType(juce::Justification::centred);
    flangerFeedbackLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerFeedbackLabel.setVisible(true);
    flangerFeedbackLabel.setBounds(51, 325, 50, 15);
    flangerTab->addAndMakeVisible(flangerFeedbackLabel);
    
    flangerPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flangerPhaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    flangerPhaseKnob.setRange(0.0, 360.0, 1.0);
    flangerPhaseKnob.setValue(0.0);
    flangerPhaseKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerPhaseKnob.setVisible(true);
    flangerPhaseKnob.setBounds(125, 270, 50, 50);
    flangerTab->addAndMakeVisible(flangerPhaseKnob);
    
    flangerPhaseLabel.setText("PHASE", juce::dontSendNotification);
    flangerPhaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    flangerPhaseLabel.setJustificationType(juce::Justification::centred);
    flangerPhaseLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    flangerPhaseLabel.setVisible(true);
    flangerPhaseLabel.setBounds(125, 325, 50, 15);
    flangerTab->addAndMakeVisible(flangerPhaseLabel);
    
    // Add listeners for flanger controls
    flangerPowerButton.addListener(this);
    flangerMixKnob.addListener(this);
    flangerRateKnob.addListener(this);
    flangerBpmButton.addListener(this);
    flangerDepthKnob.addListener(this);
    flangerFeedbackKnob.addListener(this);
    flangerPhaseKnob.addListener(this);
    
    auto phaserTab = new juce::Component();
    
    // Initialize phaser controls
    // Row 1: Power and Mix controls
    phaserPowerButton.setButtonText("PHASER ON");
    phaserPowerButton.setClickingTogglesState(true);
    phaserPowerButton.setToggleState(false, juce::dontSendNotification);
    phaserPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    phaserPowerButton.setVisible(true);
    phaserPowerButton.setBounds(69, 65, 80, 25);
    phaserTab->addAndMakeVisible(phaserPowerButton);
    
    phaserMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserMixKnob.setRange(0.0, 100.0, 1.0);
    phaserMixKnob.setValue(50.0);
    phaserMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserMixKnob.setVisible(true);
    phaserMixKnob.setBounds(169, 60, 40, 40);
    phaserTab->addAndMakeVisible(phaserMixKnob);
    
    phaserMixLabel.setText("MIX", juce::dontSendNotification);
    phaserMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserMixLabel.setJustificationType(juce::Justification::centred);
    phaserMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserMixLabel.setVisible(true);
    phaserMixLabel.setBounds(169, 102, 40, 12);
    phaserTab->addAndMakeVisible(phaserMixLabel);
    
    // Row 2: Rate, BPM Sync, and Poles
    phaserRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserRateKnob.setRange(0.1, 10.0, 0.1);
    phaserRateKnob.setValue(1.0);
    phaserRateKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserRateKnob.setVisible(true);
    phaserRateKnob.setBounds(83, 125, 50, 50);
    phaserTab->addAndMakeVisible(phaserRateKnob);
    
    phaserRateLabel.setText("RATE", juce::dontSendNotification);
    phaserRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserRateLabel.setJustificationType(juce::Justification::centred);
    phaserRateLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserRateLabel.setVisible(true);
    phaserRateLabel.setBounds(83, 180, 50, 15);
    phaserTab->addAndMakeVisible(phaserRateLabel);
    
    phaserBpmButton.setButtonText("BPM SYNC");
    phaserBpmButton.setClickingTogglesState(true);
    phaserBpmButton.setToggleState(false, juce::dontSendNotification);
    phaserBpmButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    phaserBpmButton.setVisible(true);
    phaserBpmButton.setBounds(153, 140, 70, 25);
    phaserTab->addAndMakeVisible(phaserBpmButton);
    
    phaserPolesValueLabel.setText("4 POLES", juce::dontSendNotification);
    phaserPolesValueLabel.setLookAndFeel(&greenLEDNumberLookAndFeel);
    phaserPolesValueLabel.setInterceptsMouseClicks(true, true);
    phaserPolesValueLabel.addMouseListener(this, true);
    phaserPolesValueLabel.setVisible(true);
    phaserPolesValueLabel.setBounds(98, 200, 100, 25);
    phaserTab->addAndMakeVisible(phaserPolesValueLabel);
    
    phaserPolesLabel.setText("POLES", juce::dontSendNotification);
    phaserPolesLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserPolesLabel.setJustificationType(juce::Justification::centred);
    phaserPolesLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserPolesLabel.setVisible(true);
    phaserPolesLabel.setBounds(98, 230, 100, 15);
    phaserTab->addAndMakeVisible(phaserPolesLabel);
    
    // Row 3: Depth 1 and Depth 2a 
    phaserDepth1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserDepth1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserDepth1Knob.setRange(0.0, 100.0, 1.0);
    phaserDepth1Knob.setValue(50.0);
    phaserDepth1Knob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserDepth1Knob.setVisible(true);
    phaserDepth1Knob.setBounds(89, 260, 50, 50);
    phaserTab->addAndMakeVisible(phaserDepth1Knob);
    
    phaserDepth1Label.setText("DEPTH 1", juce::dontSendNotification);
    phaserDepth1Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserDepth1Label.setJustificationType(juce::Justification::centred);
    phaserDepth1Label.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserDepth1Label.setVisible(true);
    phaserDepth1Label.setBounds(89, 315, 50, 15);
    phaserTab->addAndMakeVisible(phaserDepth1Label);
    
    phaserDepth2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserDepth2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserDepth2Knob.setRange(0.0, 100.0, 1.0);
    phaserDepth2Knob.setValue(30.0);
    phaserDepth2Knob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserDepth2Knob.setVisible(true);
    phaserDepth2Knob.setBounds(159, 260, 50, 50);
    phaserTab->addAndMakeVisible(phaserDepth2Knob);
    
    phaserDepth2Label.setText("DEPTH 2", juce::dontSendNotification);
    phaserDepth2Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserDepth2Label.setJustificationType(juce::Justification::centred);
    phaserDepth2Label.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserDepth2Label.setVisible(true);
    phaserDepth2Label.setBounds(159, 315, 50, 15);
    phaserTab->addAndMakeVisible(phaserDepth2Label);
    
    // Row 4: Frequency, Feedback, and Phase
    phaserFrequencyKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserFrequencyKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserFrequencyKnob.setRange(20.0, 2000.0, 1.0);
    phaserFrequencyKnob.setValue(500.0);
    phaserFrequencyKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserFrequencyKnob.setVisible(true);
    phaserFrequencyKnob.setBounds(54, 340, 50, 50);
    phaserTab->addAndMakeVisible(phaserFrequencyKnob);
    
    phaserFrequencyLabel.setText("FREQ", juce::dontSendNotification);
    phaserFrequencyLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserFrequencyLabel.setJustificationType(juce::Justification::centred);
    phaserFrequencyLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserFrequencyLabel.setVisible(true);
    phaserFrequencyLabel.setBounds(54, 395, 50, 15);
    phaserTab->addAndMakeVisible(phaserFrequencyLabel);
    
    phaserFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserFeedbackKnob.setRange(0.0, 100.0, 1.0);
    phaserFeedbackKnob.setValue(25.0);
    phaserFeedbackKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserFeedbackKnob.setVisible(true);
    phaserFeedbackKnob.setBounds(124, 340, 50, 50);
    phaserTab->addAndMakeVisible(phaserFeedbackKnob);
    
    phaserFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    phaserFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserFeedbackLabel.setJustificationType(juce::Justification::centred);
    phaserFeedbackLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserFeedbackLabel.setVisible(true);
    phaserFeedbackLabel.setBounds(124, 395, 50, 15);
    phaserTab->addAndMakeVisible(phaserFeedbackLabel);
    
    phaserPhaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    phaserPhaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    phaserPhaseKnob.setRange(0.0, 360.0, 1.0);
    phaserPhaseKnob.setValue(0.0);
    phaserPhaseKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserPhaseKnob.setVisible(true);
    phaserPhaseKnob.setBounds(194, 340, 50, 50);
    phaserTab->addAndMakeVisible(phaserPhaseKnob);
    
    phaserPhaseLabel.setText("PHASE", juce::dontSendNotification);
    phaserPhaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    phaserPhaseLabel.setJustificationType(juce::Justification::centred);
    phaserPhaseLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    phaserPhaseLabel.setVisible(true);
    phaserPhaseLabel.setBounds(194, 395, 50, 15);
    phaserTab->addAndMakeVisible(phaserPhaseLabel);
    
    // Add listeners for phaser controls
    phaserPowerButton.addListener(this);
    phaserMixKnob.addListener(this);
    phaserRateKnob.addListener(this);
    phaserBpmButton.addListener(this);
    phaserDepth1Knob.addListener(this);
    phaserDepth2Knob.addListener(this);
    phaserFrequencyKnob.addListener(this);
    phaserFeedbackKnob.addListener(this);
    phaserPhaseKnob.addListener(this);
    
    auto reverbTab = new juce::Component();
    
    // Initialize reverb controls
    // Row 1: Power and Mix controls
    reverbPowerButton.setButtonText("REVERB ON");
    reverbPowerButton.setClickingTogglesState(true);
    reverbPowerButton.setToggleState(false, juce::dontSendNotification);
    reverbPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    reverbPowerButton.setVisible(true);
    reverbPowerButton.setBounds(68, 65, 80, 25);
    reverbTab->addAndMakeVisible(reverbPowerButton);
    reverbPowerButton.addListener(this);
    
    reverbMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbMixKnob.setRange(0.0, 100.0, 1.0);
    reverbMixKnob.setValue(30.0);
    reverbMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbMixKnob.setVisible(true);
    reverbMixKnob.setBounds(168, 60, 40, 40);
    reverbTab->addAndMakeVisible(reverbMixKnob);
    reverbMixKnob.addListener(this);
    
    reverbMixLabel.setText("MIX", juce::dontSendNotification);
    reverbMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbMixLabel.setJustificationType(juce::Justification::centred);
    reverbMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbMixLabel.setVisible(true);
    reverbMixLabel.setBounds(168, 102, 40, 12);
    reverbTab->addAndMakeVisible(reverbMixLabel);
    
    // Row 2: Reverb Type (clickable label like distortion)
    reverbTypeLabel.setText("TYPE", juce::dontSendNotification);
    reverbTypeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbTypeLabel.setJustificationType(juce::Justification::centred);
    reverbTypeLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbTypeLabel.setVisible(true);
    reverbTypeLabel.setBounds(49, 125, 200, 15);
    reverbTab->addAndMakeVisible(reverbTypeLabel);
    
    // Type value label (interactive)
    reverbTypeValueLabel.setText("HALL", juce::dontSendNotification);
    reverbTypeValueLabel.setJustificationType(juce::Justification::centred);
    reverbTypeValueLabel.setLookAndFeel(&greenLEDNumberLookAndFeel);
    reverbTypeValueLabel.setInterceptsMouseClicks(true, true); // Enable mouse interaction for self and children
    reverbTypeValueLabel.setEditable(false, false, false); // Make it respond to mouse but not editable
    reverbTypeValueLabel.addMouseListener(this, true); // Add mouse listener with childEvents = true
    reverbTypeValueLabel.setVisible(true);
    reverbTypeValueLabel.setBounds(49, 145, 200, 20);
    reverbTab->addAndMakeVisible(reverbTypeValueLabel);
    
    // Row 3: Low Cut and High Cut
    reverbLowCutKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbLowCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbLowCutKnob.setRange(20.0, 1000.0, 1.0);
    reverbLowCutKnob.setValue(80.0);
    reverbLowCutKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbLowCutKnob.setVisible(true);
    reverbLowCutKnob.setBounds(89, 185, 50, 50);
    reverbTab->addAndMakeVisible(reverbLowCutKnob);
    reverbLowCutKnob.addListener(this);
    
    reverbLowCutLabel.setText("LOW CUT", juce::dontSendNotification);
    reverbLowCutLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbLowCutLabel.setJustificationType(juce::Justification::centred);
    reverbLowCutLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbLowCutLabel.setVisible(true);
    reverbLowCutLabel.setBounds(89, 240, 50, 15);
    reverbTab->addAndMakeVisible(reverbLowCutLabel);
    
    reverbHighCutKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbHighCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbHighCutKnob.setRange(1000.0, 20000.0, 10.0);
    reverbHighCutKnob.setValue(8000.0);
    reverbHighCutKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbHighCutKnob.setVisible(true);
    reverbHighCutKnob.setBounds(159, 185, 50, 50);
    reverbTab->addAndMakeVisible(reverbHighCutKnob);
    reverbHighCutKnob.addListener(this);
    
    reverbHighCutLabel.setText("HIGH CUT", juce::dontSendNotification);
    reverbHighCutLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbHighCutLabel.setJustificationType(juce::Justification::centred);
    reverbHighCutLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbHighCutLabel.setVisible(true);
    reverbHighCutLabel.setBounds(159, 240, 50, 15);
    reverbTab->addAndMakeVisible(reverbHighCutLabel);
    
    // Row 4: Size and Pre Delay
    reverbSizeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbSizeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbSizeKnob.setRange(0.0, 100.0, 1.0);
    reverbSizeKnob.setValue(50.0);
    reverbSizeKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbSizeKnob.setVisible(true);
    reverbSizeKnob.setBounds(89, 265, 50, 50);
    reverbTab->addAndMakeVisible(reverbSizeKnob);
    reverbSizeKnob.addListener(this);
    
    reverbSizeLabel.setText("SIZE", juce::dontSendNotification);
    reverbSizeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbSizeLabel.setJustificationType(juce::Justification::centred);
    reverbSizeLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbSizeLabel.setVisible(true);
    reverbSizeLabel.setBounds(89, 320, 50, 15);
    reverbTab->addAndMakeVisible(reverbSizeLabel);
    
    reverbPreDelayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbPreDelayKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbPreDelayKnob.setRange(0.0, 200.0, 1.0);
    reverbPreDelayKnob.setValue(20.0);
    reverbPreDelayKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbPreDelayKnob.setVisible(true);
    reverbPreDelayKnob.setBounds(159, 265, 50, 50);
    reverbTab->addAndMakeVisible(reverbPreDelayKnob);
    reverbPreDelayKnob.addListener(this);
    
    reverbPreDelayLabel.setText("PRE DELAY", juce::dontSendNotification);
    reverbPreDelayLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbPreDelayLabel.setJustificationType(juce::Justification::centred);
    reverbPreDelayLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbPreDelayLabel.setVisible(true);
    reverbPreDelayLabel.setBounds(159, 320, 50, 15);
    reverbTab->addAndMakeVisible(reverbPreDelayLabel);
    
    // Row 5: Damp and Width
    reverbDampKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbDampKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbDampKnob.setRange(0.0, 100.0, 1.0);
    reverbDampKnob.setValue(50.0);
    reverbDampKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbDampKnob.setVisible(true);
    reverbDampKnob.setBounds(89, 345, 50, 50);
    reverbTab->addAndMakeVisible(reverbDampKnob);
    reverbDampKnob.addListener(this);
    
    reverbDampLabel.setText("DAMP", juce::dontSendNotification);
    reverbDampLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbDampLabel.setJustificationType(juce::Justification::centred);
    reverbDampLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbDampLabel.setVisible(true);
    reverbDampLabel.setBounds(89, 400, 50, 15);
    reverbTab->addAndMakeVisible(reverbDampLabel);
    
    reverbWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbWidthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    reverbWidthKnob.setRange(0.0, 100.0, 1.0);
    reverbWidthKnob.setValue(80.0);
    reverbWidthKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbWidthKnob.setVisible(true);
    reverbWidthKnob.setBounds(159, 345, 50, 50);
    reverbTab->addAndMakeVisible(reverbWidthKnob);
    reverbWidthKnob.addListener(this);
    
    reverbWidthLabel.setText("WIDTH", juce::dontSendNotification);
    reverbWidthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    reverbWidthLabel.setJustificationType(juce::Justification::centred);
    reverbWidthLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    reverbWidthLabel.setVisible(true);
    reverbWidthLabel.setBounds(159, 400, 50, 15);
    reverbTab->addAndMakeVisible(reverbWidthLabel);
    
    // Digital screen background color for tabs
    juce::Colour digitalBg = juce::Colour(0xff001100); // Dark green tint
    
    effectsModule.addTab("CHORUS", digitalBg, chorusTab, true);
    effectsModule.addTab("COMP", digitalBg, compressorTab, true);
    effectsModule.addTab("DELAY", digitalBg, delayTab, true);
    effectsModule.addTab("DIST", digitalBg, distortionTab, true);
    effectsModule.addTab("EQ", digitalBg, equalizerTab, true);
    effectsModule.addTab("FLANGE", digitalBg, flangerTab, true);
    effectsModule.addTab("PHASER", digitalBg, phaserTab, true);
    effectsModule.addTab("REVERB", digitalBg, reverbTab, true);
    
    // Apply digital screen look and feel
    effectsModule.setLookAndFeel(&digitalScreenLookAndFeel);
    
    // CHORUS EFFECT CONTROLS
    // Rate knob
    chorusRateKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusRateKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusRateKnob.setRange(0.1, 10.0, 0.1);
    chorusRateKnob.setValue(2.0);
    chorusRateKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusRateKnob);
    
    chorusRateLabel.setText("RATE", juce::dontSendNotification);
    chorusRateLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusRateLabel.setJustificationType(juce::Justification::centred);
    chorusRateLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusRateLabel);
    
    // Delay 1 knob
    chorusDelay1Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDelay1Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDelay1Knob.setRange(1.0, 50.0, 1.0);
    chorusDelay1Knob.setValue(20.0);
    chorusDelay1Knob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDelay1Knob);
    
    chorusDelay1Label.setText("DELAY 1", juce::dontSendNotification);
    chorusDelay1Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDelay1Label.setJustificationType(juce::Justification::centred);
    chorusDelay1Label.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDelay1Label);
    
    // Delay 2 knob
    chorusDelay2Knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDelay2Knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDelay2Knob.setRange(1.0, 50.0, 1.0);
    chorusDelay2Knob.setValue(30.0);
    chorusDelay2Knob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDelay2Knob);
    
    chorusDelay2Label.setText("DELAY 2", juce::dontSendNotification);
    chorusDelay2Label.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDelay2Label.setJustificationType(juce::Justification::centred);
    chorusDelay2Label.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDelay2Label);
    
    // Depth knob
    chorusDepthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusDepthKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusDepthKnob.setRange(0.0, 1.0, 0.01);
    chorusDepthKnob.setValue(0.5);
    chorusDepthKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDepthKnob);
    
    chorusDepthLabel.setText("DEPTH", juce::dontSendNotification);
    chorusDepthLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusDepthLabel.setJustificationType(juce::Justification::centred);
    chorusDepthLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusDepthLabel);
    
    // Feed knob
    chorusFeedKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusFeedKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusFeedKnob.setRange(0.0, 0.9, 0.01);
    chorusFeedKnob.setValue(0.3);
    chorusFeedKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusFeedKnob);
    
    chorusFeedLabel.setText("FEED", juce::dontSendNotification);
    chorusFeedLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusFeedLabel.setJustificationType(juce::Justification::centred);
    chorusFeedLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusFeedLabel);
    
    // LPF knob
    chorusLpfKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusLpfKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusLpfKnob.setRange(0.0, 100.0, 1.0);
    chorusLpfKnob.setValue(0.0);
    chorusLpfKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusLpfKnob);
    
    chorusLpfLabel.setText("LPF", juce::dontSendNotification);
    chorusLpfLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusLpfLabel.setJustificationType(juce::Justification::centred);
    chorusLpfLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusLpfLabel);
    
    // Mix knob
    chorusMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    chorusMixKnob.setRange(0.0, 1.0, 0.01);
    chorusMixKnob.setValue(0.5);
    chorusMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusMixKnob);
    
    chorusMixLabel.setText("MIX", juce::dontSendNotification);
    chorusMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    chorusMixLabel.setJustificationType(juce::Justification::centred);
    chorusMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    chorusTab->addAndMakeVisible(chorusMixLabel);
    
    // Power button
    chorusPowerButton.setButtonText("CHORUS ON");
    chorusPowerButton.setClickingTogglesState(true);
    chorusPowerButton.setToggleState(false, juce::dontSendNotification);
    chorusPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    chorusTab->addAndMakeVisible(chorusPowerButton);
    
    // Add listeners for chorus controls
    chorusRateKnob.addListener(this);
    chorusDelay1Knob.addListener(this);
    chorusDelay2Knob.addListener(this);
    chorusDepthKnob.addListener(this);
    chorusFeedKnob.addListener(this);
    chorusLpfKnob.addListener(this);
    chorusMixKnob.addListener(this);
    chorusPowerButton.addListener(this);
    
    // COMPRESSOR EFFECT CONTROLS
    // Threshold knob
    compressorThresholdKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorThresholdKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorThresholdKnob.setRange(-60.0, 0.0, 0.1);
    compressorThresholdKnob.setValue(-20.0);
    compressorThresholdKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorThresholdKnob);
    
    compressorThresholdLabel.setText("THRESH", juce::dontSendNotification);
    compressorThresholdLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorThresholdLabel.setJustificationType(juce::Justification::centred);
    compressorThresholdLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorThresholdLabel);
    
    // Ratio knob
    compressorRatioKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorRatioKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorRatioKnob.setRange(1.0, 20.0, 0.1);
    compressorRatioKnob.setValue(4.0);
    compressorRatioKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorRatioKnob);
    
    compressorRatioLabel.setText("RATIO", juce::dontSendNotification);
    compressorRatioLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorRatioLabel.setJustificationType(juce::Justification::centred);
    compressorRatioLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorRatioLabel);
    
    // Attack knob
    compressorAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorAttackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorAttackKnob.setRange(0.1, 100.0, 0.1);
    compressorAttackKnob.setValue(5.0);
    compressorAttackKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorAttackKnob);
    
    compressorAttackLabel.setText("ATTACK", juce::dontSendNotification);
    compressorAttackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorAttackLabel.setJustificationType(juce::Justification::centred);
    compressorAttackLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorAttackLabel);
    
    // Release knob
    compressorReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorReleaseKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorReleaseKnob.setRange(10.0, 1000.0, 1.0);
    compressorReleaseKnob.setValue(100.0);
    compressorReleaseKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorReleaseKnob);
    
    compressorReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    compressorReleaseLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorReleaseLabel.setJustificationType(juce::Justification::centred);
    compressorReleaseLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorReleaseLabel);
    
    // Gain knob
    compressorGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorGainKnob.setRange(0.0, 30.0, 0.1);
    compressorGainKnob.setValue(0.0);
    compressorGainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorGainKnob);
    
    compressorGainLabel.setText("GAIN", juce::dontSendNotification);
    compressorGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorGainLabel.setJustificationType(juce::Justification::centred);
    compressorGainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorGainLabel);
    
    // Multiband button
    compressorMultibandButton.setButtonText("MULTIBAND");
    compressorMultibandButton.setClickingTogglesState(true);
    compressorMultibandButton.setToggleState(false, juce::dontSendNotification);
    compressorMultibandButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    compressorTab->addAndMakeVisible(compressorMultibandButton);
    
    // Mix knob
    compressorMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    compressorMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    compressorMixKnob.setRange(0.0, 1.0, 0.01);
    compressorMixKnob.setValue(1.0);
    compressorMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorMixKnob);
    
    compressorMixLabel.setText("MIX", juce::dontSendNotification);
    compressorMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    compressorMixLabel.setJustificationType(juce::Justification::centred);
    compressorMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorMixLabel);
    
    // Power button
    compressorPowerButton.setButtonText("COMP ON");
    compressorPowerButton.setClickingTogglesState(true);
    compressorPowerButton.setToggleState(false, juce::dontSendNotification);
    compressorPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    compressorTab->addAndMakeVisible(compressorPowerButton);
    
    // Compressor value displays
    compressorRatioValueLabel.setText("4.0:1", juce::dontSendNotification);
    compressorRatioValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorRatioValueLabel.setJustificationType(juce::Justification::centred);
    compressorRatioValueLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorRatioValueLabel);
    
    compressorAttackValueLabel.setText("5.0ms", juce::dontSendNotification);
    compressorAttackValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorAttackValueLabel.setJustificationType(juce::Justification::centred);
    compressorAttackValueLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorAttackValueLabel);
    
    compressorReleaseValueLabel.setText("100ms", juce::dontSendNotification);
    compressorReleaseValueLabel.setFont(juce::Font("Times New Roman", 7.0f, juce::Font::bold));
    compressorReleaseValueLabel.setJustificationType(juce::Justification::centred);
    compressorReleaseValueLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    compressorTab->addAndMakeVisible(compressorReleaseValueLabel);
    
    // Add listeners for compressor controls
    compressorThresholdKnob.addListener(this);
    compressorRatioKnob.addListener(this);
    compressorAttackKnob.addListener(this);
    compressorReleaseKnob.addListener(this);
    compressorGainKnob.addListener(this);
    compressorMixKnob.addListener(this);
    compressorMultibandButton.addListener(this);
    compressorPowerButton.addListener(this);
    
    // DELAY EFFECT CONTROLS
    // Feedback knob
    delayFeedbackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFeedbackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFeedbackKnob.setRange(0.0, 0.95, 0.01);
    delayFeedbackKnob.setValue(0.3);
    delayFeedbackKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFeedbackKnob);
    
    delayFeedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    delayFeedbackLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFeedbackLabel.setJustificationType(juce::Justification::centred);
    delayFeedbackLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFeedbackLabel);
    
    // BPM button
    delayBpmButton.setButtonText("BPM");
    delayBpmButton.setClickingTogglesState(true);
    delayBpmButton.setToggleState(true, juce::dontSendNotification);
    delayBpmButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayBpmButton);
    
    // Link button
    delayLinkButton.setButtonText("LINK");
    delayLinkButton.setClickingTogglesState(true);
    delayLinkButton.setToggleState(true, juce::dontSendNotification);
    delayLinkButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayLinkButton);
    
    // Left delay time slider
    delayLeftTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayLeftTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayLeftTimeSlider.setRange(1.0, 2000.0, 1.0);
    delayLeftTimeSlider.setValue(250.0);
    delayLeftTimeSlider.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayLeftTimeSlider);
    
    delayLeftTimeLabel.setText("L TIME", juce::dontSendNotification);
    delayLeftTimeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayLeftTimeLabel.setJustificationType(juce::Justification::centred);
    delayLeftTimeLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayLeftTimeLabel);
    
    // Right delay time slider
    delayRightTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayRightTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayRightTimeSlider.setRange(1.0, 2000.0, 1.0);
    delayRightTimeSlider.setValue(375.0);
    delayRightTimeSlider.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayRightTimeSlider);
    
    delayRightTimeLabel.setText("R TIME", juce::dontSendNotification);
    delayRightTimeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayRightTimeLabel.setJustificationType(juce::Justification::centred);
    delayRightTimeLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayRightTimeLabel);
    
    // Left triplet button
    delayLeftTripletButton.setButtonText("L 3");
    delayLeftTripletButton.setClickingTogglesState(true);
    delayLeftTripletButton.setToggleState(false, juce::dontSendNotification);
    delayLeftTripletButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayLeftTripletButton);
    
    // Left dot timing button
    delayLeftDotButton.setButtonText("L .");
    delayLeftDotButton.setClickingTogglesState(true);
    delayLeftDotButton.setToggleState(false, juce::dontSendNotification);
    delayLeftDotButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayLeftDotButton);
    
    // Right triplet button
    delayRightTripletButton.setButtonText("R 3");
    delayRightTripletButton.setClickingTogglesState(true);
    delayRightTripletButton.setToggleState(false, juce::dontSendNotification);
    delayRightTripletButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayRightTripletButton);
    
    // Right dot timing button
    delayRightDotButton.setButtonText("R .");
    delayRightDotButton.setClickingTogglesState(true);
    delayRightDotButton.setToggleState(false, juce::dontSendNotification);
    delayRightDotButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayRightDotButton);
    
    // Filter frequency knob
    delayFilterFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFilterFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFilterFreqKnob.setRange(20.0, 20000.0, 1.0);
    delayFilterFreqKnob.setValue(8000.0);
    delayFilterFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFilterFreqKnob);
    
    delayFilterFreqLabel.setText("FREQ", juce::dontSendNotification);
    delayFilterFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFilterFreqLabel.setJustificationType(juce::Justification::centred);
    delayFilterFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFilterFreqLabel);
    
    // Filter Q knob
    delayFilterQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayFilterQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayFilterQKnob.setRange(0.1, 10.0, 0.1);
    delayFilterQKnob.setValue(1.0);
    delayFilterQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFilterQKnob);
    
    delayFilterQLabel.setText("Q", juce::dontSendNotification);
    delayFilterQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayFilterQLabel.setJustificationType(juce::Justification::centred);
    delayFilterQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayFilterQLabel);
    
    // Normal button
    delayNormalButton.setButtonText("NORMAL");
    delayNormalButton.setClickingTogglesState(true);
    delayNormalButton.setToggleState(true, juce::dontSendNotification);
    delayNormalButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayNormalButton);
    
    // Ping-pong button
    delayPingPongButton.setButtonText("PING-PONG");
    delayPingPongButton.setClickingTogglesState(true);
    delayPingPongButton.setToggleState(false, juce::dontSendNotification);
    delayPingPongButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayPingPongButton);
    
    // Mix knob
    delayMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayMixKnob.setRange(0.0, 1.0, 0.01);
    delayMixKnob.setValue(0.25);
    delayMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayMixKnob);
    
    delayMixLabel.setText("MIX", juce::dontSendNotification);
    delayMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    delayMixLabel.setJustificationType(juce::Justification::centred);
    delayMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    delayTab->addAndMakeVisible(delayMixLabel);
    
    // Power button
    delayPowerButton.setButtonText("DELAY ON");
    delayPowerButton.setClickingTogglesState(true);
    delayPowerButton.setToggleState(false, juce::dontSendNotification);
    delayPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    delayTab->addAndMakeVisible(delayPowerButton);
    
    // DISTORTION EFFECT CONTROLS
    // Type label
    distortionTypeLabel.setText("TYPE", juce::dontSendNotification);
    distortionTypeLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionTypeLabel.setJustificationType(juce::Justification::centred);
    distortionTypeLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionTypeLabel);
    
    // Type value label (interactive)
    distortionTypeValueLabel.setText("TUBE", juce::dontSendNotification);
    distortionTypeValueLabel.setJustificationType(juce::Justification::centred);
    distortionTypeValueLabel.setLookAndFeel(&greenLEDNumberLookAndFeel);
    distortionTypeValueLabel.setInterceptsMouseClicks(true, true); // Enable mouse interaction for self and children
    distortionTypeValueLabel.setEditable(false, false, false); // Make it respond to mouse but not editable
    distortionTypeValueLabel.addMouseListener(this, true); // Add mouse listener with childEvents = true
    distortionTab->addAndMakeVisible(distortionTypeValueLabel);
    
    // Drive knob
    distortionDriveKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionDriveKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionDriveKnob.setRange(0.0, 100.0, 0.1);
    distortionDriveKnob.setValue(50.0);
    distortionDriveKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionDriveKnob.addListener(this);
    distortionTab->addAndMakeVisible(distortionDriveKnob);
    
    distortionDriveLabel.setText("DRIVE", juce::dontSendNotification);
    distortionDriveLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionDriveLabel.setJustificationType(juce::Justification::centred);
    distortionDriveLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionDriveLabel);
    
    // Mix knob
    distortionMixKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionMixKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionMixKnob.setRange(0.0, 1.0, 0.01);
    distortionMixKnob.setValue(1.0);
    distortionMixKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionMixKnob.addListener(this);
    distortionTab->addAndMakeVisible(distortionMixKnob);
    
    distortionMixLabel.setText("MIX", juce::dontSendNotification);
    distortionMixLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionMixLabel.setJustificationType(juce::Justification::centred);
    distortionMixLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionMixLabel);
    
    // Power button
    distortionPowerButton.setButtonText("DIST ON");
    distortionPowerButton.setClickingTogglesState(true);
    distortionPowerButton.setToggleState(false, juce::dontSendNotification);
    distortionPowerButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionPowerButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionPowerButton);
    
    // Filter section label
    distortionFilterSectionLabel.setText("FILTER", juce::dontSendNotification);
    distortionFilterSectionLabel.setFont(juce::Font("Times New Roman", 10.0f, juce::Font::bold));
    distortionFilterSectionLabel.setJustificationType(juce::Justification::centred);
    distortionFilterSectionLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionFilterSectionLabel);
    
    // DISTORTION FILTER CONTROLS
    // Filter mode buttons (Off/Pre/Post)
    distortionFilterOffButton.setButtonText("OFF");
    distortionFilterOffButton.setClickingTogglesState(true);
    distortionFilterOffButton.setToggleState(true, juce::dontSendNotification);
    distortionFilterOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterOffButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterOffButton);
    
    distortionFilterPreButton.setButtonText("PRE");
    distortionFilterPreButton.setClickingTogglesState(true);
    distortionFilterPreButton.setToggleState(false, juce::dontSendNotification);
    distortionFilterPreButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterPreButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterPreButton);
    
    distortionFilterPostButton.setButtonText("POST");
    distortionFilterPostButton.setClickingTogglesState(true);
    distortionFilterPostButton.setToggleState(false, juce::dontSendNotification);
    distortionFilterPostButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterPostButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterPostButton);
    
    // Filter type buttons (LP/BP/HP)
    distortionFilterLPButton.setButtonText("LP");
    distortionFilterLPButton.setClickingTogglesState(true);
    distortionFilterLPButton.setToggleState(true, juce::dontSendNotification);
    distortionFilterLPButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterLPButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterLPButton);
    
    distortionFilterBPButton.setButtonText("BP");
    distortionFilterBPButton.setClickingTogglesState(true);
    distortionFilterBPButton.setToggleState(false, juce::dontSendNotification);
    distortionFilterBPButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterBPButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterBPButton);
    
    distortionFilterHPButton.setButtonText("HP");
    distortionFilterHPButton.setClickingTogglesState(true);
    distortionFilterHPButton.setToggleState(false, juce::dontSendNotification);
    distortionFilterHPButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    distortionFilterHPButton.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterHPButton);
    
    // Filter frequency knob
    distortionFilterFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionFilterFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionFilterFreqKnob.setRange(20.0, 20000.0, 1.0);
    distortionFilterFreqKnob.setValue(800.0);
    distortionFilterFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionFilterFreqKnob.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterFreqKnob);
    
    distortionFilterFreqLabel.setText("FREQ", juce::dontSendNotification);
    distortionFilterFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionFilterFreqLabel.setJustificationType(juce::Justification::centred);
    distortionFilterFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionFilterFreqLabel);
    
    // Filter Q knob
    distortionFilterQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distortionFilterQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    distortionFilterQKnob.setRange(0.1, 10.0, 0.1);
    distortionFilterQKnob.setValue(2.0);
    distortionFilterQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionFilterQKnob.addListener(this);
    distortionTab->addAndMakeVisible(distortionFilterQKnob);
    
    distortionFilterQLabel.setText("Q", juce::dontSendNotification);
    distortionFilterQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    distortionFilterQLabel.setJustificationType(juce::Justification::centred);
    distortionFilterQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    distortionTab->addAndMakeVisible(distortionFilterQLabel);
    
    // Add delay listeners
    delayFeedbackKnob.addListener(this);
    delayBpmButton.addListener(this);
    delayLinkButton.addListener(this);
    delayLeftTimeSlider.addListener(this);
    delayRightTimeSlider.addListener(this);
    delayLeftTripletButton.addListener(this);
    delayLeftDotButton.addListener(this);
    delayRightTripletButton.addListener(this);
    delayRightDotButton.addListener(this);
    delayFilterFreqKnob.addListener(this);
    delayFilterQKnob.addListener(this);
    delayNormalButton.addListener(this);
    delayPingPongButton.addListener(this);
    delayMixKnob.addListener(this);
    delayPowerButton.addListener(this);
    
    // EQ EFFECT CONTROLS
    // Band 1 (Left) filter type buttons
    eq1ShelfButton.setButtonText("SHELF"); // Low shelf - for later implementation
    eq1ShelfButton.setClickingTogglesState(true);
    eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq1ShelfButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1ShelfButton.addListener(this);
    eq1ShelfButton.setVisible(true);
    eq1ShelfButton.setBounds(17, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1ShelfButton);
    
    eq1PeakButton.setButtonText("PEAK");
    eq1PeakButton.setClickingTogglesState(true);
    eq1PeakButton.setToggleState(true, juce::dontSendNotification);
    eq1PeakButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1PeakButton.addListener(this);
    eq1PeakButton.setVisible(true);
    eq1PeakButton.setBounds(62, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1PeakButton);
    
    eq1PassButton.setButtonText("HIPASS");
    eq1PassButton.setClickingTogglesState(true);
    eq1PassButton.setToggleState(false, juce::dontSendNotification);
    eq1PassButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1PassButton.addListener(this);
    eq1PassButton.setVisible(true);
    eq1PassButton.setBounds(107, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq1PassButton);
    
    // Band 1 on/off button
    eq1OnOffButton.setButtonText("BAND 1");
    eq1OnOffButton.setClickingTogglesState(true);
    eq1OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq1OnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq1OnOffButton.addListener(this);
    eq1OnOffButton.setVisible(true);
    eq1OnOffButton.setBounds(17, 370, 50, 22); // Below filter type buttons
    equalizerTab->addAndMakeVisible(eq1OnOffButton);
    
    // Band 1 knobs
    eq1FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1FreqKnob.setValue(400.0);
    eq1FreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1FreqKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1FreqKnob);
    
    eq1FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq1FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1FreqLabel.setJustificationType(juce::Justification::centred);
    eq1FreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1FreqLabel);
    
    eq1QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1QKnob.setRange(0.1, 10.0, 0.1);
    eq1QKnob.setValue(1.0);
    eq1QKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1QKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1QKnob);
    
    eq1QLabel.setText("Q", juce::dontSendNotification);
    eq1QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1QLabel.setJustificationType(juce::Justification::centred);
    eq1QLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1QLabel);
    
    eq1GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1GainKnob.setRange(-15.0, 15.0, 0.1);
    eq1GainKnob.setValue(0.0);
    eq1GainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1GainKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq1GainKnob);
    
    eq1GainLabel.setText("GAIN", juce::dontSendNotification);
    eq1GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1GainLabel.setJustificationType(juce::Justification::centred);
    eq1GainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq1GainLabel);
    
    // Band 2 (Right) filter type buttons
    eq2ShelfButton.setButtonText("SHELF"); // High shelf - for later implementation
    eq2ShelfButton.setClickingTogglesState(true);
    eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq2ShelfButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2ShelfButton.addListener(this);
    eq2ShelfButton.setVisible(true);
    eq2ShelfButton.setBounds(152, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2ShelfButton);
    
    eq2PeakButton.setButtonText("PEAK");
    eq2PeakButton.setClickingTogglesState(true);
    eq2PeakButton.setToggleState(true, juce::dontSendNotification);
    eq2PeakButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2PeakButton.addListener(this);
    eq2PeakButton.setVisible(true);
    eq2PeakButton.setBounds(197, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2PeakButton);
    
    eq2PassButton.setButtonText("LOPASS");
    eq2PassButton.setClickingTogglesState(true);
    eq2PassButton.setToggleState(false, juce::dontSendNotification);
    eq2PassButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2PassButton.addListener(this);
    eq2PassButton.setVisible(true);
    eq2PassButton.setBounds(242, 342, 40, 22);
    equalizerTab->addAndMakeVisible(eq2PassButton);
    
    // Band 2 on/off button
    eq2OnOffButton.setButtonText("BAND 2");
    eq2OnOffButton.setClickingTogglesState(true);
    eq2OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq2OnOffButton.setLookAndFeel(&greenDigitalButtonLookAndFeel);
    eq2OnOffButton.addListener(this);
    eq2OnOffButton.setVisible(true);
    eq2OnOffButton.setBounds(232, 370, 50, 22); // Below filter type buttons
    equalizerTab->addAndMakeVisible(eq2OnOffButton);
    
    // Band 2 knobs
    eq2FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2FreqKnob.setValue(4000.0);
    eq2FreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2FreqKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2FreqKnob);
    
    eq2FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq2FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2FreqLabel.setJustificationType(juce::Justification::centred);
    eq2FreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2FreqLabel);
    
    eq2QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2QKnob.setRange(0.1, 10.0, 0.1);
    eq2QKnob.setValue(1.0);
    eq2QKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2QKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2QKnob);
    
    eq2QLabel.setText("Q", juce::dontSendNotification);
    eq2QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2QLabel.setJustificationType(juce::Justification::centred);
    eq2QLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2QLabel);
    
    eq2GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2GainKnob.setRange(-15.0, 15.0, 0.1);
    eq2GainKnob.setValue(0.0);
    eq2GainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2GainKnob.addListener(this);
    equalizerTab->addAndMakeVisible(eq2GainKnob);
    
    eq2GainLabel.setText("GAIN", juce::dontSendNotification);
    eq2GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2GainLabel.setJustificationType(juce::Justification::centred);
    eq2GainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    equalizerTab->addAndMakeVisible(eq2GainLabel);
    
    // NEW EQ CONTROLS ROW - Additional frequency and Q knobs
    // Band 1 new frequency knob
    eq1NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1NewFreqKnob.setValue(400.0);
    eq1NewFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewFreqKnob.addListener(this);
    eq1NewFreqKnob.setVisible(true);
    eq1NewFreqKnob.setBounds(8, 400, 35, 35); // New row below existing controls
    equalizerTab->addAndMakeVisible(eq1NewFreqKnob);
    
    eq1NewFreqLabel.setText("FREQ1", juce::dontSendNotification);
    eq1NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq1NewFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewFreqLabel.setVisible(true);
    eq1NewFreqLabel.setBounds(8, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewFreqLabel);
    
    // Band 1 new Q knob
    eq1NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewQKnob.setRange(0.1, 30.0, 0.1);
    eq1NewQKnob.setValue(1.0);
    eq1NewQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewQKnob.addListener(this);
    eq1NewQKnob.setVisible(true);
    eq1NewQKnob.setBounds(53, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq1NewQKnob);
    
    eq1NewQLabel.setText("Q1", juce::dontSendNotification);
    eq1NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewQLabel.setJustificationType(juce::Justification::centred);
    eq1NewQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewQLabel.setVisible(true);
    eq1NewQLabel.setBounds(53, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewQLabel);
    
    // Band 1 new gain knob
    eq1NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq1NewGainKnob.setValue(0.0);
    eq1NewGainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewGainKnob.addListener(this);
    eq1NewGainKnob.setVisible(true);
    eq1NewGainKnob.setBounds(98, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq1NewGainKnob);
    
    eq1NewGainLabel.setText("GAIN1", juce::dontSendNotification);
    eq1NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewGainLabel.setJustificationType(juce::Justification::centred);
    eq1NewGainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq1NewGainLabel.setVisible(true);
    eq1NewGainLabel.setBounds(98, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq1NewGainLabel);
    
    // Band 2 new frequency knob
    eq2NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2NewFreqKnob.setValue(4000.0);
    eq2NewFreqKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewFreqKnob.addListener(this);
    eq2NewFreqKnob.setVisible(true);
    eq2NewFreqKnob.setBounds(163, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewFreqKnob);
    
    eq2NewFreqLabel.setText("FREQ2", juce::dontSendNotification);
    eq2NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq2NewFreqLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewFreqLabel.setVisible(true);
    eq2NewFreqLabel.setBounds(163, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewFreqLabel);
    
    // Band 2 new Q knob
    eq2NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewQKnob.setRange(0.1, 30.0, 0.1);
    eq2NewQKnob.setValue(1.0);
    eq2NewQKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewQKnob.addListener(this);
    eq2NewQKnob.setVisible(true);
    eq2NewQKnob.setBounds(208, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewQKnob);
    
    eq2NewQLabel.setText("Q2", juce::dontSendNotification);
    eq2NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewQLabel.setJustificationType(juce::Justification::centred);
    eq2NewQLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewQLabel.setVisible(true);
    eq2NewQLabel.setBounds(208, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewQLabel);
    
    // Band 2 new gain knob
    eq2NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq2NewGainKnob.setValue(0.0);
    eq2NewGainKnob.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewGainKnob.addListener(this);
    eq2NewGainKnob.setVisible(true);
    eq2NewGainKnob.setBounds(253, 400, 35, 35);
    equalizerTab->addAndMakeVisible(eq2NewGainKnob);
    
    eq2NewGainLabel.setText("GAIN2", juce::dontSendNotification);
    eq2NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewGainLabel.setJustificationType(juce::Justification::centred);
    eq2NewGainLabel.setLookAndFeel(&greenDigitalKnobLookAndFeel);
    eq2NewGainLabel.setVisible(true);
    eq2NewGainLabel.setBounds(253, 440, 35, 15);
    equalizerTab->addAndMakeVisible(eq2NewGainLabel);
    
    // EQ Point labels - positioned above buttons (bracket lines will be painted separately)
    eq1PointLabel.setText("BAND 1", juce::dontSendNotification);
    eq1PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq1PointLabel.setJustificationType(juce::Justification::centred);
    eq1PointLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
    eq1PointLabel.setBounds(45, 314, 70, 15);
    equalizerTab->addAndMakeVisible(eq1PointLabel);
    
    eq2PointLabel.setText("BAND 2", juce::dontSendNotification);
    eq2PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq2PointLabel.setJustificationType(juce::Justification::centred);
    eq2PointLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    eq2PointLabel.setBounds(183, 314, 70, 15);
    equalizerTab->addAndMakeVisible(eq2PointLabel);
    
    // EFFECTS PRESET CONTROLS - Placeholder functionality
    effectsPresetPrevButton.setButtonText("<");
    effectsPresetPrevButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetPrevButton);
    
    effectsPresetNextButton.setButtonText(">");
    effectsPresetNextButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetNextButton);
    
    effectsPresetNameLabel.setText("DEFAULT", juce::dontSendNotification);
    effectsPresetNameLabel.setJustificationType(juce::Justification::centred);
    effectsPresetNameLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    effectsPresetNameLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    addAndMakeVisible(effectsPresetNameLabel);
    
    effectsPresetSaveButton.setButtonText("SAVE");
    effectsPresetSaveButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetSaveButton);
    
    effectsPresetLoadButton.setButtonText("LOAD");
    effectsPresetLoadButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetLoadButton);
    
    effectsPresetInitButton.setButtonText("INIT");
    effectsPresetInitButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    addAndMakeVisible(effectsPresetInitButton);
    
    // Set up preset button callbacks
    effectsPresetSaveButton.onClick = [this]() {
        showSavePresetDialog();
    };
    
    effectsPresetLoadButton.onClick = [this]() {
        showLoadPresetDialog();
    };
    
    effectsPresetInitButton.onClick = [this]() {
        showInitPresetDialog();
    };
    
    effectsPresetPrevButton.onClick = [this]() {
        audioProcessor.previousPreset();
        updatePresetDisplay();
    };
    
    effectsPresetNextButton.onClick = [this]() {
        audioProcessor.nextPreset();
        updatePresetDisplay();
    };
    
    // Add border component behind effects module
    addAndMakeVisible(effectsBorder);
    addAndMakeVisible(effectsModule);
    
    // Initialize envelope display with default values
    updateEnvelopeDisplay();
    
    // Initialize preset display
    updatePresetDisplay();
}

SynthesizerComponent::~SynthesizerComponent()
{
    // Reset custom look and feel to avoid dangling pointers
    adsrAttackKnob.setLookAndFeel(nullptr);
    adsrDecayKnob.setLookAndFeel(nullptr);
    adsrSustainKnob.setLookAndFeel(nullptr);
    adsrReleaseKnob.setLookAndFeel(nullptr);
    waveTypeSineButton.setLookAndFeel(nullptr);
    waveTypeSawButton.setLookAndFeel(nullptr);
    waveTypeSquareButton.setLookAndFeel(nullptr);
    waveTypeTriangleButton.setLookAndFeel(nullptr);
    waveTypeWhiteNoiseButton.setLookAndFeel(nullptr);
    waveTypePinkNoiseButton.setLookAndFeel(nullptr);
    waveTypeRandomPhaseButton.setLookAndFeel(nullptr);
    
    // Reset LED label look and feel
    volumeControlsVolumeLabel.setLookAndFeel(nullptr);
    volumeControlsDetuneLabel.setLookAndFeel(nullptr);
    volumeControlsStereoWidthLabel.setLookAndFeel(nullptr);
    volumeControlsPanLabel.setLookAndFeel(nullptr);
    phaseControlsPhaseLabel.setLookAndFeel(nullptr);
    adsrAttackLabel.setLookAndFeel(nullptr);
    adsrDecayLabel.setLookAndFeel(nullptr);
    adsrSustainLabel.setLookAndFeel(nullptr);
    adsrReleaseLabel.setLookAndFeel(nullptr);
    pitchControlsOctaveLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountLabel.setLookAndFeel(nullptr);
    
    // Reset LED number look and feel
    pitchControlsOctaveValueLabel.setLookAndFeel(nullptr);
    pitchControlsSemitoneValueLabel.setLookAndFeel(nullptr);
    pitchControlsFineTuneValueLabel.setLookAndFeel(nullptr);
    pitchControlsVoiceCountValueLabel.setLookAndFeel(nullptr);
    
    // Reset oscillator 2 controls look and feel
    osc2SineButton.setLookAndFeel(nullptr);
    osc2SawButton.setLookAndFeel(nullptr);
    osc2SquareButton.setLookAndFeel(nullptr);
    osc2TriangleButton.setLookAndFeel(nullptr);
    osc2WhiteNoiseButton.setLookAndFeel(nullptr);
    osc2PinkNoiseButton.setLookAndFeel(nullptr);
    osc2VolumeKnob.setLookAndFeel(nullptr);
    osc2VolumeLabel.setLookAndFeel(nullptr);
    // osc2VoicesKnob uses default look and feel (no custom styling needed)
    osc2VoicesLabel.setLookAndFeel(nullptr);
    osc2PanKnob.setLookAndFeel(nullptr);
    osc2PanLabel.setLookAndFeel(nullptr);
    // osc2OctaveKnob uses default look and feel (no custom styling needed)
    osc2OctaveLabel.setLookAndFeel(nullptr);
    // osc2SemitoneKnob uses default look and feel (no custom styling needed)
    osc2SemitoneLabel.setLookAndFeel(nullptr);
    // osc2FineTuneKnob uses default look and feel (no custom styling needed)
    osc2FineTuneLabel.setLookAndFeel(nullptr);
    osc2RandomPhaseButton.setLookAndFeel(nullptr);
    osc2PhaseKnob.setLookAndFeel(nullptr);
    osc2PhaseLabel.setLookAndFeel(nullptr);
    osc2TitleLabel.setLookAndFeel(nullptr);
    osc2AttackKnob.setLookAndFeel(nullptr);
    osc2AttackLabel.setLookAndFeel(nullptr);
    osc2DecayKnob.setLookAndFeel(nullptr);
    osc2DecayLabel.setLookAndFeel(nullptr);
    osc2SustainKnob.setLookAndFeel(nullptr);
    osc2SustainLabel.setLookAndFeel(nullptr);
    osc2ReleaseKnob.setLookAndFeel(nullptr);
    osc2ReleaseLabel.setLookAndFeel(nullptr);
    osc2AdsrLinkButton.setLookAndFeel(nullptr);
    
    // Filter controls cleanup
    filterCutoffKnob.setLookAndFeel(nullptr);
    filterCutoffLabel.setLookAndFeel(nullptr);
    filterResonanceKnob.setLookAndFeel(nullptr);
    filterResonanceLabel.setLookAndFeel(nullptr);
    osc1FilterEnableButton.setLookAndFeel(nullptr);
    osc2FilterEnableButton.setLookAndFeel(nullptr);
    filterLPButton.setLookAndFeel(nullptr);
    filterHPButton.setLookAndFeel(nullptr);
    filterBPButton.setLookAndFeel(nullptr);
    filter12dBButton.setLookAndFeel(nullptr);
    filter24dBButton.setLookAndFeel(nullptr);
    osc2DetuneKnob.setLookAndFeel(nullptr);
    osc2DetuneLabel.setLookAndFeel(nullptr);
    osc2StereoKnob.setLookAndFeel(nullptr);
    osc2StereoLabel.setLookAndFeel(nullptr);
    
    // Reset macro knobs look and feel
    macro1Knob.setLookAndFeel(nullptr);
    macro2Knob.setLookAndFeel(nullptr);
    macro3Knob.setLookAndFeel(nullptr);
    macro4Knob.setLookAndFeel(nullptr);
    macro5Knob.setLookAndFeel(nullptr);
    macro6Knob.setLookAndFeel(nullptr);
    macro7Knob.setLookAndFeel(nullptr);
    macro8Knob.setLookAndFeel(nullptr);
    
    // Reset effects preset controls look and feel
    effectsPresetPrevButton.setLookAndFeel(nullptr);
    effectsPresetNextButton.setLookAndFeel(nullptr);
    effectsPresetSaveButton.setLookAndFeel(nullptr);
    effectsPresetLoadButton.setLookAndFeel(nullptr);
    effectsPresetInitButton.setLookAndFeel(nullptr);
    
    // pulseWidthSlider.setLookAndFeel(nullptr); // commented out
}

void SynthesizerComponent::paint(juce::Graphics& g)
{
    // Black background
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    // Add hollow spruce wood border around whole page
    auto pageBounds = getLocalBounds().toFloat();
    auto borderThickness = 20.0f;
    
    // Outer spruce wood border
    g.setColour(juce::Colour(0xff3d2914));
    g.fillRoundedRectangle(pageBounds, 8.0f);
    
    // Cut out inner area to make it hollow
    auto innerBounds = pageBounds.reduced(borderThickness);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(innerBounds, 4.0f);
    
    // Add spruce wood grain to border
    juce::Random borderRandom(42);
    for (int i = 0; i < 30; ++i)
    {
        // Top border grain
        auto y = pageBounds.getY() + borderRandom.nextFloat() * borderThickness;
        auto startX = pageBounds.getX();
        auto endX = pageBounds.getRight();
        g.setColour(juce::Colour(0xff1a0f08).withAlpha(0.2f + borderRandom.nextFloat() * 0.3f));
        g.drawLine(startX, y, endX, y, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Bottom border grain
        y = pageBounds.getBottom() - borderThickness + borderRandom.nextFloat() * borderThickness;
        g.drawLine(startX, y, endX, y, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Left border grain
        auto x = pageBounds.getX() + borderRandom.nextFloat() * borderThickness;
        auto startY = pageBounds.getY();
        auto endY = pageBounds.getBottom();
        g.drawLine(x, startY, x, endY, 1.0f + borderRandom.nextFloat() * 1.0f);
        
        // Right border grain
        x = pageBounds.getRight() - borderThickness + borderRandom.nextFloat() * borderThickness;
        g.drawLine(x, startY, x, endY, 1.0f + borderRandom.nextFloat() * 1.0f);
    }
    
    // Add futuristic outline around the actual controls bounds
    // Calculate the tight bounds that encompass all controls
    auto masterBounds = getLocalBounds();
    masterBounds.reduce(20, 20);
    masterBounds.removeFromTop(20);
    
    // Start from wave buttons and go to the bottom of phase section
    auto masterTop = masterBounds.getY();
    auto masterLeft = masterBounds.getX();
    
    // Width should encompass the tuning section (which is the widest at 320px)
    auto masterWidth = 320 + 63; // tuning width + a bit more padding
    
    // Height should go from wave buttons to bottom of phase section
    auto masterHeight = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20; // all sections + spacing + extra
    
    // Apply oscillator offset for easy group movement (defaults to 0,0 for no change)
    auto masterOutlineBounds = juce::Rectangle<float>(masterLeft - 12 + oscillatorBackgroundOffsetX, masterTop - 12 + oscillatorBackgroundOffsetY, masterWidth + 24, masterHeight + 24);
    
    // Create 3D depth effect with multiple layers
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Main raised surface - metallic modular synth module look
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    
    // Much darker metallic grey base
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add subtle metallic texture
    juce::Random metallicRandom(42);
    g.setColour(juce::Colour(0xff202020).withAlpha(0.3f));
    for (int i = 0; i < 200; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Add lighter metallic highlights
    metallicRandom.setSeed(123);
    g.setColour(juce::Colour(0xff606060).withAlpha(0.2f));
    for (int i = 0; i < 150; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff707070).withAlpha(0.8f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight  
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Add corner screws for modular synth module look
    auto screwRadius = 6.0f;
    auto screwInset = 15.0f; // Increased inset to ensure screws are visible
    
    // Top-left screw
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Oscillator 1" label to top center of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);
    
    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);
    
    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);
    
    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1, 
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);
    
    // Engraved 3D text effect
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("OSCILLATOR 1", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("OSCILLATOR 1", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("OSCILLATOR 1", textBounds, juce::Justification::centred, true);
    
    // Futuristic outline with glow effect on the raised surface
    
    // Add metal slots for knobs - match exact positions from resized()
    auto slotBounds = getLocalBounds();
    slotBounds.reduce(20, 20);
    // Apply oscillator offset to match the UI components movement
    slotBounds = slotBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    
    // Skip to ADSR section (match resized() layout exactly)
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(40); // wave buttons
    slotBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    slotBounds.removeFromTop(60); // envelope visualization 
    slotBounds.removeFromTop(15); // increased spacing between envelope and knobs
    
    // ADSR knobs metal slots - match exact layout from resized()
    auto adsrRow = slotBounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(slotBounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) - same as layout
    adsrSection = adsrSection.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // Same calculation as resized()
    
    // Attack knob slot - square metal
    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Decay knob slot - square metal
    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Sustain knob slot - square metal
    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    adsrSection.removeFromLeft(15); // spacing
    
    // Release knob slot - square metal
    auto releaseArea = adsrSection;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(20); // spacing
    
    // Volume section knobs metal slots - square metal
    auto volumeRow = slotBounds.removeFromTop(100);
    auto volumeControlsSection = volumeRow.removeFromLeft(slotBounds.getWidth() / 3); // Full section for all 4 knobs
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) - same as layout
    volumeControlsSection = volumeControlsSection.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    auto volumeKnobWidth = (volumeControlsSection.getWidth() - 45) / 4; // Width for each knob
    auto volumeSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Detune knob slot - square metal
    auto detuneSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Stereo Width knob slot - square metal
    auto stereoWidthSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    volumeControlsSection.removeFromLeft(15); // spacing
    
    // Pan knob slot - square metal
    auto panSection = volumeControlsSection;
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 40, panSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 39, panSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 37, panSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    // Skip to phase section
    slotBounds.removeFromTop(40); // spacing - match resized() method spacing
    slotBounds.removeFromTop(80); // tuning controls
    slotBounds.removeFromTop(20); // spacing
    
    // Phase knob metal slot - match exact resized() positioning
    auto phaseRow = slotBounds.removeFromTop(80);
    phaseRow.removeFromLeft(100); // random phase button
    phaseRow.removeFromLeft(15); // spacing
    auto phaseKnobArea = phaseRow.removeFromLeft(80);
    phaseKnobArea.removeFromTop(-5); // negative value to move background up even more
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 40, phaseKnobArea.getCentreY() - 46, 80, 94, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 39, phaseKnobArea.getCentreY() - 45, 78, 92, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 37, phaseKnobArea.getCentreY() - 43, 74, 88, 2.0f, 1.0f);
    
    
    // Draw futuristic section outlines for each row of controls
    auto sectionBounds = getLocalBounds();
    sectionBounds.reduce(20, 20);
    // Apply oscillator offset to match the UI components movement
    sectionBounds = sectionBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    sectionBounds.removeFromTop(20);
    
    // Wave type buttons section outline
    auto waveSectionBounds = sectionBounds.removeFromTop(40);
    waveSectionBounds = waveSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3); // Same width as ADSR
    waveSectionBounds.reduce(-2, -2); // Expand the bounds slightly
    
    
    sectionBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    
    // Skip envelope visualization and spacing
    sectionBounds.removeFromTop(60 + 15); // envelope height + increased spacing
    
    // ADSR section outline
    auto adsrSectionBounds = sectionBounds.removeFromTop(100);
    adsrSectionBounds = adsrSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) section outline
    adsrSectionBounds = adsrSectionBounds.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
    adsrSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Volume/Detune/Stereo/Pan section outline
    auto volumeSectionBounds = sectionBounds.removeFromTop(100);
    volumeSectionBounds = volumeSectionBounds.removeFromLeft(volumeSectionBounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) section outline
    volumeSectionBounds = volumeSectionBounds.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    volumeSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    sectionBounds.removeFromTop(40); // increased spacing even more to match octave controls pushed down further
    
    // Octave/Semitone/Fine/Voices section outline
    auto tuningSectionBounds = sectionBounds.removeFromTop(80);
    tuningSectionBounds = tuningSectionBounds.removeFromLeft(320); // Fixed width for tuning controls
    
    // Apply group offset for MOVEABLE Pitch Controls Group (Row 5) section outline
    tuningSectionBounds = tuningSectionBounds.translated(
        static_cast<int>(pitchControlsGroupOffsetX), 
        static_cast<int>(pitchControlsGroupOffsetY)
    );
    
    tuningSectionBounds.reduce(-5, -5); // Expand the bounds
    
    
    // Individual background boxes for tuning controls - center them
    auto tuningControlsRow = tuningSectionBounds.reduced(3, 3);
    tuningControlsRow.removeFromLeft(2); // Shift right slightly by reducing bounds
    tuningControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    sectionBounds.removeFromTop(20); // spacing
    
    // Phase section outline
    auto phaseSectionBounds = sectionBounds.removeFromTop(80);
    phaseSectionBounds = phaseSectionBounds.removeFromLeft(220); // Fixed width for phase controls
    phaseSectionBounds.reduce(-5, -5); // Expand the bounds
    
    // Second oscillator section
    sectionBounds.removeFromTop(20); // spacing
    auto secondOscSectionBounds = sectionBounds.removeFromTop(100);
    // Center the oscillator 2 section to match layout
    auto sectionWidth = 200; // Same width as in layout function
    secondOscSectionBounds = secondOscSectionBounds.withSizeKeepingCentre(sectionWidth, 100);
    secondOscSectionBounds = secondOscSectionBounds.translated(
        static_cast<int>(secondOscillatorGroupOffsetX), 
        static_cast<int>(secondOscillatorGroupOffsetY)
    );
    drawSecondOscillatorBackground(g, secondOscillatorBounds);
    // drawOsc2PitchControlsBackground(g, osc2PitchControlsBounds); // REMOVED - individual pitch control backgrounds
    // drawOsc2PhaseControlsBackground(g, osc2PhaseControlsBounds); // Removed - was covering the knob
    
    // Filter section background
    drawFilterSectionBackground(g, filterSectionBounds);
    
    // Macro knobs background
    drawMacroKnobsBackground(g, macroKnobsBounds);
    
    // Main window border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::paintOverChildren(juce::Graphics& g)
{
    // Precision alignment grid overlay - symmetrical and aligned to boundaries
    // ENABLED FOR ALIGNMENT - DRAWN OVER ALL CHILD COMPONENTS
    auto gridBounds = getLocalBounds().toFloat();
    auto centerX = gridBounds.getWidth() / 2.0f;
    auto centerY = gridBounds.getHeight() / 2.0f;
    
    // Main synth page grid temporarily commented out
    /*
    g.setColour(juce::Colours::cyan.withAlpha(0.3f));
    
    // Vertical grid lines - symmetrical from center
    for (int i = 1; i * 20 < centerX; ++i)
    {
        float offset = i * 20.0f;
        // Right side
        g.drawLine(centerX + offset, 0, centerX + offset, gridBounds.getHeight(), 0.5f);
        // Left side
        g.drawLine(centerX - offset, 0, centerX - offset, gridBounds.getHeight(), 0.5f);
    }
    
    // Horizontal grid lines - symmetrical from center
    for (int i = 1; i * 20 < centerY; ++i)
    {
        float offset = i * 20.0f;
        // Bottom half
        g.drawLine(0, centerY + offset, gridBounds.getWidth(), centerY + offset, 0.5f);
        // Top half
        g.drawLine(0, centerY - offset, gridBounds.getWidth(), centerY - offset, 0.5f);
    }
    
    // Center lines
    g.setColour(juce::Colours::cyan.withAlpha(0.6f));
    g.drawLine(centerX, 0, centerX, gridBounds.getHeight(), 1.0f); // Vertical center
    g.drawLine(0, centerY, gridBounds.getWidth(), centerY, 1.0f);   // Horizontal center
    
    // Major grid lines every 100 pixels - also symmetrical
    g.setColour(juce::Colours::cyan.withAlpha(0.5f));
    for (int i = 1; i * 100 < centerX; ++i)
    {
        float offset = i * 100.0f;
        g.drawLine(centerX + offset, 0, centerX + offset, gridBounds.getHeight(), 1.0f);
        g.drawLine(centerX - offset, 0, centerX - offset, gridBounds.getHeight(), 1.0f);
    }
    for (int i = 1; i * 100 < centerY; ++i)
    {
        float offset = i * 100.0f;
        g.drawLine(0, centerY + offset, gridBounds.getWidth(), centerY + offset, 1.0f);
        g.drawLine(0, centerY - offset, gridBounds.getWidth(), centerY - offset, 1.0f);
    }
    */
    
    // Draw white bracket lines for EQ bands (only if we're in the effects module tab)
    if (effectsModule.getCurrentTabIndex() == 4) // EQ tab index
    {
        g.setColour(juce::Colours::lime); // Using named color lime
        
        // Get the effects module bounds to calculate relative positions
        auto effectsBounds = effectsModule.getBounds();
        
        // Band 1 bracket lines (upside-down brackets around left group with symmetric gap for label)
        // Top horizontal line - left part (equal length segments with gap for "BAND 1" label)
        g.drawLine(effectsBounds.getX() + 31, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 61, effectsBounds.getY() + 350, 1.0f);
        // Top horizontal line - right part (same length as left part)
        g.drawLine(effectsBounds.getX() + 101, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 131, effectsBounds.getY() + 350, 1.0f);
        // Left vertical line
        g.drawLine(effectsBounds.getX() + 31, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 31, effectsBounds.getY() + 365, 1.0f);
        // Right vertical line  
        g.drawLine(effectsBounds.getX() + 131, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 131, effectsBounds.getY() + 365, 1.0f);
        
        // Band 2 bracket lines (upside-down brackets around right group with symmetric gap for label)
        // Top horizontal line - left part (equal length segments with gap for "BAND 2" label)
        g.drawLine(effectsBounds.getX() + 169, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 199, effectsBounds.getY() + 350, 1.0f);
        // Top horizontal line - right part (same length as left part)
        g.drawLine(effectsBounds.getX() + 239, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 269, effectsBounds.getY() + 350, 1.0f);
        // Left vertical line
        g.drawLine(effectsBounds.getX() + 169, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 169, effectsBounds.getY() + 365, 1.0f);
        // Right vertical line
        g.drawLine(effectsBounds.getX() + 269, effectsBounds.getY() + 350, 
                   effectsBounds.getX() + 269, effectsBounds.getY() + 365, 1.0f);
    }
    
    // Draw macro indicators on top of everything
    drawMacroIndicators(g);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Original oscillator bounds - unchanged!
    bounds.reduce(20, 20);
    bounds.removeFromTop(20); // spacing
    
    // Apply oscillator offset to move all UI components together
    bounds = bounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    
    // Use grouped layout methods
    layoutWaveTypeButtons(bounds);
    layoutADSREnvelope(bounds);
    layoutADSRKnobs(bounds);
    layoutVolumeKnobs(bounds);
    layoutOctaveControls(bounds);
    layoutPhaseControls(bounds);
    layoutLFOModule(bounds);
    layoutEffectsModule(bounds);
    layoutSecondOscillator(bounds);
    
    // MACRO KNOBS - Center bottom positioning (2 rows of 4 columns)
    auto totalBounds = getLocalBounds();
    auto macroKnobSize = 45; // 10% smaller than 50
    auto macroLabelHeight = 15;
    auto macroKnobSpacing = 18; // Slightly smaller spacing too
    auto macroRowSpacing = 10;
    auto totalMacroWidth = (4 * macroKnobSize) + (3 * macroKnobSpacing);
    auto totalMacroHeight = (2 * (macroKnobSize + macroLabelHeight)) + macroRowSpacing;
    
    // Position at center bottom
    auto macroStartX = (totalBounds.getWidth() - totalMacroWidth) / 2;
    auto macroY = totalBounds.getHeight() - totalMacroHeight - 12; // 12px margin from bottom (moved down 3px)
    
    // Store bounds for background drawing (with reduced padding for less width) - keep background at original position
    auto backgroundY = totalBounds.getHeight() - totalMacroHeight - 20; // Original background position
    macroKnobsBounds = juce::Rectangle<int>(macroStartX - 40, backgroundY - 13, totalMacroWidth + 80, totalMacroHeight + 11);
    
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

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeControlsVolumeKnob)
    {
        audioProcessor.setOsc1Volume(static_cast<float>(volumeControlsVolumeKnob.getValue()));
    }
    else if (slider == &volumeControlsDetuneKnob)
    {
        audioProcessor.setOsc1Detune(static_cast<float>(volumeControlsDetuneKnob.getValue()));
    }
    else if (slider == &volumeControlsStereoWidthKnob)
    {
        audioProcessor.setSynthStereoWidth(static_cast<float>(volumeControlsStereoWidthKnob.getValue()));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setSynthPan(static_cast<float>(volumeControlsPanKnob.getValue()));
    }
    else if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setSynthPhase(static_cast<float>(phaseControlsPhaseKnob.getValue()));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(adsrAttackKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2AttackKnob.setValue(adsrAttackKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Attack(static_cast<float>(adsrAttackKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(adsrDecayKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2DecayKnob.setValue(adsrDecayKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Decay(static_cast<float>(adsrDecayKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2SustainKnob.setValue(adsrSustainKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(adsrReleaseKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            osc2ReleaseKnob.setValue(adsrReleaseKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Release(static_cast<float>(adsrReleaseKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2VolumeKnob)
    {
        audioProcessor.setOsc2Volume(static_cast<float>(osc2VolumeKnob.getValue()));
    }
    else if (slider == &osc2DetuneKnob)
    {
        audioProcessor.setOsc2Detune(static_cast<float>(osc2DetuneKnob.getValue()));
    }
    else if (slider == &osc2StereoKnob)
    {
        audioProcessor.setOsc2Stereo(static_cast<float>(osc2StereoKnob.getValue()));
    }
    else if (slider == &osc2AttackKnob)
    {
        audioProcessor.setOsc2Attack(static_cast<float>(osc2AttackKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrAttackKnob.setValue(osc2AttackKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Attack(static_cast<float>(osc2AttackKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2DecayKnob)
    {
        audioProcessor.setOsc2Decay(static_cast<float>(osc2DecayKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrDecayKnob.setValue(osc2DecayKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Decay(static_cast<float>(osc2DecayKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2SustainKnob)
    {
        audioProcessor.setOsc2Sustain(static_cast<float>(osc2SustainKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrSustainKnob.setValue(osc2SustainKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Sustain(static_cast<float>(osc2SustainKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2ReleaseKnob)
    {
        audioProcessor.setOsc2Release(static_cast<float>(osc2ReleaseKnob.getValue()));
        
        // If ADSR is linked, also update oscillator 1
        if (osc2AdsrLinkButton.getToggleState())
        {
            adsrReleaseKnob.setValue(osc2ReleaseKnob.getValue(), juce::dontSendNotification);
            audioProcessor.setOsc2Release(static_cast<float>(osc2ReleaseKnob.getValue()));
        }
        
        updateEnvelopeDisplay();
    }
    // osc2VoicesKnob removed - now handled by mouse events
    else if (slider == &osc2PanKnob)
    {
        audioProcessor.setOsc2Pan(static_cast<float>(osc2PanKnob.getValue()));
    }
    // osc2OctaveKnob removed - now handled by mouse events
    // osc2SemitoneKnob removed - now handled by mouse events
    // osc2FineTuneKnob removed - now handled by mouse events
    else if (slider == &osc2PhaseKnob)
    {
        audioProcessor.setOsc2Phase(static_cast<float>(osc2PhaseKnob.getValue()));
    }
    else if (slider == &filterCutoffKnob)
    {
        audioProcessor.setFilterCutoff(static_cast<float>(filterCutoffKnob.getValue()));
    }
    else if (slider == &filterResonanceKnob)
    {
        audioProcessor.setFilterResonance(static_cast<float>(filterResonanceKnob.getValue()));
    }
    // Chorus effect sliders
    else if (slider == &chorusRateKnob)
    {
        audioProcessor.setChorusRate(static_cast<float>(chorusRateKnob.getValue()));
    }
    else if (slider == &chorusDelay1Knob)
    {
        audioProcessor.setChorusDelay1(static_cast<float>(chorusDelay1Knob.getValue()));
    }
    else if (slider == &chorusDelay2Knob)
    {
        audioProcessor.setChorusDelay2(static_cast<float>(chorusDelay2Knob.getValue()));
    }
    else if (slider == &chorusDepthKnob)
    {
        audioProcessor.setChorusDepth(static_cast<float>(chorusDepthKnob.getValue()));
    }
    else if (slider == &chorusFeedKnob)
    {
        audioProcessor.setChorusFeedback(static_cast<float>(chorusFeedKnob.getValue()));
    }
    else if (slider == &chorusLpfKnob)
    {
        // Convert 0-100% to filter frequency: 0% = 20000Hz (no filter), 100% = 200Hz (heavy filter)
        float percentage = static_cast<float>(chorusLpfKnob.getValue()) / 100.0f; // 0.0 to 1.0
        float filterFreq = 20000.0f - (percentage * (20000.0f - 200.0f)); // 20000Hz to 200Hz
        audioProcessor.setChorusLPF(filterFreq);
    }
    else if (slider == &chorusMixKnob)
    {
        audioProcessor.setChorusMix(static_cast<float>(chorusMixKnob.getValue()));
    }
    // Flanger effect sliders
    else if (slider == &flangerRateKnob)
    {
        audioProcessor.setFlangerRate(static_cast<float>(flangerRateKnob.getValue()));
    }
    else if (slider == &flangerDepthKnob)
    {
        audioProcessor.setFlangerDepth(static_cast<float>(flangerDepthKnob.getValue()));
    }
    else if (slider == &flangerFeedbackKnob)
    {
        audioProcessor.setFlangerFeedback(static_cast<float>(flangerFeedbackKnob.getValue()));
    }
    else if (slider == &flangerMixKnob)
    {
        audioProcessor.setFlangerMix(static_cast<float>(flangerMixKnob.getValue()));
    }
    else if (slider == &flangerPhaseKnob)
    {
        audioProcessor.setFlangerPhase(static_cast<float>(flangerPhaseKnob.getValue()));
    }
    // Phaser effect sliders
    else if (slider == &phaserRateKnob)
    {
        audioProcessor.setPhaserRate(static_cast<float>(phaserRateKnob.getValue()));
    }
    else if (slider == &phaserDepth1Knob)
    {
        audioProcessor.setPhaserDepth1(static_cast<float>(phaserDepth1Knob.getValue()));
    }
    else if (slider == &phaserDepth2Knob)
    {
        audioProcessor.setPhaserDepth2(static_cast<float>(phaserDepth2Knob.getValue()));
    }
    else if (slider == &phaserFeedbackKnob)
    {
        audioProcessor.setPhaserFeedback(static_cast<float>(phaserFeedbackKnob.getValue()));
    }
    else if (slider == &phaserMixKnob)
    {
        audioProcessor.setPhaserMix(static_cast<float>(phaserMixKnob.getValue()));
    }
    else if (slider == &phaserPhaseKnob)
    {
        audioProcessor.setPhaserPhase(static_cast<float>(phaserPhaseKnob.getValue()));
    }
    else if (slider == &phaserFrequencyKnob)
    {
        audioProcessor.setPhaserFrequency(static_cast<float>(phaserFrequencyKnob.getValue()));
    }
    // Compressor controls
    else if (slider == &compressorThresholdKnob)
    {
        audioProcessor.setCompressorThreshold(static_cast<float>(compressorThresholdKnob.getValue()));
    }
    else if (slider == &compressorRatioKnob)
    {
        float ratio = static_cast<float>(compressorRatioKnob.getValue());
        audioProcessor.setCompressorRatio(ratio);
        compressorRatioValueLabel.setText(juce::String(ratio, 1) + ":1", juce::dontSendNotification);
    }
    else if (slider == &compressorAttackKnob)
    {
        float attack = static_cast<float>(compressorAttackKnob.getValue());
        audioProcessor.setCompressorAttack(attack);
        compressorAttackValueLabel.setText(juce::String(attack, 1) + "ms", juce::dontSendNotification);
    }
    else if (slider == &compressorReleaseKnob)
    {
        float release = static_cast<float>(compressorReleaseKnob.getValue());
        audioProcessor.setCompressorRelease(release);
        compressorReleaseValueLabel.setText(juce::String(release, 0) + "ms", juce::dontSendNotification);
    }
    else if (slider == &compressorGainKnob)
    {
        audioProcessor.setCompressorGain(static_cast<float>(compressorGainKnob.getValue()));
    }
    else if (slider == &compressorMixKnob)
    {
        audioProcessor.setCompressorMix(static_cast<float>(compressorMixKnob.getValue()));
    }
    else if (slider == &distortionDriveKnob)
    {
        audioProcessor.setDistortionDrive(static_cast<float>(distortionDriveKnob.getValue()));
    }
    else if (slider == &distortionMixKnob)
    {
        audioProcessor.setDistortionMix(static_cast<float>(distortionMixKnob.getValue()));
    }
    else if (slider == &distortionFilterFreqKnob)
    {
        audioProcessor.setDistortionFilterFreq(static_cast<float>(distortionFilterFreqKnob.getValue()));
    }
    else if (slider == &distortionFilterQKnob)
    {
        audioProcessor.setDistortionFilterQ(static_cast<float>(distortionFilterQKnob.getValue()));
    }
    
    // Delay effect sliders
    else if (slider == &delayFeedbackKnob)
    {
        audioProcessor.setDelayFeedback(static_cast<float>(delayFeedbackKnob.getValue()));
    }
    else if (slider == &delayMixKnob)
    {
        audioProcessor.setDelayMix(static_cast<float>(delayMixKnob.getValue()));
    }
    else if (slider == &delayLeftTimeSlider)
    {
        float leftTime = static_cast<float>(delayLeftTimeSlider.getValue());
        audioProcessor.setDelayLeftTime(leftTime);
        
        // If link is enabled, also update right time
        if (delayLinkButton.getToggleState())
        {
            delayRightTimeSlider.setValue(leftTime, juce::dontSendNotification);
            audioProcessor.setDelayRightTime(leftTime);
        }
    }
    else if (slider == &delayRightTimeSlider)
    {
        float rightTime = static_cast<float>(delayRightTimeSlider.getValue());
        audioProcessor.setDelayRightTime(rightTime);
        
        // If link is enabled, also update left time
        if (delayLinkButton.getToggleState())
        {
            delayLeftTimeSlider.setValue(rightTime, juce::dontSendNotification);
            audioProcessor.setDelayLeftTime(rightTime);
        }
    }
    else if (slider == &delayFilterFreqKnob)
    {
        audioProcessor.setDelayFilterFreq(static_cast<float>(delayFilterFreqKnob.getValue()));
    }
    else if (slider == &delayFilterQKnob)
    {
        audioProcessor.setDelayFilterQ(static_cast<float>(delayFilterQKnob.getValue()));
    }
    
    /*
    else if (slider == &pulseWidthSlider)
    {
        audioProcessor.setOsc1PulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    }
    */
    
    // REVERB CONTROLS
    else if (slider == &reverbMixKnob)
    {
        audioProcessor.setReverbMix(static_cast<float>(reverbMixKnob.getValue()));
    }
    else if (slider == &reverbLowCutKnob)
    {
        audioProcessor.setReverbLowCut(static_cast<float>(reverbLowCutKnob.getValue()));
    }
    else if (slider == &reverbHighCutKnob)
    {
        audioProcessor.setReverbHighCut(static_cast<float>(reverbHighCutKnob.getValue()));
    }
    else if (slider == &reverbSizeKnob)
    {
        audioProcessor.setReverbSize(static_cast<float>(reverbSizeKnob.getValue()));
    }
    else if (slider == &reverbPreDelayKnob)
    {
        audioProcessor.setReverbPreDelay(static_cast<float>(reverbPreDelayKnob.getValue()));
    }
    else if (slider == &reverbDampKnob)
    {
        audioProcessor.setReverbDamping(static_cast<float>(reverbDampKnob.getValue()));
    }
    else if (slider == &reverbWidthKnob)
    {
        audioProcessor.setReverbWidth(static_cast<float>(reverbWidthKnob.getValue()));
    }
    // EQ CONTROLS
    else if (slider == &eq1FreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1FreqKnob.getValue()));
        parametricEQ.getBand(0).frequency = static_cast<float>(eq1FreqKnob.getValue());
        eq1NewFreqKnob.setValue(eq1FreqKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq1QKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1QKnob.getValue()));
        parametricEQ.getBand(0).q = static_cast<float>(eq1QKnob.getValue());
        eq1NewQKnob.setValue(eq1QKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.repaint();
    }
    else if (slider == &eq1GainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1GainKnob.getValue()));
        parametricEQ.getBand(0).gain = static_cast<float>(eq1GainKnob.getValue());
        eq1NewGainKnob.setValue(eq1GainKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2FreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2FreqKnob.getValue()));
        parametricEQ.getBand(1).frequency = static_cast<float>(eq2FreqKnob.getValue());
        eq2NewFreqKnob.setValue(eq2FreqKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2QKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2QKnob.getValue()));
        parametricEQ.getBand(1).q = static_cast<float>(eq2QKnob.getValue());
        eq2NewQKnob.setValue(eq2QKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2GainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2GainKnob.getValue()));
        parametricEQ.getBand(1).gain = static_cast<float>(eq2GainKnob.getValue());
        eq2NewGainKnob.setValue(eq2GainKnob.getValue(), juce::dontSendNotification); // Sync with new knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    // NEW EQ KNOBS - Additional controls that auto-sync with graphic
    else if (slider == &eq1NewFreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1NewFreqKnob.getValue()));
        parametricEQ.getBand(0).frequency = static_cast<float>(eq1NewFreqKnob.getValue());
        eq1FreqKnob.setValue(eq1NewFreqKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq1NewQKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1NewQKnob.getValue()));
        parametricEQ.getBand(0).q = static_cast<float>(eq1NewQKnob.getValue());
        eq1QKnob.setValue(eq1NewQKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewFreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2NewFreqKnob.getValue()));
        parametricEQ.getBand(1).frequency = static_cast<float>(eq2NewFreqKnob.getValue());
        eq2FreqKnob.setValue(eq2NewFreqKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.resized(); // Update visual positions
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewQKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2NewQKnob.getValue()));
        parametricEQ.getBand(1).q = static_cast<float>(eq2NewQKnob.getValue());
        eq2QKnob.setValue(eq2NewQKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq1NewGainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1NewGainKnob.getValue()));
        parametricEQ.getBand(0).gain = static_cast<float>(eq1NewGainKnob.getValue());
        eq1GainKnob.setValue(eq1NewGainKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    else if (slider == &eq2NewGainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2NewGainKnob.getValue()));
        parametricEQ.getBand(1).gain = static_cast<float>(eq2NewGainKnob.getValue());
        eq2GainKnob.setValue(eq2NewGainKnob.getValue(), juce::dontSendNotification); // Sync with existing knob
        parametricEQ.repaint();
    }
    // Macro knobs - update all linked parameters
    else if (slider == &macro1Knob)
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

void SynthesizerComponent::buttonClicked(juce::Button* button)
{
    if (button == &waveTypeSineButton)
    {
        if (waveTypeSineButton.getToggleState())
        {
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(0); // 0 = sine wave
        }
        else
        {
            waveTypeSineButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeSawButton)
    {
        if (waveTypeSawButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(1); // 1 = saw wave
        }
        else
        {
            waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeSquareButton)
    {
        if (waveTypeSquareButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(2); // 2 = square wave
        }
        else
        {
            waveTypeSquareButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeTriangleButton)
    {
        if (waveTypeTriangleButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(3); // 3 = triangle wave
        }
        else
        {
            waveTypeTriangleButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeWhiteNoiseButton)
    {
        if (waveTypeWhiteNoiseButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypePinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(4); // 4 = white noise
        }
        else
        {
            waveTypeWhiteNoiseButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypePinkNoiseButton)
    {
        if (waveTypePinkNoiseButton.getToggleState())
        {
            waveTypeSineButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSawButton.setToggleState(false, juce::dontSendNotification);
            waveTypeSquareButton.setToggleState(false, juce::dontSendNotification);
            waveTypeTriangleButton.setToggleState(false, juce::dontSendNotification);
            waveTypeWhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc1Type(5); // 5 = pink noise
        }
        else
        {
            waveTypePinkNoiseButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &waveTypeRandomPhaseButton)
    {
        // Random phase button is independent - doesn't affect other buttons
        audioProcessor.setOsc1RandomPhase(waveTypeRandomPhaseButton.getToggleState());
    }
    else if (button == &osc2SineButton)
    {
        if (osc2SineButton.getToggleState())
        {
            // Select sine wave for oscillator 2
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(0); // 0 = sine
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting sine, keep at least one selected
            if (!osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SineButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2SawButton)
    {
        if (osc2SawButton.getToggleState())
        {
            // Select saw wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(1); // 1 = saw
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting saw, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SawButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2SquareButton)
    {
        if (osc2SquareButton.getToggleState())
        {
            // Select square wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(2); // 2 = square
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting square, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2SquareButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2TriangleButton)
    {
        if (osc2TriangleButton.getToggleState())
        {
            // Select triangle wave for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(3); // 3 = triangle
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting triangle, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2TriangleButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2WhiteNoiseButton)
    {
        if (osc2WhiteNoiseButton.getToggleState())
        {
            // Select white noise for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2PinkNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(4); // 4 = white noise
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting white noise, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2PinkNoiseButton.getToggleState())
            {
                osc2WhiteNoiseButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2PinkNoiseButton)
    {
        if (osc2PinkNoiseButton.getToggleState())
        {
            // Select pink noise for oscillator 2
            osc2SineButton.setToggleState(false, juce::dontSendNotification);
            osc2SawButton.setToggleState(false, juce::dontSendNotification);
            osc2SquareButton.setToggleState(false, juce::dontSendNotification);
            osc2TriangleButton.setToggleState(false, juce::dontSendNotification);
            osc2WhiteNoiseButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOsc2Type(5); // 5 = pink noise
            audioProcessor.setOsc2Enabled(true);
        }
        else
        {
            // If deselecting pink noise, keep at least one selected
            if (!osc2SineButton.getToggleState() && !osc2SawButton.getToggleState() && !osc2SquareButton.getToggleState() && !osc2TriangleButton.getToggleState() && !osc2WhiteNoiseButton.getToggleState())
            {
                osc2PinkNoiseButton.setToggleState(true, juce::dontSendNotification);
            }
            else
            {
                audioProcessor.setOsc2Enabled(false);
            }
        }
    }
    else if (button == &osc2RandomPhaseButton)
    {
        audioProcessor.setOsc2RandomPhase(osc2RandomPhaseButton.getToggleState());
    }
    else if (button == &osc2AdsrLinkButton)
    {
        if (osc2AdsrLinkButton.getToggleState())
        {
            // Store oscillator 2's current ADSR values before linking
            osc2OriginalAttack = static_cast<float>(osc2AttackKnob.getValue());
            osc2OriginalDecay = static_cast<float>(osc2DecayKnob.getValue());
            osc2OriginalSustain = static_cast<float>(osc2SustainKnob.getValue());
            osc2OriginalRelease = static_cast<float>(osc2ReleaseKnob.getValue());
            
            // Link oscillator 2 ADSR to oscillator 1 - copy current values
            osc2AttackKnob.setValue(adsrAttackKnob.getValue(), juce::dontSendNotification);
            osc2DecayKnob.setValue(adsrDecayKnob.getValue(), juce::dontSendNotification);
            osc2SustainKnob.setValue(adsrSustainKnob.getValue(), juce::dontSendNotification);
            osc2ReleaseKnob.setValue(adsrReleaseKnob.getValue(), juce::dontSendNotification);
            
            // Update audio processor with linked values
            audioProcessor.setOsc2Attack(static_cast<float>(adsrAttackKnob.getValue()));
            audioProcessor.setOsc2Decay(static_cast<float>(adsrDecayKnob.getValue()));
            audioProcessor.setOsc2Sustain(static_cast<float>(adsrSustainKnob.getValue()));
            audioProcessor.setOsc2Release(static_cast<float>(adsrReleaseKnob.getValue()));
            
            // Update the envelope display
            updateEnvelopeDisplay();
        }
        else
        {
            // Restore oscillator 2's original ADSR values when unlinking
            osc2AttackKnob.setValue(osc2OriginalAttack, juce::dontSendNotification);
            osc2DecayKnob.setValue(osc2OriginalDecay, juce::dontSendNotification);
            osc2SustainKnob.setValue(osc2OriginalSustain, juce::dontSendNotification);
            osc2ReleaseKnob.setValue(osc2OriginalRelease, juce::dontSendNotification);
            
            // Update audio processor with restored values
            audioProcessor.setOsc2Attack(osc2OriginalAttack);
            audioProcessor.setOsc2Decay(osc2OriginalDecay);
            audioProcessor.setOsc2Sustain(osc2OriginalSustain);
            audioProcessor.setOsc2Release(osc2OriginalRelease);
            
            // Update the envelope display
            updateEnvelopeDisplay();
        }
    }
    else if (button == &osc1FilterEnableButton)
    {
        audioProcessor.setOsc1FilterEnabled(osc1FilterEnableButton.getToggleState());
    }
    else if (button == &osc2FilterEnableButton)
    {
        audioProcessor.setOsc2FilterEnabled(osc2FilterEnableButton.getToggleState());
    }
    else if (button == &filterLPButton)
    {
        // Radio button behavior - LP cannot be unselected, only HP can be selected instead
        if (!filterLPButton.getToggleState())
        {
            // Prevent deselection - keep LP selected
            filterLPButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // LP selected - deselect all other filters
        filterHPButton.setToggleState(false, juce::dontSendNotification);
        filterBPButton.setToggleState(false, juce::dontSendNotification);
        filterNotchButton.setToggleState(false, juce::dontSendNotification);
        filterCombButton.setToggleState(false, juce::dontSendNotification);
        filterFormantButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(true);
        audioProcessor.setFilterHPEnabled(false);
        audioProcessor.setFilterBPEnabled(false);
        audioProcessor.setFilterNotchEnabled(false);
        audioProcessor.setFilterCombEnabled(false);
        audioProcessor.setFilterFormantEnabled(false);
    }
    else if (button == &filterHPButton)
    {
        // Radio button behavior - HP cannot be unselected, only LP can be selected instead
        if (!filterHPButton.getToggleState())
        {
            // Prevent deselection - keep HP selected
            filterHPButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // HP selected - deselect all other filters
        filterLPButton.setToggleState(false, juce::dontSendNotification);
        filterBPButton.setToggleState(false, juce::dontSendNotification);
        filterNotchButton.setToggleState(false, juce::dontSendNotification);
        filterCombButton.setToggleState(false, juce::dontSendNotification);
        filterFormantButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(false);
        audioProcessor.setFilterHPEnabled(true);
        audioProcessor.setFilterBPEnabled(false);
        audioProcessor.setFilterNotchEnabled(false);
        audioProcessor.setFilterCombEnabled(false);
        audioProcessor.setFilterFormantEnabled(false);
    }
    else if (button == &filterBPButton)
    {
        // Radio button behavior - BP cannot be unselected, only LP or HP can be selected instead
        if (!filterBPButton.getToggleState())
        {
            // Prevent deselection - keep BP selected
            filterBPButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // BP selected - deselect all other filters
        filterLPButton.setToggleState(false, juce::dontSendNotification);
        filterHPButton.setToggleState(false, juce::dontSendNotification);
        filterNotchButton.setToggleState(false, juce::dontSendNotification);
        filterCombButton.setToggleState(false, juce::dontSendNotification);
        filterFormantButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(false);
        audioProcessor.setFilterHPEnabled(false);
        audioProcessor.setFilterBPEnabled(true);
        audioProcessor.setFilterNotchEnabled(false);
        audioProcessor.setFilterCombEnabled(false);
        audioProcessor.setFilterFormantEnabled(false);
    }
    else if (button == &filterNotchButton)
    {
        // Radio button behavior - Notch cannot be unselected, only LP, HP, or BP can be selected instead
        if (!filterNotchButton.getToggleState())
        {
            // Prevent deselection - keep Notch selected
            filterNotchButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // Notch selected - deselect all other filters
        filterLPButton.setToggleState(false, juce::dontSendNotification);
        filterHPButton.setToggleState(false, juce::dontSendNotification);
        filterBPButton.setToggleState(false, juce::dontSendNotification);
        filterCombButton.setToggleState(false, juce::dontSendNotification);
        filterFormantButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(false);
        audioProcessor.setFilterHPEnabled(false);
        audioProcessor.setFilterBPEnabled(false);
        audioProcessor.setFilterNotchEnabled(true);
        audioProcessor.setFilterCombEnabled(false);
        audioProcessor.setFilterFormantEnabled(false);
    }
    else if (button == &filterCombButton)
    {
        // Radio button behavior - Comb cannot be unselected, only LP, HP, BP, or Notch can be selected instead
        if (!filterCombButton.getToggleState())
        {
            // Prevent deselection - keep Comb selected
            filterCombButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // Comb selected - deselect LP, HP, BP, Notch, and Formant
        filterLPButton.setToggleState(false, juce::dontSendNotification);
        filterHPButton.setToggleState(false, juce::dontSendNotification);
        filterBPButton.setToggleState(false, juce::dontSendNotification);
        filterNotchButton.setToggleState(false, juce::dontSendNotification);
        filterFormantButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(false);
        audioProcessor.setFilterHPEnabled(false);
        audioProcessor.setFilterBPEnabled(false);
        audioProcessor.setFilterNotchEnabled(false);
        audioProcessor.setFilterCombEnabled(true);
        audioProcessor.setFilterFormantEnabled(false);
    }
    else if (button == &filterFormantButton)
    {
        // Radio button behavior - Formant cannot be unselected, only other filters can be selected instead
        if (!filterFormantButton.getToggleState())
        {
            // Prevent deselection - keep Formant selected
            filterFormantButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // Formant selected - deselect all other filters
        filterLPButton.setToggleState(false, juce::dontSendNotification);
        filterHPButton.setToggleState(false, juce::dontSendNotification);
        filterBPButton.setToggleState(false, juce::dontSendNotification);
        filterNotchButton.setToggleState(false, juce::dontSendNotification);
        filterCombButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilterLPEnabled(false);
        audioProcessor.setFilterHPEnabled(false);
        audioProcessor.setFilterBPEnabled(false);
        audioProcessor.setFilterNotchEnabled(false);
        audioProcessor.setFilterCombEnabled(false);
        audioProcessor.setFilterFormantEnabled(true);
    }
    else if (button == &filter12dBButton)
    {
        // Radio button behavior - 12dB cannot be unselected, only 24dB can be selected instead
        if (!filter12dBButton.getToggleState())
        {
            // Prevent deselection - keep 12dB selected
            filter12dBButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // 12dB selected - deselect 24dB
        filter24dBButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilter12dBEnabled(true);
        audioProcessor.setFilter24dBEnabled(false);
    }
    else if (button == &filter24dBButton)
    {
        // Radio button behavior - 24dB cannot be unselected, only 12dB can be selected instead
        if (!filter24dBButton.getToggleState())
        {
            // Prevent deselection - keep 24dB selected
            filter24dBButton.setToggleState(true, juce::dontSendNotification);
            return;
        }
        
        // 24dB selected - deselect 12dB
        filter12dBButton.setToggleState(false, juce::dontSendNotification);
        audioProcessor.setFilter12dBEnabled(false);
        audioProcessor.setFilter24dBEnabled(true);
    }
    // EQ On/Off button
    else if (button == &eqOnOffButton)
    {
        if (eqOnOffButton.getToggleState())
        {
            eqOnOffButton.setButtonText("EQ ON");
            audioProcessor.setEQEnabled(true);
            parametricEQ.setVisible(true);
            parametricEQ.syncWithDSPState(); // Sync when enabled
        }
        else
        {
            eqOnOffButton.setButtonText("EQ OFF");
            audioProcessor.setEQEnabled(false);
            parametricEQ.setVisible(true); // Keep visible but show it's disabled
        }
        parametricEQ.repaint();
    }
    // EQ Band 1 buttons
    else if (button == &eq1PeakButton)
    {
        if (eq1PeakButton.getToggleState())
        {
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ1Type(0); // 0 = Peak
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1PeakButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq1ShelfButton)
    {
        if (eq1ShelfButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ1Type(1); // 1 = Low Shelf
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1ShelfButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq1PassButton)
    {
        if (eq1PassButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(0, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ1Type(2); // 2 = High Pass
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq1PassButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    // EQ Band 2 buttons
    else if (button == &eq2PeakButton)
    {
        if (eq2PeakButton.getToggleState())
        {
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ2Type(0); // 0 = Peak
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2PeakButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq2ShelfButton)
    {
        if (eq2ShelfButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ2Type(1); // 1 = High Shelf
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2ShelfButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &eq2PassButton)
    {
        if (eq2PassButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ.setBandFilterType(1, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ2Type(2); // 2 = Low Pass
            parametricEQ.syncWithDSPState();
        }
        else
        {
            eq2PassButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    // EQ Band on/off buttons
    else if (button == &eq1OnOffButton)
    {
        audioProcessor.setEQ1Enabled(eq1OnOffButton.getToggleState());
        parametricEQ.syncWithDSPState();
    }
    else if (button == &eq2OnOffButton)
    {
        audioProcessor.setEQ2Enabled(eq2OnOffButton.getToggleState());
        parametricEQ.syncWithDSPState();
    }
    // Chorus effect power button
    else if (button == &chorusPowerButton)
    {
        audioProcessor.setChorusEnabled(chorusPowerButton.getToggleState());
    }
    // Flanger effect buttons
    else if (button == &flangerPowerButton)
    {
        audioProcessor.setFlangerEnabled(flangerPowerButton.getToggleState());
    }
    else if (button == &flangerBpmButton)
    {
        // TODO: Implement BPM sync for flanger if needed
        // For now, just toggle the button state
    }
    // Phaser effect buttons
    else if (button == &phaserPowerButton)
    {
        audioProcessor.setPhaserEnabled(phaserPowerButton.getToggleState());
    }
    else if (button == &phaserBpmButton)
    {
        // TODO: Implement BPM sync for phaser if needed
        // For now, just toggle the button state
    }
    // Compressor effect buttons
    else if (button == &compressorPowerButton)
    {
        audioProcessor.setCompressorEnabled(compressorPowerButton.getToggleState());
    }
    else if (button == &compressorMultibandButton)
    {
        audioProcessor.setCompressorMultiband(compressorMultibandButton.getToggleState());
    }
    else if (button == &distortionPowerButton)
    {
        audioProcessor.setDistortionEnabled(distortionPowerButton.getToggleState());
    }
    else if (button == &delayPowerButton)
    {
        audioProcessor.setDelayEnabled(delayPowerButton.getToggleState());
    }
    
    // Distortion filter position buttons (exclusive)
    else if (button == &distortionFilterOffButton)
    {
        if (distortionFilterOffButton.getToggleState())
        {
            distortionFilterPreButton.setToggleState(false, juce::dontSendNotification);
            distortionFilterPostButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setDistortionFilterPosition(0); // OFF
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
            audioProcessor.setDistortionFilterPosition(1); // PRE
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
            audioProcessor.setDistortionFilterPosition(2); // POST
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
            audioProcessor.setDistortionFilterType(1); // LP
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
            audioProcessor.setDistortionFilterType(2); // BP
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
            audioProcessor.setDistortionFilterType(3); // HP
        }
        else
        {
            distortionFilterHPButton.setToggleState(true, juce::dontSendNotification); // Force stay on
        }
    }
    
    // Delay effect buttons
    else if (button == &delayBpmButton)
    {
        audioProcessor.setDelaySync(delayBpmButton.getToggleState());
    }
    else if (button == &delayLinkButton)
    {
        // Link button controls if left/right times are linked
        if (delayLinkButton.getToggleState())
        {
            // When linking, sync right time to left time
            float leftTime = delayLeftTimeSlider.getValue();
            delayRightTimeSlider.setValue(leftTime, juce::sendNotification);
        }
    }
    else if (button == &delayNormalButton)
    {
        if (delayNormalButton.getToggleState())
        {
            delayPingPongButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setDelayPingPong(false);
        }
        else
        {
            delayNormalButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &delayPingPongButton)
    {
        if (delayPingPongButton.getToggleState())
        {
            delayNormalButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setDelayPingPong(true);
        }
        else
        {
            delayPingPongButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &delayLeftTripletButton)
    {
        audioProcessor.setDelayTriplet(delayLeftTripletButton.getToggleState());
    }
    else if (button == &delayLeftDotButton)
    {
        audioProcessor.setDelayDotted(delayLeftDotButton.getToggleState());
    }
    else if (button == &delayRightTripletButton)
    {
        audioProcessor.setDelayRTriplet(delayRightTripletButton.getToggleState());
    }
    else if (button == &delayRightDotButton)
    {
        audioProcessor.setDelayRDotted(delayRightDotButton.getToggleState());
    }
    
    // REVERB CONTROLS
    else if (button == &reverbPowerButton)
    {
        audioProcessor.setReverbEnabled(reverbPowerButton.getToggleState());
    }
}

bool SynthesizerComponent::hitTest(int x, int y)
{
    // Check if this click should be handled by arc interaction
    auto mapping = findMacroMappingAtPosition(juce::Point<int>(x, y));
    if (mapping != nullptr)
    {
        // Arc handle found, this component should handle the event
        return true;
    }
    
    // For everything else, use default hit testing (sliders will handle their own restricted areas)
    return juce::Component::hitTest(x, y);
}

void SynthesizerComponent::mouseDown(const juce::MouseEvent& event)
{
    // FIRST: Check for arc interaction before any other mouse handling
    // This ensures arc handles get priority over other components
    juce::Logger::writeToLog("Mouse down at position: " + juce::String(event.getPosition().x) + ", " + juce::String(event.getPosition().y));
    auto mapping = findMacroMappingAtPosition(event.getPosition());
    if (mapping != nullptr)
    {
        juce::Logger::writeToLog("Found mapping for macro " + juce::String(mapping->macroIndex));
        isDraggingArc = true;
        draggedMapping = mapping;
        dragStartPoint = event.getPosition();
        
        // Determine which endpoint we're dragging by checking handle positions
        auto bounds = mapping->targetSlider->getBounds();
        if (mapping->targetSlider->getParentComponent() != this)
        {
            bounds = getLocalArea(mapping->targetSlider->getParentComponent(), bounds);
        }
        
        auto arcCenter = bounds.getCentre().toFloat();
        float radius = bounds.getWidth() * 0.4f;
        float handleRadius = 5.0f;
        
        float minAngle = juce::jmap(static_cast<float>(mapping->userMinRange), 
                                  static_cast<float>(mapping->targetSlider->getMinimum()),
                                  static_cast<float>(mapping->targetSlider->getMaximum()),
                                  2.35619f, 0.785398f);
        float maxAngle = juce::jmap(static_cast<float>(mapping->userMaxRange), 
                                  static_cast<float>(mapping->targetSlider->getMinimum()),
                                  static_cast<float>(mapping->targetSlider->getMaximum()),
                                  2.35619f, 0.785398f);
        
        float minX = arcCenter.x + radius * std::cos(minAngle - juce::MathConstants<float>::halfPi);
        float minY = arcCenter.y + radius * std::sin(minAngle - juce::MathConstants<float>::halfPi);
        float maxX = arcCenter.x + radius * std::cos(maxAngle - juce::MathConstants<float>::halfPi);
        float maxY = arcCenter.y + radius * std::sin(maxAngle - juce::MathConstants<float>::halfPi);
        
        float distToMin = event.getPosition().toFloat().getDistanceFrom(juce::Point<float>(minX, minY));
        float distToMax = event.getPosition().toFloat().getDistanceFrom(juce::Point<float>(maxX, maxY));
        
        draggingMinRange = (distToMin < distToMax);
        juce::Logger::writeToLog("Dragging " + juce::String(draggingMinRange ? "min" : "max") + " range handle");
        repaint();
        return; // Don't process other mouse events when dragging arc
    }
    
    // SECOND: Handle other component-specific mouse events only if no arc was clicked
    if (event.eventComponent == &pitchControlsOctaveValueLabel)
    {
        isDraggingOctave = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOctave = octaveValue;
    }
    else if (event.eventComponent == &pitchControlsSemitoneValueLabel)
    {
        isDraggingSemitone = true;
        dragStartY = event.getScreenPosition().y;
        dragStartSemitone = semitoneValue;
    }
    else if (event.eventComponent == &pitchControlsFineTuneValueLabel)
    {
        isDraggingFineTune = true;
        dragStartY = event.getScreenPosition().y;
        dragStartFineTune = fineTuneValue;
    }
    else if (event.eventComponent == &pitchControlsVoiceCountValueLabel)
    {
        isDraggingVoiceCount = true;
        dragStartY = event.getScreenPosition().y;
        dragStartVoiceCount = voiceCountValue;
    }
    // Oscillator 2 pitch controls
    else if (event.eventComponent == &osc2OctaveValueLabel)
    {
        isDraggingOsc2Octave = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2Octave = osc2OctaveValue;
    }
    else if (event.eventComponent == &osc2SemitoneValueLabel)
    {
        isDraggingOsc2Semitone = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2Semitone = osc2SemitoneValue;
    }
    else if (event.eventComponent == &osc2FineTuneValueLabel)
    {
        isDraggingOsc2FineTune = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2FineTune = osc2FineTuneValue;
    }
    else if (event.eventComponent == &osc2VoicesValueLabel)
    {
        isDraggingOsc2VoiceCount = true;
        dragStartY = event.getScreenPosition().y;
        dragStartOsc2VoiceCount = osc2VoiceCountValue;
    }
    else if (event.eventComponent == &distortionTypeValueLabel)
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
        audioProcessor.setDistortionType(distortionTypeValue);
        
        // Still set up drag state in case drag events work
        isDraggingDistortionType = true;
        dragStartY = event.getScreenPosition().y;
        dragStartDistortionType = distortionTypeValue;
        juce::Logger::writeToLog("Distortion type mouseDown triggered - new type: " + typeNames[distortionTypeValue - 1]); // Debug output
    }
    else if (event.eventComponent == &reverbTypeValueLabel)
    {
        // Handle click to cycle through reverb types
        reverbTypeValue++;
        if (reverbTypeValue > 5)
            reverbTypeValue = 1;
            
        // Update text based on type value
        juce::StringArray reverbTypeNames = {
            "PLATE", "HALL", "VINTAGE", "ROOM", "AMBIENCE"
        };
        
        reverbTypeValueLabel.setText(reverbTypeNames[reverbTypeValue - 1], juce::dontSendNotification);
        audioProcessor.setReverbType(reverbTypeValue);
        
        // Set up drag state for drag functionality
        isDraggingReverbType = true;
        dragStartY = event.getScreenPosition().y;
        dragStartReverbType = reverbTypeValue;
        
        juce::Logger::writeToLog("Reverb type mouseDown triggered - new type: " + reverbTypeNames[reverbTypeValue - 1]); // Debug output
    }
    else if (event.eventComponent == &phaserPolesValueLabel)
    {
        // Handle click to cycle through phaser poles
        phaserPolesValue++;
        if (phaserPolesValue > 16)
            phaserPolesValue = 1;
            
        // Update text based on poles value
        phaserPolesValueLabel.setText(juce::String(phaserPolesValue) + " POLES", juce::dontSendNotification);
        
        // Set up drag state for drag functionality
        isDraggingPhaserPoles = true;
        dragStartY = event.getScreenPosition().y;
        dragStartPhaserPoles = phaserPolesValue;
        
        juce::Logger::writeToLog("Phaser poles mouseDown triggered - new poles: " + juce::String(phaserPolesValue)); // Debug output
    }
}

void SynthesizerComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDraggingOctave)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newOctave = dragStartOctave + (deltaY / 10); // 10 pixels per octave
        
        // Clamp to valid range
        newOctave = juce::jlimit(-4, 4, newOctave);
        
        if (newOctave != octaveValue)
        {
            octaveValue = newOctave;
            pitchControlsOctaveValueLabel.setText(juce::String(octaveValue), juce::dontSendNotification);
            audioProcessor.setOsc1Octave(octaveValue);
        }
    }
    else if (isDraggingSemitone)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newSemitone = dragStartSemitone + (deltaY / 5); // 5 pixels per semitone
        
        // Clamp to valid range (-12 to +12 semitones)
        newSemitone = juce::jlimit(-12, 12, newSemitone);
        
        if (newSemitone != semitoneValue)
        {
            semitoneValue = newSemitone;
            pitchControlsSemitoneValueLabel.setText(juce::String(semitoneValue), juce::dontSendNotification);
            audioProcessor.setOsc1Semitone(semitoneValue);
        }
    }
    else if (isDraggingFineTune)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newFineTune = dragStartFineTune + (deltaY / 2); // 2 pixels per cent
        
        // Clamp to valid range (-100 to +100 cents)
        newFineTune = juce::jlimit(-100, 100, newFineTune);
        
        if (newFineTune != fineTuneValue)
        {
            fineTuneValue = newFineTune;
            pitchControlsFineTuneValueLabel.setText(juce::String(fineTuneValue), juce::dontSendNotification);
            audioProcessor.setOsc1FineTune(fineTuneValue);
        }
    }
    else if (isDraggingVoiceCount)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newVoiceCount = dragStartVoiceCount + (deltaY / 8); // 8 pixels per voice
        
        // Clamp to valid range (1 to 16 voices)
        newVoiceCount = juce::jlimit(1, 16, newVoiceCount);
        
        if (newVoiceCount != voiceCountValue)
        {
            voiceCountValue = newVoiceCount;
            pitchControlsVoiceCountValueLabel.setText(juce::String(voiceCountValue), juce::dontSendNotification);
            audioProcessor.setOsc1VoiceCount(voiceCountValue);
        }
    }
    // Oscillator 2 pitch controls
    else if (isDraggingOsc2Octave)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newOctave = dragStartOsc2Octave + (deltaY / 10); // 10 pixels per octave
        
        // Clamp to valid range
        newOctave = juce::jlimit(-4, 4, newOctave);
        
        if (newOctave != osc2OctaveValue)
        {
            osc2OctaveValue = newOctave;
            osc2OctaveValueLabel.setText(juce::String(osc2OctaveValue), juce::dontSendNotification);
            audioProcessor.setOsc2Octave(osc2OctaveValue);
        }
    }
    else if (isDraggingOsc2Semitone)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newSemitone = dragStartOsc2Semitone + (deltaY / 5); // 5 pixels per semitone
        
        // Clamp to valid range (-12 to +12 semitones)
        newSemitone = juce::jlimit(-12, 12, newSemitone);
        
        if (newSemitone != osc2SemitoneValue)
        {
            osc2SemitoneValue = newSemitone;
            osc2SemitoneValueLabel.setText(juce::String(osc2SemitoneValue), juce::dontSendNotification);
            audioProcessor.setOsc2Semitone(osc2SemitoneValue);
        }
    }
    else if (isDraggingOsc2FineTune)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newFineTune = dragStartOsc2FineTune + (deltaY / 2); // 2 pixels per cent
        
        // Clamp to valid range (-100 to +100 cents)
        newFineTune = juce::jlimit(-100, 100, newFineTune);
        
        if (newFineTune != osc2FineTuneValue)
        {
            osc2FineTuneValue = newFineTune;
            osc2FineTuneValueLabel.setText(juce::String(osc2FineTuneValue), juce::dontSendNotification);
            audioProcessor.setOsc2FineTune(osc2FineTuneValue);
        }
    }
    else if (isDraggingOsc2VoiceCount)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newVoiceCount = dragStartOsc2VoiceCount + (deltaY / 8); // 8 pixels per voice
        
        // Clamp to valid range (1 to 16 voices)
        newVoiceCount = juce::jlimit(1, 16, newVoiceCount);
        
        if (newVoiceCount != osc2VoiceCountValue)
        {
            osc2VoiceCountValue = newVoiceCount;
            osc2VoicesValueLabel.setText(juce::String(osc2VoiceCountValue), juce::dontSendNotification);
            audioProcessor.setOsc2VoiceCount(osc2VoiceCountValue);
        }
    }
    else if (isDraggingDistortionType)
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
            audioProcessor.setDistortionType(distortionTypeValue); // Now supported!
        }
    }
    else if (isDraggingReverbType)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newType = dragStartReverbType + (deltaY / 10); // 10 pixels per type
        
        // Clamp to valid range (1 to 5 types)
        newType = juce::jlimit(1, 5, newType);
        
        if (newType != reverbTypeValue)
        {
            reverbTypeValue = newType;
            
            // Update text based on type value
            juce::StringArray reverbTypeNames = {"PLATE", "HALL", "VINTAGE", "ROOM", "AMBIENCE"};
            reverbTypeValueLabel.setText(reverbTypeNames[reverbTypeValue - 1], juce::dontSendNotification);
            audioProcessor.setReverbType(reverbTypeValue);
        }
    }
    else if (isDraggingPhaserPoles)
    {
        int deltaY = dragStartY - event.getScreenPosition().y; // Inverted: up = positive
        int newPoles = dragStartPhaserPoles + (deltaY / 8); // 8 pixels per pole
        
        // Clamp to valid range (1 to 16 poles)
        newPoles = juce::jlimit(1, 16, newPoles);
        
        if (newPoles != phaserPolesValue)
        {
            phaserPolesValue = newPoles;
            
            // Update text based on poles value
            phaserPolesValueLabel.setText(juce::String(phaserPolesValue) + " POLES", juce::dontSendNotification);
            // audioProcessor.setPhaserPoles(phaserPolesValue); // Add this when audio processor supports it
        }
    }
    
    // Handle arc dragging
    if (isDraggingArc && draggedMapping != nullptr)
    {
        auto bounds = draggedMapping->targetSlider->getBounds();
        if (draggedMapping->targetSlider->getParentComponent() != this)
        {
            bounds = getLocalArea(draggedMapping->targetSlider->getParentComponent(), bounds);
        }
        
        auto arcCenter = bounds.getCentre().toFloat();
        auto mousePos = event.getPosition().toFloat();
        auto centerToMouse = mousePos - arcCenter;
        
        // Calculate angle from center to mouse position
        float mouseAngle = std::atan2(centerToMouse.y, centerToMouse.x);
        
        // Normalize angle to 0-2π range
        if (mouseAngle < 0) mouseAngle += 2.0f * juce::MathConstants<float>::pi;
        
        // Convert angle to slider value range (adjust for knob's angle mapping)
        // Knobs typically go from 2.35619f (bottom-left) to 0.785398f (bottom-right)
        float normalizedAngle;
        if (mouseAngle > juce::MathConstants<float>::pi)
        {
            normalizedAngle = juce::jmap(mouseAngle, 2.35619f, 2.0f * juce::MathConstants<float>::pi, 0.0f, 0.5f);
        }
        else
        {
            normalizedAngle = juce::jmap(mouseAngle, 0.0f, 0.785398f, 0.5f, 1.0f);
        }
        
        normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
        
        // Convert to slider's value range
        double newValue = juce::jmap(static_cast<double>(normalizedAngle), 
                                   0.0, 1.0,
                                   draggedMapping->targetSlider->getMinimum(),
                                   draggedMapping->targetSlider->getMaximum());
        
        // Update the appropriate range
        if (draggingMinRange)
        {
            draggedMapping->userMinRange = newValue;
            // Ensure min doesn't exceed max
            if (draggedMapping->userMinRange > draggedMapping->userMaxRange)
                draggedMapping->userMinRange = draggedMapping->userMaxRange;
        }
        else
        {
            draggedMapping->userMaxRange = newValue;
            // Ensure max doesn't go below min
            if (draggedMapping->userMaxRange < draggedMapping->userMinRange)
                draggedMapping->userMaxRange = draggedMapping->userMinRange;
        }
        
        repaint();
    }
}

void SynthesizerComponent::mouseUp(const juce::MouseEvent& event)
{
    isDraggingOctave = false;
    isDraggingSemitone = false;
    isDraggingFineTune = false;
    isDraggingVoiceCount = false;
    // Oscillator 2 drag states
    isDraggingOsc2Octave = false;
    isDraggingOsc2Semitone = false;
    isDraggingOsc2FineTune = false;
    isDraggingOsc2VoiceCount = false;
    isDraggingDistortionType = false;
    isDraggingReverbType = false;
    isDraggingPhaserPoles = false;
    
    // Cleanup arc dragging
    if (isDraggingArc)
    {
        isDraggingArc = false;
        draggedMapping = nullptr;
        repaint();
    }
}

void SynthesizerComponent::updateEnvelopeDisplay()
{
    adsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(adsrAttackKnob.getValue()),
        static_cast<float>(adsrDecayKnob.getValue()),
        static_cast<float>(adsrSustainKnob.getValue()),
        static_cast<float>(adsrReleaseKnob.getValue())
    );
    
    osc2AdsrEnvelopeVisualizer.updateEnvelope(
        static_cast<float>(osc2AttackKnob.getValue()),
        static_cast<float>(osc2DecayKnob.getValue()),
        static_cast<float>(osc2SustainKnob.getValue()),
        static_cast<float>(osc2ReleaseKnob.getValue())
    );
}

// ============================================================================
// Grouped Layout and Background Methods
// ============================================================================

void SynthesizerComponent::layoutWaveTypeButtons(juce::Rectangle<int>& bounds)
{
    auto buttonHeight = 40;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonSection = buttonRow.removeFromLeft(bounds.getWidth() / 3); // Same width as ADSR
    
    // Apply group offset for MOVEABLE Wave Type Buttons Group (Row 1)
    auto offsetButtonSection = buttonSection.translated(
        static_cast<int>(waveTypeButtonsGroupOffsetX), 
        static_cast<int>(waveTypeButtonsGroupOffsetY)
    );
    auto offsetButtonRow = buttonRow.translated(
        static_cast<int>(waveTypeButtonsGroupOffsetX), 
        static_cast<int>(waveTypeButtonsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    waveButtonsBounds = offsetButtonSection;
    
    // Calculate button width and spacing to fill the section
    auto totalButtons = 6;
    auto buttonSpacing = 10;
    auto totalSpacing = (totalButtons - 1) * buttonSpacing;
    auto buttonWidth = (offsetButtonSection.getWidth() - totalSpacing) / totalButtons;
    
    waveTypeSineButton.setBounds(offsetButtonSection.getX(), offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSawButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing), offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeSquareButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 2, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeTriangleButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 3, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypeWhiteNoiseButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 4, offsetButtonRow.getY(), buttonWidth, buttonHeight);
    waveTypePinkNoiseButton.setBounds(offsetButtonSection.getX() + (buttonWidth + buttonSpacing) * 5, offsetButtonRow.getY(), buttonWidth, buttonHeight);
}

void SynthesizerComponent::layoutADSREnvelope(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(7); // spacing
    
    auto envelopeSection = bounds.removeFromTop(60);
    auto envelopeArea = envelopeSection.removeFromLeft(bounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Envelope Visualizer Group (Row 2)
    auto offsetEnvelopeArea = envelopeArea.translated(
        static_cast<int>(adsrEnvelopeGroupOffsetX), 
        static_cast<int>(adsrEnvelopeGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    adsrEnvelopeBounds = offsetEnvelopeArea;
    
    adsrEnvelopeVisualizer.setBounds(offsetEnvelopeArea);
    
    bounds.removeFromTop(15); // spacing
}

void SynthesizerComponent::layoutADSRKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    auto adsrRow = bounds.removeFromTop(controlHeight);
    auto adsrSection = adsrRow.removeFromLeft(bounds.getWidth() / 3);
    
    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3)
    auto offsetAdsrSection = adsrSection.translated(
        static_cast<int>(adsrKnobsGroupOffsetX), 
        static_cast<int>(adsrKnobsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    adsrKnobsBounds = offsetAdsrSection;
    
    // Calculate smaller knob width for 4 knobs in 1/3 of screen width
    auto adsrKnobWidth = (offsetAdsrSection.getWidth() - 45) / 4; // 45 = 3 spacings of 15px each
    
    auto attackArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrAttackLabel.setBounds(attackArea.removeFromTop(20));
    adsrAttackKnob.setBounds(attackArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto decayArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrDecayLabel.setBounds(decayArea.removeFromTop(20));
    adsrDecayKnob.setBounds(decayArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto sustainArea = offsetAdsrSection.removeFromLeft(adsrKnobWidth);
    adsrSustainLabel.setBounds(sustainArea.removeFromTop(20));
    adsrSustainKnob.setBounds(sustainArea);
    
    offsetAdsrSection.removeFromLeft(15); // spacing
    
    auto releaseArea = offsetAdsrSection;
    adsrReleaseLabel.setBounds(releaseArea.removeFromTop(20));
    adsrReleaseKnob.setBounds(releaseArea);
}

void SynthesizerComponent::layoutVolumeKnobs(juce::Rectangle<int>& bounds)
{
    auto controlHeight = 100;
    bounds.removeFromTop(20); // spacing
    auto volumeRow = bounds.removeFromTop(controlHeight);
    
    // Apply group offset for MOVEABLE Volume Controls Group (Row 4)
    auto volumeSection = volumeRow.removeFromLeft(volumeRow.getWidth() / 3);
    auto offsetVolumeSection = volumeSection.translated(
        static_cast<int>(volumeControlsGroupOffsetX), 
        static_cast<int>(volumeControlsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    volumeKnobsBounds = offsetVolumeSection;
    
    auto adsrKnobWidth = (offsetVolumeSection.getWidth() - 45) / 4;
    auto workingRow = offsetVolumeSection;
    
    auto volumeKnobArea = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsVolumeLabel.setBounds(volumeKnobArea.removeFromTop(20));
    volumeControlsVolumeKnob.setBounds(volumeKnobArea);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto detuneSection = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsDetuneLabel.setBounds(detuneSection.removeFromTop(20));
    volumeControlsDetuneKnob.setBounds(detuneSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto stereoWidthSection = workingRow.removeFromLeft(adsrKnobWidth);
    volumeControlsStereoWidthLabel.setBounds(stereoWidthSection.removeFromTop(20));
    volumeControlsStereoWidthKnob.setBounds(stereoWidthSection);
    
    workingRow.removeFromLeft(15); // spacing
    
    auto panSection = workingRow;
    volumeControlsPanLabel.setBounds(panSection.removeFromTop(20));
    volumeControlsPanKnob.setBounds(panSection);
}

void SynthesizerComponent::layoutOctaveControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(40); // spacing
    auto bottomControlsRow = bounds.removeFromTop(80);
    
    auto pitchControlsSection = bottomControlsRow.removeFromLeft(320); // Fixed width for tuning controls
    
    // Apply group offset for MOVEABLE Pitch Controls Group (Row 5)
    auto offsetPitchControlsSection = pitchControlsSection.translated(
        static_cast<int>(pitchControlsGroupOffsetX), 
        static_cast<int>(pitchControlsGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    octaveControlsBounds = offsetPitchControlsSection;
    
    auto workingRow = offsetPitchControlsSection;
    
    // Octave control (draggable label)
    auto octaveArea = workingRow.removeFromLeft(60);
    pitchControlsOctaveLabel.setBounds(octaveArea.removeFromTop(20));
    pitchControlsOctaveValueLabel.setBounds(octaveArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Semitone control (draggable label)
    auto semitoneArea = workingRow.removeFromLeft(60);
    pitchControlsSemitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    pitchControlsSemitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Fine tune control (draggable label)
    auto fineTuneArea = workingRow.removeFromLeft(60);
    pitchControlsFineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    pitchControlsFineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));
    
    workingRow.removeFromLeft(10); // spacing
    
    // Voice count control (draggable label)
    auto voiceCountArea = workingRow.removeFromLeft(60);
    pitchControlsVoiceCountLabel.setBounds(voiceCountArea.removeFromTop(20));
    pitchControlsVoiceCountValueLabel.setBounds(voiceCountArea.removeFromTop(30));
}

void SynthesizerComponent::layoutPhaseControls(juce::Rectangle<int>& bounds)
{
    bounds.removeFromTop(20); // spacing
    auto phaseRow = bounds.removeFromTop(80);
    
    // Store bounds for background drawing
    phaseControlsBounds = phaseRow.removeFromLeft(220); // Fixed width for phase controls
    
    auto workingRow = phaseControlsBounds;
    
    // Random phase button
    auto waveTypeRandomPhaseButtonWidth = 100;
    auto randomPhaseArea = workingRow.removeFromLeft(waveTypeRandomPhaseButtonWidth);
    waveTypeRandomPhaseButton.setBounds(randomPhaseArea.removeFromTop(40));
    
    workingRow.removeFromLeft(15); // spacing
    
    // Phase knob
    auto phaseKnobArea = workingRow.removeFromLeft(80);
    phaseControlsPhaseLabel.setBounds(phaseKnobArea.removeFromTop(20));
    phaseControlsPhaseKnob.setBounds(phaseKnobArea);
}

void SynthesizerComponent::layoutLFOModule(juce::Rectangle<int>& bounds)
{
    // Position LFO module in bottom left corner - height reduced by 2 pixels from bottom
    auto totalBounds = getLocalBounds();
    auto lfoArea = juce::Rectangle<int>(20, totalBounds.getHeight() - 162, 409, 142); // Width: 409, Height: 144-2=142, Y: -162
    lfoModule.setBounds(lfoArea);
}

void SynthesizerComponent::layoutEffectsModule(juce::Rectangle<int>& bounds)
{
    // Position effects module with reduced width and increased height
    auto totalBounds = getLocalBounds();
    int effectsWidth = 300;  // Much narrower than oscillators
    int effectsHeight = 530; // Added 50px more height for additional content space
    int borderPadding = 6; // Extra space for border
    
    int centerX = (totalBounds.getWidth() - effectsWidth - borderPadding * 2) / 2;
    int centerY = (totalBounds.getHeight() - effectsHeight - borderPadding * 2) / 2 - 50; // Move up 30px total for preset controls
    
    // Preset controls area above the effects module
    auto presetControlsHeight = 35;
    auto presetY = centerY - presetControlsHeight - 10; // 10px gap above effects module
    
    // Preset controls layout
    auto buttonWidth = 40;
    auto buttonHeight = 25;
    auto labelWidth = 120;
    auto spacing = 10;
    
    // Calculate positions for preset controls (centered above effects module)
    auto totalPresetWidth = (4 * buttonWidth) + labelWidth + (4 * spacing);
    auto presetStartX = centerX + borderPadding + (effectsWidth - totalPresetWidth) / 2;
    
    // Previous button
    effectsPresetPrevButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Preset name label
    effectsPresetNameLabel.setBounds(presetStartX, presetY, labelWidth, presetControlsHeight);
    presetStartX += labelWidth + spacing;
    
    // Next button
    effectsPresetNextButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Save button
    effectsPresetSaveButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Load button
    effectsPresetLoadButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + spacing;
    
    // Init button
    effectsPresetInitButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    
    // Position border component (larger to encompass effects module)
    auto borderArea = juce::Rectangle<int>(centerX, centerY, effectsWidth + borderPadding * 2, effectsHeight + borderPadding * 2);
    effectsBorder.setBounds(borderArea);
    
    // Position effects module inside the border
    auto effectsArea = juce::Rectangle<int>(centerX + borderPadding, centerY + borderPadding, effectsWidth, effectsHeight);
    effectsModule.setBounds(effectsArea);
    
    // Layout effects controls within the tab content area
    auto tabContentArea = effectsArea.reduced(8, 35); // Account for tab bar and borders
    layoutChorusControls(tabContentArea);
    layoutCompressorControls(tabContentArea);
    layoutDelayControls(tabContentArea);
    layoutDistortionControls(tabContentArea);
}

void SynthesizerComponent::layoutChorusControls(juce::Rectangle<int>& bounds)
{
    // Layout with even larger knobs and more symmetrical spacing to match FX unit style
    auto knobSize = 62; // Further increased from 55 for even more prominence
    auto labelHeight = 22; // Increased for better proportion with larger knobs
    auto knobSpacing = 25; // Increased spacing for better symmetry
    auto rowSpacing = 18; // Increased row spacing
    auto buttonWidth = 80; // Power button width
    auto buttonHeight = 32; // Taller power button to match larger knobs
    
    // Calculate positions for perfectly centered layout
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto startX = (bounds.getWidth() - totalKnobWidth) / 2;
    auto startY = 35; // Moved down a bit more for better spacing
    
    // Top row: Power button and Mix knob closer together
    auto topRowY = startY;
    
    // Calculate closer positioning for power button and mix knob (moved left 20 pixels)
    auto topRowWidth = buttonWidth + 25 + knobSize; // Button + smaller gap + knob
    auto topRowStartX = (bounds.getWidth() - topRowWidth) / 2 - 3; // Center the top row, moved left 3px (2 + 1)
    
    // Power button (moved 12 pixels to the right from new position)
    auto powerButtonX = topRowStartX + 12;
    chorusPowerButton.setBounds(powerButtonX, topRowY + 60, buttonWidth, buttonHeight);
    
    // Mix knob (positioned closer to power button)
    auto mixX = topRowStartX + buttonWidth + 25; // 25px gap instead of 40px
    auto mixKnobY = topRowY + (buttonHeight - knobSize) / 2; // Center with power button
    chorusMixKnob.setBounds(mixX, mixKnobY + 60, knobSize, knobSize);
    chorusMixLabel.setBounds(mixX, mixKnobY + 60 + knobSize + 3, knobSize, labelHeight);
    
    // First knob row: Rate, Delay 1, Delay 2 (moved right 7 pixels and down 60 pixels)
    auto row1Y = topRowY + std::max(buttonHeight, knobSize) + 25 + 60; // Space after top row + 60 pixels
    auto row2StartX = startX + 7; // Moved right 7 pixels
    
    // Rate knob and label
    chorusRateKnob.setBounds(row2StartX, row1Y, knobSize, knobSize);
    chorusRateLabel.setBounds(row2StartX, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Delay 1 knob and label
    auto delay1_x = row2StartX + knobSize + knobSpacing;
    chorusDelay1Knob.setBounds(delay1_x, row1Y, knobSize, knobSize);
    chorusDelay1Label.setBounds(delay1_x, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Delay 2 knob and label
    auto delay2_x = row2StartX + 2 * (knobSize + knobSpacing);
    chorusDelay2Knob.setBounds(delay2_x, row1Y, knobSize, knobSize);
    chorusDelay2Label.setBounds(delay2_x, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Second knob row: Depth, Feed, LPF (moved right 7 pixels and down 60 pixels)
    auto row2Y = row1Y + knobSize + labelHeight + rowSpacing;
    auto row3StartX = startX + 7; // Moved right 7 pixels
    
    // Depth knob and label
    chorusDepthKnob.setBounds(row3StartX, row2Y, knobSize, knobSize);
    chorusDepthLabel.setBounds(row3StartX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // Feed knob and label
    auto feedX = row3StartX + knobSize + knobSpacing;
    chorusFeedKnob.setBounds(feedX, row2Y, knobSize, knobSize);
    chorusFeedLabel.setBounds(feedX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // LPF knob and label
    auto lpfX = row3StartX + 2 * (knobSize + knobSpacing);
    chorusLpfKnob.setBounds(lpfX, row2Y, knobSize, knobSize);
    chorusLpfLabel.setBounds(lpfX, row2Y + knobSize + 3, knobSize, labelHeight);
}

void SynthesizerComponent::layoutCompressorControls(juce::Rectangle<int>& bounds)
{
    // Layout with same large knobs and symmetrical spacing as chorus
    auto knobSize = 62; // Same size as chorus for consistency
    auto labelHeight = 22; // Same as chorus
    auto knobSpacing = 25; // Same spacing as chorus
    auto rowSpacing = 18; // Same row spacing as chorus
    auto buttonWidth = 100; // Wider button for "MULTIBAND" text
    auto buttonHeight = 32; // Same height as chorus power button
    
    // Calculate positions for perfectly centered layout
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto startX = (bounds.getWidth() - totalKnobWidth) / 2;
    auto startY = 35; // Same positioning as chorus
    
    // Top row: Power button (left) and Mix knob (right)
    auto topRowY = startY;
    
    // Power button (moved 28 pixels to the right)
    auto powerButtonX = startX + 28;
    compressorPowerButton.setBounds(powerButtonX, topRowY + 60, buttonWidth, buttonHeight);
    
    // Mix knob (positioned on right side, moved left 37 pixels with row)
    auto mixX = startX + 2 * (knobSize + knobSpacing) - 37; // Right side position, moved left 37px
    auto mixKnobY = topRowY + 60 + (buttonHeight - knobSize) / 2; // Center with power button
    compressorMixKnob.setBounds(mixX, mixKnobY, knobSize, knobSize);
    compressorMixLabel.setBounds(mixX, mixKnobY + knobSize + 3, knobSize, labelHeight);
    
    // First knob row: Threshold, Ratio, Attack (moved right 7 pixels and down 60 pixels)
    auto row1Y = topRowY + std::max(buttonHeight, knobSize) + 25 + 60; // Space after top row + 60 pixels down
    auto row2StartX = startX + 7; // Moved right 7 pixels
    
    // Threshold knob and label
    compressorThresholdKnob.setBounds(row2StartX, row1Y, knobSize, knobSize);
    compressorThresholdLabel.setBounds(row2StartX, row1Y + knobSize + 3, knobSize, labelHeight);
    
    // Ratio knob and label
    auto ratio_x = row2StartX + knobSize + knobSpacing;
    compressorRatioKnob.setBounds(ratio_x, row1Y, knobSize, knobSize);
    compressorRatioLabel.setBounds(ratio_x, row1Y + knobSize + 3, knobSize, labelHeight);
    compressorRatioValueLabel.setBounds(ratio_x, row1Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Attack knob and label
    auto attack_x = row2StartX + 2 * (knobSize + knobSpacing);
    compressorAttackKnob.setBounds(attack_x, row1Y, knobSize, knobSize);
    compressorAttackLabel.setBounds(attack_x, row1Y + knobSize + 3, knobSize, labelHeight);
    compressorAttackValueLabel.setBounds(attack_x, row1Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Second knob row: Release, Gain, and Multiband button (moved right 7 pixels and down 60 pixels)
    auto row2Y = row1Y + knobSize + labelHeight + rowSpacing;
    auto row3StartX = startX + 7; // Moved right 7 pixels
    
    // Release knob and label
    compressorReleaseKnob.setBounds(row3StartX, row2Y, knobSize, knobSize);
    compressorReleaseLabel.setBounds(row3StartX, row2Y + knobSize + 3, knobSize, labelHeight);
    compressorReleaseValueLabel.setBounds(row3StartX, row2Y + knobSize + labelHeight + 6, knobSize, 12);
    
    // Gain knob and label (using updated ratio_x from row 2)
    auto gainX = row3StartX + knobSize + knobSpacing;
    compressorGainKnob.setBounds(gainX, row2Y, knobSize, knobSize);
    compressorGainLabel.setBounds(gainX, row2Y + knobSize + 3, knobSize, labelHeight);
    
    // Multiband button (moved 20 pixels to the left)
    auto multibandY = row2Y + (knobSize - buttonHeight) / 2; // Center with knobs
    compressorMultibandButton.setBounds(attack_x - 20, multibandY, buttonWidth, buttonHeight);
}

void SynthesizerComponent::layoutDelayControls(juce::Rectangle<int>& bounds)
{
    // Layout with same large knobs and green styling as other effects
    auto knobSize = 62; // Same size as other effects for consistency
    auto labelHeight = 22; // Same as other effects
    auto knobSpacing = 20; // Slightly tighter spacing due to more controls
    auto rowSpacing = 15; // Tighter row spacing
    auto buttonWidth = 70; // Standard button width
    auto buttonHeight = 28; // Standard button height
    auto smallButtonWidth = 35; // For triplet/dot buttons
    
    // Calculate positions for layout
    auto startY = 25; // Same as other effects but slightly higher due to more rows
    
    // Row 1: Delay ON button and Mix knob (top row, moved right 3 pixels)
    auto row1Y = startY;
    auto powerButtonX = (bounds.getWidth() - buttonWidth - knobSpacing - knobSize) / 2 + 3;
    delayPowerButton.setBounds(powerButtonX, row1Y, buttonWidth, buttonHeight);
    
    // Mix knob in same row as power button
    auto mixRow1X = powerButtonX + buttonWidth + knobSpacing;
    delayMixKnob.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2, knobSize, knobSize);
    delayMixLabel.setBounds(mixRow1X, row1Y - (knobSize - buttonHeight) / 2 + knobSize + 3, knobSize, labelHeight);
    
    // Row 2: BPM and LINK buttons (moved right 7 pixels)
    auto row2Y = row1Y + std::max(buttonHeight, knobSize) + 20;
    auto bpmX = bounds.getWidth() / 2 - buttonWidth - 10 + 7;
    delayBpmButton.setBounds(bpmX, row2Y, buttonWidth, buttonHeight);
    
    auto linkX = bounds.getWidth() / 2 + 10 + 7;
    delayLinkButton.setBounds(linkX, row2Y, buttonWidth, buttonHeight);
    
    // Row 3: Left Time, Feedback, Right Time knobs (moved right 6 pixels)
    auto row3Y = row2Y + buttonHeight + 20;
    auto totalKnobWidth = (3 * knobSize) + (2 * knobSpacing);
    auto knobStartX = (bounds.getWidth() - totalKnobWidth) / 2 + 6;
    
    // Left time
    delayLeftTimeSlider.setBounds(knobStartX, row3Y, knobSize, knobSize);
    delayLeftTimeLabel.setBounds(knobStartX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Feedback (center)
    auto feedbackX = knobStartX + knobSize + knobSpacing;
    delayFeedbackKnob.setBounds(feedbackX, row3Y, knobSize, knobSize);
    delayFeedbackLabel.setBounds(feedbackX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Right time
    auto rightTimeX = knobStartX + 2 * (knobSize + knobSpacing);
    delayRightTimeSlider.setBounds(rightTimeX, row3Y, knobSize, knobSize);
    delayRightTimeLabel.setBounds(rightTimeX, row3Y + knobSize + 3, knobSize, labelHeight);
    
    // Row 4: Triplet and dot buttons under time knobs (moved right 3 pixels with row 3)
    auto row4Y = row3Y + knobSize + labelHeight + 10;
    
    // Left triplet and dot buttons (under left time) - positioned relative to knobStartX which already moved 3px
    auto leftTripletX = knobStartX + (knobSize - 2 * smallButtonWidth - 5) / 2;
    delayLeftTripletButton.setBounds(leftTripletX, row4Y, smallButtonWidth, buttonHeight);
    delayLeftDotButton.setBounds(leftTripletX + smallButtonWidth + 5, row4Y, smallButtonWidth, buttonHeight);
    
    // Right triplet and dot buttons (under right time) - positioned relative to rightTimeX which moved with knobStartX
    auto rightTripletX = rightTimeX + (knobSize - 2 * smallButtonWidth - 5) / 2;
    delayRightTripletButton.setBounds(rightTripletX, row4Y, smallButtonWidth, buttonHeight);
    delayRightDotButton.setBounds(rightTripletX + smallButtonWidth + 5, row4Y, smallButtonWidth, buttonHeight);
    
    // Row 5: Filter Freq and Filter Q knobs (separate row, moved right 7 pixels)
    auto row5Y = row4Y + buttonHeight + 15;
    auto filterKnobWidth = (2 * knobSize) + knobSpacing;
    auto filterKnobStartX = (bounds.getWidth() - filterKnobWidth) / 2 + 7;
    
    delayFilterFreqKnob.setBounds(filterKnobStartX, row5Y, knobSize, knobSize);
    delayFilterFreqLabel.setBounds(filterKnobStartX, row5Y + knobSize + 3, knobSize, labelHeight);
    
    auto filterQX = filterKnobStartX + knobSize + knobSpacing;
    delayFilterQKnob.setBounds(filterQX, row5Y, knobSize, knobSize);
    delayFilterQLabel.setBounds(filterQX, row5Y + knobSize + 3, knobSize, labelHeight);
    
    // Row 6: Normal and Ping-Pong buttons (separate row, moved right 6 pixels)
    auto row6Y = row5Y + knobSize + labelHeight + 15;
    auto buttonStartX = (bounds.getWidth() - buttonWidth) / 2 + 6;
    delayNormalButton.setBounds(buttonStartX, row6Y, buttonWidth, buttonHeight);
    delayPingPongButton.setBounds(buttonStartX, row6Y + buttonHeight + 5, buttonWidth, buttonHeight);
}

void SynthesizerComponent::layoutDistortionControls(juce::Rectangle<int>& bounds)
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

void SynthesizerComponent::layoutSecondOscillator(juce::Rectangle<int>& bounds)
{
    // Move to top-right position - use a specific position instead of removing from bounds
    auto totalWidth = getWidth();
    auto totalHeight = getHeight();
    
    // Position oscillator 2 in top-right corner - match oscillator 1 size exactly
    auto osc2Width = 320 + 63 + 24; // Same calculation as oscillator 1: tuning width + padding + border = 407
    auto osc2Height = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20 + 24; // Same calculation as oscillator 1: all sections + spacing + border = 606
    auto osc2X = totalWidth - osc2Width - 20; // 20px margin to touch wood border
    auto osc2Y = 20; // 20px margin from top edge
    
    auto osc2Section = juce::Rectangle<int>(osc2X, osc2Y, osc2Width, osc2Height);
    
    // Apply group offset for MOVEABLE Second Oscillator Group (Row 6)
    auto offsetOsc2Section = osc2Section.translated(
        static_cast<int>(secondOscillatorGroupOffsetX), 
        static_cast<int>(secondOscillatorGroupOffsetY)
    );
    
    // Store bounds for background drawing (with offset applied)
    secondOscillatorBounds = offsetOsc2Section;
    
    // Layout similar to oscillator 1 - organized in rows
    auto workingArea = offsetOsc2Section;
    
    // Title label (emblem)
    auto titleHeight = 20;
    auto titleArea = workingArea.removeFromTop(titleHeight);
    osc2TitleLabel.setBounds(titleArea);
    
    workingArea.removeFromTop(15); // reduced spacing to push buttons up slightly
    
    // Wave type buttons row - stretch to align pink noise button with right screw
    auto buttonHeight = 40; // Match oscillator 1 height
    auto waveButtonsRow = workingArea.removeFromTop(buttonHeight);
    auto buttonWidth = 55; // Back to previous setting
    auto screwInset = 15; // Screw position from edge (from background drawing code)
    auto buttonStartX = 17; // Move buttons slightly to the right
    
    // Calculate spacing to stretch row so pink noise button aligns with right screw (extend further)
    auto availableWidth = waveButtonsRow.getWidth() - buttonStartX - screwInset - buttonWidth + 8; // Add 8px stretch for more extension
    auto totalButtonWidth = 6 * buttonWidth; // Total width of all buttons
    auto totalSpacingWidth = availableWidth - totalButtonWidth + buttonWidth; // Available space for 5 gaps
    auto buttonSpacing = totalSpacingWidth / 5; // Spacing between buttons
    
    auto buttonArea = waveButtonsRow.withX(waveButtonsRow.getX() + buttonStartX);
    
    // Set button bounds with explicit height like oscillator 1
    osc2SineButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);
    
    osc2SawButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);
    
    osc2SquareButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);
    
    osc2TriangleButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);
    
    osc2WhiteNoiseButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    buttonArea.removeFromLeft(buttonWidth + buttonSpacing);
    
    osc2PinkNoiseButton.setBounds(buttonArea.getX(), buttonArea.getY(), buttonWidth, buttonHeight);
    
    workingArea.removeFromTop(5); // reduced spacing between rows
    
    // ADSR envelope visualizer row - match oscillator 1 proportions exactly
    auto envelopeHeight = 60; // Match oscillator 1 height
    auto osc2EnvelopeArea = workingArea.removeFromTop(envelopeHeight);
    // Use the same width calculation as oscillator 1's original bounds
    auto originalBounds = getLocalBounds();
    originalBounds.reduce(20, 20);
    auto envelopeWidth = originalBounds.getWidth() / 3; // Match oscillator 1 exact calculation
    auto centeredEnvelopeArea = osc2EnvelopeArea.withSizeKeepingCentre(envelopeWidth, envelopeHeight);
    osc2AdsrEnvelopeVisualizer.setBounds(centeredEnvelopeArea.translated(2, 1));
    
    workingArea.removeFromTop(5); // reduced spacing between envelope and knobs
    
    // Apply offset to all elements below ADSR visualizer
    auto elementsOffset = juce::Point<int>(-6, 12); // Move left 6px, down 12px
    workingArea = workingArea.translated(elementsOffset.x, elementsOffset.y);
    
    // ADSR knobs row - match oscillator 1 dimensions
    auto knobHeight = 80; // Match oscillator 1 knob height
    auto knobLabelHeight = 20; // Match oscillator 1 label height
    auto adsrKnobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);
    auto knobWidth = 80;
    auto knobSpacing = 15; // Match oscillator 1 spacing
    
    // Stretch the ADSR knobs row 12 pixels and shift right 6 pixels (attack stays in place)
    auto totalKnobWidth = 4 * knobWidth + 3 * knobSpacing + 12; // Add 12px stretch
    auto knobStartX = (adsrKnobsRow.getWidth() - totalKnobWidth) / 2 + 6; // Shift right 6px
    auto knobArea = adsrKnobsRow.withX(adsrKnobsRow.getX() + knobStartX).withWidth(totalKnobWidth);
    auto stretchedSpacing = knobSpacing + (12.0f / 3.0f); // Distribute extra 12px across 3 gaps
    
    // Attack knob
    auto attackArea = knobArea.removeFromLeft(knobWidth);
    osc2AttackLabel.setBounds(attackArea.removeFromTop(knobLabelHeight));
    osc2AttackKnob.setBounds(attackArea);
    knobArea.removeFromLeft(stretchedSpacing);
    
    // Decay knob
    auto decayArea = knobArea.removeFromLeft(knobWidth);
    osc2DecayLabel.setBounds(decayArea.removeFromTop(knobLabelHeight));
    osc2DecayKnob.setBounds(decayArea);
    knobArea.removeFromLeft(stretchedSpacing);
    
    // Sustain knob
    auto sustainArea = knobArea.removeFromLeft(knobWidth);
    osc2SustainLabel.setBounds(sustainArea.removeFromTop(knobLabelHeight));
    osc2SustainKnob.setBounds(sustainArea);
    knobArea.removeFromLeft(stretchedSpacing);
    
    // Release knob
    auto releaseArea = knobArea.removeFromLeft(knobWidth);
    osc2ReleaseLabel.setBounds(releaseArea.removeFromTop(knobLabelHeight));
    osc2ReleaseKnob.setBounds(releaseArea);
    
    // Position ADSR Link button at bottom right of oscillator 1
    auto linkButtonHeight = 25;
    auto linkButtonWidth = 100;
    auto linkButtonArea = juce::Rectangle<int>(
        volumeKnobsBounds.getRight() - linkButtonWidth - 40 + 760, // 760px to the right (5px left from 765)
        volumeKnobsBounds.getBottom() - linkButtonHeight - 5 + 160, // 160px lower than bottom edge of oscillator 1 (10px higher)
        linkButtonWidth,
        linkButtonHeight
    );
    osc2AdsrLinkButton.setBounds(linkButtonArea);
    
    // No spacing needed since link button is now outside the row flow
    
    // First additional row - knobs (volume, detune, stereo, pan) - move down 15 pixels
    workingArea.removeFromTop(20); // increased spacing to push rows down 15px
    auto knobsRow = workingArea.removeFromTop(knobHeight + knobLabelHeight);
    
    // Stretch the stereo knobs row 12 pixels and shift right 6 pixels (volume stays in place)
    auto totalKnobsWidth = 4 * knobWidth + 3 * knobSpacing + 12; // Add 12px stretch
    auto knobsStartX = (knobsRow.getWidth() - totalKnobsWidth) / 2 + 6; // Shift right 6px
    auto knobsArea = knobsRow.withX(knobsRow.getX() + knobsStartX).withWidth(totalKnobsWidth);
    auto stretchedKnobSpacing = knobSpacing + (12.0f / 3.0f); // Distribute extra 12px across 3 gaps
    
    // Volume knob
    auto volumeArea = knobsArea.removeFromLeft(knobWidth);
    osc2VolumeLabel.setBounds(volumeArea.removeFromTop(knobLabelHeight));
    osc2VolumeKnob.setBounds(volumeArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);
    
    // Detune knob
    auto detuneArea = knobsArea.removeFromLeft(knobWidth);
    osc2DetuneLabel.setBounds(detuneArea.removeFromTop(knobLabelHeight));
    osc2DetuneKnob.setBounds(detuneArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);
    
    // Stereo knob
    auto stereoArea = knobsArea.removeFromLeft(knobWidth);
    osc2StereoLabel.setBounds(stereoArea.removeFromTop(knobLabelHeight));
    osc2StereoKnob.setBounds(stereoArea);
    knobsArea.removeFromLeft(stretchedKnobSpacing);
    
    // Pan knob
    auto panArea = knobsArea.removeFromLeft(knobWidth);
    osc2PanLabel.setBounds(panArea.removeFromTop(knobLabelHeight));
    osc2PanKnob.setBounds(panArea);
    
    workingArea.removeFromTop(33); // increased spacing to move pitch controls down 28px
    
    // Second additional row - pitch controls (octave, semitone, fine tune, voices) - label style like oscillator 1
    auto pitchControlsHeight = 50; // 20 for label + 30 for value
    auto pitchControlsRow = workingArea.removeFromTop(pitchControlsHeight);
    
    // Store bounds for background drawing
    osc2PitchControlsBounds = pitchControlsRow;
    
    // Move the pitch controls 20 pixels to the right
    auto pitchControlWidth = 60; // Match oscillator 1
    auto pitchControlSpacing = 10; // Match oscillator 1
    auto totalPitchControlsWidth = 4 * pitchControlWidth + 3 * pitchControlSpacing;
    auto pitchControlsStartX = (pitchControlsRow.getWidth() - totalPitchControlsWidth) / 2 - 36; // Move 36px left from center
    auto pitchControlsArea = pitchControlsRow.withX(pitchControlsRow.getX() + pitchControlsStartX).withWidth(totalPitchControlsWidth);
    
    // Octave control
    auto octaveArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2OctaveLabel.setBounds(octaveArea.removeFromTop(20));
    osc2OctaveValueLabel.setBounds(octaveArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);
    
    // Semitone control
    auto semitoneArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2SemitoneLabel.setBounds(semitoneArea.removeFromTop(20));
    osc2SemitoneValueLabel.setBounds(semitoneArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);
    
    // Fine tune control
    auto fineTuneArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2FineTuneLabel.setBounds(fineTuneArea.removeFromTop(20));
    osc2FineTuneValueLabel.setBounds(fineTuneArea.removeFromTop(30));
    pitchControlsArea.removeFromLeft(pitchControlSpacing);
    
    // Voices control
    auto voicesArea = pitchControlsArea.removeFromLeft(pitchControlWidth);
    osc2VoicesLabel.setBounds(voicesArea.removeFromTop(20));
    osc2VoicesValueLabel.setBounds(voicesArea.removeFromTop(30));
    
    workingArea.removeFromTop(5); // reduced spacing between sliders and random phase button
    
    // Random phase button and phase knob row - increase height for better knob visibility
    auto phaseControlsRow = workingArea.removeFromTop(100);
    
    // Position random phase button and phase knob side by side - move 40px left
    auto randomPhaseButtonWidth = 100; // Match oscillator 1 width
    auto phaseControlsWidth = randomPhaseButtonWidth + knobSpacing + knobWidth;
    auto phaseControlsStartX = (phaseControlsRow.getWidth() - phaseControlsWidth) / 2 - 70; // Move 70px left
    auto phaseControlsArea = phaseControlsRow.withX(phaseControlsRow.getX() + phaseControlsStartX).withWidth(phaseControlsWidth);
    
    // Random phase button (left side)
    auto randomPhaseButtonArea = phaseControlsArea.removeFromLeft(randomPhaseButtonWidth);
    // Center the button vertically in the row, then move up 5px
    auto randomPhaseButtonHeight = 40; // Match oscillator 1 height
    auto buttonVerticalOffset = (phaseControlsRow.getHeight() - randomPhaseButtonHeight) / 2 - 3; // Move up 3px (was 5px, now down 2px)
    randomPhaseButtonArea = randomPhaseButtonArea.withY(randomPhaseButtonArea.getY() + buttonVerticalOffset).withHeight(randomPhaseButtonHeight);
    // Move button 2 pixels to the left
    randomPhaseButtonArea = randomPhaseButtonArea.translated(-2, 0);
    osc2RandomPhaseButton.setBounds(randomPhaseButtonArea);
    
    phaseControlsArea.removeFromLeft(knobSpacing);
    
    // Phase knob (right side)
    auto phaseKnobArea = phaseControlsArea.removeFromLeft(knobWidth);
    
    // Store bounds for background drawing - use the full area before any modifications
    osc2PhaseControlsBounds = phaseKnobArea;
    
    auto phaseLabelArea = phaseKnobArea.removeFromTop(15); // Reduced from 20 to 15 to bring label closer
    osc2PhaseLabel.setBounds(phaseLabelArea.translated(0, 31)); // Move label down 31 pixels (28 + 3)
    
    // Match oscillator 1 phase knob size: 80 width × 60 height, move down 33px
    auto matchingKnobArea = juce::Rectangle<int>(phaseKnobArea.getX(), phaseKnobArea.getY() + 33, 80, 60);
    osc2PhaseKnob.setBounds(matchingKnobArea);
    
    // Filter section positioning - under oscillator 2 in landscape format
    auto filterSectionX = offsetOsc2Section.getX();
    auto filterSectionY = offsetOsc2Section.getBottom() - 5; // Overlap 5px with oscillator 2
    auto filterSectionWidth = offsetOsc2Section.getWidth();
    auto filterSectionHeight = 141; // Height for buttons + knobs + spacing
    
    // Store bounds for background drawing
    filterSectionBounds = juce::Rectangle<int>(filterSectionX, filterSectionY, filterSectionWidth, filterSectionHeight);
    
    // Single row of filter type and slope buttons in landscape format
    auto filterButtonHeight = 25;
    auto filterButtonSpacing = 3; // Even more reduced spacing for narrower row
    
    // Calculate button widths and positions for landscape layout
    auto typeButtonWidth = 45;
    auto slopeButtonWidth = 50;
    auto totalButtonsWidth = (6 * typeButtonWidth) + (2 * slopeButtonWidth) + (7 * filterButtonSpacing);
    auto buttonsStartX = filterSectionX + (filterSectionWidth - totalButtonsWidth) / 2;
    
    auto currentX = buttonsStartX;
    
    // Filter type buttons in single row: LP, HP, BP, NOTCH, COMB, FORMANT (lowered by 40px)
    auto filterButtonsY = filterSectionY + 43;
    auto filterLPButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterLPButton.setBounds(filterLPButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    auto filterHPButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterHPButton.setBounds(filterHPButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    auto filterBPButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterBPButton.setBounds(filterBPButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    auto filterNotchButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterNotchButton.setBounds(filterNotchButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    auto filterCombButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterCombButton.setBounds(filterCombButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    auto filterFormantButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, typeButtonWidth, filterButtonHeight);
    filterFormantButton.setBounds(filterFormantButtonArea);
    currentX += typeButtonWidth + filterButtonSpacing;
    
    // Filter slope buttons continue in same row
    auto filter12dBButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, slopeButtonWidth, filterButtonHeight);
    filter12dBButton.setBounds(filter12dBButtonArea);
    currentX += slopeButtonWidth + filterButtonSpacing;
    
    auto filter24dBButtonArea = juce::Rectangle<int>(currentX, filterButtonsY, slopeButtonWidth, filterButtonHeight);
    filter24dBButton.setBounds(filter24dBButtonArea);
    
    // Knobs row below buttons - landscape layout with OSC enable buttons
    auto filterKnobRowY = filterButtonsY + filterButtonHeight - 2;
    auto filterKnobWidth = 80;
    auto filterKnobHeight = 60;
    auto filterKnobSpacing = 20;
    auto oscButtonWidth = 60;
    auto oscButtonHeight = 30;
    
    // Calculate total width for knobs and osc buttons layout
    auto filterTotalKnobsWidth = (2 * filterKnobWidth) + (2 * oscButtonWidth) + (3 * filterKnobSpacing);
    auto filterKnobsStartX = filterSectionX + (filterSectionWidth - filterTotalKnobsWidth) / 2;
    
    currentX = filterKnobsStartX;
    
    // OSC1 Filter Enable button
    auto osc1FilterButtonArea = juce::Rectangle<int>(currentX, filterKnobRowY + (filterKnobHeight - oscButtonHeight) / 2, oscButtonWidth, oscButtonHeight);
    osc1FilterEnableButton.setBounds(osc1FilterButtonArea);
    currentX += oscButtonWidth + filterKnobSpacing;
    
    // Filter cutoff knob
    auto filterCutoffKnobArea = juce::Rectangle<int>(currentX, filterKnobRowY, filterKnobWidth, filterKnobHeight);
    filterCutoffKnob.setBounds(filterCutoffKnobArea);
    auto filterCutoffLabelArea = juce::Rectangle<int>(filterCutoffKnobArea.getX() - 15, filterCutoffKnobArea.getY() + 2, filterKnobWidth, 20);
    filterCutoffLabel.setBounds(filterCutoffLabelArea);
    currentX += filterKnobWidth + filterKnobSpacing;
    
    // Filter Resonance knob
    auto filterResonanceKnobArea = juce::Rectangle<int>(currentX, filterKnobRowY, filterKnobWidth, filterKnobHeight);
    filterResonanceKnob.setBounds(filterResonanceKnobArea);
    auto filterResonanceLabelArea = juce::Rectangle<int>(filterResonanceKnobArea.getX() - 21, filterResonanceKnobArea.getY() + 2, filterKnobWidth, 20);
    filterResonanceLabel.setBounds(filterResonanceLabelArea);
    currentX += filterKnobWidth + filterKnobSpacing;
    
    // OSC2 Filter Enable button
    auto osc2FilterButtonArea = juce::Rectangle<int>(currentX, filterKnobRowY + (filterKnobHeight - oscButtonHeight) / 2, oscButtonWidth, oscButtonHeight);
    osc2FilterEnableButton.setBounds(osc2FilterButtonArea);
}

// ============================================================================
// Grouped Background Drawing Methods
// ============================================================================

void SynthesizerComponent::drawWaveTypeButtonsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Wave type buttons don't have additional backgrounds beyond the button styling
    // This method is here for consistency and future expansion
}

void SynthesizerComponent::drawADSREnvelopeBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // ADSR envelope visualization doesn't have additional background
    // This method is here for consistency and future expansion
}

void SynthesizerComponent::drawADSRKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Calculate knob positions to match layout exactly
    auto adsrKnobWidth = (bounds.getWidth() - 45) / 4; // Same calculation as layout
    auto workingBounds = bounds;
    
    // Attack knob slot
    auto attackArea = workingBounds.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Decay knob slot
    auto decayArea = workingBounds.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Sustain knob slot
    auto sustainArea = workingBounds.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Release knob slot
    auto releaseArea = workingBounds;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void SynthesizerComponent::drawVolumeKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Calculate knob positions to match layout exactly
    auto adsrKnobWidth = (bounds.getWidth() - 45) / 4; // Same calculation as layout
    auto workingBounds = bounds;
    
    // Volume knob slot
    auto volumeSection = workingBounds.removeFromLeft(adsrKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Detune knob slot
    auto detuneSection = workingBounds.removeFromLeft(adsrKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Stereo width knob slot
    auto stereoWidthSection = workingBounds.removeFromLeft(adsrKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
    
    workingBounds.removeFromLeft(15); // spacing
    
    // Pan knob slot
    auto panSection = workingBounds;
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 40, panSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 39, panSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 37, panSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void SynthesizerComponent::drawOctaveControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Individual background boxes for tuning controls
    auto tuningControlsRow = bounds.reduced(3, 3);
    tuningControlsRow.removeFromLeft(2); // Shift right slightly
    tuningControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    tuningControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
}

void SynthesizerComponent::drawOsc2PitchControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Individual background boxes for oscillator 2 pitch controls
    auto pitchControlsRow = bounds.reduced(3, 3);
    pitchControlsRow.removeFromLeft(2); // Shift right slightly
    pitchControlsRow.removeFromTop(-10); // Shift up by expanding bounds
    
    // Octave control box
    auto octaveControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Semitone control box
    auto semitoneControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Fine tune control box
    auto fineTuneControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
    
    pitchControlsRow.removeFromLeft(10); // spacing
    
    // Voice count control box
    auto voiceCountControlArea = pitchControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
}

void SynthesizerComponent::drawPhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Phase knob background - matching the phase knob position exactly
    auto workingBounds = bounds;
    
    // Skip random phase button area
    workingBounds.removeFromLeft(100); // random phase button
    workingBounds.removeFromLeft(15); // spacing
    
    // Phase knob background
    auto phaseKnobArea = workingBounds.removeFromLeft(80);
    phaseKnobArea.removeFromTop(-5); // negative value to move background up to match current positioning
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 40, phaseKnobArea.getCentreY() - 46, 80, 94, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 39, phaseKnobArea.getCentreY() - 45, 78, 92, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 37, phaseKnobArea.getCentreY() - 43, 74, 88, 2.0f, 1.0f);
}

void SynthesizerComponent::drawOsc2PhaseControlsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Phase knob background for oscillator 2 - matching oscillator 1 style
    // Use the bounds directly with proper centering
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    
    // Adjust background size to fit within the 100px total height (20 label + 80 knob)
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(centreX - 40, centreY - 50, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(centreX - 39, centreY - 49, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(centreX - 37, centreY - 47, 74, 94, 2.0f, 1.0f);
}

void SynthesizerComponent::drawSecondOscillatorBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same as oscillator 1
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Main raised surface - metallic modular synth module look
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);
    
    // Much darker metallic grey base
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Add subtle metallic texture
    juce::Random metallicRandom(42);
    g.setColour(juce::Colour(0xff202020).withAlpha(0.3f));
    for (int i = 0; i < 200; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Add lighter metallic highlights
    metallicRandom.setSeed(123);
    g.setColour(juce::Colour(0xff606060).withAlpha(0.2f));
    for (int i = 0; i < 150; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff707070).withAlpha(0.8f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight  
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Add corner screws for modular synth module look
    auto screwRadius = 6.0f;
    auto screwInset = 15.0f;
    auto topScrewInset = screwInset - 2.0f; // Move top screws up 3 pixels higher
    
    // Top-left screw (moved up slightly and right by 2px)
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset + 2.0f, raisedBounds.getY() + topScrewInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw (moved up slightly and right by 2px from original)
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2 + 2.0f, raisedBounds.getY() + topScrewInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Oscillator 2" label to top center of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);
    
    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);
    
    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);
    
    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1, 
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);
    
    // Engraved 3D text effect
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("OSCILLATOR 2", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for raised effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("OSCILLATOR 2", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("OSCILLATOR 2", textBounds, juce::Justification::centred, true);
    
    // Add metal slots for knobs - match oscillator 2 layout
    // Get oscillator 2 bounds from secondOscillatorBounds
    auto slotBounds = secondOscillatorBounds;
    
    // Skip title area
    slotBounds.removeFromTop(20); // title height
    slotBounds.removeFromTop(10); // spacing
    
    // Wave buttons section
    slotBounds.removeFromTop(40); // wave buttons height
    slotBounds.removeFromTop(10); // spacing
    
    // ADSR envelope visualization
    slotBounds.removeFromTop(60); // envelope height
    slotBounds.removeFromTop(20); // increased spacing from 10 to 20 (moves ADSR row down 10 pixels)
    
    // ADSR knobs metal slots - 4 knobs in a row - matching oscillator 1 style with minimal spacing
    auto adsrRow = slotBounds.removeFromTop(80);
    auto adsrKnobWidth = (adsrRow.getWidth() - 3) / 4; // Minimal spacing: reduced from 9 to 3 (3x1 instead of 3x3)
    
    // Attack knob slot - elongated metal (matching oscillator 1) - extended down 2px and moved down 2px
    auto attackArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40 + 5, attackArea.getCentreY() - 60 + 7 - 2.5f + 2, 80, 117, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39 + 5, attackArea.getCentreY() - 59 + 7 - 2.5f + 2, 78, 115, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37 + 5, attackArea.getCentreY() - 57 + 7 - 2.5f + 2, 74, 111, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Decay knob slot - elongated metal (matching oscillator 1) - moved down 5px and right 3px, shortened by 2px
    auto decayArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40 + 3, decayArea.getCentreY() - 60 + 5 + 1, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39 + 3, decayArea.getCentreY() - 59 + 5 + 1, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37 + 3, decayArea.getCentreY() - 57 + 5 + 1, 74, 112, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Sustain knob slot - elongated metal (matching oscillator 1) - height reduced by 2px, moved down 5px, moved left 1px
    auto sustainArea = adsrRow.removeFromLeft(adsrKnobWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40 - 1, sustainArea.getCentreY() - 60 + 1 + 5, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39 - 1, sustainArea.getCentreY() - 59 + 1 + 5, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37 - 1, sustainArea.getCentreY() - 57 + 1 + 5, 74, 112, 2.0f, 1.0f);
    
    adsrRow.removeFromLeft(1); // minimal spacing reduced from 3 to 1
    
    // Release knob slot - elongated metal (matching oscillator 1) - height reduced by 2px, moved down 5px, moved left 5px
    auto releaseArea = adsrRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40 - 5, releaseArea.getCentreY() - 60 + 1 + 5, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39 - 5, releaseArea.getCentreY() - 59 + 1 + 5, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37 - 5, releaseArea.getCentreY() - 57 + 1 + 5, 74, 112, 2.0f, 1.0f);
    
    // Duplicate ADSR knob backgrounds for stereo knob row (121px lower)
    
    // Duplicate Attack knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40 + 5, attackArea.getCentreY() - 60 + 7 - 2.5f + 2 + 121, 80, 117, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39 + 5, attackArea.getCentreY() - 59 + 7 - 2.5f + 2 + 121, 78, 115, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37 + 5, attackArea.getCentreY() - 57 + 7 - 2.5f + 2 + 121, 74, 111, 2.0f, 1.0f);
    
    // Duplicate Decay knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40 + 3, decayArea.getCentreY() - 60 + 5 + 1 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39 + 3, decayArea.getCentreY() - 59 + 5 + 1 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37 + 3, decayArea.getCentreY() - 57 + 5 + 1 + 121, 74, 112, 2.0f, 1.0f);
    
    // Duplicate Sustain knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40 - 1, sustainArea.getCentreY() - 60 + 1 + 5 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39 - 1, sustainArea.getCentreY() - 59 + 1 + 5 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37 - 1, sustainArea.getCentreY() - 57 + 1 + 5 + 121, 74, 112, 2.0f, 1.0f);
    
    // Duplicate Release knob background - moved down 121px from original position
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40 - 5, releaseArea.getCentreY() - 60 + 1 + 5 + 121, 80, 118, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39 - 5, releaseArea.getCentreY() - 59 + 1 + 5 + 121, 78, 116, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37 - 5, releaseArea.getCentreY() - 57 + 1 + 5 + 121, 74, 112, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Volume controls metal slots - only stereo knob background enabled
    auto volumeRow = slotBounds.removeFromTop(80);
    auto volumeKnobWidth = (volumeRow.getWidth() - 45) / 4;
    
    // Volume knob slot - BACKGROUND REMOVED
    auto volumeArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(volumeArea.getCentreX() - 40, volumeArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(volumeArea.getCentreX() - 39, volumeArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(volumeArea.getCentreX() - 37, volumeArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Detune knob slot - BACKGROUND REMOVED
    auto detuneArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(detuneArea.getCentreX() - 40, detuneArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(detuneArea.getCentreX() - 39, detuneArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(detuneArea.getCentreX() - 37, detuneArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    volumeRow.removeFromLeft(15); // spacing
    
    // Stereo knob slot - only duplicate background enabled, shifted down 100 pixels
    auto stereoArea = volumeRow.removeFromLeft(volumeKnobWidth);
    // Original stereo background removed - only keeping the duplicate
    
    // Osc2 Phase knob background - custom positioned for phase control (shifted down 244 pixels and left 70 pixels from stereo position)
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoArea.getCentreX() - 40 - 70, stereoArea.getCentreY() - 50 + 244, 80, 100, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 39 - 70, stereoArea.getCentreY() - 49 + 244, 78, 98, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoArea.getCentreX() - 37 - 70, stereoArea.getCentreY() - 47 + 244, 74, 94, 2.0f, 1.0f);
    
    // volumeRow.removeFromLeft(15); // spacing
    
    // Pan knob slot - BACKGROUND REMOVED
    // auto panArea = volumeRow;
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(panArea.getCentreX() - 40, panArea.getCentreY() - 50, 80, 100, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(panArea.getCentreX() - 39, panArea.getCentreY() - 49, 78, 98, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(panArea.getCentreX() - 37, panArea.getCentreY() - 47, 74, 94, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Pitch controls metal slots - 3 pitch controls + 1 voices control - matching oscillator 1 style, squeezed very close together
    auto pitchRow = slotBounds.removeFromTop(60);
    auto totalWidth = 4 * 70; // 4 backgrounds at 70px width + no gaps = completely touching
    auto pitchControlWidth = 70; // Fixed width per control
    auto gapWidth = 0; // No gap between backgrounds - completely touching
    auto startX = (pitchRow.getWidth() - totalWidth) / 2 - 50 + 20 - 5 - 5 - 3; // Center the squeezed row, then move left 50px, then right 20px, then left 5px, then left 5px more, then left 3px
    auto squeezedRow = pitchRow.withX(pitchRow.getX() + startX).withWidth(totalWidth).translated(0, 30 + 40 - 10 + 5 - 1 - 1 - 1); // Move down 30px, then down 40px more, then up 10px, then down 5px, then up 1px, then up 1px more, then up 1px again
    
    // Octave control slot - matching oscillator 1 style
    auto octaveArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveArea.getCentreX() - 35, octaveArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 34, octaveArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveArea.getCentreX() - 32, octaveArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Semitone control slot - matching oscillator 1 style
    auto semitoneArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneArea.getCentreX() - 35, semitoneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 34, semitoneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneArea.getCentreX() - 32, semitoneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Fine tune control slot - matching oscillator 1 style
    auto fineTuneArea = squeezedRow.removeFromLeft(pitchControlWidth);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneArea.getCentreX() - 35, fineTuneArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 34, fineTuneArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneArea.getCentreX() - 32, fineTuneArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    squeezedRow.removeFromLeft(gapWidth); // Small gap
    
    // Voice count control slot - matching oscillator 1 style
    auto voiceArea = squeezedRow;
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceArea.getCentreX() - 35, voiceArea.getCentreY() - 35, 70, 70, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 34, voiceArea.getCentreY() - 34, 68, 68, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceArea.getCentreX() - 32, voiceArea.getCentreY() - 32, 64, 64, 2.0f, 1.0f);
    
    slotBounds.removeFromTop(10); // spacing
    
    // Phase control slot - single knob - BACKGROUND REMOVED
    auto phaseRow = slotBounds.removeFromTop(60);
    // auto phaseArea = phaseRow.removeFromRight(80); // Position on right side
    // g.setColour(juce::Colour(0xff0f0f0f));
    // g.fillRoundedRectangle(phaseArea.getCentreX() - 40, phaseArea.getCentreY() - 40, 80, 80, 4.0f);
    // g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    // g.drawRoundedRectangle(phaseArea.getCentreX() - 39, phaseArea.getCentreY() - 39, 78, 78, 3.0f, 2.0f);
    // g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    // g.drawRoundedRectangle(phaseArea.getCentreX() - 37, phaseArea.getCentreY() - 37, 74, 74, 2.0f, 1.0f);
}

void SynthesizerComponent::drawFilterSectionBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same style as oscillator backgrounds
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Main background (dark gray)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Create raised effect with gradients
    auto raisedBounds = masterOutlineBounds.reduced(4.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff505050).withAlpha(0.7f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight  
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Add corner screws for modular synth module look
    auto screwRadius = 6.0f;
    auto screwInset = 15.0f;
    
    // Top-left screw
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);
    
    // Top-right screw
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);
    
    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);
    
    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
    
    // Add "Filter" label to top center of module
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 30, raisedBounds.getY() + 5, 60, 20);
    
    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);
    
    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);
    
    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1, 
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);
    
    // Engraved 3D text effect - same as oscillator
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("FILTER", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("FILTER", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("FILTER", textBounds, juce::Justification::centred, true);
}

void SynthesizerComponent::drawMacroKnobsBackground(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Convert to float for precise drawing
    auto masterOutlineBounds = bounds.toFloat();
    
    // Create 3D depth effect with multiple layers - same style as oscillator backgrounds but no screws
    
    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);
    
    // Main background (dark gray)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);
    
    // Create raised effect with gradients
    auto raisedBounds = masterOutlineBounds.reduced(4.0f);
    
    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);
    
    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff505050).withAlpha(0.7f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1, 
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight  
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8, 
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1, 
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8, 
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
    
    // Add "MACRO" label to top center of module
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 35, raisedBounds.getY() + 5, 70, 20);
    
    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);
    
    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);
    
    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1, 
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);
    
    // Engraved 3D text effect - same as oscillator
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
    
    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("MACRO", textBounds.translated(1, 1), juce::Justification::centred, true);
    
    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("MACRO", textBounds.translated(-1, -1), juce::Justification::centred, true);
    
    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("MACRO", textBounds, juce::Justification::centred, true);
}

//==============================================================================
// Macro Mapping System Implementation

void SynthesizerComponent::createMacroMapping(int macroIndex, juce::Slider* targetSlider)
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
    
    // Debug output to identify duplicate mapping issues
    juce::String sliderName = "Unknown";
    if (targetSlider == &flangerMixKnob) sliderName = "flangerMixKnob";
    else if (targetSlider == &compressorMixKnob) sliderName = "compressorMixKnob";
    else if (targetSlider == &eq1NewGainKnob) sliderName = "eq1NewGainKnob";
    else if (targetSlider == &eq2NewGainKnob) sliderName = "eq2NewGainKnob";
    
    DBG("Macro " + juce::String(macroIndex) + " linked to " + sliderName + " at value " + juce::String(currentValue));
    
    // Trigger repaint to show new indicator
    repaint();
}

void SynthesizerComponent::updateMacroMappings(int macroIndex, double macroValue)
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
            
            // Manually trigger the parameter update to the audio processor
            triggerParameterUpdate(mapping.targetSlider, newValue);
        }
    }
    
    // Trigger visual update for the circular indicators
    repaint();
}

void SynthesizerComponent::removeMacroMapping(int macroIndex, juce::Slider* targetSlider)
{
    auto oldSize = macroMappings.size();
    macroMappings.erase(
        std::remove_if(macroMappings.begin(), macroMappings.end(),
            [macroIndex, targetSlider](const MacroMapping& mapping) {
                return mapping.macroIndex == macroIndex && mapping.targetSlider == targetSlider;
            }),
        macroMappings.end());
    
    // If we removed something, trigger repaint to update indicators
    if (macroMappings.size() != oldSize)
    {
        repaint();
    }
}

juce::Slider* SynthesizerComponent::findSliderAt(juce::Point<int> position)
{
    // List of all sliders to check for drop detection
    std::vector<juce::Slider*> allSliders = {
        // Volume and oscillator controls
        &volumeControlsVolumeKnob, &volumeControlsDetuneKnob, &volumeControlsStereoWidthKnob, &volumeControlsPanKnob,
        &phaseControlsPhaseKnob, &osc2VolumeKnob, &osc2DetuneKnob, &osc2StereoKnob, &osc2PanKnob, &osc2PhaseKnob,
        
        // ADSR controls
        &adsrAttackKnob, &adsrDecayKnob, &adsrSustainKnob, &adsrReleaseKnob,
        &osc2AttackKnob, &osc2DecayKnob, &osc2SustainKnob, &osc2ReleaseKnob,
        
        // Filter controls
        &filterCutoffKnob, &filterResonanceKnob,
        
        // EQ controls - using only the NEW knobs to avoid duplicates
        &eq1NewFreqKnob, &eq1NewQKnob, &eq1NewGainKnob, &eq2NewFreqKnob, &eq2NewQKnob, &eq2NewGainKnob,
        
        // Effects controls
        &chorusRateKnob, &chorusDelay1Knob, &chorusDelay2Knob, &chorusDepthKnob, &chorusFeedKnob, &chorusLpfKnob, &chorusMixKnob,
        &flangerMixKnob, &flangerRateKnob, &flangerDepthKnob, &flangerFeedbackKnob, &flangerPhaseKnob,
        &phaserMixKnob, &phaserRateKnob, &phaserDepth1Knob, &phaserDepth2Knob, &phaserFeedbackKnob, &phaserPhaseKnob, &phaserFrequencyKnob,
        &compressorThresholdKnob, &compressorRatioKnob, &compressorAttackKnob, &compressorReleaseKnob, &compressorGainKnob, &compressorMixKnob,
        &distortionDriveKnob, &distortionMixKnob, &distortionFilterFreqKnob, &distortionFilterQKnob,
        &delayMixKnob, &delayFeedbackKnob, &delayFilterFreqKnob, &delayFilterQKnob,
        &reverbMixKnob, &reverbSizeKnob, &reverbPreDelayKnob, &reverbLowCutKnob, &reverbHighCutKnob, &reverbDampKnob, &reverbWidthKnob
    };
    
    // Check each slider to see if the position is within its bounds
    // Return the slider with the smallest bounds that contains the position (most specific match)
    juce::Slider* bestMatch = nullptr;
    int smallestArea = INT_MAX;
    
    for (auto* slider : allSliders)
    {
        if (slider && slider->isVisible())
        {
            // Convert slider bounds to the main component's coordinate system
            auto sliderBounds = getLocalArea(slider->getParentComponent(), slider->getBounds());
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


void SynthesizerComponent::drawMacroIndicators(juce::Graphics& g)
{
    // Draw arcs for all valid mappings (multiple arcs per macro are allowed)
    // But validate coordinates to prevent phantom arcs from coordinate conversion issues
    
    for (const auto& mapping : macroMappings)
    {
        if (mapping.targetSlider && mapping.targetSlider->isVisible())
        {
            // Validate the coordinate conversion before drawing
            auto sliderParent = mapping.targetSlider->getParentComponent();
            if (sliderParent && sliderParent->isVisible())
            {
                // Get converted bounds
                auto sliderBounds = getLocalArea(sliderParent, mapping.targetSlider->getBounds());
                
                // Only draw if bounds are reasonable (not phantom coordinates)
                if (sliderBounds.getWidth() > 0 && sliderBounds.getHeight() > 0 && 
                    sliderBounds.getX() >= 0 && sliderBounds.getY() >= 0 &&
                    sliderBounds.getRight() <= getWidth() && sliderBounds.getBottom() <= getHeight())
                {
                    drawCircularIndicator(g, mapping.targetSlider, mapping);
                }
            }
        }
    }
}

void SynthesizerComponent::drawCircularIndicator(juce::Graphics& g, juce::Slider* slider, const MacroMapping& mapping)
{
    if (!slider) return;
    
    // Convert slider bounds to the main component's coordinate system
    // This is crucial for FX knobs that are in child tab components
    auto sliderBounds = getLocalArea(slider->getParentComponent(), slider->getBounds());
    
    // Additional safety check for phantom coordinates
    if (sliderBounds.getWidth() <= 0 || sliderBounds.getHeight() <= 0 || 
        sliderBounds.getX() < 0 || sliderBounds.getY() < 0 ||
        sliderBounds.getRight() > getWidth() || sliderBounds.getBottom() > getHeight())
    {
        return; // Skip drawing for invalid coordinates
    }
    
    auto center = sliderBounds.getCentre();
    
    // Calculate radius - slightly larger than the knob
    float radius = std::max(sliderBounds.getWidth(), sliderBounds.getHeight()) * 0.6f;
    
    // Get current macro knob value 
    double macroValue = getMacroKnobValue(mapping.macroIndex);
    double sliderRange = mapping.maxRange - mapping.minRange;
    
    // Use the user-defined range for the arc (adjustable by dragging)
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
    
    // 4. Draw a subtle base position indicator (where the knob was when linked)
    g.setColour(mapping.indicatorColor.withAlpha(0.6f));
    float baseDotRadius = 2.0f;
    float baseX = center.x + radius * std::cos(baseAngle - juce::MathConstants<float>::halfPi);
    float baseY = center.y + radius * std::sin(baseAngle - juce::MathConstants<float>::halfPi);
    g.fillEllipse(baseX - baseDotRadius, baseY - baseDotRadius, baseDotRadius * 2, baseDotRadius * 2);
    
    // 5. Draw drag handles at arc endpoints for easier interaction
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

double SynthesizerComponent::getMacroKnobValue(int macroIndex)
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
        default: return 0.5; // Default center position
    }
}

void SynthesizerComponent::triggerParameterUpdate(juce::Slider* slider, double newValue)
{
    // Mirror the logic from sliderValueChanged to update audio processor parameters
    // without sending slider notifications (to avoid visual movement)
    
    if (slider == &volumeControlsVolumeKnob)
    {
        audioProcessor.setOsc1Volume(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsDetuneKnob)
    {
        audioProcessor.setOsc1Detune(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsStereoWidthKnob)
    {
        audioProcessor.setSynthStereoWidth(static_cast<float>(newValue));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setSynthPan(static_cast<float>(newValue));
    }
    else if (slider == &phaseControlsPhaseKnob)
    {
        audioProcessor.setSynthPhase(static_cast<float>(newValue));
    }
    else if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(newValue));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc2Attack(static_cast<float>(newValue));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(newValue));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc2Decay(static_cast<float>(newValue));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(newValue));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc2Sustain(static_cast<float>(newValue));
        }
        
        updateEnvelopeDisplay();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(newValue));
        
        // If ADSR is linked, also update oscillator 2
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc2Release(static_cast<float>(newValue));
        }
        
        updateEnvelopeDisplay();
    }
    // Add more slider checks as needed for other parameters
    // For now, focusing on the main commonly-used parameters
    else if (slider == &filterCutoffKnob)
    {
        audioProcessor.setFilterCutoff(static_cast<float>(newValue));
    }
    else if (slider == &filterResonanceKnob)
    {
        audioProcessor.setFilterResonance(static_cast<float>(newValue));
    }
    // EQ parameters
    else if (slider == &eq1FreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1QKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1GainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2FreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2QKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2GainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    // New EQ knobs
    else if (slider == &eq1NewFreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1NewQKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq1NewGainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewFreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewQKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    else if (slider == &eq2NewGainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(newValue));
        parametricEQ.syncWithDSPState();
    }
    // Oscillator 2 controls
    else if (slider == &osc2VolumeKnob)
    {
        audioProcessor.setOsc2Volume(static_cast<float>(newValue));
    }
    else if (slider == &osc2DetuneKnob)
    {
        audioProcessor.setOsc2Detune(static_cast<float>(newValue));
    }
    else if (slider == &osc2StereoKnob)
    {
        audioProcessor.setOsc2Stereo(static_cast<float>(newValue));
    }
    else if (slider == &osc2AttackKnob)
    {
        audioProcessor.setOsc2Attack(static_cast<float>(newValue));
        // If ADSR is linked, also update main ADSR
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc1Attack(static_cast<float>(newValue));
        }
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2DecayKnob)
    {
        audioProcessor.setOsc2Decay(static_cast<float>(newValue));
        // If ADSR is linked, also update main ADSR
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc1Decay(static_cast<float>(newValue));
        }
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2SustainKnob)
    {
        audioProcessor.setOsc2Sustain(static_cast<float>(newValue));
        // If ADSR is linked, also update main ADSR
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc1Sustain(static_cast<float>(newValue));
        }
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2ReleaseKnob)
    {
        audioProcessor.setOsc2Release(static_cast<float>(newValue));
        // If ADSR is linked, also update main ADSR
        if (osc2AdsrLinkButton.getToggleState())
        {
            audioProcessor.setOsc1Release(static_cast<float>(newValue));
        }
        updateEnvelopeDisplay();
    }
    else if (slider == &osc2PanKnob)
    {
        audioProcessor.setOsc2Pan(static_cast<float>(newValue));
    }
    else if (slider == &osc2PhaseKnob)
    {
        audioProcessor.setOsc2Phase(static_cast<float>(newValue));
    }
    // Chorus FX
    else if (slider == &chorusRateKnob)
    {
        audioProcessor.setChorusRate(static_cast<float>(newValue));
    }
    else if (slider == &chorusDelay1Knob)
    {
        audioProcessor.setChorusDelay1(static_cast<float>(newValue));
    }
    else if (slider == &chorusDelay2Knob)
    {
        audioProcessor.setChorusDelay2(static_cast<float>(newValue));
    }
    else if (slider == &chorusDepthKnob)
    {
        audioProcessor.setChorusDepth(static_cast<float>(newValue));
    }
    else if (slider == &chorusFeedKnob)
    {
        audioProcessor.setChorusFeedback(static_cast<float>(newValue));
    }
    else if (slider == &chorusLpfKnob)
    {
        float filterFreq = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 20.0f, 20000.0f);
        audioProcessor.setChorusLPF(filterFreq);
    }
    else if (slider == &chorusMixKnob)
    {
        audioProcessor.setChorusMix(static_cast<float>(newValue));
    }
    // Flanger FX
    else if (slider == &flangerRateKnob)
    {
        audioProcessor.setFlangerRate(static_cast<float>(newValue));
    }
    else if (slider == &flangerDepthKnob)
    {
        audioProcessor.setFlangerDepth(static_cast<float>(newValue));
    }
    else if (slider == &flangerFeedbackKnob)
    {
        audioProcessor.setFlangerFeedback(static_cast<float>(newValue));
    }
    else if (slider == &flangerMixKnob)
    {
        audioProcessor.setFlangerMix(static_cast<float>(newValue));
    }
    else if (slider == &flangerPhaseKnob)
    {
        audioProcessor.setFlangerPhase(static_cast<float>(newValue));
    }
    // Phaser FX
    else if (slider == &phaserRateKnob)
    {
        audioProcessor.setPhaserRate(static_cast<float>(newValue));
    }
    else if (slider == &phaserDepth1Knob)
    {
        audioProcessor.setPhaserDepth1(static_cast<float>(newValue));
    }
    else if (slider == &phaserDepth2Knob)
    {
        audioProcessor.setPhaserDepth2(static_cast<float>(newValue));
    }
    else if (slider == &phaserFeedbackKnob)
    {
        audioProcessor.setPhaserFeedback(static_cast<float>(newValue));
    }
    else if (slider == &phaserMixKnob)
    {
        audioProcessor.setPhaserMix(static_cast<float>(newValue));
    }
    else if (slider == &phaserPhaseKnob)
    {
        audioProcessor.setPhaserPhase(static_cast<float>(newValue));
    }
    else if (slider == &phaserFrequencyKnob)
    {
        audioProcessor.setPhaserFrequency(static_cast<float>(newValue));
    }
    // Compressor FX
    else if (slider == &compressorThresholdKnob)
    {
        audioProcessor.setCompressorThreshold(static_cast<float>(newValue));
    }
    else if (slider == &compressorRatioKnob)
    {
        float ratio = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 20.0f);
        audioProcessor.setCompressorRatio(ratio);
    }
    else if (slider == &compressorAttackKnob)
    {
        float attack = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 0.1f, 100.0f);
        audioProcessor.setCompressorAttack(attack);
    }
    else if (slider == &compressorReleaseKnob)
    {
        float release = juce::jmap(static_cast<float>(newValue), 0.0f, 1.0f, 1.0f, 1000.0f);
        audioProcessor.setCompressorRelease(release);
    }
    else if (slider == &compressorGainKnob)
    {
        audioProcessor.setCompressorGain(static_cast<float>(newValue));
    }
    else if (slider == &compressorMixKnob)
    {
        audioProcessor.setCompressorMix(static_cast<float>(newValue));
    }
    // Distortion FX
    else if (slider == &distortionDriveKnob)
    {
        audioProcessor.setDistortionDrive(static_cast<float>(newValue));
    }
    else if (slider == &distortionMixKnob)
    {
        audioProcessor.setDistortionMix(static_cast<float>(newValue));
    }
    else if (slider == &distortionFilterFreqKnob)
    {
        audioProcessor.setDistortionFilterFreq(static_cast<float>(newValue));
    }
    else if (slider == &distortionFilterQKnob)
    {
        audioProcessor.setDistortionFilterQ(static_cast<float>(newValue));
    }
    // Delay FX
    else if (slider == &delayFeedbackKnob)
    {
        audioProcessor.setDelayFeedback(static_cast<float>(newValue));
    }
    else if (slider == &delayMixKnob)
    {
        audioProcessor.setDelayMix(static_cast<float>(newValue));
    }
    else if (slider == &delayFilterFreqKnob)
    {
        audioProcessor.setDelayFilterFreq(static_cast<float>(newValue));
    }
    else if (slider == &delayFilterQKnob)
    {
        audioProcessor.setDelayFilterQ(static_cast<float>(newValue));
    }
    // Reverb FX
    else if (slider == &reverbMixKnob)
    {
        audioProcessor.setReverbMix(static_cast<float>(newValue));
    }
    else if (slider == &reverbLowCutKnob)
    {
        audioProcessor.setReverbLowCut(static_cast<float>(newValue));
    }
    else if (slider == &reverbHighCutKnob)
    {
        audioProcessor.setReverbHighCut(static_cast<float>(newValue));
    }
    else if (slider == &reverbSizeKnob)
    {
        audioProcessor.setReverbSize(static_cast<float>(newValue));
    }
    else if (slider == &reverbPreDelayKnob)
    {
        audioProcessor.setReverbPreDelay(static_cast<float>(newValue));
    }
    else if (slider == &reverbDampKnob)
    {
        audioProcessor.setReverbDamping(static_cast<float>(newValue));
    }
    else if (slider == &reverbWidthKnob)
    {
        audioProcessor.setReverbWidth(static_cast<float>(newValue));
    }
}

MacroMapping* SynthesizerComponent::findMacroMappingAtPosition(juce::Point<int> position)
{
    juce::Logger::writeToLog("Checking " + juce::String(macroMappings.size()) + " macro mappings");
    for (auto& mapping : macroMappings)
    {
        if (mapping.targetSlider && mapping.targetSlider->isVisible())
        {
            auto bounds = mapping.targetSlider->getBounds();
            if (mapping.targetSlider->getParentComponent() != this)
            {
                bounds = getLocalArea(mapping.targetSlider->getParentComponent(), bounds);
            }
            
            // Check if clicking on drag handles first (easier to click)
            auto center = bounds.getCentre();
            float radius = bounds.getWidth() * 0.4f;
            float handleRadius = 5.0f;
            
            // Calculate handle positions
            float minAngle = juce::jmap(static_cast<float>(mapping.userMinRange), 
                                      static_cast<float>(mapping.targetSlider->getMinimum()),
                                      static_cast<float>(mapping.targetSlider->getMaximum()),
                                      2.35619f, 0.785398f);
            float maxAngle = juce::jmap(static_cast<float>(mapping.userMaxRange), 
                                      static_cast<float>(mapping.targetSlider->getMinimum()),
                                      static_cast<float>(mapping.targetSlider->getMaximum()),
                                      2.35619f, 0.785398f);
            
            float minX = center.x + radius * std::cos(minAngle - juce::MathConstants<float>::halfPi);
            float minY = center.y + radius * std::sin(minAngle - juce::MathConstants<float>::halfPi);
            float maxX = center.x + radius * std::cos(maxAngle - juce::MathConstants<float>::halfPi);
            float maxY = center.y + radius * std::sin(maxAngle - juce::MathConstants<float>::halfPi);
            
            // Check if clicking on min handle
            if (position.toFloat().getDistanceFrom(juce::Point<float>(minX, minY)) <= handleRadius + 3.0f)
            {
                juce::Logger::writeToLog("Clicked on min handle for macro " + juce::String(mapping.macroIndex));
                return &mapping;
            }
            
            // Check if clicking on max handle
            if (position.toFloat().getDistanceFrom(juce::Point<float>(maxX, maxY)) <= handleRadius + 3.0f)
            {
                juce::Logger::writeToLog("Clicked on max handle for macro " + juce::String(mapping.macroIndex));
                return &mapping;
            }
            
            // Fallback to arc detection
            if (isPointOnArc(position, center, radius, minAngle, maxAngle))
            {
                juce::Logger::writeToLog("Clicked on arc for macro " + juce::String(mapping.macroIndex));
                return &mapping;
            }
        }
    }
    return nullptr;
}

bool SynthesizerComponent::isPointOnArc(juce::Point<int> point, juce::Point<int> center, float radius, float startAngle, float endAngle)
{
    auto pos = point.toFloat();
    auto centerFloat = center.toFloat();
    
    // Check if point is within radius range (with some tolerance)
    float distance = centerFloat.getDistanceFrom(pos);
    if (distance < radius - 10.0f || distance > radius + 10.0f)
        return false;
    
    // Calculate angle of the position
    auto centerToPos = pos - centerFloat;
    float posAngle = std::atan2(centerToPos.y, centerToPos.x);
    if (posAngle < 0) posAngle += 2.0f * juce::MathConstants<float>::pi;
    
    // Check if position angle is within arc range (considering wrap-around)
    if (startAngle <= endAngle)
    {
        return posAngle >= startAngle && posAngle <= endAngle;
    }
    else
    {
        return posAngle >= startAngle || posAngle <= endAngle;
    }
}

void SynthesizerComponent::updateMappingRange(MacroMapping* mapping, juce::Point<int> dragPosition)
{
    if (!mapping || !mapping->targetSlider)
        return;
        
    auto bounds = mapping->targetSlider->getBounds();
    if (mapping->targetSlider->getParentComponent() != this)
    {
        bounds = getLocalArea(mapping->targetSlider->getParentComponent(), bounds);
    }
    
    auto arcCenter = bounds.getCentre().toFloat();
    auto mousePos = dragPosition.toFloat();
    auto centerToMouse = mousePos - arcCenter;
    
    // Calculate angle from center to mouse position
    float mouseAngle = std::atan2(centerToMouse.y, centerToMouse.x);
    
    // Normalize angle to 0-2π range
    if (mouseAngle < 0) mouseAngle += 2.0f * juce::MathConstants<float>::pi;
    
    // Convert angle to slider value range (adjust for knob's angle mapping)
    float normalizedAngle;
    if (mouseAngle > juce::MathConstants<float>::pi)
    {
        normalizedAngle = juce::jmap(mouseAngle, 2.35619f, 2.0f * juce::MathConstants<float>::pi, 0.0f, 0.5f);
    }
    else
    {
        normalizedAngle = juce::jmap(mouseAngle, 0.0f, 0.785398f, 0.5f, 1.0f);
    }
    
    normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
    
    // Convert to slider's value range
    double newValue = juce::jmap(static_cast<double>(normalizedAngle), 
                               0.0, 1.0,
                               mapping->targetSlider->getMinimum(),
                               mapping->targetSlider->getMaximum());
    
    // Update the appropriate range
    if (draggingMinRange)
    {
        mapping->userMinRange = newValue;
        // Ensure min doesn't exceed max
        if (mapping->userMinRange > mapping->userMaxRange)
            mapping->userMinRange = mapping->userMaxRange;
    }
    else
    {
        mapping->userMaxRange = newValue;
        // Ensure max doesn't go below min
        if (mapping->userMaxRange < mapping->userMinRange)
            mapping->userMaxRange = mapping->userMinRange;
    }
    
    repaint();
}

// Preset Management Implementation

void SynthesizerComponent::showSavePresetDialog()
{
    // Create default filename with timestamp
    juce::String defaultName = "MyPreset_" + juce::String(juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S"));
    juce::File defaultFile = audioProcessor.getPresetDirectory().getChildFile(defaultName + ".sxs2");
    
    auto chooser = std::make_shared<juce::FileChooser>("Save Preset",
                                                       defaultFile,
                                                       "*.sxs2",
                                                       true);
    
    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;
                                
                            if (file.existsAsFile())
                            {
                                // File already exists, ask for confirmation
                                int result = juce::AlertWindow::showYesNoCancelBox(
                                    juce::AlertWindow::QuestionIcon,
                                    "File Exists",
                                    "The file \"" + file.getFileName() + "\" already exists. Do you want to replace it?",
                                    "Replace",
                                    "Cancel",
                                    "",
                                    nullptr,
                                    nullptr);
                                    
                                if (result != 1) // If not "Replace"
                                    return;
                            }
                            
                            // Ensure the file has the correct extension
                            if (!file.hasFileExtension(".sxs2"))
                            {
                                file = file.withFileExtension(".sxs2");
                            }
                            
                            // Save the preset
                            if (audioProcessor.savePresetToFile(file, "User created preset"))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset saved successfully to:\n" + file.getFullPathName());
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to save preset to:\n" + file.getFullPathName());
                                });
                            }
                        });
}

void SynthesizerComponent::showLoadPresetDialog()
{
    auto chooser = std::make_shared<juce::FileChooser>("Load Preset",
                                                       audioProcessor.getPresetDirectory(),
                                                       "*.sxs2",
                                                       true);
    
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;
                                
                            if (audioProcessor.loadPreset(file.getFullPathName()))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset '" + file.getFileNameWithoutExtension() + "' loaded successfully!");
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to load preset: " + file.getFileName());
                                });
                            }
                        });
}

void SynthesizerComponent::showInitPresetDialog()
{
    juce::AlertWindow::showYesNoCancelBox(
        juce::AlertWindow::QuestionIcon,
        "Initialize Synthesizer",
        "This will reset all parameters to their default values. Any unsaved changes will be lost.\n\nAre you sure you want to continue?",
        "Yes, Initialize",
        "Cancel",
        juce::String(),
        this,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1) // Yes button clicked
            {
                audioProcessor.initializeAllParameters();
                updatePresetDisplay();
            }
        })
    );
}

void SynthesizerComponent::updatePresetDisplay()
{
    effectsPresetNameLabel.setText(audioProcessor.getCurrentPresetName(), 
                                 juce::dontSendNotification);
    repaint();
}

void SynthesizerComponent::updateAllGuiControls()
{
    // Main synthesizer controls (using correct control names)
    volumeControlsVolumeKnob.setValue(audioProcessor.getMasterVolume(), juce::dontSendNotification);
    volumeControlsDetuneKnob.setValue(audioProcessor.getOsc1Detune(), juce::dontSendNotification);
    volumeControlsStereoWidthKnob.setValue(audioProcessor.getSynthStereoWidth(), juce::dontSendNotification);
    volumeControlsPanKnob.setValue(audioProcessor.getSynthPan(), juce::dontSendNotification);
    phaseControlsPhaseKnob.setValue(audioProcessor.getSynthPhase(), juce::dontSendNotification);
    
    // Main ADSR envelope
    adsrAttackKnob.setValue(audioProcessor.getOsc1Attack(), juce::dontSendNotification);
    adsrDecayKnob.setValue(audioProcessor.getOsc1Decay(), juce::dontSendNotification);
    adsrSustainKnob.setValue(audioProcessor.getOsc1Sustain(), juce::dontSendNotification);
    adsrReleaseKnob.setValue(audioProcessor.getOsc1Release(), juce::dontSendNotification);
    
    // Oscillator 1 controls
    pulseWidthSlider.setValue(audioProcessor.getOsc1PulseWidth(), juce::dontSendNotification);
    
    // Update oscillator 1 wave type buttons
    int osc1Type = audioProcessor.getOsc1Type();
    waveTypeSineButton.setToggleState(osc1Type == 0, juce::dontSendNotification);
    waveTypeSawButton.setToggleState(osc1Type == 1, juce::dontSendNotification);
    waveTypeSquareButton.setToggleState(osc1Type == 2, juce::dontSendNotification);
    waveTypeTriangleButton.setToggleState(osc1Type == 3, juce::dontSendNotification);
    waveTypeWhiteNoiseButton.setToggleState(osc1Type == 4, juce::dontSendNotification);
    waveTypePinkNoiseButton.setToggleState(osc1Type == 5, juce::dontSendNotification);
    waveTypeRandomPhaseButton.setToggleState(osc1Type == 6, juce::dontSendNotification);
    
    // Oscillator 2 controls
    osc2VolumeKnob.setValue(audioProcessor.getOsc2Volume(), juce::dontSendNotification);
    osc2DetuneKnob.setValue(audioProcessor.getOsc2Detune(), juce::dontSendNotification);
    osc2StereoKnob.setValue(audioProcessor.getOsc2Stereo(), juce::dontSendNotification);
    osc2PanKnob.setValue(audioProcessor.getOsc2Pan(), juce::dontSendNotification);
    osc2PhaseKnob.setValue(audioProcessor.getOsc2Phase(), juce::dontSendNotification);
    osc2RandomPhaseButton.setToggleState(audioProcessor.getOsc2RandomPhase(), juce::dontSendNotification);
    
    // Oscillator 2 ADSR
    osc2AttackKnob.setValue(audioProcessor.getOsc2Attack(), juce::dontSendNotification);
    osc2DecayKnob.setValue(audioProcessor.getOsc2Decay(), juce::dontSendNotification);
    osc2SustainKnob.setValue(audioProcessor.getOsc2Sustain(), juce::dontSendNotification);
    osc2ReleaseKnob.setValue(audioProcessor.getOsc2Release(), juce::dontSendNotification);
    
    // Update oscillator 2 wave type buttons
    int osc2Type = audioProcessor.getOsc2Type();
    osc2SineButton.setToggleState(osc2Type == 0, juce::dontSendNotification);
    osc2SawButton.setToggleState(osc2Type == 1, juce::dontSendNotification);
    osc2SquareButton.setToggleState(osc2Type == 2, juce::dontSendNotification);
    osc2TriangleButton.setToggleState(osc2Type == 3, juce::dontSendNotification);
    osc2WhiteNoiseButton.setToggleState(osc2Type == 4, juce::dontSendNotification);
    osc2PinkNoiseButton.setToggleState(osc2Type == 5, juce::dontSendNotification);
    
    // Filter controls
    filterCutoffKnob.setValue(audioProcessor.getFilterCutoff(), juce::dontSendNotification);
    filterResonanceKnob.setValue(audioProcessor.getFilterResonance(), juce::dontSendNotification);
    osc1FilterEnableButton.setToggleState(audioProcessor.getOsc1FilterEnabled(), juce::dontSendNotification);
    osc2FilterEnableButton.setToggleState(audioProcessor.getOsc2FilterEnabled(), juce::dontSendNotification);
    
    // Effects controls - update the knobs with loaded preset values
    
    // Chorus controls
    chorusRateKnob.setValue(audioProcessor.getChorusRate(), juce::dontSendNotification);
    chorusDelay1Knob.setValue(audioProcessor.getChorusDelay1(), juce::dontSendNotification);
    chorusDelay2Knob.setValue(audioProcessor.getChorusDelay2(), juce::dontSendNotification);
    chorusDepthKnob.setValue(audioProcessor.getChorusDepth(), juce::dontSendNotification);
    chorusFeedKnob.setValue(audioProcessor.getChorusFeedback(), juce::dontSendNotification);
    chorusLpfKnob.setValue(audioProcessor.getChorusLPF(), juce::dontSendNotification);
    chorusMixKnob.setValue(audioProcessor.getChorusMix(), juce::dontSendNotification);
    chorusPowerButton.setToggleState(audioProcessor.getChorusEnabled(), juce::dontSendNotification);
    
    // Compressor controls
    compressorThresholdKnob.setValue(audioProcessor.getCompressorThreshold(), juce::dontSendNotification);
    compressorRatioKnob.setValue(audioProcessor.getCompressorRatio(), juce::dontSendNotification);
    compressorAttackKnob.setValue(audioProcessor.getCompressorAttack(), juce::dontSendNotification);
    compressorReleaseKnob.setValue(audioProcessor.getCompressorRelease(), juce::dontSendNotification);
    compressorGainKnob.setValue(audioProcessor.getCompressorGain(), juce::dontSendNotification);
    compressorMixKnob.setValue(audioProcessor.getCompressorMix(), juce::dontSendNotification);
    compressorPowerButton.setToggleState(audioProcessor.getCompressorEnabled(), juce::dontSendNotification);
    compressorMultibandButton.setToggleState(audioProcessor.getCompressorMultiband(), juce::dontSendNotification);
    
    // Delay controls
    delayFeedbackKnob.setValue(audioProcessor.getDelayFeedback(), juce::dontSendNotification);
    delayFilterFreqKnob.setValue(audioProcessor.getDelayFilterFreq(), juce::dontSendNotification);
    delayFilterQKnob.setValue(audioProcessor.getDelayFilterQ(), juce::dontSendNotification);
    delayMixKnob.setValue(audioProcessor.getDelayMix(), juce::dontSendNotification);
    delayPowerButton.setToggleState(audioProcessor.getDelayEnabled(), juce::dontSendNotification);
    
    // Distortion controls
    distortionDriveKnob.setValue(audioProcessor.getDistortionDrive(), juce::dontSendNotification);
    distortionMixKnob.setValue(audioProcessor.getDistortionMix(), juce::dontSendNotification);
    distortionFilterFreqKnob.setValue(audioProcessor.getDistortionFilterFreq(), juce::dontSendNotification);
    distortionFilterQKnob.setValue(audioProcessor.getDistortionFilterQ(), juce::dontSendNotification);
    distortionPowerButton.setToggleState(audioProcessor.getDistortionEnabled(), juce::dontSendNotification);
    
    // EQ controls
    eq1FreqKnob.setValue(audioProcessor.getEQ1Frequency(), juce::dontSendNotification);
    eq1QKnob.setValue(audioProcessor.getEQ1Q(), juce::dontSendNotification);
    eq1GainKnob.setValue(audioProcessor.getEQ1Gain(), juce::dontSendNotification);
    eq2FreqKnob.setValue(audioProcessor.getEQ2Frequency(), juce::dontSendNotification);
    eq2QKnob.setValue(audioProcessor.getEQ2Q(), juce::dontSendNotification);
    eq2GainKnob.setValue(audioProcessor.getEQ2Gain(), juce::dontSendNotification);
    eqOnOffButton.setToggleState(audioProcessor.getEQEnabled(), juce::dontSendNotification);
    
    // Flanger controls
    flangerMixKnob.setValue(audioProcessor.getFlangerMix(), juce::dontSendNotification);
    flangerRateKnob.setValue(audioProcessor.getFlangerRate(), juce::dontSendNotification);
    flangerDepthKnob.setValue(audioProcessor.getFlangerDepth(), juce::dontSendNotification);
    flangerFeedbackKnob.setValue(audioProcessor.getFlangerFeedback(), juce::dontSendNotification);
    flangerPhaseKnob.setValue(audioProcessor.getFlangerPhase(), juce::dontSendNotification);
    flangerPowerButton.setToggleState(audioProcessor.getFlangerEnabled(), juce::dontSendNotification);
    
    // Phaser controls
    phaserMixKnob.setValue(audioProcessor.getPhaserMix(), juce::dontSendNotification);
    phaserRateKnob.setValue(audioProcessor.getPhaserRate(), juce::dontSendNotification);
    phaserDepth1Knob.setValue(audioProcessor.getPhaserDepth1(), juce::dontSendNotification);
    phaserDepth2Knob.setValue(audioProcessor.getPhaserDepth2(), juce::dontSendNotification);
    phaserFrequencyKnob.setValue(audioProcessor.getPhaserFrequency(), juce::dontSendNotification);
    phaserFeedbackKnob.setValue(audioProcessor.getPhaserFeedback(), juce::dontSendNotification);
    phaserPhaseKnob.setValue(audioProcessor.getPhaserPhase(), juce::dontSendNotification);
    phaserPowerButton.setToggleState(audioProcessor.getPhaserEnabled(), juce::dontSendNotification);
    
    // Reverb controls
    reverbMixKnob.setValue(audioProcessor.getReverbMix(), juce::dontSendNotification);
    reverbLowCutKnob.setValue(audioProcessor.getReverbLowCut(), juce::dontSendNotification);
    reverbHighCutKnob.setValue(audioProcessor.getReverbHighCut(), juce::dontSendNotification);
    reverbSizeKnob.setValue(audioProcessor.getReverbSize(), juce::dontSendNotification);
    reverbPreDelayKnob.setValue(audioProcessor.getReverbPreDelay(), juce::dontSendNotification);
    reverbDampKnob.setValue(audioProcessor.getReverbDamping(), juce::dontSendNotification);
    reverbWidthKnob.setValue(audioProcessor.getReverbWidth(), juce::dontSendNotification);
    reverbPowerButton.setToggleState(audioProcessor.getReverbEnabled(), juce::dontSendNotification);
    
    // Macro controls (commented out - not implemented in processor)
    // macro1Knob.setValue(audioProcessor.getMacro1(), juce::dontSendNotification);
    // macro2Knob.setValue(audioProcessor.getMacro2(), juce::dontSendNotification);
    // macro3Knob.setValue(audioProcessor.getMacro3(), juce::dontSendNotification);
    // macro4Knob.setValue(audioProcessor.getMacro4(), juce::dontSendNotification);
    // macro5Knob.setValue(audioProcessor.getMacro5(), juce::dontSendNotification);
    // macro6Knob.setValue(audioProcessor.getMacro6(), juce::dontSendNotification);
    // macro7Knob.setValue(audioProcessor.getMacro7(), juce::dontSendNotification);
    // macro8Knob.setValue(audioProcessor.getMacro8(), juce::dontSendNotification);
    
    // Update envelope display
    updateEnvelopeDisplay();
    
    // Update preset display
    updatePresetDisplay();
    
    // Trigger a repaint to update the visuals
    repaint();
}

