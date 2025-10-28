#include "VolumeControlsComponent.h"
#include "../PluginProcessor.h"

VolumeControlsComponent::VolumeControlsComponent(SummonerXSerum2AudioProcessor& processor,
                                                 CustomKnobLookAndFeel* knobLookAndFeel,
                                                 LEDLabelLookAndFeel* labelLookAndFeel)
    : audioProcessor(processor),
      customKnobLookAndFeel(knobLookAndFeel),
      ledLabelLookAndFeel(labelLookAndFeel)
{
    // Make component transparent so parent backgrounds show through
    setOpaque(false);

    // VOLUME CONTROLS GROUP - Exact copy from original code
    volumeControlsVolumeLabel.setText("VOLUME", juce::dontSendNotification);
    volumeControlsVolumeLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsVolumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsVolumeLabel.setJustificationType(juce::Justification::centred);
    volumeControlsVolumeLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsVolumeLabel);

    volumeControlsVolumeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsVolumeKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsVolumeKnob.setValue(0.75); // Start oscillator 1 at 75% volume
    volumeControlsVolumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsVolumeKnob.setLookAndFeel(customKnobLookAndFeel);
    volumeControlsVolumeKnob.addListener(this);
    addAndMakeVisible(volumeControlsVolumeKnob);

    volumeControlsDetuneLabel.setText("DETUNE", juce::dontSendNotification);
    volumeControlsDetuneLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsDetuneLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsDetuneLabel.setJustificationType(juce::Justification::centred);
    volumeControlsDetuneLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsDetuneLabel);

    volumeControlsDetuneKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsDetuneKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsDetuneKnob.setValue(0.0);
    volumeControlsDetuneKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsDetuneKnob.setLookAndFeel(customKnobLookAndFeel);
    volumeControlsDetuneKnob.addListener(this);
    addAndMakeVisible(volumeControlsDetuneKnob);

    volumeControlsStereoWidthLabel.setText("STEREO", juce::dontSendNotification);
    volumeControlsStereoWidthLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsStereoWidthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsStereoWidthLabel.setJustificationType(juce::Justification::centred);
    volumeControlsStereoWidthLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsStereoWidthLabel);

    volumeControlsStereoWidthKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsStereoWidthKnob.setRange(0.0, 1.0, 0.01);
    volumeControlsStereoWidthKnob.setValue(0.5);
    volumeControlsStereoWidthKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsStereoWidthKnob.setLookAndFeel(customKnobLookAndFeel);
    volumeControlsStereoWidthKnob.addListener(this);
    addAndMakeVisible(volumeControlsStereoWidthKnob);

    volumeControlsPanLabel.setText("PAN", juce::dontSendNotification);
    volumeControlsPanLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    volumeControlsPanLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeControlsPanLabel.setJustificationType(juce::Justification::centred);
    volumeControlsPanLabel.setLookAndFeel(ledLabelLookAndFeel);
    addAndMakeVisible(volumeControlsPanLabel);

    volumeControlsPanKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeControlsPanKnob.setRange(-50.0, 50.0, 1.0);
    volumeControlsPanKnob.setValue(0.0);
    volumeControlsPanKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    volumeControlsPanKnob.setLookAndFeel(customKnobLookAndFeel);
    volumeControlsPanKnob.addListener(this);
    addAndMakeVisible(volumeControlsPanKnob);
}

VolumeControlsComponent::~VolumeControlsComponent()
{
    volumeControlsVolumeKnob.setLookAndFeel(nullptr);
    volumeControlsDetuneKnob.setLookAndFeel(nullptr);
    volumeControlsStereoWidthKnob.setLookAndFeel(nullptr);
    volumeControlsPanKnob.setLookAndFeel(nullptr);

    volumeControlsVolumeLabel.setLookAndFeel(nullptr);
    volumeControlsDetuneLabel.setLookAndFeel(nullptr);
    volumeControlsStereoWidthLabel.setLookAndFeel(nullptr);
    volumeControlsPanLabel.setLookAndFeel(nullptr);
}

void VolumeControlsComponent::resized()
{
    // Exact copy of layout logic from layoutVolumeKnobs
    // The component's bounds ARE the offsetVolumeSection
    auto offsetVolumeSection = getLocalBounds();

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

void VolumeControlsComponent::sliderValueChanged(juce::Slider* slider)
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
        audioProcessor.setOsc1StereoWidth(static_cast<float>(volumeControlsStereoWidthKnob.getValue()));
    }
    else if (slider == &volumeControlsPanKnob)
    {
        audioProcessor.setOsc1Pan(static_cast<float>(volumeControlsPanKnob.getValue()));
    }
}

void VolumeControlsComponent::updateAllGuiControls()
{
    volumeControlsVolumeKnob.setValue(audioProcessor.getOsc1Volume(), juce::dontSendNotification);
    volumeControlsDetuneKnob.setValue(audioProcessor.getOsc1Detune(), juce::dontSendNotification);
    volumeControlsStereoWidthKnob.setValue(audioProcessor.getOsc1StereoWidth(), juce::dontSendNotification);
    volumeControlsPanKnob.setValue(audioProcessor.getOsc1Pan(), juce::dontSendNotification);
}
