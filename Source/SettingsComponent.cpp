#include "SettingsComponent.h"

class SettingsButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
        auto font = juce::Font("Press Start 2P", 10.0f, juce::Font::plain);
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
        juce::Colour fillColour = isButtonDown ? juce::Colours::darkgrey
            : isMouseOverButton ? juce::Colours::lightgrey
            : backgroundColour;

        g.setColour(fillColour);
        g.fillRect(bounds);
    }
};

SettingsComponent::SettingsComponent(SummonerXSerum2AudioProcessor& processor)
{
    static SettingsButtonLookAndFeel customSettingsButtons;
    juce::PropertiesFile::Options options;
    options.applicationName = "Summoner";
    options.filenameSuffix = ".settings";
    options.folderName = "SummonerApp";
    options.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(options);
#if JUCE_WINDOWS
    // Serum path removed - will use internal synthesizer
#elif JUCE_MAC
    defaultPath = "/Library/Audio/Plug-Ins/VST3/Serum2.vst3";
#else
    defaultPath = "Serum2.vst3";
#endif
    pathLabel.setText("Serum Path:", juce::dontSendNotification);
    pathLabel.setColour(juce::Label::textColourId, juce::Colours::indianred);
    pathLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::italic));
    addAndMakeVisible(pathLabel);
    pathDisplay.setText(loadSavedPath(), juce::dontSendNotification);
    pathDisplay.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    pathDisplay.setColour(juce::Label::textColourId, juce::Colours::white);
    pathDisplay.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    pathDisplay.setJustificationType(juce::Justification::centredLeft);
    pathDisplay.setBorderSize(juce::BorderSize<int>(2));
    addAndMakeVisible(pathDisplay);
    browseButton.setLookAndFeel(&customSettingsButtons);
    addAndMakeVisible(browseButton);
    browseButton.setButtonText("Browse");
    browseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
    browseButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);
    browseButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    browseButton.onClick = [this]() { browseForPath(); };
    addAndMakeVisible(resetButton);
    resetButton.setButtonText("Reset Path");
    resetButton.setLookAndFeel(&customSettingsButtons);
    resetButton.onClick = [this]() { resetSavedPath(); };
    resetButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
    resetButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);
    resetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    DBG("SettingsComponent constructed with path: " << loadSavedPath());

    logoutButton.setLookAndFeel(&customSettingsButtons);
    logoutButton.setButtonText("Logout");
    logoutButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    logoutButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    logoutButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    logoutButton.onClick = [this]() {
        if (logoutButton.getButtonText() == "Logout" && onLogout) {
            onLogout();
        } else if (logoutButton.getButtonText() == "Login" && onLogin) {
            onLogin();
        }
        };
    addAndMakeVisible(logoutButton);

    // Credits label
    creditsLabel.setText("Credits: 0", juce::dontSendNotification);
    creditsLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    creditsLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(creditsLabel);

    // Purchase credits button
    purchaseCreditsButton.setLookAndFeel(&customSettingsButtons);
    purchaseCreditsButton.setButtonText("Purchase Credits");
    purchaseCreditsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    purchaseCreditsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    purchaseCreditsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    purchaseCreditsButton.onClick = [this]() {
        // Placeholder for future implementation
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Purchase Credits",
            "Credit purchasing functionality will be implemented soon!");
    };
    addAndMakeVisible(purchaseCreditsButton);

    // Skin selection label
    skinLabel.setText("Skin:", juce::dontSendNotification);
    skinLabel.setFont(juce::Font("Press Start 2P", 12.0f, juce::Font::plain));
    skinLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(skinLabel);

    // Default skin button
    defaultSkinButton.setLookAndFeel(&customSettingsButtons);
    defaultSkinButton.setButtonText("Particles");
    defaultSkinButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
    defaultSkinButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);
    defaultSkinButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    defaultSkinButton.setClickingTogglesState(true);
    defaultSkinButton.setToggleState(false, juce::dontSendNotification); // Start with particles unselected
    defaultSkinButton.onClick = [this]() {
        defaultSkinButton.setToggleState(true, juce::dontSendNotification);
        hackerSkinButton.setToggleState(false, juce::dontSendNotification);
        isHackerSkin = false;
        if (onSkinChanged)
            onSkinChanged(false);
        repaint();
    };
    addAndMakeVisible(defaultSkinButton);

    // Hacker skin button
    hackerSkinButton.setLookAndFeel(&customSettingsButtons);
    hackerSkinButton.setButtonText("Hacker");
    hackerSkinButton.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
    hackerSkinButton.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgoldenrod);
    hackerSkinButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    hackerSkinButton.setClickingTogglesState(true);
    hackerSkinButton.setToggleState(true, juce::dontSendNotification); // Start with hacker selected as default
    hackerSkinButton.onClick = [this]() {
        hackerSkinButton.setToggleState(true, juce::dontSendNotification);
        defaultSkinButton.setToggleState(false, juce::dontSendNotification);
        isHackerSkin = true;
        initializeMatrixColumns(); // Reinitialize columns when hacker skin is activated
        if (onSkinChanged)
            onSkinChanged(true);
        repaint();
    };
    addAndMakeVisible(hackerSkinButton);

    // Initialize mystical floating boxes effect
    floatingBoxes.reserve(40); // Reserve space for up to 40 boxes
    
    // Initialize matrix effect
    initializeMatrixColumns();
    
    startTimer(50); // 50ms timer for smooth animation (20 FPS)
}

