#include "OscillatorBackgroundPainter.h"

void OscillatorBackgroundPainter::paintOscillator1Background(juce::Graphics& g,
                                                              juce::Rectangle<int> bounds,
                                                              float oscillatorBackgroundOffsetX,
                                                              float oscillatorBackgroundOffsetY,
                                                              float adsrKnobsGroupOffsetX,
                                                              float adsrKnobsGroupOffsetY,
                                                              float volumeControlsGroupOffsetX,
                                                              float volumeControlsGroupOffsetY,
                                                              float pitchControlsGroupOffsetX,
                                                              float pitchControlsGroupOffsetY)
{
    // Add futuristic outline around the actual controls bounds
    // Calculate the tight bounds that encompass all controls
    auto masterBounds = bounds;
    masterBounds.reduce(20, 20);
    masterBounds.removeFromTop(20);

    // Start from wave buttons and go to the bottom of phase section
    auto masterTop = masterBounds.getY();
    auto masterLeft = masterBounds.getX();

    // Width should encompass the tuning section (which is the widest at 320px)
    auto masterWidth = 320 + 63; // tuning width + a bit more padding

    // Height should go from wave buttons to bottom of phase section
    auto masterHeight = 40 + 7 + 60 + 15 + 100 + 20 + 100 + 40 + 80 + 20 + 80 + 20; // all sections + spacing + extra

    // Apply oscillator offset for easy group movement (defaults to 0,0 for no change)
    auto masterOutlineBounds = juce::Rectangle<float>(masterLeft - 12 + oscillatorBackgroundOffsetX, masterTop - 12 + oscillatorBackgroundOffsetY, masterWidth + 24, masterHeight + 24);

    // Draw the main metallic panel
    drawMetallicPanel(g, masterOutlineBounds);

    // Draw corner screws
    drawPanelScrews(g, masterOutlineBounds.reduced(2.0f));

    // Draw oscillator label
    drawOscillatorLabel(g, masterOutlineBounds.reduced(2.0f));

    // Add metal slots for knobs - match exact positions from resized()
    auto slotBounds = bounds;
    slotBounds.reduce(20, 20);
    // Apply oscillator offset to match the UI components movement
    slotBounds = slotBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));

    // Skip to ADSR section (match resized() layout exactly)
    slotBounds.removeFromTop(20); // spacing
    slotBounds.removeFromTop(40); // wave buttons
    slotBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more
    slotBounds.removeFromTop(60); // envelope visualization
    slotBounds.removeFromTop(15); // increased spacing between envelope and knobs

    // ADSR knobs metal slots - match exact layout from resized()
    auto adsrRow = slotBounds.removeFromTop(100);
    auto adsrSection = adsrRow.removeFromLeft(slotBounds.getWidth() / 3);

    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) - same as layout
    adsrSection = adsrSection.translated(
        static_cast<int>(adsrKnobsGroupOffsetX),
        static_cast<int>(adsrKnobsGroupOffsetY)
    );

    drawADSRKnobSlots(g, adsrSection);

    slotBounds.removeFromTop(20); // spacing

    // Volume section knobs metal slots - square metal
    auto volumeRow = slotBounds.removeFromTop(100);
    auto volumeControlsSection = volumeRow.removeFromLeft(slotBounds.getWidth() / 3); // Full section for all 4 knobs

    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) - same as layout
    volumeControlsSection = volumeControlsSection.translated(
        static_cast<int>(volumeControlsGroupOffsetX),
        static_cast<int>(volumeControlsGroupOffsetY)
    );

    drawVolumeKnobSlots(g, volumeControlsSection);

    // Skip to phase section
    slotBounds.removeFromTop(40); // spacing - match resized() method spacing
    slotBounds.removeFromTop(80); // tuning controls
    slotBounds.removeFromTop(20); // spacing

    // Phase knob metal slot - match exact resized() positioning
    auto phaseRow = slotBounds.removeFromTop(80);
    drawPhaseKnobSlot(g, phaseRow);

    // Draw futuristic section outlines for each row of controls
    auto sectionBounds = bounds;
    sectionBounds.reduce(20, 20);
    // Apply oscillator offset to match the UI components movement
    sectionBounds = sectionBounds.translated(static_cast<int>(oscillatorBackgroundOffsetX), static_cast<int>(oscillatorBackgroundOffsetY));
    sectionBounds.removeFromTop(20);

    // Wave type buttons section outline
    auto waveSectionBounds = sectionBounds.removeFromTop(40);
    waveSectionBounds = waveSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3); // Same width as ADSR
    waveSectionBounds.reduce(-2, -2); // Expand the bounds slightly

    sectionBounds.removeFromTop(7); // further reduced spacing to match envelope shift up more

    // Skip envelope visualization and spacing
    sectionBounds.removeFromTop(60 + 15); // envelope height + increased spacing

    // ADSR section outline
    auto adsrSectionBounds = sectionBounds.removeFromTop(100);
    adsrSectionBounds = adsrSectionBounds.removeFromLeft(sectionBounds.getWidth() / 3);

    // Apply group offset for MOVEABLE ADSR Knobs Group (Row 3) section outline
    adsrSectionBounds = adsrSectionBounds.translated(
        static_cast<int>(adsrKnobsGroupOffsetX),
        static_cast<int>(adsrKnobsGroupOffsetY)
    );

    adsrSectionBounds.reduce(-5, -5); // Expand the bounds

    sectionBounds.removeFromTop(20); // spacing

    // Volume/Detune/Stereo/Pan section outline
    auto volumeSectionBounds = sectionBounds.removeFromTop(100);
    volumeSectionBounds = volumeSectionBounds.removeFromLeft(volumeSectionBounds.getWidth() / 3);

    // Apply group offset for MOVEABLE Volume Controls Group (Row 4) section outline
    volumeSectionBounds = volumeSectionBounds.translated(
        static_cast<int>(volumeControlsGroupOffsetX),
        static_cast<int>(volumeControlsGroupOffsetY)
    );

    volumeSectionBounds.reduce(-5, -5); // Expand the bounds

    sectionBounds.removeFromTop(40); // increased spacing even more to match octave controls pushed down further

    // Octave/Semitone/Fine/Voices section outline
    auto tuningSectionBounds = sectionBounds.removeFromTop(80);
    tuningSectionBounds = tuningSectionBounds.removeFromLeft(320); // Fixed width for tuning controls

    // Apply group offset for MOVEABLE Pitch Controls Group (Row 5) section outline
    tuningSectionBounds = tuningSectionBounds.translated(
        static_cast<int>(pitchControlsGroupOffsetX),
        static_cast<int>(pitchControlsGroupOffsetY)
    );

    tuningSectionBounds.reduce(-5, -5); // Expand the bounds

    // Draw tuning control boxes
    drawTuningControlBoxes(g, tuningSectionBounds);

    sectionBounds.removeFromTop(20); // spacing

    // Phase section outline
    auto phaseSectionBounds = sectionBounds.removeFromTop(80);
    phaseSectionBounds = phaseSectionBounds.removeFromLeft(220); // Fixed width for phase controls
    phaseSectionBounds.reduce(-5, -5); // Expand the bounds
}

