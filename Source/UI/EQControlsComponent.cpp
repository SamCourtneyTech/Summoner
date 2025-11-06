#include "EQControlsComponent.h"
#include "../PluginProcessor.h"
#include "../SynthesizerComponent.h"
#include "ParametricEQComponent.h"
#include "GreenDigitalKnobLookAndFeel.h"
#include "GreenDigitalButtonLookAndFeel.h"

EQControlsComponent::EQControlsComponent(SynthesizerComponent& parent,
                                         SummonerXSerum2AudioProcessor& processor)
    : parentSynthesizer(parent),
      audioProcessor(processor),
      parametricEQ(nullptr),
      greenDigitalKnobLookAndFeel(nullptr),
      greenDigitalButtonLookAndFeel(nullptr)
{
    // EQ EFFECT CONTROLS
    // Band 1 (Left) filter type buttons
    eq1ShelfButton.setButtonText("SHELF"); // Low shelf - for later implementation
    eq1ShelfButton.setClickingTogglesState(true);
    eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq1ShelfButton.addListener(this);
    eq1ShelfButton.setVisible(true);
    eq1ShelfButton.setBounds(17, 342, 40, 22);
    addAndMakeVisible(eq1ShelfButton);

    eq1PeakButton.setButtonText("PEAK");
    eq1PeakButton.setClickingTogglesState(true);
    eq1PeakButton.setToggleState(true, juce::dontSendNotification);
    eq1PeakButton.addListener(this);
    eq1PeakButton.setVisible(true);
    eq1PeakButton.setBounds(62, 342, 40, 22);
    addAndMakeVisible(eq1PeakButton);

    eq1PassButton.setButtonText("HIPASS");
    eq1PassButton.setClickingTogglesState(true);
    eq1PassButton.setToggleState(false, juce::dontSendNotification);
    eq1PassButton.addListener(this);
    eq1PassButton.setVisible(true);
    eq1PassButton.setBounds(107, 342, 40, 22);
    addAndMakeVisible(eq1PassButton);

    // Band 1 on/off button
    eq1OnOffButton.setButtonText("BAND 1");
    eq1OnOffButton.setClickingTogglesState(true);
    eq1OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq1OnOffButton.addListener(this);
    eq1OnOffButton.setVisible(true);
    eq1OnOffButton.setBounds(17, 370, 50, 22); // Below filter type buttons
    addAndMakeVisible(eq1OnOffButton);

    // Band 1 knobs
    eq1FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1FreqKnob.setValue(400.0);
    eq1FreqKnob.addListener(this);
    addAndMakeVisible(eq1FreqKnob);

    eq1FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq1FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1FreqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq1FreqLabel);

    eq1QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1QKnob.setRange(0.1, 10.0, 0.1);
    eq1QKnob.setValue(1.0);
    eq1QKnob.addListener(this);
    addAndMakeVisible(eq1QKnob);

    eq1QLabel.setText("Q", juce::dontSendNotification);
    eq1QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1QLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq1QLabel);

    eq1GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1GainKnob.setRange(-15.0, 15.0, 0.1);
    eq1GainKnob.setValue(0.0);
    eq1GainKnob.addListener(this);
    addAndMakeVisible(eq1GainKnob);

    eq1GainLabel.setText("GAIN", juce::dontSendNotification);
    eq1GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1GainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq1GainLabel);

    // Band 2 (Right) filter type buttons
    eq2ShelfButton.setButtonText("SHELF"); // High shelf - for later implementation
    eq2ShelfButton.setClickingTogglesState(true);
    eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
    eq2ShelfButton.addListener(this);
    eq2ShelfButton.setVisible(true);
    eq2ShelfButton.setBounds(152, 342, 40, 22);
    addAndMakeVisible(eq2ShelfButton);

    eq2PeakButton.setButtonText("PEAK");
    eq2PeakButton.setClickingTogglesState(true);
    eq2PeakButton.setToggleState(true, juce::dontSendNotification);
    eq2PeakButton.addListener(this);
    eq2PeakButton.setVisible(true);
    eq2PeakButton.setBounds(197, 342, 40, 22);
    addAndMakeVisible(eq2PeakButton);

    eq2PassButton.setButtonText("LOPASS");
    eq2PassButton.setClickingTogglesState(true);
    eq2PassButton.setToggleState(false, juce::dontSendNotification);
    eq2PassButton.addListener(this);
    eq2PassButton.setVisible(true);
    eq2PassButton.setBounds(242, 342, 40, 22);
    addAndMakeVisible(eq2PassButton);

    // Band 2 on/off button
    eq2OnOffButton.setButtonText("BAND 2");
    eq2OnOffButton.setClickingTogglesState(true);
    eq2OnOffButton.setToggleState(true, juce::dontSendNotification); // Enabled by default
    eq2OnOffButton.addListener(this);
    eq2OnOffButton.setVisible(true);
    eq2OnOffButton.setBounds(232, 370, 50, 22); // Below filter type buttons
    addAndMakeVisible(eq2OnOffButton);

    // Band 2 knobs
    eq2FreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2FreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2FreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2FreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2FreqKnob.setValue(4000.0);
    eq2FreqKnob.addListener(this);
    addAndMakeVisible(eq2FreqKnob);

    eq2FreqLabel.setText("FREQ", juce::dontSendNotification);
    eq2FreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2FreqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq2FreqLabel);

    eq2QKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2QKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2QKnob.setRange(0.1, 10.0, 0.1);
    eq2QKnob.setValue(1.0);
    eq2QKnob.addListener(this);
    addAndMakeVisible(eq2QKnob);

    eq2QLabel.setText("Q", juce::dontSendNotification);
    eq2QLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2QLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq2QLabel);

    eq2GainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2GainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2GainKnob.setRange(-15.0, 15.0, 0.1);
    eq2GainKnob.setValue(0.0);
    eq2GainKnob.addListener(this);
    addAndMakeVisible(eq2GainKnob);

    eq2GainLabel.setText("GAIN", juce::dontSendNotification);
    eq2GainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2GainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(eq2GainLabel);

    // NEW EQ CONTROLS ROW - Additional frequency and Q knobs
    // Band 1 new frequency knob
    eq1NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq1NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq1NewFreqKnob.setValue(400.0);
    eq1NewFreqKnob.addListener(this);
    eq1NewFreqKnob.setVisible(true);
    eq1NewFreqKnob.setBounds(8, 400, 35, 35); // New row below existing controls
    addAndMakeVisible(eq1NewFreqKnob);

    eq1NewFreqLabel.setText("FREQ1", juce::dontSendNotification);
    eq1NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq1NewFreqLabel.setVisible(true);
    eq1NewFreqLabel.setBounds(8, 440, 35, 15);
    addAndMakeVisible(eq1NewFreqLabel);

    // Band 1 new Q knob
    eq1NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewQKnob.setRange(0.1, 30.0, 0.1);
    eq1NewQKnob.setValue(1.0);
    eq1NewQKnob.addListener(this);
    eq1NewQKnob.setVisible(true);
    eq1NewQKnob.setBounds(53, 400, 35, 35);
    addAndMakeVisible(eq1NewQKnob);

    eq1NewQLabel.setText("Q1", juce::dontSendNotification);
    eq1NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewQLabel.setJustificationType(juce::Justification::centred);
    eq1NewQLabel.setVisible(true);
    eq1NewQLabel.setBounds(53, 440, 35, 15);
    addAndMakeVisible(eq1NewQLabel);

    // Band 1 new gain knob
    eq1NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq1NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq1NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq1NewGainKnob.setValue(0.0);
    eq1NewGainKnob.addListener(this);
    eq1NewGainKnob.setVisible(true);
    eq1NewGainKnob.setBounds(98, 400, 35, 35);
    addAndMakeVisible(eq1NewGainKnob);

    eq1NewGainLabel.setText("GAIN1", juce::dontSendNotification);
    eq1NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq1NewGainLabel.setJustificationType(juce::Justification::centred);
    eq1NewGainLabel.setVisible(true);
    eq1NewGainLabel.setBounds(98, 440, 35, 15);
    addAndMakeVisible(eq1NewGainLabel);

    // Band 2 new frequency knob
    eq2NewFreqKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewFreqKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewFreqKnob.setRange(20.0, 20000.0, 1.0);
    eq2NewFreqKnob.setSkewFactor(0.3f); // Logarithmic scale for frequency
    eq2NewFreqKnob.setValue(4000.0);
    eq2NewFreqKnob.addListener(this);
    eq2NewFreqKnob.setVisible(true);
    eq2NewFreqKnob.setBounds(163, 400, 35, 35);
    addAndMakeVisible(eq2NewFreqKnob);

    eq2NewFreqLabel.setText("FREQ2", juce::dontSendNotification);
    eq2NewFreqLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewFreqLabel.setJustificationType(juce::Justification::centred);
    eq2NewFreqLabel.setVisible(true);
    eq2NewFreqLabel.setBounds(163, 440, 35, 15);
    addAndMakeVisible(eq2NewFreqLabel);

    // Band 2 new Q knob
    eq2NewQKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewQKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewQKnob.setRange(0.1, 30.0, 0.1);
    eq2NewQKnob.setValue(1.0);
    eq2NewQKnob.addListener(this);
    eq2NewQKnob.setVisible(true);
    eq2NewQKnob.setBounds(208, 400, 35, 35);
    addAndMakeVisible(eq2NewQKnob);

    eq2NewQLabel.setText("Q2", juce::dontSendNotification);
    eq2NewQLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewQLabel.setJustificationType(juce::Justification::centred);
    eq2NewQLabel.setVisible(true);
    eq2NewQLabel.setBounds(208, 440, 35, 15);
    addAndMakeVisible(eq2NewQLabel);

    // Band 2 new gain knob
    eq2NewGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    eq2NewGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    eq2NewGainKnob.setRange(-15.0, 15.0, 0.1);
    eq2NewGainKnob.setValue(0.0);
    eq2NewGainKnob.addListener(this);
    eq2NewGainKnob.setVisible(true);
    eq2NewGainKnob.setBounds(253, 400, 35, 35);
    addAndMakeVisible(eq2NewGainKnob);

    eq2NewGainLabel.setText("GAIN2", juce::dontSendNotification);
    eq2NewGainLabel.setFont(juce::Font("Times New Roman", 8.0f, juce::Font::bold));
    eq2NewGainLabel.setJustificationType(juce::Justification::centred);
    eq2NewGainLabel.setVisible(true);
    eq2NewGainLabel.setBounds(253, 440, 35, 15);
    addAndMakeVisible(eq2NewGainLabel);

    // EQ Point labels - positioned above buttons (bracket lines will be painted separately)
    eq1PointLabel.setText("BAND 1", juce::dontSendNotification);
    eq1PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq1PointLabel.setJustificationType(juce::Justification::centred);
    eq1PointLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
    eq1PointLabel.setBounds(45, 314, 70, 15);
    addAndMakeVisible(eq1PointLabel);

    eq2PointLabel.setText("BAND 2", juce::dontSendNotification);
    eq2PointLabel.setFont(juce::Font("Times New Roman", 9.0f, juce::Font::bold));
    eq2PointLabel.setJustificationType(juce::Justification::centred);
    eq2PointLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    eq2PointLabel.setBounds(183, 314, 70, 15);
    addAndMakeVisible(eq2PointLabel);

    // EQ Master on/off button
    eqOnOffButton.setButtonText("EQ ON");
    eqOnOffButton.setClickingTogglesState(true);
    eqOnOffButton.setToggleState(true, juce::dontSendNotification);
    eqOnOffButton.addListener(this);
    eqOnOffButton.setVisible(true);
    eqOnOffButton.setBounds(124, 10, 50, 22); // Position at top center of EQ controls
    addAndMakeVisible(eqOnOffButton);
}

