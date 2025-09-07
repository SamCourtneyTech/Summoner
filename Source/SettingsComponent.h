#pragma once
#include <JuceHeader.h>
#include <array>

class SummonerXSerum2AudioProcessor;

class SettingsComponent : public juce::Component, public juce::Timer
{
public:
    explicit SettingsComponent(SummonerXSerum2AudioProcessor& processor);
    ~SettingsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(bool)> onSkinChanged;
    void timerCallback() override;

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
    juce::Random random;
    juce::ApplicationProperties applicationProperties;
    // Direct API access - no authentication needed
    
    // Skin selection buttons
    juce::Label skinLabel;
    juce::TextButton defaultSkinButton;
    juce::TextButton hackerSkinButton;
    bool isHackerSkin = true;
    
    // OpenAI API Key input
    juce::Label apiKeyLabel;
    juce::TextEditor apiKeyInput;
    juce::TextButton saveApiKeyButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};