void OscillatorBackgroundPainter::drawMetallicPanel(juce::Graphics& g, juce::Rectangle<float> masterOutlineBounds)
{
    // Create 3D depth effect with multiple layers

    // Shadow layer (bottom-right offset for depth)
    auto shadowBounds = masterOutlineBounds.translated(3.0f, 3.0f);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(shadowBounds, 10.0f);

    // Dark base layer (for depth)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.fillRoundedRectangle(masterOutlineBounds, 10.0f);

    // Main raised surface - metallic modular synth module look
    auto raisedBounds = masterOutlineBounds.reduced(2.0f);

    // Much darker metallic grey base
    g.setColour(juce::Colour(0xff202020));
    g.fillRoundedRectangle(raisedBounds, 8.0f);

    // Add very dark metallic gradient for depth
    juce::ColourGradient metallicGradient(juce::Colour(0xff2a2a2a), raisedBounds.getX(), raisedBounds.getY(),
                                         juce::Colour(0xff151515), raisedBounds.getX(), raisedBounds.getBottom(), false);
    g.setGradientFill(metallicGradient);
    g.fillRoundedRectangle(raisedBounds, 8.0f);

    // Add subtle metallic texture
    juce::Random metallicRandom(42);
    g.setColour(juce::Colour(0xff202020).withAlpha(0.3f));
    for (int i = 0; i < 200; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }

    // Add lighter metallic highlights
    metallicRandom.setSeed(123);
    g.setColour(juce::Colour(0xff606060).withAlpha(0.2f));
    for (int i = 0; i < 150; ++i)
    {
        auto x = raisedBounds.getX() + metallicRandom.nextFloat() * raisedBounds.getWidth();
        auto y = raisedBounds.getY() + metallicRandom.nextFloat() * raisedBounds.getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }

    // Top and left highlights (simulating light from top-left)
    g.setColour(juce::Colour(0xff707070).withAlpha(0.8f));
    // Top highlight
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getY() + 1,
               raisedBounds.getRight() - 8, raisedBounds.getY() + 1, 2.0f);
    // Left highlight
    g.drawLine(raisedBounds.getX() + 1, raisedBounds.getY() + 8,
               raisedBounds.getX() + 1, raisedBounds.getBottom() - 8, 2.0f);

    // Bottom and right shadows (simulating shadow from top-left light)
    g.setColour(juce::Colour(0xff202020).withAlpha(0.9f));
    // Bottom shadow
    g.drawLine(raisedBounds.getX() + 8, raisedBounds.getBottom() - 1,
               raisedBounds.getRight() - 8, raisedBounds.getBottom() - 1, 2.0f);
    // Right shadow
    g.drawLine(raisedBounds.getRight() - 1, raisedBounds.getY() + 8,
               raisedBounds.getRight() - 1, raisedBounds.getBottom() - 8, 2.0f);
}

