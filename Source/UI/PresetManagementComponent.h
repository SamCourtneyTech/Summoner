#pragma once
#include <JuceHeader.h>

class SummonerXSerum2AudioProcessor;

class PresetManagementComponent : public juce::Component
{
public:
    PresetManagementComponent(SummonerXSerum2AudioProcessor& processor);
    ~PresetManagementComponent() override;

    void resized() override;

    // Preset management functions
    void showSavePresetDialog();
    void showLoadPresetDialog();
    void showInitPresetDialog();
    void updatePresetDisplay();
    void updateAllGuiControls();

    // Access to buttons for setting look and feel
    juce::TextButton& getPrevButton() { return presetPrevButton; }
    juce::TextButton& getNextButton() { return presetNextButton; }
    juce::TextButton& getSaveButton() { return presetSaveButton; }
    juce::TextButton& getLoadButton() { return presetLoadButton; }
    juce::TextButton& getInitButton() { return presetInitButton; }
    juce::Label& getNameLabel() { return presetNameLabel; }

private:
    SummonerXSerum2AudioProcessor& audioProcessor;

    // Preset controls
    juce::TextButton presetPrevButton;
    juce::TextButton presetNextButton;
    juce::Label presetNameLabel;
    juce::TextButton presetSaveButton;
    juce::TextButton presetLoadButton;
    juce::TextButton presetInitButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManagementComponent)
};
