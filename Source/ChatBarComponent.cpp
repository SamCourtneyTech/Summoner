#include "ChatBarComponent.h"
#include <sstream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <string>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "PluginProcessor.h" 
#include "PluginEditor.h"

class ChatBarButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        auto font = juce::Font("Press Start 2P", 15.0f, juce::Font::plain);
        g.setFont(font);
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(),
            juce::Justification::centred, 1);
    }
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        juce::Colour fillColour = isButtonDown ? juce::Colours::darkblue
            : isMouseOverButton ? juce::Colours::blue
            : backgroundColour;
        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};

ChatBarComponent::ChatBarComponent(SummonerXSerum2AudioProcessor& p) : processor(p)
{
    static ChatBarButtonLookAndFeel customSummonButton;

    // Initialize appProps for accessing the access token
    juce::PropertiesFile::Options options;
    options.applicationName = "Summoner";
    options.filenameSuffix = ".settings";
    options.folderName = "SummonerApp";
    options.osxLibrarySubFolder = "Application Support";
    appProps.setStorageParameters(options);

    addAndMakeVisible(chatInput);
    addAndMakeVisible(sendButton);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::outlineColourId, juce::Colours::dimgrey);
    chatInput.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::whitesmoke);
    chatInput.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    chatInput.setBorder(juce::BorderSize<int>(2));
    sendButton.setButtonText("Summon");
    sendButton.setColour(juce::TextButton::buttonColourId, juce::Colours::black);
    sendButton.setColour(juce::TextButton::textColourOnId, juce::Colours::whitesmoke);
    sendButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    sendButton.setLookAndFeel(&customSummonButton);

    // No credits system needed

    sendButton.onClick = [this]() {
        if (requestInProgress)
        {
            DBG("Request already in progress, ignoring new request");
            return;
        }
        juce::String userInput = chatInput.getText();
        if (userInput.isEmpty())
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Input Error",
                "Please enter a prompt before summoning.");
            return;
        }

        if (onLoadingStateChanged)
        {
            onLoadingStateChanged(true);
        }

        sendPromptToGenerateParameters(userInput);
        };

    // Enable Enter key to trigger summon button
    chatInput.onReturnKey = [this]() {
        sendButton.onClick();
    };

    // Initialize mystical floating boxes effect
    floatingBoxes.reserve(40); // Reserve space for up to 40 boxes
    
    // Initialize matrix effect
    initializeMatrixColumns();
    
    startTimer(50); // 50ms timer for smooth animation (20 FPS)
}

ChatBarComponent::~ChatBarComponent()
{
    stopTimer();
    sendButton.setLookAndFeel(nullptr);
}

void ChatBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    if (isHackerSkin)
    {
        // Draw Matrix raincode effect
        g.setFont(juce::Font("Courier New", 12.0f, juce::Font::plain));
        
        for (const auto& column : matrixColumns)
        {
            if (column.active)
            {
                for (int i = 0; i < column.characters.size(); ++i)
                {
                    if (column.alphas[i] > 0.0f)
                    {
                        float brightness = column.alphas[i];
                        juce::Colour color = juce::Colour::fromRGB(
                            (juce::uint8)(brightness * 0),     // No red
                            (juce::uint8)(brightness * 255),   // Full green scaled by alpha
                            (juce::uint8)(brightness * 0)      // No blue
                        );
                        
                        g.setColour(color);
                        g.drawSingleLineText(
                            juce::String::charToString(column.characters[i]),
                            column.x,
                            i * 15 + 15
                        );
                    }
                }
            }
        }
    }
    else
    {
        // Draw mystical floating boxes
        for (const auto& box : floatingBoxes)
        {
            if (box.alpha > 0.0f)
            {
                g.setColour(box.color.withAlpha(box.alpha));
                g.fillRect(box.x, box.y, box.size, box.size);
            }
        }
    }
    
    // No credits display needed
}

void ChatBarComponent::resized()
{
    auto area = getLocalBounds();
    auto chatBarHeight = 25;
    auto chatBarWidth = 600;
    auto buttonWidth = 100;
    auto yPosition = (getHeight() - chatBarHeight) / 2;
    chatInput.setBounds((getWidth() - chatBarWidth - buttonWidth - 10) / 2, yPosition + -50, chatBarWidth, chatBarHeight);
    sendButton.setBounds(chatInput.getRight() + 10, yPosition - 50, buttonWidth, chatBarHeight);
    // No credits label to position
    
    // Reinitialize Matrix columns when component is resized (if hacker skin is active)
    if (isHackerSkin)
    {
        initializeMatrixColumns();
    }
}

