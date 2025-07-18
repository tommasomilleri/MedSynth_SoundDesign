/*! \file JuceSynthFrameworkAudioProcessor.h
 *  \brief Dichiarazione del processore audio principale per il synth.
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
 *  \brief Classe che estende juce::AudioProcessor per realizzare il synth.
 *
 *  Contiene il JuceSynth, il ValueTreeState per parametri,
 *  gli effetti audio (filtro, riverbero, chorus), oversampling
 *  e la logica di cambio strumento.
 */
class JuceSynthFrameworkAudioProcessor : public juce::AudioProcessor {
  public:
    /**
     *  \brief Restituisce lo strumento corrente selezionato.
     *  \return Enumerazione InstrumentType del currentInstrument.
     */
    InstrumentType getCurrentInstrument() const noexcept { return currentInstrument; }
    /**
     *  \brief Restituisce il riferimento al ValueTreeState per i parametri.
     *  \return Riferimento a juce::AudioProcessorValueTreeState.
     */
    juce::AudioProcessorValueTreeState &getValueTreeState() noexcept { return tree; }
    /**
     *  \brief Stato attuale della tastiera MIDI interna.
     *  \note Utilizzato per disegno dell'editor e gestione degli eventi MIDI.
     */
    juce::MidiKeyboardState keyboardState;

    /** \brief Costruttore: registra parametri e inizializza il synth. */
    JuceSynthFrameworkAudioProcessor();
    /** \brief Distruttore virtuale. */
    ~JuceSynthFrameworkAudioProcessor() override;

    /** \brief Nome del plugin. */
    const juce::String getName() const override;
    /** \brief True se accetta input MIDI. */
    bool acceptsMidi() const override;
    /** \brief True se genera output MIDI. */
    bool producesMidi() const override;
    /** \brief True se è un effetto MIDI. */
    bool isMidiEffect() const override;
    /** \brief Tempo di tail-end in secondi. */
    double getTailLengthSeconds() const override;

    /** \brief Numero di programmi (preset) disponibili. */
    int getNumPrograms() override;
    /** \brief Indice del programma corrente. */
    int getCurrentProgram() override;
    /** \brief Seleziona il programma in base all'indice. */
    void setCurrentProgram(int index) override;
    /** \brief Nome del programma dato l'indice. */
    const juce::String getProgramName(int index) override;
    /** \brief Cambia il nome di un programma. */
    void changeProgramName(int index, const juce::String &newName) override;

    /**
     *  \brief Verifica se il layout bus è supportato.
     *  \param layouts Struttura BusesLayout da validare.
     *  \return True se supportato.
     */
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    /** \brief Rilascio risorse non più necessarie. */
    void releaseResources() override;

    /**
     *  \brief Serializza lo stato per salvataggio preset/DAW.
     *  \param destData MemoryBlock di destinazione.
     */
    void getStateInformation(juce::MemoryBlock &destData) override;
    /**
     *  \brief Deserializza lo stato da DAW o file.
     *  \param data Puntatore ai dati.
     *  \param size Dimensione dei dati.
     */
    void setStateInformation(const void *data, int size) override;

    /**
     *  \brief Prepara il processore per il playback.
     *  \param sampleRate Frequenza di campionamento (Hz).
     *  \param samplesPerBlock Dimensione del blocco audio.
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    /**
     *  \brief Processa blocchi audio e MIDI.
     *  \param buffer Buffer audio di input/output.
     *  \param midiMessages Buffer MIDI in ingresso.
     */
    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer &midiMessages) override;

    /** \brief Crea l'editor grafico custom. */
    juce::AudioProcessorEditor *createEditor() override;
    /** \brief Indica se è disponibile un editor GUI. */
    bool hasEditor() const override;

    /**
     *  \brief Cambia lo strumento corrente.
     *  \param newInstrument Tipo di strumento da impostare.
     */
    void setInstrument(InstrumentType newInstrument);

    /** \brief Aggiorna i coefficienti del filtro DSP in base ai parametri correnti. */
    void updateFilter();

  private:
    /** \brief Sintetizzatore interno con voci multiple. */
    juce::Synthesiser mySynth;

    /** \brief Configurazione attuale per lo strumento. */
    std::unique_ptr<InstrumentConfig> currentConfig;
    /** \brief Strumento corrente (default: Lute). */
    InstrumentType currentInstrument = InstrumentType::Lute;

    /** \brief ValueTreeState per parametri utente.
     *  \note Gestisce parametri audio e MIDI.
     */
    juce::AudioProcessorValueTreeState tree;

    /**
     *  \brief Filtro DSP duplicato per cutoff/resonance.
     *  \tparam FilterType Tipo di filtro IIR.
     */
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        dspFilt;

    /** \brief Riverbero di tipo JUCE predefinito. */
    juce::Reverb reverb;
    /** \brief Parametri del riverbero. */
    juce::Reverb::Parameters reverbParameters;

    /** \brief Effetto Chorus DSP. */
    juce::dsp::Chorus<float> chorus;

    /**
     *  \brief Valore di guadagno master liscio.
     *  \note Utilizza smoothing moltiplicativo.
     */
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
        masterGainSmoothed{1.0f};

    /**
     *  \brief Upsampler DSP per oversampling.
     *  \note Migliora qualità di filtri non lineari.
     */
    juce::dsp::Oversampling<float> upsampler;

    /**
     *  \brief Downsampler DSP complementare.
     */
    juce::dsp::Oversampling<float> downsampler;

    /** \brief Convolution per riverbero specifico di liuto. */
    juce::dsp::Convolution luteReverb;

    /**
     *  \brief Filtro StateVariable TPT per modulazioni multiple.
     */
    juce::dsp::StateVariableTPTFilter<float> stateVariableFilter;

    /**
     *  \brief Filtro shelving duplicato (low-shelf).
     */
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        lowShelfFilter;

    /** \brief Frequenza di campionamento ultima memorizzata (Hz). */
    double lastSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessor)
};