SettingsComponent::~SettingsComponent()
{
    stopTimer();
    browseButton.setLookAndFeel(nullptr);
    resetButton.setLookAndFeel(nullptr);
    logoutButton.setLookAndFeel(nullptr);
    purchaseCreditsButton.setLookAndFeel(nullptr);
    defaultSkinButton.setLookAndFeel(nullptr);
    hackerSkinButton.setLookAndFeel(nullptr);
}

void SettingsComponent::resetSavedPath()
{
    auto* userSettings = applicationProperties.getUserSettings();
    if (userSettings != nullptr)
    {
        userSettings->removeValue("pluginPath");
        userSettings->saveIfNeeded();
        pathDisplay.setText(defaultPath, juce::dontSendNotification);
        if (onPathChanged)
        {
            onPathChanged(defaultPath);
        }
        DBG("Path reset to default: " << defaultPath);
    }
    else
    {
        DBG("User settings not initialized.");
    }
}

void SettingsComponent::browseForPath()
{
    DBG("Browse button clicked!");
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select Serum2.vst3 or its Containing Folder",
        juce::File(defaultPath),
        "*.vst3"
    );
    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fileChooser)
        {
            DBG("FileChooser callback triggered.");
            auto selectedFileOrFolder = fileChooser.getResult();
            if (selectedFileOrFolder.exists())
            {
                if (selectedFileOrFolder.isDirectory())
                {
                    DBG("Selected directory: " << selectedFileOrFolder.getFullPathName());
                    auto serumFile = selectedFileOrFolder.getChildFile("Serum2.vst3");
                    if (serumFile.existsAsFile())
                    {
                        DBG("Found Serum2.vst3 in directory: " << serumFile.getFullPathName());
                        pathDisplay.setText(serumFile.getFullPathName(), juce::dontSendNotification);
                        savePath(serumFile.getFullPathName());
                        if (onPathChanged)
                            onPathChanged(serumFile.getFullPathName());
                    }
                    else
                    {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::WarningIcon,
                            "Invalid Folder",
                            "The selected folder does not contain Serum2.vst3. Please select the correct folder or file."
                        );
                    }
                }
                else if (selectedFileOrFolder.getFileName() == "Serum2.vst3" && (selectedFileOrFolder.existsAsFile() || selectedFileOrFolder.isDirectory()))
                {
                    DBG("Selected Serum2.vst3: " << selectedFileOrFolder.getFullPathName());
                    pathDisplay.setText(selectedFileOrFolder.getFullPathName(), juce::dontSendNotification);
                    savePath(selectedFileOrFolder.getFullPathName());
                    if (onPathChanged)
                        onPathChanged(selectedFileOrFolder.getFullPathName());
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Invalid Selection",
                        "Please select the Serum2.vst3 file or its containing folder."
                    );
                }
            }
            else
            {
                DBG("No valid file or folder selected, or dialog canceled.");
            }
        });
}