void ChatBarComponent::sendPromptToGenerateParameters(const juce::String& userPrompt)
{
    requestInProgress = true;

    std::thread([this, userPrompt]() {
        // Direct API call - no authentication system needed beyond API key

        // Use OpenAI API directly
        juce::URL endpoint("https://api.openai.com/v1/chat/completions");

        // Create OpenAI API request body
        juce::DynamicObject::Ptr requestBody = new juce::DynamicObject();
        requestBody->setProperty("model", "gpt-3.5-turbo");
        requestBody->setProperty("max_tokens", 4096);
        requestBody->setProperty("temperature", 0.7);
        
        // Create messages array
        juce::Array<juce::var> messages;
        juce::DynamicObject::Ptr systemMessage = new juce::DynamicObject();
        systemMessage->setProperty("role", "system");
        systemMessage->setProperty("content", """Interpret the user's request with creativity within the specified ranges and default values, leveraging sound design knowledge to produce engaging and innovative soundscapes using the provided parameters of a Synth VST. While every response should include all 117 parameters in order formatted as {{\"Parameter Name\", \"Value\"}} in a consistent list, allow for variations that reflect musicality and style. The user's request will be inputted at the bottom of this prompt. Follow these guidelines: Use the full spectrum of provided values and descriptions to address specific or abstract prompts (e.g., \"bright and plucky\", \"deep and textured\") while staying within bounds. Be imaginative in assigning values to create sound textures that meet the user's description, but adhere strictly to parameter names and ensure all 117 parameters are included every time. Return the parameters in the format {{\"Parameter Name\", \"Value\"}}, even if a parameter's default value remains unchanged. Here are the 117 parameters and their default values: [{\"masterVolume\", \"3.0\"}, {\"osc1Detune\", \"0.0 cents\"}, {\"osc1StereoWidth\", \"0.5\"}, {\"osc1Pan\", \"0.0\"}, {\"osc1Phase\", \"0.0 degrees\"}, {\"osc1Attack\", \"0.1 s\"}, {\"osc1Decay\", \"0.2 s\"}, {\"osc1Sustain\", \"0.7\"}, {\"osc1Release\", \"0.3 s\"}, {\"osc1Type\", \"Saw\"}, {\"osc1PulseWidth\", \"0.5\"}, {\"osc1Octave\", \"0 Oct\"}, {\"osc1Semitone\", \"0 semitones\"}, {\"osc1FineTune\", \"0 cents\"}, {\"osc1RandomPhase\", \"on\"}, {\"osc1VoiceCount\", \"1\"}, {\"osc1Volume\", \"0.5\"}, {\"osc2Enabled\", \"on\"},{\"osc2Type\", \"Saw\"}, {\"osc2Volume\", \"0.0\"}, {\"osc2Detune\", \"0.0 cents\"}, {\"osc2Stereo\", \"0.5\"}, {\"osc2Pan\", \"0.0\"}, {\"osc2Octave\", \"0 Oct\"}, {\"osc2Semitone\", \"0 semitones\"}, {\"osc2FineTune\", \"0 cents\"}, {\"osc2RandomPhase\", \"on\"}, {\"osc2Phase\", \"0.0 degrees\"}, {\"osc2Attack\", \"0.1 s\"}, {\"osc2Decay\", \"0.2 s\"}, {\"osc2Sustain\", \"0.7\"}, {\"osc2Release\", \"0.3 s\"}, {\"osc2VoiceCount\", \"1\"} {\"filterCutoff\", \"1000 Hz\"}, {\"filterResonance\", \"0.0\"}, {\"osc1FilterEnabled\", \"off\"}, {\"osc2FilterEnabled\", \"off\"}, {\"filterLPEnabled\", \"on\"}, {\"filterHPEnabled\", \"off\"}, {\"filterBPEnabled\", \"off\"}, {\"filterNotchEnabled\", \"off\"}, {\"filterCombEnabled\", \"off\"}, {\"filterFormantEnabled\", \"off\"}, {\"filter12dBEnabled\", \"on\"}, {\"filter24dBEnabled\", \"off\"}, {\"chorusEnabled\", \"off\"}, {\"chorusRate\", \"2.0 Hz\"}, {\"chorusDelay1\", \"20.0 ms\"}, {\"chorusDelay2\", \"30.0 ms\"}, {\"chorusDepth\", \"5.0\"}, {\"chorusFeedback\", \"0.2\"}, {\"chorusLPF\", \"20000 Hz\"}, {\"chorusMix\", \"0.5\"}, {\"flangerEnabled\", \"off\"}, {\"flangerRate\", \"1.0 Hz\"}, {\"flangerDepth\", \"2.0\"}, {\"flangerFeedback\", \"25.0\"}, {\"flangerMix\", \"50.0\"}, {\"flangerPhase\", \"0.0 degrees\"}, {\"phaserEnabled\", \"off\"}, {\"phaserRate\", \"1.0 Hz\"}, {\"phaserDepth1\", \"50.0\"}, {\"phaserDepth2\", \"30.0\"}, {\"phaserFeedback\", \"25.0\"}, {\"phaserMix\", \"50.0\"}, {\"phaserPhase\", \"0.0 degrees\"}, {\"phaserFrequency\", \"500 Hz\"}, {\"phaserPoles\", \"4\"}, {\"compressorEnabled\", \"off\"}, {\"compressorThreshold\", \"-20.0 dB\"}, {\"compressorRatio\", \"4.0\"}, {\"compressorAttack\", \"5.0 ms\"}, {\"compressorRelease\", \"100.0 ms\"}, {\"compressorGain\", \"0.0 dB\"}, {\"compressorMix\", \"1.0\"}, {\"compressorMultiband\", \"off\"}, {\"distortionEnabled\", \"off\"}, {\"distortionType\", \"Tube\"}, {\"distortionDrive\", \"50.0\"}, {\"distortionMix\", \"1.0\"}, {\"distortionFilterPosition\", \"Off\"}, {\"distortionFilterType\", \"Low Pass\"}, {\"distortionFilterFreq\", \"1000 Hz\"}, {\"distortionFilterQ\", \"0.707\"}, {\"delayEnabled\", \"off\"}, {\"delayFeedback\", \"0.3\"}, {\"delayMix\", \"0.3\"}, {\"delayPingPong\", \"off\"}, {\"delayLeftTime\", \"250.0 ms\"}, {\"delayRightTime\", \"250.0 ms\"}, {\"delaySync\", \"off\"}, {\"delayTriplet\", \"off\"}, {\"delayDotted\", \"off\"}, {\"delayRTriplet\", \"off\"}, {\"delayRDotted\", \"off\"}, {\"delayFilterFreq\", \"8000 Hz\"}, {\"delayFilterQ\", \"0.707\"}, {\"reverbEnabled\", \"off\"}, {\"reverbMix\", \"0.3\"}, {\"reverbType\", \"Hall\"}, {\"reverbLowCut\", \"80 Hz\"}, {\"reverbHighCut\", \"8000 Hz\"}, {\"reverbSize\", \"0.5\"}, {\"reverbPreDelay\", \"20.0 ms\"}, {\"reverbDamping\", \"0.5\"}, {\"reverbWidth\", \"1.0\"}, {\"eqEnabled\", \"off\"}, {\"eq1Enabled\", \"on\"}, {\"eq1Frequency\", \"400 Hz\"}, {\"eq1Q\", \"1.0\"}, {\"eq1Gain\", \"0.0 dB\"}, {\"eq1Type\", \"Peak\"}, {\"eq2Enabled\", \"on\"}, {\"eq2Frequency\", \"4000 Hz\"}, {\"eq2Q\", \"1.0\"}, {\"eq2Gain\", \"0.0 dB\"}, {\"eq2Type\", \"Peak\"}]] Here are those 117 parameter's respective ranges that you can choose from: [{\"masterVolume\", \"0.0 - 5.0\"}, {\"osc1Detune\", \"0.0 - 100.0 cents\"}, {\"osc1StereoWidth\", \"0.0 - 1.0\"}, {\"osc1Pan\", \"-1.0 - 1.0\"}, {\"osc1Phase\", \"0.0 - 360.0 degrees\"}, {\"osc1Attack\", \"0.0 s - 10.0 s\"}, {\"osc1Decay\", \"0.0 s - 10.0 s\"}, {\"osc1Sustain\", \"0.0 - 1.0\"}, {\"osc1Release\", \"0.0 s - 10.0 s\"}, {\"osc1Type\", \"Sine, Saw, Square, Triangle, White Noise, Pink Noise\"}, {\"osc1PulseWidth\", \"0.0 - 1.0\"}, {\"osc1Octave\", \"-4 Oct - +4 Oct\"}, {\"osc1Semitone\", \"-12 semitones - +12 semitones\"}, {\"osc1FineTune\", \"-100 cents - +100 cents\"}, {\"osc1RandomPhase\", \"off, on\"}, {\"osc1VoiceCount\", \"1 - 16\"}, {\"osc1Volume\", \"0.0 - 1.0\"}, {\"osc2Enabled\", \"off, on\"}, {\"osc2Type\", \"Sine, Saw, Square, Triangle, White Noise, Pink Noise\"}, {\"osc2Volume\", \"0.0 - 1.0\"}, {\"osc2Detune\", \"0.0 - 100.0 cents\"}, {\"osc2Stereo\", \"0.0 - 1.0\"}, {\"osc2Pan\", \"-1.0 - 1.0\"}, {\"osc2Octave\", \"-4 Oct - +4 Oct\"}, {\"osc2Semitone\", \"-12 semitones - +12 semitones\"}, {\"osc2FineTune\", \"-100 cents - +100 cents\"}, {\"osc2RandomPhase\", \"off, on\"}, {\"osc2Phase\", \"0.0 - 360.0 degrees\"}, {\"osc2Attack\", \"0.0 s - 10.0 s\"}, {\"osc2Decay\", \"0.0 s - 10.0 s\"}, {\"osc2Sustain\", \"0.0 - 1.0\"}, {\"osc2Release\", \"0.0 s - 10.0 s\"}, {\"osc2VoiceCount\", \"1 - 16\"}, {\"filterCutoff\", \"20 Hz - 20000 Hz\"}, {\"filterResonance\", \"0.0 - 1.0\"}, {\"osc1FilterEnabled\", \"off, on\"}, {\"osc2FilterEnabled\", \"off, on\"}, {\"filterLPEnabled\", \"off, on\"}, {\"filterHPEnabled\", \"off, on\"},  {\"filterBPEnabled\", \"off, on\"}, {\"filterNotchEnabled\", \"off, on\"}, {\"filterCombEnabled\", \"off, on\"}, {\"filterFormantEnabled\", \"off, on\"}, {\"filter12dBEnabled\", \"off, on\"}, {\"filter24dBEnabled\", \"off, on\"}, {\"chorusEnabled\", \"off, on\"}, {\"chorusRate\", \"0.1 Hz - 10.0 Hz\"}, {\"chorusDelay1\", \"1.0 ms - 50.0 ms\"}, {\"chorusDelay2\", \"1.0 ms - 50.0 ms\"}, {\"chorusDepth\", \"0.0 - 20.0\"}, {\"chorusFeedback\", \"0.0 - 0.95\"}, {\"chorusLPF\", \"200 Hz - 20000 Hz\"}, {\"chorusMix\", \"0.0 - 1.0\"}, {\"flangerEnabled\", \"off, on\"}, {\"flangerRate\", \"0.1 Hz - 10.0 Hz\"}, {\"flangerDepth\", \"0.1 - 10.0\"}, {\"flangerFeedback\", \"0.0 - 100.0\"}, {\"flangerMix\", \"0.0 - 100.0\"}, {\"flangerPhase\", \"0.0 - 360.0 degrees\"}, {\"phaserEnabled\", \"off, on\"}, {\"phaserRate\", \"0.1 Hz - 10.0 Hz\"}, {\"phaserDepth1\", \"0.0 - 100.0\"}, {\"phaserDepth2\", \"0.0 - 100.0\"}, {\"phaserFeedback\", \"0.0 - 100.0\"}, {\"phaserMix\", \"0.0 - 100.0\"}, {\"phaserPhase\", \"0.0 - 360.0 degrees\"}, {\"phaserFrequency\", \"20 Hz - 2000 Hz\"}, {\"phaserPoles\", \"1 - 16\"}, {\"compressorEnabled\", \"off, on\"}, {\"compressorThreshold\", \"-60.0 dB - 0.0 dB\"}, {\"compressorRatio\", \"1.0 - 20.0\"}, {\"compressorAttack\", \"0.1 ms - 100.0 ms\"}, {\"compressorRelease\", \"10.0 ms - 1000.0 ms\"}, {\"compressorGain\", \"0.0 dB - 30.0 dB\"}, {\"compressorMix\", \"0.0 - 1.0\"}, {\"compressorMultiband\", \"off, on\"}, {\"distortionEnabled\", \"off, on\"}, {\"distortionType\", \"Tube, SoftClip, HardClip, Diode 1, Diode 2, Linear Fold, Sine Fold, Zero-Square, Downsample, Asymmetric, Rectify, Sine Shaper, Stomp Box, Tape Sat, Overdrive, Soft Sat\"}, {\"distortionDrive\", \"0.0 - 100.0\"},  {\"distortionMix\", \"0.0 - 1.0\"}, {\"distortionFilterPosition\", \"Off, Pre, Post\"}, {\"distortionFilterType\", \"Low Pass, High Pass, Band Pass\"}, {\"distortionFilterFreq\", \"20 Hz - 20000 Hz\"}, {\"distortionFilterQ\", \"0.1 - 30.0\"}, {\"delayEnabled\", \"off, on\"}, {\"delayFeedback\", \"0.0 - 0.95\"}, {\"delayMix\", \"0.0 - 1.0\"}, {\"delayPingPong\", \"off, on\"}, {\"delayLeftTime\", \"1.0 ms - 2000.0 ms\"}, {\"delayRightTime\", \"1.0 ms - 2000.0 ms\"}, {\"delaySync\", \"off, on\"}, {\"delayTriplet\", \"off, on\"}, {\"delayDotted\", \"off, on\"}, {\"delayRTriplet\", \"off, on\"}, {\"delayRDotted\", \"off, on\"}, {\"delayFilterFreq\", \"20 Hz - 20000 Hz\"}, {\"delayFilterQ\", \"0.1 - 30.0\"}, {\"reverbEnabled\", \"off, on\"}, {\"reverbMix\", \"0.0 - 100.0\"}, {\"reverbType\", \"Plate, Hall, Vintage, Room, Ambience\"}, {\"reverbLowCut\", \"20 Hz - 1000 Hz\"}, {\"reverbHighCut\", \"1000 Hz - 20000 Hz\"}, {\"reverbSize\", \"0.0 - 100.0\"}, {\"reverbPreDelay\", \"0.0 ms - 200.0 ms\"}, {\"reverbDamping\", \"0.0 - 100.0\"}, {\"reverbWidth\", \"0.0 - 100.0\"}, {\"eqEnabled\", \"off, on\"}, {\"eq1Enabled\", \"off, on\"}, {\"eq1Frequency\", \"20 Hz - 20000 Hz\"}, {\"eq1Q\", \"0.1 - 30.0\"}, {\"eq1Gain\", \"-15.0 dB - 15.0 dB\"}, {\"eq1Type\", \"Peak, Low Shelf, High Shelf\"}, {\"eq2Enabled\", \"off, on\"}, {\"eq2Frequency\", \"20 Hz - 20000 Hz\"}, {\"eq2Q\", \"0.1 - 30.0\"}, {\"eq2Gain\", \"-15.0 dB - 15.0 dB\"}, {\"eq2Type\", \"Peak, Low Shelf, High Shelf\"}] Here are those 117 parameter's respective ranges that you can choose from: [{\"masterVolume\", \"0.0 - 5.0\"}, {\"osc1Detune\", \"0.0 - 100.0 cents\"}, {\"osc1StereoWidth\", \"0.0 - 1.0\"}, {\"osc1Pan\", \"-1.0 - 1.0\"}, {\"osc1Phase\", \"0.0 - 360.0 degrees\"}, {\"osc1Attack\", \"0.0 s - 10.0 s\"}, {\"osc1Decay\", \"0.0 s - 10.0 s\"}, {\"osc1Sustain\", \"0.0 - 1.0\"}, {\"osc1Release\", \"0.0 s - 10.0 s\"}, {\"osc1Type\", \"Sine, Saw, Square, Triangle, White Noise, Pink Noise\"}, {\"osc1PulseWidth\", \"0.0 - 1.0\"}, {\"osc1Octave\", \"-4 Oct - +4 Oct\"}, {\"osc1Semitone\", \"-12 semitones - +12 semitones\"}, {\"osc1FineTune\", \"-100 cents - +100 cents\"}, {\"osc1RandomPhase\", \"off, on\"}, {\"osc1VoiceCount\", \"1 - 16\"}, {\"osc1Volume\", \"0.0 - 1.0\"}, {\"osc2Enabled\", \"off, on\"}, {\"osc2Type\", \"Sine, Saw, Square, Triangle, White Noise, Pink Noise\"}, {\"osc2Volume\", \"0.0 - 1.0\"}, {\"osc2Detune\", \"0.0 - 100.0 cents\"}, {\"osc2Stereo\", \"0.0 - 1.0\"}, {\"osc2Pan\", \"-1.0 - 1.0\"}, {\"osc2Octave\", \"-4 Oct - +4 Oct\"}, {\"osc2Semitone\", \"-12 semitones - +12 semitones\"}, {\"osc2FineTune\", \"-100 cents - +100 cents\"}, {\"osc2RandomPhase\", \"off, on\"}, {\"osc2Phase\", \"0.0 - 360.0 degrees\"}, {\"osc2Attack\", \"0.0 s - 10.0 s\"}, {\"osc2Decay\", \"0.0 s - 10.0 s\"}, {\"osc2Sustain\", \"0.0 - 1.0\"}, {\"osc2Release\", \"0.0 s - 10.0 s\"}, {\"osc2VoiceCount\", \"1 - 16\"}, {\"filterCutoff\", \"20 Hz - 20000 Hz\"}, {\"filterResonance\", \"0.0 - 1.0\"}, {\"osc1FilterEnabled\", \"off, on\"}, {\"osc2FilterEnabled\", \"off, on\"}, {\"filterLPEnabled\", \"off, on\"}, {\"filterHPEnabled\", \"off, on\"},  {\"filterBPEnabled\", \"off, on\"}, {\"filterNotchEnabled\", \"off, on\"}, {\"filterCombEnabled\", \"off, on\"}, {\"filterFormantEnabled\", \"off, on\"}, {\"filter12dBEnabled\", \"off, on\"}, {\"filter24dBEnabled\", \"off, on\"}, {\"chorusEnabled\", \"off, on\"}, {\"chorusRate\", \"0.1 Hz - 10.0 Hz\"}, {\"chorusDelay1\", \"1.0 ms - 50.0 ms\"}, {\"chorusDelay2\", \"1.0 ms - 50.0 ms\"}, {\"chorusDepth\", \"0.0 - 20.0\"}, {\"chorusFeedback\", \"0.0 - 0.95\"}, {\"chorusLPF\", \"200 Hz - 20000 Hz\"}, {\"chorusMix\", \"0.0 - 1.0\"}, {\"flangerEnabled\", \"off, on\"}, {\"flangerRate\", \"0.1 Hz - 10.0 Hz\"}, {\"flangerDepth\", \"0.1 - 10.0\"}, {\"flangerFeedback\", \"0.0 - 100.0\"}, {\"flangerMix\", \"0.0 - 100.0\"}, {\"flangerPhase\", \"0.0 - 360.0 degrees\"}, {\"phaserEnabled\", \"off, on\"}, {\"phaserRate\", \"0.1 Hz - 10.0 Hz\"}, {\"phaserDepth1\", \"0.0 - 100.0\"}, {\"phaserDepth2\", \"0.0 - 100.0\"}, {\"phaserFeedback\", \"0.0 - 100.0\"}, {\"phaserMix\", \"0.0 - 100.0\"}, {\"phaserPhase\", \"0.0 - 360.0 degrees\"}, {\"phaserFrequency\", \"20 Hz - 2000 Hz\"}, {\"phaserPoles\", \"1 - 16\"}, {\"compressorEnabled\", \"off, on\"}, {\"compressorThreshold\", \"-60.0 dB - 0.0 dB\"}, {\"compressorRatio\", \"1.0 - 20.0\"}, {\"compressorAttack\", \"0.1 ms - 100.0 ms\"}, {\"compressorRelease\", \"10.0 ms - 1000.0 ms\"}, {\"compressorGain\", \"0.0 dB - 30.0 dB\"}, {\"compressorMix\", \"0.0 - 1.0\"}, {\"compressorMultiband\", \"off, on\"}, {\"distortionEnabled\", \"off, on\"}, {\"distortionType\", \"Tube, SoftClip, HardClip, Diode 1, Diode 2, Linear Fold, Sine Fold, Zero-Square, Downsample, Asymmetric, Rectify, Sine Shaper, Stomp Box, Tape Sat, Overdrive, Soft Sat\"}, {\"distortionDrive\", \"0.0 - 100.0\"},  {\"distortionMix\", \"0.0 - 1.0\"}, {\"distortionFilterPosition\", \"Off, Pre, Post\"}, {\"distortionFilterType\", \"Low Pass, High Pass, Band Pass\"}, {\"distortionFilterFreq\", \"20 Hz - 20000 Hz\"}, {\"distortionFilterQ\", \"0.1 - 30.0\"}, {\"delayEnabled\", \"off, on\"}, {\"delayFeedback\", \"0.0 - 0.95\"}, {\"delayMix\", \"0.0 - 1.0\"}, {\"delayPingPong\", \"off, on\"}, {\"delayLeftTime\", \"1.0 ms - 2000.0 ms\"}, {\"delayRightTime\", \"1.0 ms - 2000.0 ms\"}, {\"delaySync\", \"off, on\"}, {\"delayTriplet\", \"off, on\"}, {\"delayDotted\", \"off, on\"}, {\"delayRTriplet\", \"off, on\"}, {\"delayRDotted\", \"off, on\"}, {\"delayFilterFreq\", \"20 Hz - 20000 Hz\"}, {\"delayFilterQ\", \"0.1 - 30.0\"}, {\"reverbEnabled\", \"off, on\"}, {\"reverbMix\", \"0.0 - 100.0\"}, {\"reverbType\", \"Plate, Hall, Vintage, Room, Ambience\"}, {\"reverbLowCut\", \"20 Hz - 1000 Hz\"}, {\"reverbHighCut\", \"1000 Hz - 20000 Hz\"}, {\"reverbSize\", \"0.0 - 100.0\"}, {\"reverbPreDelay\", \"0.0 ms - 200.0 ms\"}, {\"reverbDamping\", \"0.0 - 100.0\"}, {\"reverbWidth\", \"0.0 - 100.0\"}, {\"eqEnabled\", \"off, on\"}, {\"eq1Enabled\", \"off, on\"}, {\"eq1Frequency\", \"20 Hz - 20000 Hz\"}, {\"eq1Q\", \"0.1 - 30.0\"}, {\"eq1Gain\", \"-15.0 dB - 15.0 dB\"}, {\"eq1Type\", \"Peak, Low Shelf, High Shelf\"}, {\"eq2Enabled\", \"off, on\"}, {\"eq2Frequency\", \"20 Hz - 20000 Hz\"}, {\"eq2Q\", \"0.1 - 30.0\"}, {\"eq2Gain\", \"-15.0 dB - 15.0 dB\"}, {\"eq2Type\", \"Peak, Low Shelf, High Shelf\"}] To facilitate accurate parsing and handling of your requests, please provide parameter adjustments in JSON format when possible. This ensures the correct interpretation and application of your specifications for this C++ program. Don't add newline characters. Even if the request doesn't seem to be related to the sound designing task, respond with the list: DO NOT RESPOND WITH ANYTHING BUT THE LIST! User input: {user_input.input}. Return as JSON.""");
        messages.add(juce::var(systemMessage.get()));
        
        juce::DynamicObject::Ptr userMessage = new juce::DynamicObject();
        userMessage->setProperty("role", "user");
        userMessage->setProperty("content", userPrompt);
        messages.add(juce::var(userMessage.get()));
        
        requestBody->setProperty("messages", messages);
        
        juce::String postData = juce::JSON::toString(requestBody.get());
        DBG("Sending POST request to OpenAI API with data: " + postData);

        juce::URL urlWithPostData = endpoint.withPOSTData(postData);

        // Get OpenAI API key from settings (you'll need to add this)
        juce::String apiKey = appProps.getUserSettings()->getValue("openai_api_key", "");
        if (apiKey.isEmpty())
        {
            juce::MessageManager::callAsync([this]() {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "API Key Missing",
                    "Please set your OpenAI API key in Settings.");
                if (onLoadingStateChanged)
                {
                    onLoadingStateChanged(false);
                }
                requestInProgress = false;
            });
            return;
        }

        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withExtraHeaders("Content-Type: application/json\nAuthorization: Bearer " + apiKey)
            .withConnectionTimeoutMs(50000);

        std::unique_ptr<juce::InputStream> stream(urlWithPostData.createInputStream(options));

        if (stream != nullptr)
        {
            juce::String response = stream->readEntireStreamAsString();
            DBG("Raw response from OpenAI API: " + response);
            DBG("Response length: " + juce::String(response.length()) + " characters");
            juce::var result = juce::JSON::parse(response);

            if (result.isObject())
            {
                auto* obj = result.getDynamicObject();
                
                // Check for OpenAI API error response
                if (obj->hasProperty("error"))
                {
                    auto errorObj = obj->getProperty("error").getDynamicObject();
                    juce::String errorMessage = errorObj->getProperty("message").toString();
                    
                    juce::MessageManager::callAsync([this, errorMessage]() {
                        DBG("OpenAI API error: " + errorMessage);
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "API Error",
                            "OpenAI API Error: " + errorMessage);
                        if (onLoadingStateChanged)
                        {
                            onLoadingStateChanged(false);
                        }
                        requestInProgress = false;
                    });
                    return;
                }
                
                // Parse OpenAI response format
                if (obj->hasProperty("choices"))
                {
                    auto choicesArray = obj->getProperty("choices").getArray();
                    if (choicesArray != nullptr && choicesArray->size() > 0)
                    {
                        auto firstChoice = choicesArray->getUnchecked(0).getDynamicObject();
                        auto message = firstChoice->getProperty("message").getDynamicObject();
                        juce::String content = message->getProperty("content").toString();
                        
                        DBG("OpenAI response content: " + content);
                        DBG("Content length: " + juce::String(content.length()) + " characters");
                        
                        // Fix JSON format - convert {"key", "value"} to {"key": "value"}
                        juce::String fixedContent = content;
                        
                        // More robust replacement for the comma issue
                        // Look for pattern: "sometext", "sometext" and replace with "sometext": "sometext"
                        juce::String result;
                        bool inQuotes = false;
                        bool foundFirstQuote = false;
                        int quoteCount = 0;
                        
                        for (int i = 0; i < fixedContent.length(); ++i)
                        {
                            juce::juce_wchar c = fixedContent[i];
                            
                            if (c == '"')
                            {
                                quoteCount++;
                                if (quoteCount % 2 == 1)
                                    inQuotes = true;
                                else
                                    inQuotes = false;
                            }
                            
                            // If we find a comma between quotes pairs, and the next non-space char is a quote
                            if (c == ',' && !inQuotes && quoteCount % 2 == 0)
                            {
                                // Look ahead to see if next quote starts a value
                                int nextQuotePos = i + 1;
                                while (nextQuotePos < fixedContent.length() && 
                                       (fixedContent[nextQuotePos] == ' ' || fixedContent[nextQuotePos] == '\t'))
                                    nextQuotePos++;
                                
                                if (nextQuotePos < fixedContent.length() && fixedContent[nextQuotePos] == '"')
                                {
                                    result += ':';  // Replace comma with colon
                                }
                                else
                                {
                                    result += c;    // Keep normal comma
                                }
                            }
                            else
                            {
                                result += c;
                            }
                        }
                        
                        DBG("Fixed JSON content: " + result);
                        
                        // Parse the JSON content from the AI response
                        juce::var parametersResult = juce::JSON::parse(result);
                        DBG("Parsed result - isArray: " + juce::String(parametersResult.isArray() ? "true" : "false") + 
                            ", isObject: " + juce::String(parametersResult.isObject() ? "true" : "false") + 
                            ", isVoid: " + juce::String(parametersResult.isVoid() ? "true" : "false"));
                        std::map<std::string, std::string> parameterMap;
                        
                        if (parametersResult.isArray())
                        {
                            // Handle array format: [{"key": "value"}, {"key2": "value2"}]
                            auto* parametersArray = parametersResult.getArray();
                            for (int i = 0; i < parametersArray->size(); ++i)
                            {
                                auto arrayItem = parametersArray->getUnchecked(i);
                                if (arrayItem.isObject())
                                {
                                    auto* itemObj = arrayItem.getDynamicObject();
                                    for (const auto& property : itemObj->getProperties())
                                    {
                                        juce::String key = property.name.toString();
                                        juce::String value = property.value.toString();
                                        parameterMap[key.toStdString()] = value.toStdString();
                                    }
                                }
                            }
                        }
                        else if (parametersResult.isObject())
                        {
                            // Handle object format: {"key": "value", "key2": "value2"}
                            auto* parametersObj = parametersResult.getDynamicObject();
                            
                            for (const auto& property : parametersObj->getProperties())
                            {
                                juce::String key = property.name.toString();
                                juce::String value = property.value.toString();
                                parameterMap[key.toStdString()] = value.toStdString();
                            }
                        }
                        
                        if (!parameterMap.empty())
                        {
                            juce::MessageManager::callAsync([this, parameterMap]() {
                                // Apply the AI response parameters to the synthesizer
                                sendAIResponseToProcessor(parameterMap);

                                // No credit system needed for direct OpenAI API calls
                                if (onLoadingStateChanged)
                                {
                                    onLoadingStateChanged(false);
                                }
                                requestInProgress = false;
                            });
                        }
                        else
                        {
                            juce::MessageManager::callAsync([this, content]() {
                                DBG("Failed to parse parameters JSON from OpenAI response: " + content);
                                juce::AlertWindow::showMessageBoxAsync(
                                    juce::AlertWindow::WarningIcon,
                                    "Response Parse Error",
                                    "Could not parse synthesizer parameters from AI response.");
                                if (onLoadingStateChanged)
                                {
                                    onLoadingStateChanged(false);
                                }
                                requestInProgress = false;
                            });
                        }
                    }
                    else
                    {
                        juce::MessageManager::callAsync([this]() {
                            DBG("No response choices found in OpenAI response");
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "Response Error",
                                "Invalid response format from OpenAI API.");
                            if (onLoadingStateChanged)
                            {
                                onLoadingStateChanged(false);
                            }
                            requestInProgress = false;
                        });
                    }
                }
                else
                {
                    juce::MessageManager::callAsync([this]() {
                        DBG("No choices found in OpenAI response");
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Response Error",
                            "No response choices found in OpenAI API response.");
                        if (onLoadingStateChanged)
                        {
                            onLoadingStateChanged(false);
                        }
                        requestInProgress = false;
                    });
                }
            }
            else
            {
                juce::MessageManager::callAsync([this, response]() {
                    DBG("Failed to parse OpenAI response: " + response);
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Error",
                        "Failed to parse OpenAI API response: " + response);
                    if (onLoadingStateChanged)
                    {
                        onLoadingStateChanged(false);
                    }
                    requestInProgress = false;
                });
            }
        }
        else
        {
            juce::MessageManager::callAsync([this]() {
                DBG("Failed to connect to OpenAI API");
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Connection Error",
                    "Failed to connect to OpenAI API. Please check your internet connection.");
                if (onLoadingStateChanged)
                {
                    onLoadingStateChanged(false);
                }
                requestInProgress = false;
            });
        }
        }).detach();
}