EQControlsComponent::~EQControlsComponent()
{
    // Reset look and feel to avoid dangling pointers
    eq1ShelfButton.setLookAndFeel(nullptr);
    eq1PeakButton.setLookAndFeel(nullptr);
    eq1PassButton.setLookAndFeel(nullptr);
    eq1OnOffButton.setLookAndFeel(nullptr);
    eq2ShelfButton.setLookAndFeel(nullptr);
    eq2PeakButton.setLookAndFeel(nullptr);
    eq2PassButton.setLookAndFeel(nullptr);
    eq2OnOffButton.setLookAndFeel(nullptr);
    eqOnOffButton.setLookAndFeel(nullptr);

    eq1FreqKnob.setLookAndFeel(nullptr);
    eq1QKnob.setLookAndFeel(nullptr);
    eq1GainKnob.setLookAndFeel(nullptr);
    eq2FreqKnob.setLookAndFeel(nullptr);
    eq2QKnob.setLookAndFeel(nullptr);
    eq2GainKnob.setLookAndFeel(nullptr);

    eq1NewFreqKnob.setLookAndFeel(nullptr);
    eq1NewQKnob.setLookAndFeel(nullptr);
    eq1NewGainKnob.setLookAndFeel(nullptr);
    eq2NewFreqKnob.setLookAndFeel(nullptr);
    eq2NewQKnob.setLookAndFeel(nullptr);
    eq2NewGainKnob.setLookAndFeel(nullptr);

    eq1FreqLabel.setLookAndFeel(nullptr);
    eq1QLabel.setLookAndFeel(nullptr);
    eq1GainLabel.setLookAndFeel(nullptr);
    eq2FreqLabel.setLookAndFeel(nullptr);
    eq2QLabel.setLookAndFeel(nullptr);
    eq2GainLabel.setLookAndFeel(nullptr);

    eq1NewFreqLabel.setLookAndFeel(nullptr);
    eq1NewQLabel.setLookAndFeel(nullptr);
    eq1NewGainLabel.setLookAndFeel(nullptr);
    eq2NewFreqLabel.setLookAndFeel(nullptr);
    eq2NewQLabel.setLookAndFeel(nullptr);
    eq2NewGainLabel.setLookAndFeel(nullptr);
}

void EQControlsComponent::paint(juce::Graphics& g)
{
    // No custom painting needed - all handled by child components
}

void EQControlsComponent::resized()
{
    // All bounds are set explicitly in constructor
    // This method can be used for dynamic layouts in the future
}

bool EQControlsComponent::hitTest(int x, int y)
{
    // Allow mouse events to pass through to the parametricEQ component
    // The parametricEQ is positioned at y=95 with height 200 (so y=95 to y=295)
    // Only intercept mouse events for areas where we have actual controls

    // If click is in the parametricEQ area, don't intercept
    if (parametricEQ != nullptr && y >= 95 && y <= 295)
    {
        return false; // Let the click pass through to parametricEQ
    }

    // For all other areas, check if we hit any of our child components
    for (auto* child : getChildren())
    {
        if (child->getBounds().contains(x, y) && child->isVisible())
        {
            return true; // We have a control here, intercept the click
        }
    }

    return false; // No controls here, let click pass through
}

void EQControlsComponent::setLookAndFeels(GreenDigitalKnobLookAndFeel* knobLAF,
                                          GreenDigitalButtonLookAndFeel* buttonLAF)
{
    greenDigitalKnobLookAndFeel = knobLAF;
    greenDigitalButtonLookAndFeel = buttonLAF;

    if (greenDigitalKnobLookAndFeel != nullptr)
    {
        eq1FreqKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1QKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1GainKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2FreqKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2QKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2GainKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1NewFreqKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1NewQKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1NewGainKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewFreqKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewQKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewGainKnob.setLookAndFeel(greenDigitalKnobLookAndFeel);

        eq1FreqLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1QLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1GainLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2FreqLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2QLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2GainLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);

        eq1NewFreqLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1NewQLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq1NewGainLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewFreqLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewQLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
        eq2NewGainLabel.setLookAndFeel(greenDigitalKnobLookAndFeel);
    }

    if (greenDigitalButtonLookAndFeel != nullptr)
    {
        eq1ShelfButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq1PeakButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq1PassButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq1OnOffButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq2ShelfButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq2PeakButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq2PassButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eq2OnOffButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
        eqOnOffButton.setLookAndFeel(greenDigitalButtonLookAndFeel);
    }
}

