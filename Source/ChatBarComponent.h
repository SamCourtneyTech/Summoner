#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <map>
#include <string>

class ChatBarComponent : public juce::Component
{
public:
    ChatBarComponent(SummonerAudioProcessor& p);
    ~ChatBarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sendPromptToChatGPT(const juce::String& userPrompt,
        std::function<void(std::vector<std::map<std::string, std::string>>)> callback);

private:
    SummonerAudioProcessor& processor;
    juce::TextEditor chatInput;
    juce::TextButton sendButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatBarComponent)
};
