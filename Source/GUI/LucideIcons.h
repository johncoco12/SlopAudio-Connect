

#pragma once
#include <JuceHeader.h>
#include <memory>
#include <cstddef>

namespace LucideIcons {

static constexpr std::size_t ICON_COUNT = 1713;

extern const char* const NAMES[ICON_COUNT];

void draw(juce::Graphics&         g,
          juce::StringRef          name,
          juce::Rectangle<float>   bounds,
          juce::Colour             colour  = juce::Colours::white,
          float                    opacity = 1.0f);

std::unique_ptr<juce::Drawable>
createDrawable(juce::StringRef name,
               juce::Colour    colour = juce::Colours::white);

juce::String getRawSvg(juce::StringRef name);

inline void drawAt(juce::Graphics& g,
                   juce::StringRef  name,
                   float            centreX,
                   float            centreY,
                   float            size,
                   juce::Colour     colour  = juce::Colours::white,
                   float            opacity = 1.0f)
{
    draw(g, name,
         { centreX - size * 0.5f, centreY - size * 0.5f, size, size },
         colour, opacity);
}

inline void drawAt(juce::Graphics& g,
                   juce::StringRef  name,
                   float            x,
                   float            y,
                   float            size,
                   juce::Colour     colour,
                   juce::RectanglePlacement placement,
                   float            opacity = 1.0f)
{
    auto d = createDrawable(name, colour);
    if (d) d->drawWithin(g, { x, y, size, size }, placement, opacity);
}

}
