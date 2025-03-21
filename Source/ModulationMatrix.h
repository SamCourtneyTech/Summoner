#pragma once
#include <JuceHeader.h>

class ModulationMatrix {
public:
    enum class Source {
        None,
        LFO,
        Envelope
    };

    enum class Destination {
        None,
        FilterCutoff,
        Osc1Pitch,
        Osc2Pitch,
        Osc3Pitch
    };

    struct ModulationEntry {
        Source source = Source::None;
        Destination destination = Destination::None;
        float amount = 0.0f; // Range: -1.0 to 1.0
    };

    ModulationMatrix() {
        // Initialize with 4 modulation slots
        entries.resize(4);
    }

    void setEntry(int index, Source src, Destination dst, float amt) {
        if (index >= 0 && index < entries.size()) {
            entries[index].source = src;
            entries[index].destination = dst;
            entries[index].amount = juce::jlimit(-1.0f, 1.0f, amt);
        }
    }

    const ModulationEntry& getEntry(int index) const {
        return entries[index];
    }

    int getNumEntries() const { return entries.size(); }

private:
    std::vector<ModulationEntry> entries;
};