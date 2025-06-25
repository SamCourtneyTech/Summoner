#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;

class CustomKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // Draw background circle (dark outline)
        g.setColour(juce::Colour(0xff333333));
        g.drawEllipse(rx, ry, rw, rw, 2.0f);
        
        // Draw the filled arc based on slider position
        if (sliderPos > 0.0f)
        {
            g.setColour(juce::Colours::white);
            juce::Path valueArc;
            valueArc.addCentredArc(centreX, centreY, radius - 1, radius - 1, 0.0f,
                                  rotaryStartAngle, angle, true);
            g.strokePath(valueArc, juce::PathStrokeType(3.0f));
        }
        
        // Draw center dot
        g.setColour(juce::Colours::white);
        auto dotSize = radius * 0.1f;
        g.fillEllipse(centreX - dotSize, centreY - dotSize, dotSize * 2, dotSize * 2);
        
        // Draw pointer line
        juce::Path pointer;
        auto pointerLength = radius * 0.7f;
        auto pointerThickness = 2.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.fillPath(pointer);
    }
};

class SynthesizerComponent : public juce::Component, private juce::Slider::Listener
{
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;

private:
    SummonerXSerum2AudioProcessor& audioProcessor;
    
    // Custom look and feel for knobs
    CustomKnobLookAndFeel customKnobLookAndFeel;
    
    // Placeholder UI elements for the synthesizer
    juce::Label titleLabel;
    juce::Label placeholderLabel;
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider filterSlider;
    juce::Label filterLabel;
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};