/*! \file JuceSynthFrameworkAudioProcessor.h
 *  Dichiarazione del processore audio principale per il synth.
 *
 *  Gestisce la sintesi MIDI, il routing audio, i parametri UTPS,
 *  filtri DSP, riverbero, chorus e oversampling.
 */
#pragma once

#include "InstrumentConfig.h"
#include "InstrumentType.h"
#include "LuteConfig.h"
#include "RebecConfig.h"
#include "SynthSound.h"
#include "SynthVoice.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>
/**
 *  \class JuceSynthFrameworkAudioProcessor
 *  Classe che estende juce::AudioProcessor per realizzare il synth.
 *
 *  Contiene il JuceSynth, il ValueTreeState per parametri,
 *  gli effetti audio (filtro, riverbero, chorus), oversampling
 *  e la logica di cambio strumento.
 */
class JuceSynthFrameworkAudioProcessor : public juce::AudioProcessor {
  public:
    /**
     *  Restituisce lo strumento corrente selezionato.
     *  \return Enumerazione InstrumentType del currentInstrument.
     */
    InstrumentType getCurrentInstrument() const noexcept { return currentInstrument; }
    /**
     *  Restituisce il riferimento al ValueTreeState per i parametri.
     *  \return Riferimento a juce::AudioProcessorValueTreeState.
     */
    juce::AudioProcessorValueTreeState &getValueTreeState() noexcept { return tree; }
    /**
     *  Stato attuale della tastiera MIDI interna.
     *  \note Utilizzato per disegno dell'editor e gestione degli eventi MIDI.
     */
    juce::MidiKeyboardState keyboardState;

    /** Costruttore: registra parametri e inizializza il synth. */
    JuceSynthFrameworkAudioProcessor();
    /** Distruttore virtuale. */
    ~JuceSynthFrameworkAudioProcessor() override;

    /** Nome del plugin. */
    const juce::String getName() const override;
    /** True se accetta input MIDI. */
    bool acceptsMidi() const override;
    /** True se genera output MIDI. */
    bool producesMidi() const override;
    /** True se è un effetto MIDI. */
    bool isMidiEffect() const override;
    /** Tempo di tail-end in secondi. */
    double getTailLengthSeconds() const override;

    /** Numero di programmi (preset) disponibili. */
    int getNumPrograms() override;
    /** Indice del programma corrente. */
    int getCurrentProgram() override;
    /** Seleziona il programma in base all'indice. */
    void setCurrentProgram(int index) override;
    /** Nome del programma dato l'indice. */
    const juce::String getProgramName(int index) override;
    /** Cambia il nome di un programma. */
    void changeProgramName(int index, const juce::String &newName) override;

    /**
     *  Verifica se il layout bus è supportato.
     *  layouts Struttura BusesLayout da validare.
     *  \return True se supportato.
     */
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    /** Rilascio risorse non più necessarie. */
    void releaseResources() override;

    /**
     *  Serializza lo stato per salvataggio preset/DAW.
     *  destData MemoryBlock di destinazione.
     */
    void getStateInformation(juce::MemoryBlock &destData) override;
    /**
     *  Deserializza lo stato da DAW o file.
     *  data Puntatore ai dati.
     *  size Dimensione dei dati.
     */
    void setStateInformation(const void *data, int size) override;

    /**
     *  Prepara il processore per il playback.
     *  sampleRate Frequenza di campionamento (Hz).
     *  samplesPerBlock Dimensione del blocco audio.
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    /**
     *  Processa blocchi audio e MIDI.
     *  buffer Buffer audio di input/output.
     *  midiMessages Buffer MIDI in ingresso.
     */
    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer &midiMessages) override;

    /** Crea l'editor grafico custom. */
    juce::AudioProcessorEditor *createEditor() override;
    /** Indica se è disponibile un editor GUI. */
    bool hasEditor() const override;

    /**
     *  Cambia lo strumento corrente.
     *  newInstrument Tipo di strumento da impostare.
     */
    void setInstrument(InstrumentType newInstrument);

    /** Aggiorna i coefficienti del filtro DSP in base ai parametri correnti. */
    void updateFilter();

  private:
    /** Sintetizzatore interno con voci multiple. */
    juce::Synthesiser mySynth;

    /** Configurazione attuale per lo strumento. */
    std::unique_ptr<InstrumentConfig> currentConfig;
    /** Strumento corrente (default: Lute). */
    InstrumentType currentInstrument = InstrumentType::Lute;

    /** ValueTreeState per parametri utente.
     *  \note Gestisce parametri audio e MIDI.
     */
    juce::AudioProcessorValueTreeState tree;

    /**
     *  Filtro DSP duplicato per cutoff/resonance.
     *  \tparam FilterType Tipo di filtro IIR.
     */
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        dspFilt;

    /** Riverbero di tipo JUCE predefinito. */
    juce::Reverb reverb;
    /** Parametri del riverbero. */
    juce::Reverb::Parameters reverbParameters;

    /** Effetto Chorus DSP. */
    juce::dsp::Chorus<float> chorus;

    /**
     *  Valore di guadagno master liscio.
     *  \note Utilizza smoothing moltiplicativo.
     */
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
        masterGainSmoothed{1.0f};

    /**
     *  Upsampler DSP per oversampling.
     *  \note Migliora qualità di filtri non lineari.
     */
    juce::dsp::Oversampling<float> upsampler;

    /**
     *  Downsampler DSP complementare.
     */
    juce::dsp::Oversampling<float> downsampler;

    /** Convolution per riverbero specifico di liuto. */
    juce::dsp::Convolution luteReverb;

    /**
     *  Filtro StateVariable TPT per modulazioni multiple.
     */
    juce::dsp::StateVariableTPTFilter<float> stateVariableFilter;

    /**
     *  Filtro shelving duplicato (low-shelf).
     */
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        lowShelfFilter;

    /** Frequenza di campionamento ultima memorizzata (Hz). */
    double lastSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessor)
};
