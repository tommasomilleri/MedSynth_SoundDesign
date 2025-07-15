
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
    void enablePluckNoise(bool shouldEnable) { pluckNoiseEnabled = shouldEnable; }

  private:
    // core DSP
    juce::dsp::IIR::Filter<float> outputFilter;

    juce::dsp::DelayLine<float> pluckDelay{2 * 48000};
    int delaySamples = 0;


    // ADSR per ampiezza e per cutoff
    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    // burst di rumore all’attacco
    juce::AudioBuffer<float> noiseBuffer;
    int noisePosition = 0;
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

    // note state
    bool noteIsActive = false;

    double currentSampleRate = 44100.0;
    float velocityLevel = 1.0f;
    bool pluckNoiseEnabled = false;
    double noteFrequency = 440.0;
    juce::dsp::IIR::Filter<float> bodyFilter1, bodyFilter2;
    juce::dsp::IIR::Filter<float> bodyFilter3;

    juce::dsp::IIR::Filter<float> voiceFilter;
    juce::dsp::IIR::Filter<float> lowShelfFilter;


    juce::dsp::Oscillator<float> osc1{[](float x) { return std::sin(x); }};
    juce::dsp::Oscillator<float> osc2{[](float x) { return std::sin(x); }}; // square/triangle



    juce::IIRFilter highPassFilter;




    /* 
    static constexpr int numPartials = 6;
    juce::dsp::Oscillator<float> partialOscs[numPartials];
    float partialAmps[8] = {
        1.00f, // fondamentale
        0.80f, // 2ª arma
        0.70f, // 3ª arma
        0.60f, // 4ª arma
        0.50f, // 5ª arma
        0.40f, // 6ª arma
        0.30f, // 7ª arma
        0.20f  // 8ª arma
    };*/
    static constexpr int numPartials = 8;
    juce::dsp::Oscillator<float> partialOscs[numPartials];

    float partialAmps[numPartials] = {
        1.00f, // f₀
        0.75f, // 2ª armonica
        0.65f, // 3ª
        0.55f, // 4ª
        0.45f, // 5ª
        0.35f, // 6ª
        0.25f, // 7ª
        0.15f  // 8ª
    };
    
    bool noiseBurstActive = false;
    int noiseBurstSamplesRemaining = 0;
    int noiseBurstTotalSamples = 0;
    float noiseBurstGain = 0.03f; 


    float lfoPhase = 0.0f;
    float lfoRate = 3.0f; // 3 Hz





    InstrumentConfig *config = nullptr;

    
    // (puoi renderli letti da InstrumentConfig)

    juce::ADSR harmonicEnv; // un inviluppo breve per le armoniche
    juce::ADSR::Parameters harmonicEnvParams;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
