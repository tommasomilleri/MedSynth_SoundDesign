
/*! \file SynthVoice.h
 *  \brief Dichiarazione della classe SynthVoice, derivata da juce::SynthesiserVoice
 *         Realizza una voce di sintesi basata su Karplus-Strong e filtri DSP.
 */
#pragma once

#include "GuitarString.h"
#include "InstrumentConfig.h"
#include "SynthSound.h"
#include <JuceHeader.h>
#include <vector>
/**
 *  \class SynthVoice
 *  \brief Implementa una voce di sintesi audio personalizzata per JUCE.
 *
 *  Utilizza oscillatori DSP, ritardi Karplus-Strong, inviluppi ADSR,
 *  filtri IIR e opzioni di emissione di rumore per modellare il suono.
 */
class SynthVoice : public juce::SynthesiserVoice {
  public:
    /** \brief Costruttore di default. */

    SynthVoice() {}
    /** \brief Distruttore virtuale. */

    ~SynthVoice() override {}
    /**
     *  \brief Inizializza la voce prima della riproduzione.
     *  \param newSampleRate      Frequenza di campionamento (Hz).
     *  \param samplesPerBlock    Dimensione del blocco di campioni.
     *  \param numChannels        Numero di canali audio.
     */
    void prepareToPlay(double newSampleRate, int samplesPerBlock, int numChannels);
    /**
     *  \brief Avvia la riproduzione di una nota MIDI.
     *  \param midiNoteNumber     Numero della nota MIDI (0-127).
     *  \param velocity           Velocità di attacco [0.0,1.0].
     *  \param sound              Puntatore al SynthesiserSound associato.
     *  \param pitchWheelPos      Posizione iniziale della manopola di pitch.
     */
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *, int pitchWheelPos) override;
    /**
     *  \brief Ferma la nota corrente, applicando o meno il tail-off.
     *  \param velocity           Velocità di rilascio (unused).
     *  \param allowTailOff       True per permettere il rilascio, false per stop immediato.
     */
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newValue) override {}
    /** \brief Gestisce i controller MIDI (non implementato). */

    void controllerMoved(int controllerNumber, int newValue) override {}
    /**
     *  \brief Rende il blocco audio successivo.
     *  \param outputBuffer       Buffer audio di destinazione.
     *  \param startSample        Indice del primo campione da scrivere.
     *  \param numSamples         Numero di campioni da processare.
     */
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;
    /**
     *  \brief Verifica se questo voice può riprodurre il dato SynthSound.
     *  \param sound              Puntatore a SynthesiserSound.
     *  \return True se il Sound è di tipo SynthSound.
     */
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    /**
     *  \brief Assegna la configurazione dello strumento.
     *  \param newConfig          Puntatore alla struttura InstrumentConfig.
     */
    void setConfig(InstrumentConfig *newConfig) { config = newConfig; }
    /**
     *  \brief Abilita o disabilita il rumore di pluck.
     *  \param shouldEnable       True per abilitare, false per disabilitare.
     */
    void enablePluckNoise(bool shouldEnable) { pluckNoiseEnabled = shouldEnable; }
    /**
     *  \brief Trasposizione dell'oscillatore 1 in ottave (log2).
     *  \note 1.0 = un'ottava superiore.
     */

    float osc1Transpose = 0.0f;

  private:
    /** \brief Filtro IIR di output (low-pass/hi-pass mix). */
    juce::dsp::IIR::Filter<float> outputFilter;

    /** \brief Linea di ritardo per Karplus-Strong sul pluck. */
    juce::dsp::DelayLine<float> pluckDelay{2 * 48000};
    /** \brief Delay attuale in campioni. */
    int delaySamples = 0;

    /** \brief Invillope ADSR per il volume. */
    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    /** \brief Invillope ADSR per il cutoff del filtro. */
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    /** \brief Buffer circolare per generare burst di rumore. */
    juce::AudioBuffer<float> noiseBuffer;
    /** \brief Posizione corrente nel buffer di rumore. */
    int noisePosition = 0;
    /** \brief Filtro low-pass per modellare il burst di rumore. */
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

    /** \brief Stato della nota (true se attiva). */
    bool noteIsActive = false;

    /** \brief Frequenza di campionamento corrente (Hz). */
    double currentSampleRate = 44100.0;
    /** \brief Livello di velocity per l'inviluppo. */
    float velocityLevel = 1.0f;
    /** \brief Abilita il rumore di pluck. */
    bool pluckNoiseEnabled = false;
    /** \brief Frequenza della nota in riproduzione. */
    double noteFrequency = 440.0;

    /** \brief Filtri corpo per modellare le risonanze (300Hz,800Hz,2200Hz). */
    juce::dsp::IIR::Filter<float> bodyFilter1, bodyFilter2, bodyFilter3;

    /** \brief Filtro low-pass variabile per la voce. */
    juce::dsp::IIR::Filter<float> voiceFilter;
    /** \brief Filtro shelving per guadagno alle basse frequenze. */
    juce::dsp::IIR::Filter<float> lowShelfFilter;

    /** \brief Oscillatori principali (sinusoidali). */
    juce::dsp::Oscillator<float> osc1{[](float x) { return std::sin(x); }};
    juce::dsp::Oscillator<float> osc2{[](float x) { return std::sin(x); }};

    /** \brief Filtro hi-pass per rimuovere sub-sonici indesiderati. */
    juce::IIRFilter highPassFilter;

    /**
     *  \brief Numero di armoniche parziali da generare.
     *  \note Utilizzato per synth additive.
     */
    static constexpr int numPartials = 8;
    /** \brief Array di oscillatori per le armoniche. */
    juce::dsp::Oscillator<float> partialOscs[numPartials];

    /** \brief Ampiezze relative dei parziali [f0..f7]. */
    float partialAmps[numPartials] = {1.0f, 0.75f, 0.65f, 0.55f, 0.45f, 0.35f, 0.25f, 0.15f};

    /** \brief Stato e proprietà del burst di rumore iniziale. */
    bool noiseBurstActive = false;
    int noiseBurstSamplesRemaining = 0;
    int noiseBurstTotalSamples = 0;
    float noiseBurstGain = 0.03f;

    /** \brief Fase LFO per modulazioni continue. */
    float lfoPhase = 0.0f;
    /** \brief Frequenza LFO (Hz). */
    float lfoRate = 3.0f;

    /** \brief Puntatore alla configurazione generica dello strumento. */
    InstrumentConfig *config = nullptr;

    /** \brief Invillope ADSR dedicato alla sintesi armonica. */
    juce::ADSR harmonicEnv;
    juce::ADSR::Parameters harmonicEnvParams;

    /** \brief Modello di chitarra medievale per Karplus-Strong avanzato. */
    std::unique_ptr<GuitarString> guitarString;
    /** \brief Flag per abilitare la modalità chitarra medievale.
     *  \note Se true, usa `guitarString` al posto di `pluckDelay`.
     */
    bool useGuitar = false;

    /** \brief Collezione di corde addizionali per effetti di chorus. */
    std::vector<std::unique_ptr<GuitarString>> corde;
    /** \brief Detune (in semitoni) per ciascuna corda secondaria. */
    std::vector<float> detuneAmounts;

    /**
     *  \brief Configura la chitarra medievale con frequenza e sample rate.
     *  \param freq               Frequenza di base per la corda.
     *  \param sampleRate         Frequenza di campionamento (Hz).
     */
    void setupMedievalGuitar(float freq, double sampleRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};