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
    
    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setRange(0.0, 1.0, 0.01);
    attackSlider.setValue(0.1);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(attackSlider);
    
    // Release control
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);
    
    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setRange(0.0, 1.0, 0.01);
    releaseSlider.setValue(0.3);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(releaseSlider);
}

SynthesizerComponent::~SynthesizerComponent()
{
    // Clean destructor implementation
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
    
    // Controls in a 2x2 grid
    auto controlHeight = 80;
    auto controlWidth = bounds.getWidth() / 2 - 10;
    
    // Top row
    auto topRow = bounds.removeFromTop(controlHeight);
    
    auto volumeArea = topRow.removeFromLeft(controlWidth);
    volumeLabel.setBounds(volumeArea.removeFromTop(20));
    volumeSlider.setBounds(volumeArea);
    
    topRow.removeFromLeft(20); // spacing
    
    auto filterArea = topRow;
    filterLabel.setBounds(filterArea.removeFromTop(20));
    filterSlider.setBounds(filterArea);
    
    bounds.removeFromTop(20); // spacing between rows
    
    // Bottom row
    auto bottomRow = bounds.removeFromTop(controlHeight);
    
    auto attackArea = bottomRow.removeFromLeft(controlWidth);
    attackLabel.setBounds(attackArea.removeFromTop(20));
    attackSlider.setBounds(attackArea);
    
    bottomRow.removeFromLeft(20); // spacing
    
    auto releaseArea = bottomRow;
    releaseLabel.setBounds(releaseArea.removeFromTop(20));
    releaseSlider.setBounds(releaseArea);
}

void SynthesizerComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audioProcessor.setSynthVolume(static_cast<float>(volumeSlider.getValue()));
    }
}