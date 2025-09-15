# Summoner VST

Summoner is a JUCE-based VST plugin that allows users to design sounds in an internal synthesizer using natural language input. It integrates OpenAI's ChatGPT to interpret user prompts and automatically generate parameter adjustments for the synthesizer engine.

## Features
- Natural language sound design prompts
- Automatic mapping of text prompts to synth parameters
- Real-time audio synthesis within the plugin

## Requirements
- [JUCE Framework](https://juce.com/) (required to build and run)
- C++17 or newer
- A valid OpenAI API key (for ChatGPT integration)
- Compatible VST3 host (e.g., Ableton Live, Reaper, Logic Pro)

## Building
1. Install JUCE and set it up on your system.
2. Clone this repository:
   ```bash
   git clone https://github.com/your-username/Summoner-VST.git
