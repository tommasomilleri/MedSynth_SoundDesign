
/** \file JuceSynthFrameworkAudioProcessor.cpp
 *  \brief Implementazione principale del processore audio del sintetizzatore.
 *
 *  In questo file definiamo come inizializzare e gestire tutte le
 *  componenti DSP del plugin, dal caricamento delle risposte impulsive
 *  fino al rendering delle voci MIDI con effetti di riverbero e filtri.
 */

#include <JuceHeader.h>
#include"SynthVoice.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <atomic>
#include"LuteConfig.h"
#include "RebecConfig.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
/**
 *  \brief Costruisce il processore audio, configurando bus e parametri.
 *
 *  Nel costruttore impostiamo i bus di input/output in stereo quando
 *  il plugin non è un synth puro, e inizializziamo un AudioProcessorValueTreeState
 *  per gestire i parametri ADSR, filtri e guadagno master. Infine, selezioniamo
 *  il "Lute" come strumento di default.
 */
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
            std::make_unique<juce::AudioParameterFloat>("reverbRoom", "Reverb Room", juce::NormalisableRange<float>(0.5f, 0.8f), 0.1f),
            std::make_unique<juce::AudioParameterFloat>("reverbDamp", "Reverb Damping", juce::NormalisableRange<float>(0.5f, 0.8f), 0.5f),
            std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", juce::NormalisableRange<float>(0.0f, 0.0f), 0.0f),
            std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", juce::NormalisableRange<float>(0.0f, 0.0f), 0.0f),
            std::make_unique<juce::AudioParameterFloat>("smoothingSamples", "Smoothing Samples", juce::NormalisableRange<float>(1.0f, 1000.0f), 100.0f),}) {
    setInstrument(InstrumentType::Lute);
}

/**
 *  \brief Distruttore di default, non rilascia risorse extra.
 */
JuceSynthFrameworkAudioProcessor::~JuceSynthFrameworkAudioProcessor() = default;

/**
 *  \brief Restituisce il nome del plugin (definito in Projucer).
 *  \return Stringa con JucePlugin_Name.
 */
const juce::String JuceSynthFrameworkAudioProcessor::getName() const { return JucePlugin_Name; }

/**
 *  \brief Indica se questo plugin accetta input MIDI.
 *  \return true se JucePlugin_WantsMidiInput è definito.
 */
bool JuceSynthFrameworkAudioProcessor::acceptsMidi() const { return JucePlugin_WantsMidiInput; }

/**
 *  \brief Indica se questo plugin produce output MIDI.
 *  \return true se JucePlugin_ProducesMidiOutput è definito.
 */
bool JuceSynthFrameworkAudioProcessor::producesMidi() const { return JucePlugin_ProducesMidiOutput; }

/**
 *  \brief Indica se si tratta di un effetto MIDI puro.
 *  \return true se JucePlugin_IsMidiEffect è definito.
 */
bool JuceSynthFrameworkAudioProcessor::isMidiEffect() const { return JucePlugin_IsMidiEffect; }

/**
 *  \brief Restituisce il tempo di tail-off in secondi.
 *  \return 0.0, in quanto non gestiamo residuali.
 */
double JuceSynthFrameworkAudioProcessor::getTailLengthSeconds() const { return 0.0; }

/**
 *  \brief Numero di programmi (preset) implementati.
 *  \return 1 (unico preset).
 */
int JuceSynthFrameworkAudioProcessor::getNumPrograms() { return 1; }

/**
 *  \brief Indice del programma corrente.
 *  \return 0, poiché abbiamo un solo preset.
 */
int JuceSynthFrameworkAudioProcessor::getCurrentProgram() { return 0; }

/**
 *  \brief Seleziona il programma corrente.
 *  \param index Indice del preset (ignorato).
 */
void JuceSynthFrameworkAudioProcessor::setCurrentProgram(int /* index */) {}

/**
 *  \brief Restituisce il nome di un programma.
 *  \param index Indice del preset (ignorato).
 *  \return Stringa vuota.
 */
const juce::String JuceSynthFrameworkAudioProcessor::getProgramName(int /* index */) { return {}; }

/**
 *  \brief Cambia nome di un programma (non implementato).
 *  \param index   Indice del preset.
 *  \param newName Nuovo nome (ignorato).
 */
void JuceSynthFrameworkAudioProcessor::changeProgramName(int /* index */, const juce::String & /* newName */) {}

/**
 *  \brief Setup iniziale prima di riprodurre audio.
 *  \param sampleRate       Frequenza di campionamento in Hz.
 *  \param samplesPerBlock  Dimensione del blocco di elaborazione.
 *
 *  In questa fase creiamo un ProcessSpec con sample rate e canali,
 *  carichiamo un impulse response per il riverbero del liuto se disponibile,
 *  e prepariamo tutti gli oggetti DSP e le voci MIDI affinché siano pronti
 *  per il rendering in processBlock().
 */

