#pragma once
#include <JuceHeader.h>
#include <atomic>
#include "PluginProcessor.h"
#include "ChatBarComponent.h"
#include "SettingsComponent.h"
#include "SynthesizerComponent.h"
#include "LoadingComponent.h"

class SummonerXSerum2AudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SummonerXSerum2AudioProcessorEditor(SummonerXSerum2AudioProcessor&);
    ~SummonerXSerum2AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void showLoadingScreen(bool show);
    void timerCallback() override;
    void refreshAccessToken();

private:
    // Custom TabbedComponent to handle tab changes
    class CustomTabbedComponent : public juce::TabbedComponent
    {
    public:
        CustomTabbedComponent(juce::TabbedButtonBar::Orientation orientation)
            : juce::TabbedComponent(orientation) {}
        
        std::function<void()> onTabChanged;
        
    protected:
        void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override
        {
            juce::TabbedComponent::currentTabChanged(newCurrentTabIndex, newCurrentTabName);
            if (onTabChanged)
                onTabChanged();
        }
    };

    bool isLoading = false;
    SummonerXSerum2AudioProcessor& audioProcessor;
    CustomTabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
    ChatBarComponent chatBar;
    SynthesizerComponent synthesizer;
    SettingsComponent settings;
    std::unique_ptr<LoadingScreenManager> loadingManager;

    juce::ApplicationProperties appProps;
    
    std::atomic<bool> creditsFetchInProgress{false};

    void fetchAndUpdateCredits(const juce::String& accessToken);
    void bringPluginToFront();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SummonerXSerum2AudioProcessorEditor)
};