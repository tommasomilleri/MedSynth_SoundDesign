#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "InstrumentConfig.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice(); // serve un costruttore vuoto
    ~SynthVoice() override {}
    void prepareToPlay(double newSampleRate, int samplesPerBlock, int numChannels);
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int pitchWheelPos) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void setConfig(InstrumentConfig *newConfig);
    void enableBowNoise(bool shouldEnable);
  private:

    
    // Filtro di uscita
    juce::dsp::IIR::Filter<float>     outputFilter;
    juce::dsp::Oscillator<float> oscillator{[](float x) { return std::sin(x); }};
    // Note state
    bool                              noteOn = false, voiceIsActive = false;
    double                            sampleRate = 44100.0;
    float                             basePressure = 0.0f;
    bool bowNoiseEnabled = false;
    double currentFrequency = 440.0;

    InstrumentConfig*                 config = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthVoice)
};