void JuceSynthFrameworkAudioProcessor::prepareToPlay(double sampleRate,
                                                     int samplesPerBlock) {
    const int numChannels = getTotalNumOutputChannels();
    lastSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec{
        lastSampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(numChannels)};
    
    auto exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    auto bundleDir = exeFile.getParentDirectory();

    auto irFile = bundleDir
                      .getChildFile("IR")
                      .getChildFile("lute_ir.wav");

    if (irFile.existsAsFile()) {
        luteReverb.loadImpulseResponse(irFile,
                                       juce::dsp::Convolution::Stereo::yes,
                                       juce::dsp::Convolution::Trim::yes,
                                       2048);
    } else {
        DBG("IR non trovato: " << irFile.getFullPathName());
    }

    luteReverb.prepare(spec);

    lowShelfFilter.reset();
lowShelfFilter.prepare(spec);


    
    stateVariableFilter.reset();
    stateVariableFilter.prepare(spec);
    updateFilter();
    dspFilt.reset();
    dspFilt.prepare(spec);
    updateFilter();
    reverb.reset();
    chorus.reset();
    chorus.prepare(spec);


    upsampler.reset();
    downsampler.reset();


    upsampler.initProcessing((size_t)samplesPerBlock);
    downsampler.initProcessing((size_t)samplesPerBlock);
    mySynth.clearVoices();
    mySynth.setNoteStealingEnabled(true);
    for (int i = 0; i < 5; ++i) {
        auto *voice = new SynthVoice();
        voice->enablePluckNoise(currentInstrument == InstrumentType::Rebec);
        voice->setConfig(currentConfig.get());
        voice->prepareToPlay(lastSampleRate, samplesPerBlock, numChannels);
        mySynth.addVoice(voice);
    }
    mySynth.clearSounds();
    mySynth.addSound(new SynthSound());
    mySynth.setCurrentPlaybackSampleRate(lastSampleRate);

    stateVariableFilter.reset();
    stateVariableFilter.prepare(spec);
    updateFilter();
}
/**
 *  \brief Renderizza un blocco audio e processa il MIDI.
 *  \param buffer       Buffer audio di output.
 *  \param midiMessages Buffer di eventi MIDI in ingresso.
 *
 *  In questo metodo chiamiamo renderNextBlock su mySynth per generare il suono
 *  delle voci, applichiamo il riverbero convolutivo del liuto e il riverbero
 *  standard di JUCE, infine moduliamo il guadagno master con smoothing.
 */
void JuceSynthFrameworkAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                    juce::MidiBuffer &midiMessages) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    buffer.clear();
    keyboardState.processNextMidiBuffer(midiMessages, 0, numSamples, true);
    mySynth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    dsp::AudioBlock<float> block(buffer);
    luteReverb.process(juce::dsp::ProcessContextReplacing<float>(block));

    reverbParameters.roomSize = *tree.getRawParameterValue("reverbRoom");
    reverbParameters.damping = *tree.getRawParameterValue("reverbDamp");
    reverb.setParameters(reverbParameters);
    reverb.processStereo(buffer.getWritePointer(0),
                         buffer.getWritePointer(1),
                         numSamples);

    float targetGain = *tree.getRawParameterValue("masterGain");
    masterGainSmoothed.setTargetValue(targetGain);

    float gain = masterGainSmoothed.getNextValue();
    block.multiplyBy(gain);
}
/**
 *  \brief Aggiorna i parametri del filtro State Variable e shelving.
 */
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
   
stateVariableFilter.setCutoffFrequency (2400.0f);
stateVariableFilter.setResonance        (0.7f);

lowShelfFilter.state =
    *juce::dsp::IIR::Coefficients<float>::makeLowShelf (
        lastSampleRate,
        200.0f,
        0.8f,
        juce::Decibels::decibelsToGain (6.0f));

}
/**
 *  \brief Indica che il plugin offre un'interfaccia GUI personalizzata.
 *  \return true.
 */
bool JuceSynthFrameworkAudioProcessor::hasEditor() const { return true; }

/**
 *  \brief Crea l'editor grafico associato a questo processore.
 *  \return Nuova istanza di JuceSynthFrameworkAudioProcessorEditor.
 */
juce::AudioProcessorEditor *JuceSynthFrameworkAudioProcessor::createEditor() { return new JuceSynthFrameworkAudioProcessorEditor(*this); }

/**
 *  \brief Serializza lo stato corrente del plugin.
 */
void JuceSynthFrameworkAudioProcessor::getStateInformation(juce::MemoryBlock & /*destData*/) {}

/**
 *  \brief Carica lo stato del plugin da dati serializzati.
 */
void JuceSynthFrameworkAudioProcessor::setStateInformation(const void * /*data*/, int /*size*/) {}

/**
 *  \brief Cambia lo strumento corrente e aggiorna la configurazione delle voci.
 */
void JuceSynthFrameworkAudioProcessor::setInstrument(InstrumentType newInstrument) {
    currentInstrument = newInstrument;
    if (newInstrument == InstrumentType::Lute)
        currentConfig = std::make_unique<LuteConfig>();
    else
        currentConfig = std::make_unique<RebecConfig>();

    for (int i = 0; i < mySynth.getNumVoices(); ++i)
        if (auto *voice = dynamic_cast<SynthVoice *>(mySynth.getVoice(i)))
            voice->setConfig(currentConfig.get());
}

/**
 *  \brief Crea l'istanza del plugin per JUCE.
 *  \return Nuovo oggetto JuceSynthFrameworkAudioProcessor.
 */
AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new JuceSynthFrameworkAudioProcessor(); }

/**
 *  \brief Metodo chiamato quando il plugin viene stoppato.
 */
void JuceSynthFrameworkAudioProcessor::releaseResources() {}

/**
 *  \brief Verifica la compatibilità del layout dei bus audio.
 */
bool JuceSynthFrameworkAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    auto mainOutput = layouts.getMainOutputChannelSet();
    if (mainOutput != juce::AudioChannelSet::mono() && mainOutput != juce::AudioChannelSet::stereo())
        return false;
#if !JucePlugin_IsSynth
    if (layouts.getMainInputChannelSet() != mainOutput)
        return false;
#endif
    return true;
}