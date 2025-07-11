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

    addAndMakeVisible(creditsLabel);
    creditsLabel.setText("Credits: 0", juce::dontSendNotification);
    creditsLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    creditsLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    
    // Make credits label clickable
    creditsLabel.setInterceptsMouseClicks(true, false);
    creditsLabel.addMouseListener(this, false);

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
    
    // Draw hover background for credits label
    if (creditsLabelHovered)
    {
        auto creditsBounds = creditsLabel.getBounds();
        auto font = creditsLabel.getFont();
        auto textWidth = font.getStringWidth(creditsLabel.getText());
        auto textHeight = font.getHeight();
        
        // Create bounds around the actual text with more visible padding
        juce::Rectangle<float> textBounds(
            creditsBounds.getX() - 2, 
            creditsBounds.getY() + (creditsBounds.getHeight() - textHeight) / 2 - 3,
            textWidth + 8, 
            textHeight + 6
        );
        
        g.setColour(juce::Colours::grey);
        g.fillRoundedRectangle(textBounds, 3.0f);
    }
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
    creditsLabel.setBounds(10, 20, 200, 30);
    
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
        // Force reload properties to ensure we have the latest values
        appProps.getUserSettings()->reload();
        
        juce::String accessToken = appProps.getUserSettings()->getValue("accessToken", "");
        bool isLoggedIn = appProps.getUserSettings()->getBoolValue("isLoggedIn", false);
        int credits = appProps.getUserSettings()->getIntValue("credits", 0);
        
        DBG("Token retrieval debug - isLoggedIn: " << (isLoggedIn ? "true" : "false") 
            << ", accessToken length: " << accessToken.length() 
            << ", credits: " << credits);
            
        // Check if user has credits before making request
        if (credits <= 0)
        {
            DBG("No credits available - showing out of credits modal");
            juce::MessageManager::callAsync([this]() {
                showOutOfCreditsModal();
                if (onLoadingStateChanged)
                {
                    onLoadingStateChanged(false);
                }
                requestInProgress = false;
            });
            return;
        }
            
        if (accessToken.isEmpty())
        {
            DBG("Access token is empty - login state: " << (isLoggedIn ? "logged in" : "not logged in"));
            juce::MessageManager::callAsync([this]() {
                // Try to refresh token first before showing error
                if (onRefreshTokenRequested)
                {
                    onRefreshTokenRequested();
                    
                    // Give it a moment then check again
                    juce::Timer::callAfterDelay(2000, [this]() {
                        juce::String refreshedToken = appProps.getUserSettings()->getValue("accessToken", "");
                        if (refreshedToken.isEmpty()) {
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "Authentication Error",
                                "No access token found. Please log in again.");
                        }
                        if (onLoadingStateChanged)
                        {
                            onLoadingStateChanged(false);
                        }
                        requestInProgress = false;
                    });
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Authentication Error", 
                        "No access token found. Please log in again.");
                    requestInProgress = false;
                }
                });
            return;
        }
        
        DBG("Using access token for request: " << accessToken.substring(0, 10) << "...[truncated]");

        juce::URL endpoint("https://ydr97n8vxe.execute-api.us-east-2.amazonaws.com/prod/generate-parameters");

        // Create JSON body
        juce::DynamicObject::Ptr jsonObject = new juce::DynamicObject();
        jsonObject->setProperty("input", userPrompt);
        juce::String postData = juce::JSON::toString(jsonObject.get());
        DBG("Sending POST request to /generate-parameters with JSON data: " + postData);

        juce::URL urlWithPostData = endpoint.withPOSTData(postData);

        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withExtraHeaders("Content-Type: application/json\nAuthorization: Bearer " + accessToken)
            .withConnectionTimeoutMs(50000);

        std::unique_ptr<juce::InputStream> stream(urlWithPostData.createInputStream(options));

        if (stream != nullptr)
        {
            juce::String response = stream->readEntireStreamAsString();
            DBG("Raw response from /generate-parameters endpoint: " + response);
            juce::var result = juce::JSON::parse(response);

            if (result.isObject())
            {
                auto* obj = result.getDynamicObject();
                
                // Check if this is an error response with "detail" field
                if (obj->hasProperty("detail"))
                {
                    juce::String detail = obj->getProperty("detail").toString();
                    
                    if (detail == "Invalid token")
                    {
                        juce::MessageManager::callAsync([this]() {
                            DBG("Invalid token response - attempting to refresh token");
                            
                            // Try to refresh the token
                            if (onRefreshTokenRequested)
                            {
                                onRefreshTokenRequested();
                            }
                            if (onLoadingStateChanged)
                            {
                                onLoadingStateChanged(false);
                            }
                            
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "Authentication Error",
                                "Your session has expired. Please try again or log in again if the issue persists.");
                            
                            requestInProgress = false;
                        });
                        return;
                    }
                    else
                    {
                        // Handle other error details
                        juce::MessageManager::callAsync([this, detail]() {
                            if (auto* parent = getParentComponent())
                            {
                                if (auto* grandParent = parent->getParentComponent())
                                {
                                    if (auto* editor = dynamic_cast<SummonerXSerum2AudioProcessorEditor*>(grandParent))
                                    {
                                        editor->showLoadingScreen(false);
                                    }
                                }
                            }
                            
                            DBG("API error response: " + detail);
                            juce::AlertWindow::showMessageBoxAsync(
                                juce::AlertWindow::WarningIcon,
                                "Error",
                                "Server error: " + detail);
                            requestInProgress = false;
                        });
                        return;
                    }
                }
                
                // If we get here, it's a successful response with parameters
                std::map<std::string, std::string> parameterMap;
                for (const auto& property : obj->getProperties())
                {
                    juce::String key = property.name.toString();
                    juce::String value = property.value.toString();
                    parameterMap[key.toStdString()] = value.toStdString();
                }

                juce::MessageManager::callAsync([this, parameterMap]() {
                    std::vector<std::map<std::string, std::string>> responses = { parameterMap };
                    processor.setResponses(responses);
                    // SerumInterface removed - internal synthesizer will handle response counter

                    // Update credits after successful response
                    int credits = fetchUserCredits();
                    setCredits(credits);
                    
                    // Notify editor to update other components
                    if (onCreditsUpdated && credits >= 0) {
                        onCreditsUpdated(credits);
                    }

                    if (onLoadingStateChanged)
                    {
                        onLoadingStateChanged(false);
                    }
                    requestInProgress = false;
                    });
            }
            else
            {
                juce::MessageManager::callAsync([this, response]() {
                    DBG("Failed to parse response from /generate-parameters: " + response);
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Error",
                        "Failed to generate parameters: " + response);
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
                DBG("Failed to connect to /generate-parameters endpoint");
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Connection Error",
                    "Failed to connect to the server. Please try again.");
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
    // TODO: Apply AI response to internal synthesizer instead of Serum
    juce::ignoreUnused(aiResponse);
}

