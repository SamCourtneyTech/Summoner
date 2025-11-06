#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;
class SynthesizerComponent;
class ParametricEQComponent;
class GreenDigitalKnobLookAndFeel;
class GreenDigitalButtonLookAndFeel;

class EQControlsComponent : public juce::Component,
                             private juce::Slider::Listener,
                             private juce::Button::Listener
{
public:
    EQControlsComponent(SynthesizerComponent& parent,
                        SummonerXSerum2AudioProcessor& processor);
    ~EQControlsComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool hitTest(int x, int y) override;

    // Sync GUI with DSP state (for preset loading)
    void syncWithDSPState();

    // Set look and feel references
    void setLookAndFeels(GreenDigitalKnobLookAndFeel* knobLAF,
                         GreenDigitalButtonLookAndFeel* buttonLAF);

    // Set the parametric EQ component reference
    void setParametricEQ(ParametricEQComponent* eq);

    // Public accessors for macro mapping system and ParametricEQ integration
    juce::Slider& getEQ1FreqKnob() { return eq1FreqKnob; }
    juce::Slider& getEQ1QKnob() { return eq1QKnob; }
    juce::Slider& getEQ1GainKnob() { return eq1GainKnob; }
    juce::Slider& getEQ2FreqKnob() { return eq2FreqKnob; }
    juce::Slider& getEQ2QKnob() { return eq2QKnob; }
    juce::Slider& getEQ2GainKnob() { return eq2GainKnob; }
    juce::Slider& getEQ1NewFreqKnob() { return eq1NewFreqKnob; }
    juce::Slider& getEQ1NewQKnob() { return eq1NewQKnob; }
    juce::Slider& getEQ1NewGainKnob() { return eq1NewGainKnob; }
    juce::Slider& getEQ2NewFreqKnob() { return eq2NewFreqKnob; }
    juce::Slider& getEQ2NewQKnob() { return eq2NewQKnob; }
    juce::Slider& getEQ2NewGainKnob() { return eq2NewGainKnob; }

    // Button accessors for ParametricEQ integration
    juce::TextButton& getEQ1OnOffButton() { return eq1OnOffButton; }
    juce::TextButton& getEQ2OnOffButton() { return eq2OnOffButton; }

private:
    // Listener callbacks
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    // References
    SynthesizerComponent& parentSynthesizer;
    SummonerXSerum2AudioProcessor& audioProcessor;
    ParametricEQComponent* parametricEQ;

    // Look and feel references
    GreenDigitalKnobLookAndFeel* greenDigitalKnobLookAndFeel;
    GreenDigitalButtonLookAndFeel* greenDigitalButtonLookAndFeel;

    // EQ Master Controls
    juce::TextButton eqOnOffButton;

    // Band 1 (Left) controls
    juce::TextButton eq1ShelfButton;
    juce::TextButton eq1PeakButton;
    juce::TextButton eq1PassButton;
    juce::TextButton eq1OnOffButton;
    juce::Slider eq1FreqKnob;
    juce::Label eq1FreqLabel;
    juce::Slider eq1QKnob;
    juce::Label eq1QLabel;
    juce::Slider eq1GainKnob;
    juce::Label eq1GainLabel;

    // Band 2 (Right) controls
    juce::TextButton eq2ShelfButton;
    juce::TextButton eq2PeakButton;
    juce::TextButton eq2PassButton;
    juce::TextButton eq2OnOffButton;
    juce::Slider eq2FreqKnob;
    juce::Label eq2FreqLabel;
    juce::Slider eq2QKnob;
    juce::Label eq2QLabel;
    juce::Slider eq2GainKnob;
    juce::Label eq2GainLabel;

    // EQ Point labels
    juce::Label eq1PointLabel;
    juce::Label eq2PointLabel;

    // Additional EQ controls in their own row
    juce::Slider eq1NewFreqKnob;
    juce::Label eq1NewFreqLabel;
    juce::Slider eq1NewQKnob;
    juce::Label eq1NewQLabel;
    juce::Slider eq1NewGainKnob;
    juce::Label eq1NewGainLabel;

    juce::Slider eq2NewFreqKnob;
    juce::Label eq2NewFreqLabel;
    juce::Slider eq2NewQKnob;
    juce::Label eq2NewQLabel;
    juce::Slider eq2NewGainKnob;
    juce::Label eq2NewGainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQControlsComponent)
};