void ChatBarComponent::sendAIResponseToProcessor(const std::map<std::string, std::string>& aiResponse)
{
    DBG("ChatBarComponent: Sending AI response to processor with " << aiResponse.size() << " parameters");
    
    // Apply the parameters directly and get feedback
    auto [successful, failed] = processor.applyResponseParameters(aiResponse);
    
    // Also store it in the processor's response system for navigation (next/prev)
    std::vector<std::map<std::string, std::string>> responses = { aiResponse };
    processor.setResponses(responses);
    
    DBG("ChatBarComponent: Parameter application complete - " << successful << " successful, " << failed << " failed");
    
    // Parameter application complete - no callback needed
    
    // Show alert if there were failures
    if (failed > 0 && successful == 0)
    {
        juce::MessageManager::callAsync([this, failed]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Parameter Application Failed",
                "Failed to apply " + juce::String(failed) + " parameters. Check console for details.");
        });
    }
    else if (failed > 0)
    {
        juce::MessageManager::callAsync([this, successful, failed]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Partial Parameter Application",
                "Applied " + juce::String(successful) + " parameters successfully, but " + 
                juce::String(failed) + " failed. Check console for details.");
        });
    }
}

// No credits system needed

// No credit system needed for direct API calls

void ChatBarComponent::mouseDown(const juce::MouseEvent& event)
{
    // No credits functionality needed
}