void OscillatorBackgroundPainter::drawPanelScrews(juce::Graphics& g, juce::Rectangle<float> raisedBounds)
{
    // Add corner screws for modular synth module look
    auto screwRadius = 6.0f;
    auto screwInset = 15.0f; // Increased inset to ensure screws are visible

    // Top-left screw
    auto tlScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(tlScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(tlScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(tlScrew.getCentreX() - 3, tlScrew.getCentreY(), tlScrew.getCentreX() + 3, tlScrew.getCentreY(), 1.5f);

    // Top-right screw
    auto trScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getY() + screwInset, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(trScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(trScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(trScrew.getCentreX() - 3, trScrew.getCentreY(), trScrew.getCentreX() + 3, trScrew.getCentreY(), 1.5f);

    // Bottom-left screw
    auto blScrew = juce::Rectangle<float>(raisedBounds.getX() + screwInset, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(blScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(blScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(blScrew.getCentreX() - 3, blScrew.getCentreY(), blScrew.getCentreX() + 3, blScrew.getCentreY(), 1.5f);

    // Bottom-right screw
    auto brScrew = juce::Rectangle<float>(raisedBounds.getRight() - screwInset - screwRadius * 2, raisedBounds.getBottom() - screwInset - screwRadius * 2, screwRadius * 2, screwRadius * 2);
    g.setColour(juce::Colour(0xff404040));
    g.fillEllipse(brScrew);
    g.setColour(juce::Colour(0xff808080));
    g.drawEllipse(brScrew, 1.0f);
    // Screw slot
    g.setColour(juce::Colour(0xff101010));
    g.drawLine(brScrew.getCentreX() - 3, brScrew.getCentreY(), brScrew.getCentreX() + 3, brScrew.getCentreY(), 1.5f);
}

void OscillatorBackgroundPainter::drawOscillatorLabel(juce::Graphics& g, juce::Rectangle<float> raisedBounds)
{
    // Add "Oscillator 1" label to top center of module component
    auto labelBounds = juce::Rectangle<float>(raisedBounds.getCentreX() - 45, raisedBounds.getY() + 5, 90, 20);

    // Slightly lighter metallic background for label
    g.setColour(juce::Colour(0xff505050));
    g.fillRoundedRectangle(labelBounds, 4.0f);

    // Darker border for depth
    g.setColour(juce::Colour(0xff303030));
    g.drawRoundedRectangle(labelBounds, 4.0f, 1.0f);

    // Light highlight on top for metallic effect
    g.setColour(juce::Colour(0xff707070));
    g.drawLine(labelBounds.getX() + 4, labelBounds.getY() + 1,
               labelBounds.getRight() - 4, labelBounds.getY() + 1, 1.0f);

    // Engraved 3D text effect
    auto textBounds = labelBounds.toNearestInt();
    g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));

    // Dark shadow below and right for engraved effect
    g.setColour(juce::Colour(0xff303030));
    g.drawText("OSCILLATOR 1", textBounds.translated(1, 1), juce::Justification::centred, true);

    // Light highlight above and left for engraved effect
    g.setColour(juce::Colour(0xff808080));
    g.drawText("OSCILLATOR 1", textBounds.translated(-1, -1), juce::Justification::centred, true);

    // Main text
    g.setColour(juce::Colour(0xff606060));
    g.drawText("OSCILLATOR 1", textBounds, juce::Justification::centred, true);
}

void OscillatorBackgroundPainter::drawADSRKnobSlots(juce::Graphics& g, juce::Rectangle<int> adsrSection)
{
    auto adsrKnobWidth = (adsrSection.getWidth() - 45) / 4; // Same calculation as resized()

    // Attack knob slot - square metal
    auto attackArea = adsrSection.removeFromLeft(adsrKnobWidth);
    attackArea.removeFromTop(15); // less label space to move higher
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(attackArea.getCentreX() - 40, attackArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 39, attackArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(attackArea.getCentreX() - 37, attackArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    adsrSection.removeFromLeft(15); // spacing

    // Decay knob slot - square metal
    auto decayArea = adsrSection.removeFromLeft(adsrKnobWidth);
    decayArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(decayArea.getCentreX() - 40, decayArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 39, decayArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(decayArea.getCentreX() - 37, decayArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    adsrSection.removeFromLeft(15); // spacing

    // Sustain knob slot - square metal
    auto sustainArea = adsrSection.removeFromLeft(adsrKnobWidth);
    sustainArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(sustainArea.getCentreX() - 40, sustainArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 39, sustainArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(sustainArea.getCentreX() - 37, sustainArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    adsrSection.removeFromLeft(15); // spacing

    // Release knob slot - square metal
    auto releaseArea = adsrSection;
    releaseArea.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(releaseArea.getCentreX() - 40, releaseArea.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 39, releaseArea.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(releaseArea.getCentreX() - 37, releaseArea.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void OscillatorBackgroundPainter::drawVolumeKnobSlots(juce::Graphics& g, juce::Rectangle<int> volumeControlsSection)
{
    auto volumeKnobWidth = (volumeControlsSection.getWidth() - 45) / 4; // Width for each knob
    auto volumeSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    volumeSection.removeFromTop(15); // less label space
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(volumeSection.getCentreX() - 40, volumeSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 39, volumeSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(volumeSection.getCentreX() - 37, volumeSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    volumeControlsSection.removeFromLeft(15); // spacing

    // Detune knob slot - square metal
    auto detuneSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    detuneSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(detuneSection.getCentreX() - 40, detuneSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 39, detuneSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(detuneSection.getCentreX() - 37, detuneSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    volumeControlsSection.removeFromLeft(15); // spacing

    // Stereo Width knob slot - square metal
    auto stereoWidthSection = volumeControlsSection.removeFromLeft(volumeKnobWidth);
    stereoWidthSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(stereoWidthSection.getCentreX() - 40, stereoWidthSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 39, stereoWidthSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(stereoWidthSection.getCentreX() - 37, stereoWidthSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);

    volumeControlsSection.removeFromLeft(15); // spacing

    // Pan knob slot - square metal
    auto panSection = volumeControlsSection;
    panSection.removeFromTop(15);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(panSection.getCentreX() - 40, panSection.getCentreY() - 66, 80, 114, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(panSection.getCentreX() - 39, panSection.getCentreY() - 65, 78, 112, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(panSection.getCentreX() - 37, panSection.getCentreY() - 63, 74, 108, 2.0f, 1.0f);
}

void OscillatorBackgroundPainter::drawPhaseKnobSlot(juce::Graphics& g, juce::Rectangle<int> phaseRow)
{
    phaseRow.removeFromLeft(100); // random phase button
    phaseRow.removeFromLeft(15); // spacing
    auto phaseKnobArea = phaseRow.removeFromLeft(80);
    phaseKnobArea.removeFromTop(-5); // negative value to move background up even more
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(phaseKnobArea.getCentreX() - 40, phaseKnobArea.getCentreY() - 46, 80, 94, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 39, phaseKnobArea.getCentreY() - 45, 78, 92, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(phaseKnobArea.getCentreX() - 37, phaseKnobArea.getCentreY() - 43, 74, 88, 2.0f, 1.0f);
}

void OscillatorBackgroundPainter::drawTuningControlBoxes(juce::Graphics& g, juce::Rectangle<int> tuningSectionBounds)
{
    // Individual background boxes for tuning controls - center them
    auto tuningControlsRow = tuningSectionBounds.reduced(3, 3);
    tuningControlsRow.removeFromLeft(2); // Shift right slightly by reducing bounds
    tuningControlsRow.removeFromTop(-10); // Shift up by expanding bounds

    // Octave control box
    auto octaveControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(octaveControlArea.getCentreX() - 35, octaveControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 34, octaveControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(octaveControlArea.getCentreX() - 32, octaveControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);

    tuningControlsRow.removeFromLeft(10); // spacing

    // Semitone control box
    auto semitoneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(semitoneControlArea.getCentreX() - 35, semitoneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 34, semitoneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(semitoneControlArea.getCentreX() - 32, semitoneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);

    tuningControlsRow.removeFromLeft(10); // spacing

    // Fine tune control box
    auto fineTuneControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(fineTuneControlArea.getCentreX() - 35, fineTuneControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 34, fineTuneControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(fineTuneControlArea.getCentreX() - 32, fineTuneControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);

    tuningControlsRow.removeFromLeft(10); // spacing

    // Voice count control box
    auto voiceCountControlArea = tuningControlsRow.removeFromLeft(60);
    g.setColour(juce::Colour(0xff0f0f0f));
    g.fillRoundedRectangle(voiceCountControlArea.getCentreX() - 35, voiceCountControlArea.getCentreY() - 48, 70, 72, 4.0f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.8f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 34, voiceCountControlArea.getCentreY() - 47, 68, 70, 3.0f, 2.0f);
    g.setColour(juce::Colour(0xff404040).withAlpha(0.4f));
    g.drawRoundedRectangle(voiceCountControlArea.getCentreX() - 32, voiceCountControlArea.getCentreY() - 45, 64, 66, 2.0f, 1.0f);
}
