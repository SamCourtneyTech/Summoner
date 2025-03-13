#include "ChatBarComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

class ChatBarButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool) override
    {
        auto font = juce::Font("Press Start 2P", 15.0f, juce::Font::plain);
        g.setFont(font);
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        juce::Colour fillColour = isButtonDown ? juce::Colours::darkblue
            : isMouseOverButton ? juce::Colours::blue
            : backgroundColour;
        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};

ChatBarComponent::ChatBarComponent(SummonerAudioProcessor& p) : processor(p)
{
    static ChatBarButtonLookAndFeel customSummonButton;
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

    sendButton.onClick = [this]() {
        juce::String userInput = chatInput.getText();
        sendPromptToChatGPT(userInput, [this](std::vector<std::map<std::string, std::string>> responses) {
            processor.setResponses(responses);
            });
        };
}

ChatBarComponent::~ChatBarComponent()
{
    sendButton.setLookAndFeel(nullptr);
}

void ChatBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void ChatBarComponent::resized()
{
    auto area = getLocalBounds();
    auto chatBarHeight = 25;
    auto chatBarWidth = 600;
    auto buttonWidth = 100;
    auto yPosition = (getHeight() - chatBarHeight) / 2;
    chatInput.setBounds((getWidth() - chatBarWidth - buttonWidth - 10) / 2, yPosition - 50, chatBarWidth, chatBarHeight);
    sendButton.setBounds(chatInput.getRight() + 10, yPosition - 50, buttonWidth, chatBarHeight);
}

void ChatBarComponent::sendPromptToChatGPT(const juce::String& userPrompt,
    std::function<void(std::vector<std::map<std::string, std::string>>)> callback)
{
    std::thread([this, userPrompt, callback]() {
        std::vector<std::map<std::string, std::string>> allResponses;
        juce::CriticalSection responseLock;
        int numResponses = 10;

        std::vector<std::thread> threads;
        for (int i = 0; i < numResponses; ++i)
        {
            threads.emplace_back([&, i]() {
                std::map<std::string, std::string> parameterMap;
                const juce::String apiEndpoint = "https://api.openai.com/v1/chat/completions";
                const juce::String apiKey = "your-api-key-here"; // Replace with your API key
                juce::String systemInstruction = R"(Interpret the user's request with creativity to generate parameters for a custom synthesizer. Return a list of parameters in the format {"Parameter Name", "Value"}. Example parameters: {"Osc1Freq", "440 Hz"}, {"Osc1Wave", "Sine"}, {"EnvAttack", "0.1 s"}. Ensure all responses are consistent and include at least 10 parameters. User prompt: )" + userPrompt;

                juce::var requestPayload(new juce::DynamicObject());
                requestPayload.getDynamicObject()->setProperty("model", "gpt-4");
                requestPayload.getDynamicObject()->setProperty("max_tokens", 2000);
                juce::var messages = juce::Array<juce::var>();
                juce::var systemMsgObj(new juce::DynamicObject());
                systemMsgObj.getDynamicObject()->setProperty("role", "system");
                systemMsgObj.getDynamicObject()->setProperty("content", systemInstruction);
                messages.getArray()->add(systemMsgObj);
                juce::var userMsgObj(new juce::DynamicObject());
                userMsgObj.getDynamicObject()->setProperty("role", "user");
                userMsgObj.getDynamicObject()->setProperty("content", userPrompt);
                messages.getArray()->add(userMsgObj);
                requestPayload.getDynamicObject()->setProperty("messages", messages);

                juce::String requestBody = juce::JSON::toString(requestPayload, false).replace("\n", "").replace("  ", "");
                juce::URL url(apiEndpoint);
                url = url.withPOSTData(requestBody);
                juce::StringArray headers;
                headers.add("Authorization: Bearer " + apiKey);
                headers.add("Content-Type: application/json");
                headers.add("Accept: application/json");

                std::unique_ptr<juce::WebInputStream> webStream = std::make_unique<juce::WebInputStream>(url, true);
                webStream->withExtraHeaders(headers.joinIntoString("\r\n"));
                webStream->withConnectionTimeout(90000);

                if (!webStream->connect(nullptr))
                {
                    DBG("Error: Failed to connect to API for response " << i);
                    return;
                }

                juce::MemoryOutputStream responseStream;
                while (!webStream->isExhausted())
                {
                    responseStream.writeFromInputStream(*webStream, 32768);
                }
                juce::String response = responseStream.toString();
                juce::var jsonResponse = juce::JSON::parse(response);

                if (auto* obj = jsonResponse.getDynamicObject())
                {
                    if (obj->hasProperty("choices"))
                    {
                        const juce::var choices = obj->getProperty("choices");
                        if (choices.isArray() && choices.getArray()->size() > 0)
                        {
                            const juce::var firstChoice = choices.getArray()->getFirst();
                            if (auto* choiceObj = firstChoice.getDynamicObject())
                            {
                                if (choiceObj->hasProperty("message"))
                                {
                                    const juce::var msg = choiceObj->getProperty("message");
                                    if (auto* msgObj = msg.getDynamicObject())
                                    {
                                        if (msgObj->hasProperty("content"))
                                        {
                                            juce::String content = msgObj->getProperty("content").toString();
                                            content = content.replace("\r\n", "\n").replace("\r", "\n");
                                            if (content.startsWith("[") && content.endsWith("]"))
                                                content = content.substring(1, content.length() - 1).trim();

                                            while (content.isNotEmpty())
                                            {
                                                if (content.contains("{") && content.contains("}"))
                                                {
                                                    int openBracePos = content.indexOf("{");
                                                    int closeBracePos = content.indexOf("}");
                                                    if (openBracePos < closeBracePos)
                                                    {
                                                        juce::String pairString = content.substring(openBracePos + 1, closeBracePos).trim();
                                                        if (pairString.contains(","))
                                                        {
                                                            int commaPos = pairString.indexOf(",");
                                                            juce::String name = pairString.substring(0, commaPos).trim().removeCharacters("\"");
                                                            juce::String value = pairString.substring(commaPos + 1).trim().removeCharacters("\"");
                                                            parameterMap[name.toStdString()] = value.toStdString();
                                                        }
                                                        content = content.substring(closeBracePos + 1).trim();
                                                    }
                                                    else
                                                        break;
                                                }
                                                else
                                                    break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                juce::ScopedLock lock(responseLock);
                allResponses.push_back(parameterMap);
                });
        }

        for (auto& thread : threads)
            if (thread.joinable())
                thread.join();

        juce::MessageManager::callAsync([this, allResponses, callback]() {
            callback(allResponses);
            });
        }).detach();
}
