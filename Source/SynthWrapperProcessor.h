
#pragma once
#include <JuceHeader.h>

class SynthWrapperProcessor : public juce::AudioProcessor {
public:
    SynthWrapperProcessor(juce::Synthesiser& synth) : internalSynth(synth) {}

    const juce::String getName() const override { return "SynthWrapper"; }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
        internalSynth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
        buffer.clear();
        internalSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override {
        return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
    }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

private:
    juce::Synthesiser& internalSynth;
};
