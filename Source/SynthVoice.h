
/*#pragma once
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
};*/
/*
#pragma once

#include "InstrumentConfig.h"
#include "SynthSound.h"
#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice {
  public:
    SynthVoice() {}
    ~SynthVoice() override {}

    void prepareToPlay(double newSampleRate, int samplesPerBlock, int numChannels);
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *, int pitchWheelPos) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override {}
    void controllerMoved(int controllerNumber, int newValue) override {}
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    void setConfig(InstrumentConfig *newConfig) { config = newConfig; }
    void enableBowNoise(bool shouldEnable) { bowNoiseEnabled = shouldEnable; }

  private:
    juce::dsp::DelayLine<float> ksDelay{48000}; // max 1s a 48kHz
    // core DSP
    juce::dsp::Oscillator<float> oscillator{[](float x) { return std::sin(x); }};
    juce::dsp::IIR::Filter<float> outputFilter;

    // ADSR per ampiezza e per cutoff
    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    // burst di rumore all’attacco
    juce::AudioBuffer<float> noiseBuffer;
    int noisePos = 0;
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

    // note state
    bool noteOn = false, voiceIsActive = false;
    double sampleRate = 44100.0;
    float basePressure = 1.0f;
    bool bowNoiseEnabled = false;
    double currentFrequency = 440.0;

    InstrumentConfig *config = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
*/
#pragma once

#include "InstrumentConfig.h"
#include "SynthSound.h"
#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice {
  public:
    SynthVoice() {}
    ~SynthVoice() override {}

    void prepareToPlay(double newSampleRate, int samplesPerBlock, int numChannels);
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *, int pitchWheelPos) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newValue) override {}
    void controllerMoved(int controllerNumber, int newValue) override {}
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    void setConfig(InstrumentConfig *newConfig) { config = newConfig; }
    void enableBowNoise(bool shouldEnable) { bowNoiseEnabled = shouldEnable; }

  private:
    // core DSP
    juce::dsp::IIR::Filter<float> outputFilter;

    juce::dsp::DelayLine<float> ksDelay{2 * 48000};
    int delaySamples = 0;


    // ADSR per ampiezza e per cutoff
    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    // burst di rumore all’attacco
    juce::AudioBuffer<float> noiseBuffer;
    int noisePos = 0;
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

    // note state
    bool noteOn = false;
    double sampleRate = 44100.0;
    float basePressure = 1.0f;
    bool bowNoiseEnabled = false;
    double currentFrequency = 440.0;
    juce::dsp::IIR::Filter<float> bodyFilter1, bodyFilter2;
    juce::dsp::IIR::Filter<float> bodyFilter3;
    juce::dsp::IIR::Filter<float> voiceFilter;
    juce::dsp::IIR::Filter<float> lowShelfFilter;
    juce::dsp::Oscillator<float> oscillator{[](float x) { return std::sin(x); }};
    juce::dsp::Oscillator<float> oscillator2{[](float x) { return std::sin(x); }}; // square/triangle

    InstrumentConfig *config = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