void ChatBarComponent::setCredits(int credits)
{
    creditsLabel.setText("Credits: " + juce::String(credits), juce::dontSendNotification);
    
    // Set color to red if credits are 0, otherwise white
    if (credits == 0) {
        creditsLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    } else {
        creditsLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

int ChatBarComponent::fetchUserCredits()
{
    juce::String accessToken = appProps.getUserSettings()->getValue("accessToken", "");
    if (accessToken.isEmpty())
    {
        DBG("No access token available to fetch credits");
        return -1; // Return -1 to indicate error, not legitimate 0 credits
    }

    juce::URL url("https://ydr97n8vxe.execute-api.us-east-2.amazonaws.com/prod/get-credits");

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders("Authorization: Bearer " + accessToken)
        .withConnectionTimeoutMs(50000);

    std::unique_ptr<juce::InputStream> stream(url.createInputStream(options));

    if (stream != nullptr)
    {
        juce::String response = stream->readEntireStreamAsString();
        DBG("Received response from /get-credits endpoint: " + response);
        juce::var result = juce::JSON::parse(response);
        
        if (result.isObject())
        {
            auto* obj = result.getDynamicObject();
            
            // Check if this is an error response with "detail" field
            if (obj->hasProperty("detail"))
            {
                juce::String detail = obj->getProperty("detail").toString();
                DBG("Credits fetch error in ChatBar: " + detail);
                
                if (detail == "Invalid token")
                {
                    DBG("Token is invalid during credit fetch - returning error");
                    // Don't handle logout here, let the parent component handle it
                }
                return -1; // Return -1 to indicate error
            }
            else if (obj->hasProperty("credits"))
            {
                // Successful response with credits
                int credits = obj->getProperty("credits").toString().getIntValue();
                DBG("Parsed credits: " + juce::String(credits));
                return credits;
            }
            else
            {
                DBG("No credits field found in response");
                return -1; // Return -1 to indicate error
            }
        }
        else
        {
            DBG("Invalid JSON response format");
            return -1; // Return -1 to indicate error
        }
    }
    DBG("Failed to fetch credits: No response from server");
    return -1; // Return -1 to indicate error
}

void ChatBarComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.originalComponent == &creditsLabel)
    {
        showCreditsModal();
    }
}

