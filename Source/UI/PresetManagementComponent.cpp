#include "PresetManagementComponent.h"
#include "../PluginProcessor.h"

PresetManagementComponent::PresetManagementComponent(SummonerXSerum2AudioProcessor& processor)
    : audioProcessor(processor)
{
    // Setup preset buttons
    presetPrevButton.setButtonText("<");
    addAndMakeVisible(presetPrevButton);

    presetNextButton.setButtonText(">");
    addAndMakeVisible(presetNextButton);

    presetNameLabel.setText("DEFAULT", juce::dontSendNotification);
    presetNameLabel.setJustificationType(juce::Justification::centred);
    presetNameLabel.setFont(juce::Font("Press Start 2P", 10.0f, juce::Font::plain));
    presetNameLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ff00));
    addAndMakeVisible(presetNameLabel);

    presetSaveButton.setButtonText("SAVE");
    addAndMakeVisible(presetSaveButton);

    presetLoadButton.setButtonText("LOAD");
    addAndMakeVisible(presetLoadButton);

    presetInitButton.setButtonText("INIT");
    addAndMakeVisible(presetInitButton);

    // Setup button callbacks
    presetSaveButton.onClick = [this]() {
        showSavePresetDialog();
    };

    presetLoadButton.onClick = [this]() {
        showLoadPresetDialog();
    };

    presetInitButton.onClick = [this]() {
        showInitPresetDialog();
    };

    presetPrevButton.onClick = [this]() {
        audioProcessor.previousPreset();
        updatePresetDisplay();
    };

    presetNextButton.onClick = [this]() {
        audioProcessor.nextPreset();
        updatePresetDisplay();
    };

    // Initialize preset display
    updatePresetDisplay();
}

PresetManagementComponent::~PresetManagementComponent()
{
}

void PresetManagementComponent::resized()
{
    auto bounds = getLocalBounds();

    // Layout will be controlled by parent component
    // This is called from SynthesizerComponent's resized() method
    // The parent sets the bounds for the entire component, and we layout children here

    int presetControlsHeight = 30;
    int buttonWidth = 50;
    int buttonHeight = 20;
    int labelWidth = 110;  // Reduced from 120 to save more space

    auto presetArea = bounds;
    int presetStartX = presetArea.getX();
    int presetY = presetArea.getY();

    // Left arrow stays in place
    presetPrevButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + 1;  // Reduced spacing from 2 to 1

    // Preset name label - moved closer to left arrow
    presetNameLabel.setBounds(presetStartX, presetY, labelWidth, presetControlsHeight);
    presetStartX += labelWidth + 1;  // Reduced spacing from 2 to 1

    // Right arrow - moved closer
    presetNextButton.setBounds(presetStartX, presetY + 5, buttonWidth, buttonHeight);
    presetStartX += buttonWidth + 5;  // Reduced spacing from 10 to 5

    // Create triangle layout: SAVE on top, LOAD and INIT below
    // Shift the whole triangle left by 20 pixels to create more room
    int saveButtonX = presetStartX - 20;

    // Save button on top (centered over the two below)
    int buttonSpacing = 5;
    int triangleOffset = (buttonWidth + buttonSpacing) / 2;
    presetSaveButton.setBounds(saveButtonX + triangleOffset, presetY - 5, buttonWidth, buttonHeight);

    // Load button bottom left
    presetLoadButton.setBounds(saveButtonX, presetY + 19, buttonWidth, buttonHeight);

    // Init button bottom right
    presetInitButton.setBounds(saveButtonX + buttonWidth + buttonSpacing, presetY + 19, buttonWidth, buttonHeight);
}

void PresetManagementComponent::showSavePresetDialog()
{
    // Create default filename with timestamp
    juce::String defaultName = "MyPreset_" + juce::String(juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S"));
    juce::File defaultFile = audioProcessor.getPresetDirectory().getChildFile(defaultName + ".sxs2");

    auto chooser = std::make_shared<juce::FileChooser>("Save Preset",
                                                       defaultFile,
                                                       "*.sxs2",
                                                       true);

    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;

                            if (file.existsAsFile())
                            {
                                // File already exists, ask for confirmation
                                int result = juce::AlertWindow::showYesNoCancelBox(
                                    juce::AlertWindow::QuestionIcon,
                                    "File Exists",
                                    "The file \"" + file.getFileName() + "\" already exists. Do you want to replace it?",
                                    "Replace",
                                    "Cancel",
                                    "",
                                    nullptr,
                                    nullptr);

                                if (result != 1) // If not "Replace"
                                    return;
                            }

                            // Ensure the file has the correct extension
                            if (!file.hasFileExtension(".sxs2"))
                            {
                                file = file.withFileExtension(".sxs2");
                            }

                            // Save the preset
                            if (audioProcessor.savePresetToFile(file, "User created preset"))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset saved successfully to:\n" + file.getFullPathName());
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to save preset to:\n" + file.getFullPathName());
                                });
                            }
                        });
}

void PresetManagementComponent::showLoadPresetDialog()
{
    auto chooser = std::make_shared<juce::FileChooser>("Load Preset",
                                                       audioProcessor.getPresetDirectory(),
                                                       "*.sxs2",
                                                       true);

    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                        [this, chooser](const juce::FileChooser& fc) mutable
                        {
                            auto file = fc.getResult();
                            if (file == juce::File{})
                                return;

                            if (audioProcessor.loadPreset(file.getFullPathName()))
                            {
                                juce::MessageManager::callAsync([this, file]() {
                                    updatePresetDisplay();
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::InfoIcon,
                                        "Success",
                                        "Preset '" + file.getFileNameWithoutExtension() + "' loaded successfully!");
                                });
                            }
                            else
                            {
                                juce::MessageManager::callAsync([file]() {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::AlertWindow::WarningIcon,
                                        "Error",
                                        "Failed to load preset: " + file.getFileName());
                                });
                            }
                        });
}

void PresetManagementComponent::showInitPresetDialog()
{
    juce::AlertWindow::showYesNoCancelBox(
        juce::AlertWindow::QuestionIcon,
        "Initialize Synthesizer",
        "This will reset all parameters to their default values. Any unsaved changes will be lost.\n\nAre you sure you want to continue?",
        "Yes, Initialize",
        "Cancel",
        juce::String(),
        this,
        juce::ModalCallbackFunction::create([this](int result) {
            if (result == 1) // Yes button clicked
            {
                audioProcessor.initializeAllParameters();
                updatePresetDisplay();
            }
        })
    );
}

void PresetManagementComponent::updatePresetDisplay()
{
    presetNameLabel.setText(audioProcessor.getCurrentPresetName(),
                          juce::dontSendNotification);
    repaint();
}

void PresetManagementComponent::updateAllGuiControls()
{
    updatePresetDisplay();
}
