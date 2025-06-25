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
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(5);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // Draw the block arc based on slider position
        if (sliderPos > 0.0f)
        {
            g.setColour(juce::Colours::white);
            
            // Create evenly spaced white blocks
            auto arcRadius = radius - 1;
            auto blockSize = 0.08f; // Smaller blocks for rounder appearance
            auto gapSize = 0.02f; // Smaller gaps for smoother look
            auto currentAngle = rotaryStartAngle;
            
            while (currentAngle < angle)
            {
                auto blockEndAngle = juce::jmin(currentAngle + blockSize, angle);
                
                // Draw white rectangular block
                auto startX = centreX + arcRadius * std::cos(currentAngle - juce::MathConstants<float>::halfPi);
                auto startY = centreY + arcRadius * std::sin(currentAngle - juce::MathConstants<float>::halfPi);
                auto endX = centreX + arcRadius * std::cos(blockEndAngle - juce::MathConstants<float>::halfPi);
                auto endY = centreY + arcRadius * std::sin(blockEndAngle - juce::MathConstants<float>::halfPi);
                
                // Draw thick line segment
                juce::Path blockPath;
                blockPath.addLineSegment(juce::Line<float>(startX, startY, endX, endY), 5.0f);
                g.fillPath(blockPath);
                
                currentAngle += blockSize + gapSize;
            }
        }
    }
};

class SynthesizerComponent : public juce::Component, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    explicit SynthesizerComponent(SummonerXSerum2AudioProcessor& processor);
    ~SynthesizerComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;

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
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    
    // Oscillator type buttons
    juce::TextButton sineWaveButton;
    juce::TextButton sawWaveButton;
    juce::TextButton squareWaveButton;
    juce::TextButton triangleWaveButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesizerComponent)
};