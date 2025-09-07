#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"  
#include <map>
#include <string>
#include <array>

class ChatBarComponent : public juce::Component, public juce::Timer
{
public:
    ChatBarComponent(SummonerXSerum2AudioProcessor& p);
    ~ChatBarComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void timerCallback() override;
    
    // Callbacks for editor communication
    std::function<void(bool)> onLoadingStateChanged;
    
    // Skin management
    void setHackerSkin(bool enabled);

private:
    // Mystical floating boxes effect
    struct FloatingBox
    {
        float x, y;
        float size;
        float alpha;
        float targetAlpha;
        float fadeSpeed;
        juce::Colour color;
        int lifeTime;
        int maxLifeTime;
    };
    
    // Matrix raincode effect
    struct MatrixColumn
    {
        int x;
        std::vector<char> characters;
        std::vector<float> alphas;
        int length;
        float speed;
        int headPosition;
        bool active;
        int spawnDelay;
    };
    
    std::vector<FloatingBox> floatingBoxes;
    std::vector<MatrixColumn> matrixColumns;
    std::array<int, 4> quadrantCounts = {0, 0, 0, 0}; // Track boxes per quadrant
    void updateFloatingBoxes();
    void createRandomBox();
    int getQuadrant(float x, float y);
    void updateMatrixEffect();
    void initializeMatrixColumns();
    char getRandomMatrixCharacter();
    bool isHackerSkin = true;
    juce::Random random;
    // All modal functionality removed
    
    void sendPromptToGenerateParameters(const juce::String& userPrompt);
    void sendAIResponseToProcessor(const std::map<std::string, std::string>& aiResponse);

    SummonerXSerum2AudioProcessor& processor;
    juce::TextEditor chatInput;
    juce::TextButton sendButton;
    juce::ApplicationProperties appProps;
    bool requestInProgress = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatBarComponent)
};