juce::String SettingsComponent::getPluginPath() const
{
    return pluginPathLabel.getText();
}

void SettingsComponent::paint(juce::Graphics& g)
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
}

void SettingsComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    pathLabel.setBounds(bounds.removeFromTop(20));
    pathDisplay.setBounds(bounds.removeFromTop(30));

    auto buttonWidth = 120;
    auto buttonHeight = 30;
    auto buttonSpacing = 10;

    // Top button row: Browse + Reset
    auto topButtonArea = bounds.removeFromTop(buttonHeight);
    browseButton.setBounds(topButtonArea.getX(), topButtonArea.getY(), buttonWidth, buttonHeight);
    resetButton.setBounds(topButtonArea.getX() + buttonWidth + buttonSpacing, topButtonArea.getY(), buttonWidth, buttonHeight);

    // Spacer between path section and credits section
    bounds.removeFromTop(buttonSpacing * 5);

    // Credits section
    creditsLabel.setBounds(bounds.getX(), bounds.getY(), buttonWidth * 2, buttonHeight);
    bounds.removeFromTop(buttonHeight + buttonSpacing);

    // Purchase credits button (original size)
    purchaseCreditsButton.setBounds(bounds.getX(), bounds.getY(), buttonWidth * 2, buttonHeight);
    bounds.removeFromTop(buttonHeight + buttonSpacing * 3);

    // Skin selection section
    skinLabel.setBounds(bounds.getX(), bounds.getY(), buttonWidth * 2, 20);
    bounds.removeFromTop(20 + buttonSpacing);
    
    // Skin buttons row: Default + Hacker
    auto skinButtonArea = bounds.removeFromTop(buttonHeight);
    defaultSkinButton.setBounds(skinButtonArea.getX(), skinButtonArea.getY(), buttonWidth, buttonHeight);
    hackerSkinButton.setBounds(skinButtonArea.getX() + buttonWidth + buttonSpacing, skinButtonArea.getY(), buttonWidth, buttonHeight);
    bounds.removeFromTop(buttonSpacing * 3);

    // Logout button
    logoutButton.setBounds(bounds.getX(), bounds.getY(), buttonWidth, buttonHeight);
    
    // Reinitialize Matrix columns when component is resized (if hacker skin is active)
    if (isHackerSkin)
    {
        initializeMatrixColumns();
    }
}


void SettingsComponent::savePath(const juce::String& path)
{
    auto* userSettings = applicationProperties.getUserSettings();
    userSettings->setValue("pluginPath", path);
    userSettings->saveIfNeeded();
}

juce::String SettingsComponent::loadSavedPath()
{
    auto* userSettings = applicationProperties.getUserSettings();
    juce::String path = userSettings->getValue("pluginPath", defaultPath);
    DBG("Loaded saved path: " << path);
    return path;
}

void SettingsComponent::updateLoginState(bool isLoggedIn)
{
    if (isLoggedIn) {
        logoutButton.setButtonText("Logout");
        logoutButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    } else {
        logoutButton.setButtonText("Login");
        logoutButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    }
}

void SettingsComponent::setCredits(int credits)
{
    currentCredits = credits;
    creditsLabel.setText("Credits: " + juce::String(credits), juce::dontSendNotification);
    
    // Set color to red if credits are 0, otherwise white
    if (credits == 0) {
        creditsLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    } else {
        creditsLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

int SettingsComponent::getCredits() const
{
    return currentCredits;
}

void SettingsComponent::timerCallback()
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

void SettingsComponent::updateFloatingBoxes()
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

void SettingsComponent::createRandomBox()
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

int SettingsComponent::getQuadrant(float x, float y)
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

void SettingsComponent::initializeMatrixColumns()
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

char SettingsComponent::getRandomMatrixCharacter()
{
    // Mix of numbers, letters, and some special characters for Matrix effect
    static const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
    return chars[random.nextInt(sizeof(chars) - 1)];
}

void SettingsComponent::updateMatrixEffect()
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
