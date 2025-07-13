#include <JuceHeader.h> // << Aggiungi questa riga PRIMA DI TUTTO
#include"SynthVoice.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <atomic>
#include"LuteConfig.h"
#include "RebecConfig.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

JuceSynthFrameworkAudioProcessor::JuceSynthFrameworkAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         )
#endif
      ,
      tree(*this, nullptr, "PARAMETERS",
           {std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 5000.0f), 0.1f),
            std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(1.0f, 2000.0f), 1.0f),
            std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f),
            std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.1f, 5000.0f), 0.1f),
            std::make_unique<juce::AudioParameterFloat>("wavetype", "WaveType", juce::NormalisableRange<float>(0.0f, 2.0f), 0.0f),
            std::make_unique<juce::AudioParameterFloat>("wavetype2", "WaveType2", juce::NormalisableRange<float>(0.0f, 2.0f), 0.0f),
            std::make_unique<juce::AudioParameterFloat>("filterType", "FilterType", juce::NormalisableRange<float>(0.0f, 2.0f), 0.0f),
            std::make_unique<juce::AudioParameterFloat>("filterCutoff", "FilterCutoff", juce::NormalisableRange<float>(20.0f, 10000.0f), 400.0f),
            std::make_unique<juce::AudioParameterFloat>("filterRes", "FilterRes", juce::NormalisableRange<float>(1.0f, 5.0f), 1.0f),
            std::make_unique<juce::AudioParameterFloat>("blend", "Osc2Blend", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f),
            std::make_unique<juce::AudioParameterFloat>("masterGain", "MasterGain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f),
            std::make_unique<juce::AudioParameterFloat>("pbup", "PBup", juce::NormalisableRange<float>(1.0f, 12.0f), 2.0f),
            std::make_unique<juce::AudioParameterFloat>("pbdown", "PBdown", juce::NormalisableRange<float>(1.0f, 12.0f), 2.0f),
            std::make_unique<juce::AudioParameterFloat>("reverbRoom", "Reverb Room", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f),
            std::make_unique<juce::AudioParameterFloat>("reverbDamp", "Reverb Damping", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f),
            std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", juce::NormalisableRange<float>(0.0f, 10.0f), 1.5f),
            std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", juce::NormalisableRange<float>(0.0f, 1.0f), 0.3f),
            std::make_unique<juce::AudioParameterFloat>("smoothingSamples", "Smoothing Samples", juce::NormalisableRange<float>(1.0f, 1000.0f), 100.0f),}) {
    setInstrument(InstrumentType::Lute);
}


JuceSynthFrameworkAudioProcessor::~JuceSynthFrameworkAudioProcessor()
{
}


const String JuceSynthFrameworkAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JuceSynthFrameworkAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JuceSynthFrameworkAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JuceSynthFrameworkAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}


double JuceSynthFrameworkAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JuceSynthFrameworkAudioProcessor::getNumPrograms()
{
    return 1;
}

int JuceSynthFrameworkAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JuceSynthFrameworkAudioProcessor::setCurrentProgram (int index)
{
}

const String JuceSynthFrameworkAudioProcessor::getProgramName (int index)
{
    return {};
}

void JuceSynthFrameworkAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void JuceSynthFrameworkAudioProcessor::prepareToPlay(double sampleRate,
                                                     int samplesPerBlock) {
    const int numChannels = getTotalNumOutputChannels();
    lastSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec{
        lastSampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(numChannels)};
    luteReverb.loadImpulseResponse (File("/path/to/lute_ir.wav"),
                                juce::dsp::Convolution::Stereo::yes,
                                juce::dsp::Convolution::Trim::yes,
                                2048);
luteReverb.prepare(spec);

// in processBlock(), dopo il mix del synth:

    
    stateVariableFilter.reset();
    stateVariableFilter.prepare(spec);
    updateFilter();
    // --- Prepara il filtro delle voci (dspFilt) ---
    dspFilt.reset();
    dspFilt.prepare(spec);
    updateFilter();
    reverb.reset(); // facoltativo: azzera lo stato interno del riverbero
    chorus.reset(); // azzera lo stato interno del chorus
    chorus.prepare(spec);


    upsampler.reset();
    downsampler.reset();


    upsampler.initProcessing((size_t)samplesPerBlock);
    downsampler.initProcessing((size_t)samplesPerBlock);
    
    // --- Prepara il synth e le sue voci ---
    mySynth.clearVoices();
    mySynth.setNoteStealingEnabled(true);
    for (int i = 0; i < 5; ++i) {
        auto *voice = new SynthVoice();
        voice->enableBowNoise(currentInstrument == InstrumentType::Rebec);
        voice->setConfig(currentConfig.get());
        voice->prepareToPlay(lastSampleRate, samplesPerBlock, numChannels);
        mySynth.addVoice(voice);
    }
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());
    mySynth.setCurrentPlaybackSampleRate(lastSampleRate);

    // --- Prepara il filtro globale (stateVariableFilter) ---
    stateVariableFilter.reset();
    stateVariableFilter.prepare(spec);
    updateFilter();
}
void JuceSynthFrameworkAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                    juce::MidiBuffer &midiMessages) {
    /*
    DBG(">> processBlock! numSamples=" << buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    buffer.clear();
    keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);

    mySynth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    juce::dsp::AudioBlock<float> block(buffer);
    auto upBlock = upsampler.processSamplesUp(block);

    juce::dsp::ProcessContextReplacing<float> ctx(block);

    stateVariableFilter.process(juce::dsp::ProcessContextReplacing<float>(upBlock));

    reverbParameters.roomSize = *tree.getRawParameterValue("reverbRoom");
    reverbParameters.damping = *tree.getRawParameterValue("reverbDamp");
    reverb.setParameters(reverbParameters);
    reverb.processStereo(upBlock.getChannelPointer(0), upBlock.getChannelPointer(1), numSamples);

    chorus.setRate(*tree.getRawParameterValue("chorusRate"));
    chorus.setDepth(*tree.getRawParameterValue("chorusDepth"));
    chorus.process(juce::dsp::ProcessContextReplacing<float>(upBlock));

    float targetGain = *tree.getRawParameterValue("masterGain");
    masterGainSmoothed.setTargetValue(targetGain);
    int smoothSamples = static_cast<int>(*tree.getRawParameterValue("smoothingSamples"));
    masterGainSmoothed.skip(smoothSamples);

    float gain = masterGainSmoothed.getNextValue();
    upBlock.multiplyBy(gain);

    downsampler.processSamplesDown(upBlock);*/
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    buffer.clear();
    keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
    mySynth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    dsp::AudioBlock<float> block(buffer);
    luteReverb.process(juce::dsp::ProcessContextReplacing<float>(block));

    dsp::ProcessContextReplacing<float> ctx(block);

    stateVariableFilter.process(ctx);

    reverbParameters.roomSize = *tree.getRawParameterValue("reverbRoom");
    reverbParameters.damping = *tree.getRawParameterValue("reverbDamp");
    reverb.setParameters(reverbParameters);
    reverb.processStereo(buffer.getWritePointer(0),
                         buffer.getWritePointer(1),
                         numSamples);


    chorus.process(ctx);

    float targetGain = *tree.getRawParameterValue("masterGain");
    masterGainSmoothed.setTargetValue(targetGain);

    float gain = masterGainSmoothed.getNextValue();
    block.multiplyBy(gain);
}
void JuceSynthFrameworkAudioProcessor::updateFilter()
{
    int menuChoice = *tree.getRawParameterValue("filterType");
    int freq = *tree.getRawParameterValue("filterCutoff");
    int res = *tree.getRawParameterValue("filterRes");
    
    if (menuChoice == 0)
        stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    else if (menuChoice == 1)
        stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    else
        stateVariableFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

    stateVariableFilter.setCutoffFrequency(freq);
    stateVariableFilter.setResonance(res);
}




bool JuceSynthFrameworkAudioProcessor::hasEditor() const
{
    return true;
}


AudioProcessorEditor* JuceSynthFrameworkAudioProcessor::createEditor()
{
    return new JuceSynthFrameworkAudioProcessorEditor(*this);
}


void JuceSynthFrameworkAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
}

void JuceSynthFrameworkAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
}
void JuceSynthFrameworkAudioProcessor::setInstrument(InstrumentType newInstrument)
{
    currentInstrument = newInstrument;

    switch (newInstrument)
    {
    case InstrumentType::Lute:
        currentConfig = std::make_unique<LuteConfig>();
        break;

    case InstrumentType::Rebec:
        currentConfig = std::make_unique<RebecConfig>();
        break;

        // QUA SARANNO DA AGGIUNGERE ALTRI STRUMENTI
    }

    for (int i = 0; i < mySynth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(mySynth.getVoice(i)))
        {
            voice->setConfig(currentConfig.get());

 
            voice->enableBowNoise(currentInstrument == InstrumentType::Rebec);
        }
    }
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceSynthFrameworkAudioProcessor();
}
void JuceSynthFrameworkAudioProcessor::releaseResources()
{
    // Se non hai risorse da liberare, lasciala vuota
}
#ifndef JucePlugin_PreferredChannelConfigurations
bool JuceSynthFrameworkAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    // se non sei un synth, l’input deve corrispondere all’output
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif
