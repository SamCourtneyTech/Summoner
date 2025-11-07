#include "ADSRKnobsComponent.h"
#include "../PluginProcessor.h"

ADSRKnobsComponent::ADSRKnobsComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    // ATTACK KNOB
    adsrAttackLabel.setText("ATTACK", juce::dontSendNotification);
    adsrAttackLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    adsrAttackLabel.setJustificationType(juce::Justification::centred);
    adsrAttackLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    adsrAttackLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrAttackLabel);

    adsrAttackKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrAttackKnob.setRange(0.01, 2.0, 0.01);
    adsrAttackKnob.setValue(0.1);
    adsrAttackKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrAttackKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrAttackKnob.addListener(this);
    addAndMakeVisible(adsrAttackKnob);

    // DECAY KNOB
    adsrDecayLabel.setText("DECAY", juce::dontSendNotification);
    adsrDecayLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    adsrDecayLabel.setJustificationType(juce::Justification::centred);
    adsrDecayLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    adsrDecayLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrDecayLabel);

    adsrDecayKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrDecayKnob.setRange(0.01, 2.0, 0.01);
    adsrDecayKnob.setValue(0.2);
    adsrDecayKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrDecayKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrDecayKnob.addListener(this);
    addAndMakeVisible(adsrDecayKnob);

    // SUSTAIN KNOB
    adsrSustainLabel.setText("SUSTAIN", juce::dontSendNotification);
    adsrSustainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    adsrSustainLabel.setJustificationType(juce::Justification::centred);
    adsrSustainLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    adsrSustainLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrSustainLabel);

    adsrSustainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrSustainKnob.setRange(0.0, 1.0, 0.01);
    adsrSustainKnob.setValue(0.7);
    adsrSustainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrSustainKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrSustainKnob.addListener(this);
    addAndMakeVisible(adsrSustainKnob);

    // RELEASE KNOB
    adsrReleaseLabel.setText("RELEASE", juce::dontSendNotification);
    adsrReleaseLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    adsrReleaseLabel.setJustificationType(juce::Justification::centred);
    adsrReleaseLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    adsrReleaseLabel.setLookAndFeel(&ledLabelLookAndFeel);
    addAndMakeVisible(adsrReleaseLabel);

    adsrReleaseKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    adsrReleaseKnob.setRange(0.01, 3.0, 0.01);
    adsrReleaseKnob.setValue(0.3);
    adsrReleaseKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    adsrReleaseKnob.setLookAndFeel(&customKnobLookAndFeel);
    adsrReleaseKnob.addListener(this);
    addAndMakeVisible(adsrReleaseKnob);
}

ADSRKnobsComponent::~ADSRKnobsComponent()
{
    // Reset custom look and feel to avoid dangling pointers
    adsrAttackKnob.setLookAndFeel(nullptr);
    adsrDecayKnob.setLookAndFeel(nullptr);
    adsrSustainKnob.setLookAndFeel(nullptr);
    adsrReleaseKnob.setLookAndFeel(nullptr);

    // Reset LED label look and feel
    adsrAttackLabel.setLookAndFeel(nullptr);
    adsrDecayLabel.setLookAndFeel(nullptr);
    adsrSustainLabel.setLookAndFeel(nullptr);
    adsrReleaseLabel.setLookAndFeel(nullptr);
}

void ADSRKnobsComponent::resized()
{
    auto bounds = getLocalBounds();

    // Calculate knob width for 4 knobs with spacing
    auto knobWidth = (bounds.getWidth() - 45) / 4; // 45 = 3 spacings of 15px each

    auto attackArea = bounds.removeFromLeft(knobWidth);
    adsrAttackLabel.setBounds(attackArea.removeFromTop(20));
    adsrAttackKnob.setBounds(attackArea);

    bounds.removeFromLeft(15); // spacing

    auto decayArea = bounds.removeFromLeft(knobWidth);
    adsrDecayLabel.setBounds(decayArea.removeFromTop(20));
    adsrDecayKnob.setBounds(decayArea);

    bounds.removeFromLeft(15); // spacing

    auto sustainArea = bounds.removeFromLeft(knobWidth);
    adsrSustainLabel.setBounds(sustainArea.removeFromTop(20));
    adsrSustainKnob.setBounds(sustainArea);

    bounds.removeFromLeft(15); // spacing

    auto releaseArea = bounds;
    adsrReleaseLabel.setBounds(releaseArea.removeFromTop(20));
    adsrReleaseKnob.setBounds(releaseArea);
}

void ADSRKnobsComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &adsrAttackKnob)
    {
        audioProcessor.setOsc1Attack(static_cast<float>(adsrAttackKnob.getValue()));
        if (onEnvelopeChanged)
            onEnvelopeChanged();
    }
    else if (slider == &adsrDecayKnob)
    {
        audioProcessor.setOsc1Decay(static_cast<float>(adsrDecayKnob.getValue()));
        if (onEnvelopeChanged)
            onEnvelopeChanged();
    }
    else if (slider == &adsrSustainKnob)
    {
        audioProcessor.setOsc1Sustain(static_cast<float>(adsrSustainKnob.getValue()));
        if (onEnvelopeChanged)
            onEnvelopeChanged();
    }
    else if (slider == &adsrReleaseKnob)
    {
        audioProcessor.setOsc1Release(static_cast<float>(adsrReleaseKnob.getValue()));
        if (onEnvelopeChanged)
            onEnvelopeChanged();
    }
}
