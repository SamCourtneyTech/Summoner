#include "WaveTypeSelectorComponent.h"
#include "../PluginProcessor.h"

WaveTypeSelectorComponent::WaveTypeSelectorComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    // WAVE TYPE BUTTONS INITIALIZATION
    waveTypeSineButton.setButtonText("SIN");
    waveTypeSineButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSineButton.setClickingTogglesState(true);
    waveTypeSineButton.setToggleState(false, juce::dontSendNotification); // Sine not selected by default
    waveTypeSineButton.addListener(this);
    addAndMakeVisible(waveTypeSineButton);

    waveTypeSawButton.setButtonText("SAW");
    waveTypeSawButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSawButton.setClickingTogglesState(true);
    waveTypeSawButton.setToggleState(true, juce::dontSendNotification); // Start with saw selected
    waveTypeSawButton.addListener(this);
    addAndMakeVisible(waveTypeSawButton);

    // Set oscillator 1 to saw wave by default
    audioProcessor.setOsc1Type(1); // 1 = saw wave

    waveTypeSquareButton.setButtonText("SQR");
    waveTypeSquareButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeSquareButton.setClickingTogglesState(true);
    waveTypeSquareButton.addListener(this);
    addAndMakeVisible(waveTypeSquareButton);

    waveTypeTriangleButton.setButtonText("TRI");
    waveTypeTriangleButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeTriangleButton.setClickingTogglesState(true);
    waveTypeTriangleButton.addListener(this);
    addAndMakeVisible(waveTypeTriangleButton);

    waveTypeWhiteNoiseButton.setButtonText("WHT");
    waveTypeWhiteNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeWhiteNoiseButton.setClickingTogglesState(true);
    waveTypeWhiteNoiseButton.addListener(this);
    addAndMakeVisible(waveTypeWhiteNoiseButton);

    waveTypePinkNoiseButton.setButtonText("PNK");
    waveTypePinkNoiseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypePinkNoiseButton.setClickingTogglesState(true);
    waveTypePinkNoiseButton.addListener(this);
    addAndMakeVisible(waveTypePinkNoiseButton);

    waveTypeRandomPhaseButton.setButtonText("RND PHASE");
    waveTypeRandomPhaseButton.setLookAndFeel(&customWaveButtonLookAndFeel);
    waveTypeRandomPhaseButton.setClickingTogglesState(true);
    waveTypeRandomPhaseButton.setToggleState(true, juce::dontSendNotification); // Default to random phase ON
    waveTypeRandomPhaseButton.addListener(this);
    addAndMakeVisible(waveTypeRandomPhaseButton);
}

WaveTypeSelectorComponent::~WaveTypeSelectorComponent()
{
    // Reset look and feel
    waveTypeSineButton.setLookAndFeel(nullptr);
    waveTypeSawButton.setLookAndFeel(nullptr);
    waveTypeSquareButton.setLookAndFeel(nullptr);
    waveTypeTriangleButton.setLookAndFeel(nullptr);
    waveTypeWhiteNoiseButton.setLookAndFeel(nullptr);
    waveTypePinkNoiseButton.setLookAndFeel(nullptr);
    waveTypeRandomPhaseButton.setLookAndFeel(nullptr);
}

void WaveTypeSelectorComponent::resized()
{
    auto bounds = getLocalBounds();

    // Calculate button width and spacing to fill the section
    auto totalButtons = 6;
    auto buttonSpacing = 10;
    auto totalSpacing = (totalButtons - 1) * buttonSpacing;
    auto buttonWidth = (bounds.getWidth() - totalSpacing) / totalButtons;
    auto buttonHeight = bounds.getHeight();

    waveTypeSineButton.setBounds(bounds.getX(), bounds.getY(), buttonWidth, buttonHeight);
    waveTypeSawButton.setBounds(bounds.getX() + (buttonWidth + buttonSpacing), bounds.getY(), buttonWidth, buttonHeight);
    waveTypeSquareButton.setBounds(bounds.getX() + (buttonWidth + buttonSpacing) * 2, bounds.getY(), buttonWidth, buttonHeight);
    waveTypeTriangleButton.setBounds(bounds.getX() + (buttonWidth + buttonSpacing) * 3, bounds.getY(), buttonWidth, buttonHeight);
    waveTypeWhiteNoiseButton.setBounds(bounds.getX() + (buttonWidth + buttonSpacing) * 4, bounds.getY(), buttonWidth, buttonHeight);
    waveTypePinkNoiseButton.setBounds(bounds.getX() + (buttonWidth + buttonSpacing) * 5, bounds.getY(), buttonWidth, buttonHeight);
}

void WaveTypeSelectorComponent::buttonClicked(juce::Button* button)
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
}