void ChatBarComponent::mouseEnter(const juce::MouseEvent& event)
{
    if (event.originalComponent == &creditsLabel)
    {
        creditsLabelHovered = true;
        repaint();
    }
}

void ChatBarComponent::mouseExit(const juce::MouseEvent& event)
{
    if (event.originalComponent == &creditsLabel)
    {
        creditsLabelHovered = false;
        repaint();
    }
}

void ChatBarComponent::showCreditsModal()
{
    if (creditsModal == nullptr)
    {
        creditsModal = std::make_unique<CreditsModalWindow>();
        creditsModal->onCloseClicked = [this]() {
            creditsModal->setVisible(false);
            removeChildComponent(creditsModal.get());
        };
        creditsModal->onPurchaseClicked = [this]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Purchase Credits",
                "Credit purchasing functionality will be implemented soon!");
        };
    }
    
    addAndMakeVisible(creditsModal.get());
    creditsModal->setBounds(getLocalBounds());
    creditsModal->toFront(true);
    
    // Automatically refresh credits when modal is opened
    DBG("Credits modal opened - refreshing credits");
    refreshCreditsFromModal();
}

void ChatBarComponent::refreshCreditsFromModal()
{
    // Use the existing fetchUserCredits method but with improved error handling
    std::thread([this]() {
        DBG("Modal credit refresh - starting background fetch");
        
        int newCredits = fetchUserCredits();
        
        // Update the UI on the main thread
        juce::MessageManager::callAsync([this, newCredits]() {
            if (newCredits >= 0) // Only update if we got a valid response (0 or positive credits)
            {
                DBG("Modal credit refresh - updating to: " << newCredits);
                setCredits(newCredits);
                
                // Update stored credits too
                appProps.getUserSettings()->setValue("credits", newCredits);
                appProps.getUserSettings()->save();
                
                DBG("Credits successfully refreshed from modal");
            }
            else
            {
                DBG("Modal credit refresh - failed to get valid credits, keeping current value");
            }
        });
    }).detach();
}

// CreditsModalWindow Implementation
ChatBarComponent::CreditsModalWindow::CreditsModalWindow()
{
    // Close button (red X)
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("X");
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    closeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    closeButton.setLookAndFeel(&closeButtonLookAndFeel);
    closeButton.onClick = [this]() {
        if (onCloseClicked)
            onCloseClicked();
    };
    
    // Title label
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Credits Information", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Press Start 2P", 20.0f, juce::Font::plain));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    
    // Info label
    addAndMakeVisible(infoLabel);
    infoLabel.setText("Each prompt you submit consumes 1 credit.",
                     juce::dontSendNotification);
    infoLabel.setFont(juce::Font("Press Start 2P", 16.0f, juce::Font::plain));
    infoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    infoLabel.setJustificationType(juce::Justification::centred);
    
    // Purchase button
    addAndMakeVisible(purchaseButton);
    purchaseButton.setButtonText("Purchase Credits");
    purchaseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    purchaseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    purchaseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    purchaseButton.setLookAndFeel(&purchaseButtonLookAndFeel);
    purchaseButton.onClick = [this]() {
        if (onPurchaseClicked)
            onPurchaseClicked();
    };
}

void ChatBarComponent::CreditsModalWindow::paint(juce::Graphics& g)
{
    // Semi-transparent overlay
    g.fillAll(juce::Colours::black.withAlpha(0.7f));
    
    // Modal window background - made larger
    auto modalBounds = getLocalBounds().reduced(60).withSizeKeepingCentre(600, 450);
    g.setColour(juce::Colours::black);
    g.fillRect(modalBounds);
    
    // Border
    g.setColour(juce::Colours::black);
    g.drawRect(modalBounds, 2);
}

void ChatBarComponent::CreditsModalWindow::resized()
{
    auto modalBounds = getLocalBounds().reduced(60).withSizeKeepingCentre(600, 450);
    
    // Close button in top-right
    closeButton.setBounds(modalBounds.getRight() - 35, modalBounds.getY() + 15, 25, 25);
    
    // Title
    titleLabel.setBounds(modalBounds.getX() + 30, modalBounds.getY() + 30, modalBounds.getWidth() - 60, 40);
    
    // Info text - larger area for better text spacing
    infoLabel.setBounds(modalBounds.getX() + 30, modalBounds.getY() + 90, modalBounds.getWidth() - 60, 280);
    
    // Purchase button
    purchaseButton.setBounds(modalBounds.getCentreX() - 100, modalBounds.getBottom() - 60, 200, 35);
}