void EQControlsComponent::setParametricEQ(ParametricEQComponent* eq)
{
    parametricEQ = eq;
}

void EQControlsComponent::syncWithDSPState()
{
    // Sync all knobs with processor state
    eq1FreqKnob.setValue(audioProcessor.getEQ1Frequency(), juce::dontSendNotification);
    eq1QKnob.setValue(audioProcessor.getEQ1Q(), juce::dontSendNotification);
    eq1GainKnob.setValue(audioProcessor.getEQ1Gain(), juce::dontSendNotification);
    eq2FreqKnob.setValue(audioProcessor.getEQ2Frequency(), juce::dontSendNotification);
    eq2QKnob.setValue(audioProcessor.getEQ2Q(), juce::dontSendNotification);
    eq2GainKnob.setValue(audioProcessor.getEQ2Gain(), juce::dontSendNotification);
    eqOnOffButton.setToggleState(audioProcessor.getEQEnabled(), juce::dontSendNotification);

    // Sync duplicate knobs
    eq1NewFreqKnob.setValue(eq1FreqKnob.getValue(), juce::dontSendNotification);
    eq1NewQKnob.setValue(eq1QKnob.getValue(), juce::dontSendNotification);
    eq1NewGainKnob.setValue(eq1GainKnob.getValue(), juce::dontSendNotification);
    eq2NewFreqKnob.setValue(eq2FreqKnob.getValue(), juce::dontSendNotification);
    eq2NewQKnob.setValue(eq2QKnob.getValue(), juce::dontSendNotification);
    eq2NewGainKnob.setValue(eq2GainKnob.getValue(), juce::dontSendNotification);

    repaint();
}

