#include "SynthesizerComponent.h"
#include "PluginProcessor.h"

SynthesizerComponent::SynthesizerComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    // Title
    titleLabel.setText("Internal Synthesizer", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Press Start 2P", 20.0f, juce::Font::plain));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    // Placeholder text
    placeholderLabel.setText("Sine wave oscillator - play MIDI keys!", juce::dontSendNotification);
    placeholderLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    placeholderLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    placeholderLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(placeholderLabel);
    
    // Volume control
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volumeLabel);
    
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    
    // Filter control  
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    filterLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    filterLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(filterLabel);
    
    filterSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    filterSlider.setRange(0.0, 1.0, 0.01);
    filterSlider.setValue(0.5);
    filterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterSlider);
    
    // Attack control
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel);
    
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setRange(0.01, 2.0, 0.01);
    attackSlider.setValue(0.1);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackSlider.setLookAndFeel(&customKnobLookAndFeel);
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);
    
    // Decay control
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayLabel);
    
    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setRange(0.01, 2.0, 0.01);
    decaySlider.setValue(0.2);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    decaySlider.setLookAndFeel(&customKnobLookAndFeel);
    decaySlider.addListener(this);
    addAndMakeVisible(decaySlider);
    
    // Sustain control
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sustainLabel);
    
    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    sustainSlider.setLookAndFeel(&customKnobLookAndFeel);
    sustainSlider.addListener(this);
    addAndMakeVisible(sustainSlider);
    
    // Release control
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);
    
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setRange(0.01, 3.0, 0.01);
    releaseSlider.setValue(0.3);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    releaseSlider.setLookAndFeel(&customKnobLookAndFeel);
    releaseSlider.addListener(this);
    addAndMakeVisible(releaseSlider);
    
    // Oscillator type buttons
    sineWaveButton.setButtonText("SINE");
    sineWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    sineWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    sineWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    sineWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sineWaveButton.setClickingTogglesState(true);
    sineWaveButton.setToggleState(true, juce::dontSendNotification); // Start with sine selected
    sineWaveButton.addListener(this);
    addAndMakeVisible(sineWaveButton);
    
    sawWaveButton.setButtonText("SAW");
    sawWaveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    sawWaveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    sawWaveButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    sawWaveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sawWaveButton.setClickingTogglesState(true);
    sawWaveButton.addListener(this);
    addAndMakeVisible(sawWaveButton);
}

SynthesizerComponent::~SynthesizerComponent()
{
    // Reset custom look and feel to avoid dangling pointers
    attackSlider.setLookAndFeel(nullptr);
    decaySlider.setLookAndFeel(nullptr);
    sustainSlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
}

void SynthesizerComponent::paint(juce::Graphics& g)
{
    // Dark background with blue gradient
    juce::ColourGradient gradient(juce::Colour(0xff0a0a0a), 0.0f, 0.0f,
                                 juce::Colour(0xff1a1a2e), 0.0f, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Border
    g.setColour(juce::Colour(0xff16213e));
    g.drawRect(getLocalBounds(), 2);
}

void SynthesizerComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20);
    
    // Title at the top
    titleLabel.setBounds(bounds.removeFromTop(40));
    bounds.removeFromTop(10);
    
    // Placeholder text
    placeholderLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(30);
    
    // Controls in a 2x3 grid (2 linear sliders, 4 ADSR knobs)
    auto controlHeight = 100;
    auto controlWidth = bounds.getWidth() / 2 - 10;
    auto knobWidth = bounds.getWidth() / 4 - 15;
    
    // Top row - linear sliders
    auto topRow = bounds.removeFromTop(controlHeight);
    
    auto volumeArea = topRow.removeFromLeft(controlWidth);
    volumeLabel.setBounds(volumeArea.removeFromTop(20));
    volumeSlider.setBounds(volumeArea);
    
    topRow.removeFromLeft(20); // spacing
    
    auto filterArea = topRow;
    filterLabel.setBounds(filterArea.removeFromTop(20));
    filterSlider.setBounds(filterArea);
    
    bounds.removeFromTop(20); // spacing between rows
    
    // Oscillator type buttons
    auto buttonHeight = 30;
    auto buttonRow = bounds.removeFromTop(buttonHeight);
    auto buttonWidth = 80;
    
    // Center the buttons
    auto totalButtonWidth = buttonWidth * 2 + 20; // 2 buttons + spacing
    auto startX = (buttonRow.getWidth() - totalButtonWidth) / 2;
    
    sineWaveButton.setBounds(startX, buttonRow.getY(), buttonWidth, buttonHeight);
    sawWaveButton.setBounds(startX + buttonWidth + 20, buttonRow.getY(), buttonWidth, buttonHeight);
    
    bounds.removeFromTop(20); // spacing between rows
    
    // Bottom row - ADSR knobs
    auto adsrRow = bounds.removeFromTop(controlHeight);
    
    auto attackArea = adsrRow.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackArea.removeFromTop(20));
    attackSlider.setBounds(attackArea);
    
    adsrRow.removeFromLeft(15); // spacing
    
    auto decayArea = adsrRow.removeFromLeft(knobWidth);
    decayLabel.setBounds(decayArea.removeFromTop(20));
    decaySlider.setBounds(decayArea);
    
    adsrRow.removeFromLeft(15); // spacing
    
    auto sustainArea = adsrRow.removeFromLeft(knobWidth);
    sustainLabel.setBounds(sustainArea.removeFromTop(20));
    sustainSlider.setBounds(sustainArea);
    
    adsrRow.removeFromLeft(15); // spacing
    
    auto releaseArea = adsrRow;
    releaseLabel.setBounds(releaseArea.removeFromTop(20));
    releaseSlider.setBounds(releaseArea);
}

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioProcessor.setSynthVolume(static_cast<float>(volumeSlider.getValue()));
    }
    else if (slider == &attackSlider)
    {
        audioProcessor.setSynthAttack(static_cast<float>(attackSlider.getValue()));
    }
    else if (slider == &decaySlider)
    {
        audioProcessor.setSynthDecay(static_cast<float>(decaySlider.getValue()));
    }
    else if (slider == &sustainSlider)
    {
        audioProcessor.setSynthSustain(static_cast<float>(sustainSlider.getValue()));
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setSynthRelease(static_cast<float>(releaseSlider.getValue()));
    }
}

void SynthesizerComponent::buttonClicked(juce::Button* button)
{
    if (button == &sineWaveButton)
    {
        if (sineWaveButton.getToggleState())
        {
            sawWaveButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(0); // 0 = sine wave
        }
        else
        {
            sineWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
    else if (button == &sawWaveButton)
    {
        if (sawWaveButton.getToggleState())
        {
            sineWaveButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setOscillatorType(1); // 1 = saw wave
        }
        else
        {
            sawWaveButton.setToggleState(true, juce::dontSendNotification); // Keep at least one selected
        }
    }
}