void ChatBarComponent::mouseEnter(const juce::MouseEvent& event)
{
    // No credits functionality needed
}

void ChatBarComponent::mouseExit(const juce::MouseEvent& event)
{
    // No credits functionality needed
}

// No credits modal needed

// No credits modal refresh needed

// All credits modal code removed

void ChatBarComponent::timerCallback()
{
    if (isHackerSkin)
    {
        updateMatrixEffect();
    }
    else
    {
        updateFloatingBoxes();
        
        // Randomly create new boxes
        if (random.nextFloat() < 0.12f && floatingBoxes.size() < 35) // 12% chance per frame, max 35 boxes
        {
            createRandomBox();
        }
    }
    
    repaint();
}

void ChatBarComponent::updateFloatingBoxes()
{
    for (auto it = floatingBoxes.begin(); it != floatingBoxes.end();)
    {
        auto& box = *it;
        
        // Update lifetime
        box.lifeTime++;
        
        // Update alpha towards target
        float alphaDiff = box.targetAlpha - box.alpha;
        box.alpha += alphaDiff * box.fadeSpeed;
        
        // Change target alpha based on lifecycle
        if (box.lifeTime < box.maxLifeTime * 0.3f)
        {
            // Fade in phase
            box.targetAlpha = 0.6f;
        }
        else if (box.lifeTime > box.maxLifeTime * 0.7f)
        {
            // Fade out phase
            box.targetAlpha = 0.0f;
        }
        else
        {
            // Stable phase with gentle pulsing
            box.targetAlpha = 0.4f + 0.2f * std::sin(box.lifeTime * 0.1f);
        }
        
        // Remove boxes that have faded out and exceeded their lifetime
        if (box.alpha < 0.01f && box.lifeTime > box.maxLifeTime)
        {
            // Update quadrant count before removing
            int quadrant = getQuadrant(box.x, box.y);
            quadrantCounts[quadrant]--;
            it = floatingBoxes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ChatBarComponent::createRandomBox()
{
    FloatingBox box;
    
    // Find the least populated quadrant for balanced distribution
    int minQuadrant = 0;
    int minCount = quadrantCounts[0];
    for (int i = 1; i < 4; ++i)
    {
        if (quadrantCounts[i] < minCount)
        {
            minCount = quadrantCounts[i];
            minQuadrant = i;
        }
    }
    
    // Generate position in the chosen quadrant
    float halfWidth = getWidth() * 0.5f;
    float halfHeight = getHeight() * 0.5f;
    
    switch (minQuadrant)
    {
        case 0: // Top-left
            box.x = random.nextFloat() * halfWidth;
            box.y = random.nextFloat() * halfHeight;
            break;
        case 1: // Top-right
            box.x = halfWidth + random.nextFloat() * halfWidth;
            box.y = random.nextFloat() * halfHeight;
            break;
        case 2: // Bottom-left
            box.x = random.nextFloat() * halfWidth;
            box.y = halfHeight + random.nextFloat() * halfHeight;
            break;
        case 3: // Bottom-right
            box.x = halfWidth + random.nextFloat() * halfWidth;
            box.y = halfHeight + random.nextFloat() * halfHeight;
            break;
    }
    
    // Random size between 3-12 pixels
    box.size = 3.0f + random.nextFloat() * 9.0f;
    
    // Start invisible
    box.alpha = 0.0f;
    box.targetAlpha = 0.0f;
    
    // Random fade speed
    box.fadeSpeed = 0.02f + random.nextFloat() * 0.08f;
    
    // Random mystical colors (blues, purples, whites)
    float colorChoice = random.nextFloat();
    if (colorChoice < 0.4f)
    {
        // Blue tones
        box.color = juce::Colour::fromRGB(100 + random.nextInt(100), 150 + random.nextInt(100), 255);
    }
    else if (colorChoice < 0.7f)
    {
        // Purple tones
        box.color = juce::Colour::fromRGB(150 + random.nextInt(100), 100 + random.nextInt(100), 255);
    }
    else
    {
        // White/silver tones
        int brightness = 200 + random.nextInt(55);
        box.color = juce::Colour::fromRGB(brightness, brightness, brightness);
    }
    
    // Random lifetime (2-8 seconds at 20 FPS)
    box.maxLifeTime = 40 + random.nextInt(120); // 40-160 frames
    box.lifeTime = 0;
    
    // Update quadrant count
    int quadrant = getQuadrant(box.x, box.y);
    quadrantCounts[quadrant]++;
    
    floatingBoxes.push_back(box);
}

int ChatBarComponent::getQuadrant(float x, float y)
{
    float halfWidth = getWidth() * 0.5f;
    float halfHeight = getHeight() * 0.5f;
    
    if (x < halfWidth && y < halfHeight)
        return 0; // Top-left
    else if (x >= halfWidth && y < halfHeight)
        return 1; // Top-right
    else if (x < halfWidth && y >= halfHeight)
        return 2; // Bottom-left
    else
        return 3; // Bottom-right
}

void ChatBarComponent::setHackerSkin(bool enabled)
{
    isHackerSkin = enabled;
    if (enabled)
    {
        initializeMatrixColumns(); // Reinitialize columns when hacker skin is activated
    }
    repaint();
}

void ChatBarComponent::initializeMatrixColumns()
{
    int columnSpacing = 12; // 12 pixels apart
    int screenWidth = getWidth();
    if (screenWidth <= 0) screenWidth = 800; // Fallback width
    int numColumns = screenWidth / columnSpacing; // Calculate columns based on screen width
    
    matrixColumns.clear();
    matrixColumns.reserve(numColumns);
    
    for (int i = 0; i < numColumns; ++i)
    {
        MatrixColumn column;
        column.x = i * columnSpacing;
        column.length = random.nextInt(15) + 10; // 10-25 characters long
        column.speed = random.nextFloat() * 2.0f + 1.0f; // Speed between 1-3
        column.headPosition = -column.length; // Start off screen
        column.active = false;
        column.spawnDelay = random.nextInt(200); // Random spawn delay
        
        // Initialize character and alpha vectors
        column.characters.resize(50); // Max height in characters
        column.alphas.resize(50);
        
        // Fill with random characters
        for (int j = 0; j < column.characters.size(); ++j)
        {
            column.characters[j] = getRandomMatrixCharacter();
            column.alphas[j] = 0.0f;
        }
        
        matrixColumns.push_back(column);
    }
}

char ChatBarComponent::getRandomMatrixCharacter()
{
    // Mix of numbers, letters, and some special characters for Matrix effect
    static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
    return chars[random.nextInt(sizeof(chars) - 1)];
}

void ChatBarComponent::updateMatrixEffect()
{
    int maxHeight = getHeight() / 15; // Screen height in character rows
    
    for (auto& column : matrixColumns)
    {
        // Handle spawn delay
        if (!column.active && column.spawnDelay > 0)
        {
            column.spawnDelay--;
            continue;
        }
        
        if (!column.active)
        {
            column.active = true;
            column.headPosition = 0;
        }
        
        // Move the column down
        column.headPosition += column.speed;
        
        // Update alphas for trailing effect
        for (int i = 0; i < column.alphas.size() && i < maxHeight; ++i)
        {
            // Distance from head
            int distanceFromHead = i - (int)column.headPosition;
            
            if (distanceFromHead >= 0 && distanceFromHead < column.length)
            {
                // Character is part of the active trail
                if (distanceFromHead == 0)
                {
                    // Head character - brightest
                    column.alphas[i] = 1.0f;
                }
                else
                {
                    // Trailing characters - fade based on distance from head
                    float fadeRatio = 1.0f - (float)distanceFromHead / (float)column.length;
                    column.alphas[i] = fadeRatio * 0.8f; // Max 80% brightness for trail
                }
                
                // Occasionally change characters for dynamic effect
                if (random.nextFloat() < 0.05f) // 5% chance per frame
                {
                    column.characters[i] = getRandomMatrixCharacter();
                }
            }
            else
            {
                // Fade out characters that are no longer part of the trail
                column.alphas[i] *= 0.95f; // Gradual fade
                if (column.alphas[i] < 0.01f)
                {
                    column.alphas[i] = 0.0f;
                }
            }
        }
        
        // Reset column when it's completely off screen
        if (column.headPosition > maxHeight + column.length)
        {
            column.active = false;
            column.headPosition = -column.length;
            column.spawnDelay = random.nextInt(300) + 50; // Random delay before restart
            column.length = random.nextInt(15) + 10; // New random length
            column.speed = random.nextFloat() * 2.0f + 1.0f; // New random speed
            
            // Clear all alphas
            for (auto& alpha : column.alphas)
            {
                alpha = 0.0f;
            }
        }
    }
}
