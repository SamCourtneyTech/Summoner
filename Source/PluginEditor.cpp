#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include "LoadingComponent.h"

#if JUCE_WINDOWS
    #include <windows.h>
#endif

SummonerXSerum2AudioProcessorEditor::SummonerXSerum2AudioProcessorEditor(SummonerXSerum2AudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    settings(p),
    synthesizer(p),
    chatBar(p),
    loadingManager(std::make_unique<LoadingScreenManager>(this))
{
    setName("SummonerXSerum2AudioProcessorEditor");
    setSize(1192, 815);

    juce::PropertiesFile::Options options;
    options.applicationName = "Summoner";
    options.filenameSuffix = ".settings";
    options.folderName = "SummonerApp";
    options.osxLibrarySubFolder = "Application Support";
    appProps.setStorageParameters(options);

    // Setup tabs
    tabs.addTab("LLM", juce::Colours::transparentBlack, &chatBar, false);
    tabs.addTab("Synthesizer", juce::Colours::transparentBlack, &synthesizer, false);
    tabs.addTab("Settings", juce::Colours::transparentBlack, &settings, false);
    addAndMakeVisible(tabs);
    
    // Setup ChatBar callbacks
    chatBar.onLoadingStateChanged = [this](bool show) {
        showLoadingScreen(show);
    };


    settings.onPathChanged = [this](const juce::String& newPath) {
        DBG("onPathChanged triggered with path: " + newPath);
        // TODO: Apply path to internal synthesizer instead of Serum
        };

    settings.onSkinChanged = [this](bool isHackerSkin) {
        chatBar.setHackerSkin(isHackerSkin);
        };

    audioProcessor.onPresetApplied = [this]() {
        tabs.setCurrentTabIndex(1);
        };

    audioProcessor.onPresetChanged = [this]() {
        // Update all GUI controls to reflect the loaded preset
        synthesizer.updateAllGuiControls();
        };

    auto initialPath = settings.loadSavedPath();
    settings.updatePathDisplay(initialPath);
    // TODO: Initialize internal synthesizer with path
}

SummonerXSerum2AudioProcessorEditor::~SummonerXSerum2AudioProcessorEditor()
{
    stopTimer();
}

void SummonerXSerum2AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    if (isLoading)
    {
        //DBG("Painting with isLoading=true, calling showLoadingScreen");
        loadingManager->showLoadingScreen(true);
        repaint();
    }
}

void SummonerXSerum2AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    tabs.setBounds(bounds);
}

void SummonerXSerum2AudioProcessorEditor::loadPluginFromSettings(const juce::String& path)
{
    juce::File pluginFile(path);
    if (pluginFile.existsAsFile())
    {
        // TODO: Load internal synthesizer instead of Serum
        DBG("Loaded plugin from: " + path);
    }
    else
    {
        DBG("Invalid plugin path: " + path);
    }
}

void SummonerXSerum2AudioProcessorEditor::showLoadingScreen(bool show)
{
    DBG("showLoadingScreen called with show=" + juce::String(show ? "true" : "false"));
    isLoading = show;
    if (loadingManager)
    {
        loadingManager->showLoadingScreen(show);
    }
    repaint();
}


void SummonerXSerum2AudioProcessorEditor::timerCallback()
{
    // Timer callback - not currently used
}

void SummonerXSerum2AudioProcessorEditor::fetchAndUpdateCredits(const juce::String& accessToken)
{
    // No longer needed
}

void SummonerXSerum2AudioProcessorEditor::bringPluginToFront()
{
    DBG("Attempting to bring plugin window to front");
    
    // Use a small delay to ensure processing is complete
    juce::Timer::callAfterDelay(500, [this]()
    {
        // Get the top-level component (plugin window)
        auto* topLevel = getTopLevelComponent();
        if (topLevel != nullptr)
        {
            DBG("Found top-level component, bringing to front");
            
            // Bring window to front
            topLevel->toFront(true);
            
            // Make sure it's visible
            topLevel->setVisible(true);
            
            // Try to grab keyboard focus
            topLevel->grabKeyboardFocus();
            
            // Also try to grab focus for this editor component specifically
            grabKeyboardFocus();
            
            // Force a repaint to ensure visibility
            topLevel->repaint();
            repaint();
            
#if JUCE_MAC
            // On macOS, try additional methods to bring window to front
            if (auto* peer = topLevel->getPeer())
            {
                peer->toFront(true);
            }
            
            // Try to activate the application
            juce::Process::makeForegroundProcess();
#endif

#if JUCE_WINDOWS
            // On Windows, try to restore and activate the window
            if (auto* peer = topLevel->getPeer())
            {
                peer->toFront(true);
                // Try to set foreground window
                auto windowHandle = peer->getNativeHandle();
                if (windowHandle != nullptr)
                {
                    SetForegroundWindow((HWND)windowHandle);
                    SetActiveWindow((HWND)windowHandle);
                    BringWindowToTop((HWND)windowHandle);
                }
            }
#endif
            
            DBG("Plugin window brought to front");
        }
        else
        {
            DBG("Could not find top-level component");
        }
    });
}