void ChatBarComponent::showOutOfCreditsModal()
{
    if (outOfCreditsModal == nullptr)
    {
        outOfCreditsModal = std::make_unique<OutOfCreditsModalWindow>();
        outOfCreditsModal->onCloseClicked = [this]() {
            outOfCreditsModal->setVisible(false);
            removeChildComponent(outOfCreditsModal.get());
        };
        outOfCreditsModal->onPurchaseClicked = [this]() {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Purchase Credits",
                "Credit purchasing functionality will be implemented soon!");
        };
    }
    
    addAndMakeVisible(outOfCreditsModal.get());
    outOfCreditsModal->setBounds(getLocalBounds());
    outOfCreditsModal->toFront(true);
}

// OutOfCreditsModalWindow Implementation
ChatBarComponent::OutOfCreditsModalWindow::OutOfCreditsModalWindow()
{
    // Close button (red X)
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("X");
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    closeButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    closeButton.setLookAndFeel(&closeButtonLookAndFeel);
    closeButton.onClick = [this]() {
        if (onCloseClicked)
            onCloseClicked();
    };
    
    // Title label
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Out of Credits", juce::dontSendNotification);
    titleLabel.setFont(juce::Font("Press Start 2P", 20.0f, juce::Font::plain));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    titleLabel.setJustificationType(juce::Justification::centred);
    
    // No credits warning label
    addAndMakeVisible(noCreditsLabel);
    noCreditsLabel.setText("Purchase more to continue! :)", juce::dontSendNotification);
    noCreditsLabel.setFont(juce::Font("Press Start 2P", 16.0f, juce::Font::plain));
    noCreditsLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    noCreditsLabel.setJustificationType(juce::Justification::centred);
    
    // Info label
    addAndMakeVisible(infoLabel);
    infoLabel.setText("Each sound summoned consumes 1 credit.",
                     juce::dontSendNotification);
    infoLabel.setFont(juce::Font("Press Start 2P", 14.0f, juce::Font::plain));
    infoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    infoLabel.setJustificationType(juce::Justification::centred);
    
    // Purchase button
    addAndMakeVisible(purchaseButton);
    purchaseButton.setButtonText("Get More Credits");
    purchaseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    purchaseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    purchaseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    purchaseButton.setLookAndFeel(&purchaseButtonLookAndFeel);
    purchaseButton.onClick = [this]() {
        if (onPurchaseClicked)
            onPurchaseClicked();
    };
}

void ChatBarComponent::OutOfCreditsModalWindow::paint(juce::Graphics& g)
{
    // Semi-transparent overlay
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
    
    // Modal window background
    auto modalBounds = getLocalBounds().reduced(60).withSizeKeepingCentre(600, 500);
    g.setColour(juce::Colours::darkred.withAlpha(0.3f));
    g.fillRect(modalBounds);
    
    // Red border to emphasize the error
    g.setColour(juce::Colours::red);
    g.drawRect(modalBounds, 3);
    
    // Inner dark background
    g.setColour(juce::Colours::black.withAlpha(0.9f));
    g.fillRect(modalBounds.reduced(3));
}

void ChatBarComponent::OutOfCreditsModalWindow::resized()
{
    auto modalBounds = getLocalBounds().reduced(60).withSizeKeepingCentre(600, 500);
    
    // Close button in top-right
    closeButton.setBounds(modalBounds.getRight() - 35, modalBounds.getY() + 15, 25, 25);
    
    // Title
    titleLabel.setBounds(modalBounds.getX() + 30, modalBounds.getY() + 40, modalBounds.getWidth() - 60, 40);
    
    // No credits warning
    noCreditsLabel.setBounds(modalBounds.getX() + 30, modalBounds.getY() + 100, modalBounds.getWidth() - 60, 40);
    
    // Info text
    infoLabel.setBounds(modalBounds.getX() + 30, modalBounds.getY() + 160, modalBounds.getWidth() - 60, 240);
    
    // Purchase button
    purchaseButton.setBounds(modalBounds.getCentreX() - 100, modalBounds.getBottom() - 70, 200, 40);
}

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
