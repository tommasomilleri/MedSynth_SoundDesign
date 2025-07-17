
#pragma once

#include "GuitarString.h"
#include "InstrumentConfig.h"
#include "SynthSound.h"
#include <JuceHeader.h>
#include <vector>

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

    float osc1Transpose = 0.0f;

  private:
    juce::dsp::IIR::Filter<float> outputFilter;

    juce::dsp::DelayLine<float> pluckDelay{2 * 48000};
    int delaySamples = 0;

    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    juce::AudioBuffer<float> noiseBuffer;
    int noisePosition = 0;
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

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
    juce::dsp::Oscillator<float> osc2{[](float x) { return std::sin(x); }}; 

    juce::IIRFilter highPassFilter;

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
    float lfoRate = 3.0f;

    InstrumentConfig *config = nullptr;


    juce::ADSR harmonicEnv;
    juce::ADSR::Parameters harmonicEnvParams;

    std::unique_ptr<GuitarString> guitarString;
    bool useGuitar = false;

    std::vector<std::unique_ptr<GuitarString>> corde;
    std::vector<float> detuneAmounts;
    void setupMedievalGuitar(float freq, double sampleRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};