void EQControlsComponent::sliderValueChanged(juce::Slider* slider)
{
    if (parametricEQ == nullptr)
        return;

    // Band 1 main knobs
    if (slider == &eq1FreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1FreqKnob.getValue()));
        parametricEQ->getBand(0).frequency = static_cast<float>(eq1FreqKnob.getValue());
        eq1NewFreqKnob.setValue(eq1FreqKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    else if (slider == &eq1QKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1QKnob.getValue()));
        parametricEQ->getBand(0).q = static_cast<float>(eq1QKnob.getValue());
        eq1NewQKnob.setValue(eq1QKnob.getValue(), juce::dontSendNotification);
        parametricEQ->repaint();
    }
    else if (slider == &eq1GainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1GainKnob.getValue()));
        parametricEQ->getBand(0).gain = static_cast<float>(eq1GainKnob.getValue());
        eq1NewGainKnob.setValue(eq1GainKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    // Band 2 main knobs
    else if (slider == &eq2FreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2FreqKnob.getValue()));
        parametricEQ->getBand(1).frequency = static_cast<float>(eq2FreqKnob.getValue());
        eq2NewFreqKnob.setValue(eq2FreqKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    else if (slider == &eq2QKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2QKnob.getValue()));
        parametricEQ->getBand(1).q = static_cast<float>(eq2QKnob.getValue());
        eq2NewQKnob.setValue(eq2QKnob.getValue(), juce::dontSendNotification);
        parametricEQ->repaint();
    }
    else if (slider == &eq2GainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2GainKnob.getValue()));
        parametricEQ->getBand(1).gain = static_cast<float>(eq2GainKnob.getValue());
        eq2NewGainKnob.setValue(eq2GainKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    // Band 1 new knobs (sync with main knobs)
    else if (slider == &eq1NewFreqKnob)
    {
        audioProcessor.setEQ1Frequency(static_cast<float>(eq1NewFreqKnob.getValue()));
        parametricEQ->getBand(0).frequency = static_cast<float>(eq1NewFreqKnob.getValue());
        eq1FreqKnob.setValue(eq1NewFreqKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    else if (slider == &eq1NewQKnob)
    {
        audioProcessor.setEQ1Q(static_cast<float>(eq1NewQKnob.getValue()));
        parametricEQ->getBand(0).q = static_cast<float>(eq1NewQKnob.getValue());
        eq1QKnob.setValue(eq1NewQKnob.getValue(), juce::dontSendNotification);
        parametricEQ->repaint();
    }
    else if (slider == &eq1NewGainKnob)
    {
        audioProcessor.setEQ1Gain(static_cast<float>(eq1NewGainKnob.getValue()));
        parametricEQ->getBand(0).gain = static_cast<float>(eq1NewGainKnob.getValue());
        eq1GainKnob.setValue(eq1NewGainKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    // Band 2 new knobs (sync with main knobs)
    else if (slider == &eq2NewFreqKnob)
    {
        audioProcessor.setEQ2Frequency(static_cast<float>(eq2NewFreqKnob.getValue()));
        parametricEQ->getBand(1).frequency = static_cast<float>(eq2NewFreqKnob.getValue());
        eq2FreqKnob.setValue(eq2NewFreqKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
    else if (slider == &eq2NewQKnob)
    {
        audioProcessor.setEQ2Q(static_cast<float>(eq2NewQKnob.getValue()));
        parametricEQ->getBand(1).q = static_cast<float>(eq2NewQKnob.getValue());
        eq2QKnob.setValue(eq2NewQKnob.getValue(), juce::dontSendNotification);
        parametricEQ->repaint();
    }
    else if (slider == &eq2NewGainKnob)
    {
        audioProcessor.setEQ2Gain(static_cast<float>(eq2NewGainKnob.getValue()));
        parametricEQ->getBand(1).gain = static_cast<float>(eq2NewGainKnob.getValue());
        eq2GainKnob.setValue(eq2NewGainKnob.getValue(), juce::dontSendNotification);
        parametricEQ->resized();
        parametricEQ->repaint();
    }
}

void EQControlsComponent::buttonClicked(juce::Button* button)
{
    if (parametricEQ == nullptr)
        return;

    // EQ Master On/Off button
    if (button == &eqOnOffButton)
    {
        if (eqOnOffButton.getToggleState())
        {
            eqOnOffButton.setButtonText("EQ ON");
            audioProcessor.setEQEnabled(true);
            parametricEQ->setVisible(true);
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eqOnOffButton.setButtonText("EQ OFF");
            audioProcessor.setEQEnabled(false);
            parametricEQ->setVisible(true); // Keep visible but show it's disabled
        }
        parametricEQ->repaint();
    }
    // EQ Band 1 filter type buttons
    else if (button == &eq1PeakButton)
    {
        if (eq1PeakButton.getToggleState())
        {
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(0, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ1Type(0); // 0 = Peak
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq1PeakButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    else if (button == &eq1ShelfButton)
    {
        if (eq1ShelfButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(0, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ1Type(1); // 1 = Low Shelf
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq1ShelfButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    else if (button == &eq1PassButton)
    {
        if (eq1PassButton.getToggleState())
        {
            eq1PeakButton.setToggleState(false, juce::dontSendNotification);
            eq1ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(0, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ1Type(2); // 2 = High Pass
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq1PassButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    // EQ Band 2 filter type buttons
    else if (button == &eq2PeakButton)
    {
        if (eq2PeakButton.getToggleState())
        {
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(1, ParametricEQComponent::FilterType::Peak);
            audioProcessor.setEQ2Type(0); // 0 = Peak
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq2PeakButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    else if (button == &eq2ShelfButton)
    {
        if (eq2ShelfButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2PassButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(1, ParametricEQComponent::FilterType::Shelf);
            audioProcessor.setEQ2Type(1); // 1 = High Shelf
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq2ShelfButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    else if (button == &eq2PassButton)
    {
        if (eq2PassButton.getToggleState())
        {
            eq2PeakButton.setToggleState(false, juce::dontSendNotification);
            eq2ShelfButton.setToggleState(false, juce::dontSendNotification);
            parametricEQ->setBandFilterType(1, ParametricEQComponent::FilterType::Pass);
            audioProcessor.setEQ2Type(2); // 2 = Low Pass
            parametricEQ->syncWithDSPState();
        }
        else
        {
            eq2PassButton.setToggleState(true, juce::dontSendNotification);
        }
    }
    // EQ Band on/off buttons
    else if (button == &eq1OnOffButton)
    {
        audioProcessor.setEQ1Enabled(eq1OnOffButton.getToggleState());
        parametricEQ->syncWithDSPState();
    }
    else if (button == &eq2OnOffButton)
    {
        audioProcessor.setEQ2Enabled(eq2OnOffButton.getToggleState());
        parametricEQ->syncWithDSPState();
    }
}
