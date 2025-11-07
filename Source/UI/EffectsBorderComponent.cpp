#include "EffectsBorderComponent.h"

EffectsBorderComponent::EffectsBorderComponent()
{
}

void EffectsBorderComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Draw realistic 3D metallic border around the effects module (even larger border)
    // Outermost dark frame (thicker)
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawRect(bounds, 6);

    // Second layer - dark gray
    g.setColour(juce::Colour(0xff303030));
    g.drawRect(bounds.reduced(6), 3);

    // Third layer - medium gray
    g.setColour(juce::Colour(0xff505050));
    g.drawRect(bounds.reduced(9), 2);

    // Inner light highlight (metallic shine)
    g.setColour(juce::Colour(0xff909090));
    g.drawRect(bounds.reduced(11), 2);

    // Inner shadow
    g.setColour(juce::Colour(0xff0a0a0a));
    g.drawRect(bounds.reduced(13), 1);
}
