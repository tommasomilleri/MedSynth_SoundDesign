#pragma once

#include "InstrumentConfig.h"
#include "InstrumentType.h"
#include "LuteConfig.h"
#include "RebecConfig.h"
#include "SynthSound.h"
#include "SynthVoice.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

class JuceSynthFrameworkAudioProcessor : public juce::AudioProcessor {
  public:
    // restituisce lo strumento corrente
    InstrumentType getCurrentInstrument() const noexcept { return currentInstrument; }

    // espone il ValueTreeState
    juce::AudioProcessorValueTreeState &getValueTreeState() noexcept { return tree; }

    // tastiera MIDI
    juce::MidiKeyboardState keyboardState;

    JuceSynthFrameworkAudioProcessor();
    ~JuceSynthFrameworkAudioProcessor() override;

    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const String getProgramName(int) override;
    void changeProgramName(int, const String &) override;
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
    void releaseResources() override;
    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *, int) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer &midiMessages) override;
    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;
    void setInstrument(InstrumentType newInstrument);
    void updateFilter();

  private:
    // --- synth interno e voce di supporto ---
    juce::Synthesiser mySynth;
    std::unique_ptr<InstrumentConfig> currentConfig;
    InstrumentType currentInstrument = InstrumentType::Lute;

    // --- parametri e struttura del plugin ---
    juce::AudioProcessorValueTreeState tree;

    // --- filtri e catena DSP globale ---
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        dspFilt;

    // riverbero e chorus
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParameters;
    juce::dsp::Chorus<float> chorus;

    // master gain con smoothing
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
        masterGainSmoothed{1.0f};

    // oversampling 2× (disabilita pure se dà problemi)
    juce::dsp::Oversampling<float> upsampler
    { (size_t) getTotalNumOutputChannels(),
      2,
      juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };

// il downsampler corrispondente
    
    juce::dsp::Oversampling<float> downsampler
    { (size_t) getTotalNumOutputChannels(),
      2,
      juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR };
    juce::dsp::Convolution luteReverb;

    // filtro State-Variable TPT
    juce::dsp::StateVariableTPTFilter<float> stateVariableFilter;

    double lastSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessor)
};
