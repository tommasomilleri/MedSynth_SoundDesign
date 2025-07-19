
/*! \file SynthVoice.h
 *   Dichiarazione della classe SynthVoice, derivata da juce::SynthesiserVoice
 *         Realizza una voce di sintesi basata su Karplus-Strong e filtri DSP.
 */
#pragma once
#include "InstrumentConfig.h"
#include "SynthSound.h"
#include <JuceHeader.h>
#include <vector>
/**
 *  \class SynthVoice
 *   Implementa una voce di sintesi audio personalizzata per JUCE.
 *
 *  Utilizza oscillatori DSP, ritardi Karplus-Strong, inviluppi ADSR,
 *  filtri IIR e opzioni di emissione di rumore per modellare il suono.
 */
class SynthVoice : public juce::SynthesiserVoice {
  public:
    /**  Costruttore di default. */

    SynthVoice() {}
    /**  Distruttore virtuale. */

    ~SynthVoice() override {}
    /**
     *   Inizializza la voce prima della riproduzione.
     *   newSampleRate      Frequenza di campionamento (Hz).
     *   samplesPerBlock    Dimensione del blocco di campioni.
     *   numChannels        Numero di canali audio.
     */
    void prepareToPlay(double newSampleRate, int samplesPerBlock, int numChannels);
    /**
     *   Avvia la riproduzione di una nota MIDI.
     *   midiNoteNumber     Numero della nota MIDI (0-127).
     *   velocity           Velocità di attacco [0.0,1.0].
     *   sound              Puntatore al SynthesiserSound associato.
     *   pitchWheelPos      Posizione iniziale della manopola di pitch.
     */
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *, int pitchWheelPos) override;
    /**
     *   Ferma la nota corrente, applicando o meno il tail-off.
     *   velocity           Velocità di rilascio (unused).
     *   allowTailOff       True per permettere il rilascio, false per stop immediato.
     */
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newValue) override {}
    /**  Gestisce i controller MIDI (non implementato). */

    void controllerMoved(int controllerNumber, int newValue) override {}
    /**
     *   Rende il blocco audio successivo.
     *   outputBuffer       Buffer audio di destinazione.
     *   startSample        Indice del primo campione da scrivere.
     *   numSamples         Numero di campioni da processare.
     */
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                         int startSample, int numSamples) override;
    /**
     *   Verifica se questo voice può riprodurre il dato SynthSound.
     *   sound              Puntatore a SynthesiserSound.
     *  \return True se il Sound è di tipo SynthSound.
     */
    bool canPlaySound(juce::SynthesiserSound *sound) override;
    /**
     *   Assegna la configurazione dello strumento.
     *   newConfig          Puntatore alla struttura InstrumentConfig.
     */
    void setConfig(InstrumentConfig *newConfig) { config = newConfig; }
    /**
     *  Abilita o disabilita il rumore di pluck.
     *  shouldEnable       True per abilitare, false per disabilitare.
     */
    void enablePluckNoise(bool shouldEnable) { pluckNoiseEnabled = shouldEnable; }
    /**
     *   Trasposizione dell'oscillatore 1 in ottave (log2).
     *  \note 1.0 = un'ottava superiore.
     */

    float osc1Transpose = 0.0f;

  private:
    /**  Filtro IIR di output (low-pass/hi-pass mix). */
    juce::dsp::IIR::Filter<float> outputFilter;

    /**  Linea di ritardo per Karplus-Strong sul pluck. */
    juce::dsp::DelayLine<float> pluckDelay{2 * 48000};
    /**  Delay attuale in campioni. */
    int delaySamples = 0;

    /**  Invillope ADSR per il volume. */
    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;
    /**  Invillope ADSR per il cutoff del filtro. */
    juce::ADSR filterEnv;
    juce::ADSR::Parameters filterEnvParams;

    /**  Buffer circolare per generare burst di rumore. */
    juce::AudioBuffer<float> noiseBuffer;
    /**  Posizione corrente nel buffer di rumore. */
    int noisePosition = 0;
    /**  Filtro low-pass per modellare il burst di rumore. */
    juce::dsp::IIR::Filter<float> noiseFilter{juce::dsp::IIR::Coefficients<float>::makeLowPass(44100.0, 8000.0f)};

    /**  Stato della nota (true se attiva). */
    bool noteIsActive = false;

    /**  Frequenza di campionamento corrente (Hz). */
    double currentSampleRate = 44100.0;
    /**  Livello di velocity per l'inviluppo. */
    float velocityLevel = 1.0f;
    /**  Abilita il rumore di pluck. */
    bool pluckNoiseEnabled = false;
    /**  Frequenza della nota in riproduzione. */
    double noteFrequency = 440.0;

    /**  Filtri corpo per modellare le risonanze (300Hz,800Hz,2200Hz). */
    juce::dsp::IIR::Filter<float> bodyFilter1, bodyFilter2, bodyFilter3;

    /**  Filtro low-pass variabile per la voce. */
    juce::dsp::IIR::Filter<float> voiceFilter;
    /**  Filtro shelving per guadagno alle basse frequenze. */
    juce::dsp::IIR::Filter<float> lowShelfFilter;

    /**  Oscillatori principali (sinusoidali). */
    juce::dsp::Oscillator<float> osc1{[](float x) { return std::sin(x); }};
    juce::dsp::Oscillator<float> osc2{[](float x) { return std::sin(x); }};

    /**  Filtro hi-pass per rimuovere sub-sonici indesiderati. */
    juce::IIRFilter highPassFilter;

    /**
     *   Numero di armoniche parziali da generare.
     *  \note Utilizzato per synth additive.
     */
    static constexpr int numPartials = 8;
    /**  Array di oscillatori per le armoniche. */
    juce::dsp::Oscillator<float> partialOscs[numPartials];

    /**  Ampiezze relative dei parziali [f0..f7]. */
    float partialAmps[numPartials] = {1.0f, 0.75f, 0.65f, 0.55f, 0.45f, 0.35f, 0.25f, 0.15f};

    /**  Stato e proprietà del burst di rumore iniziale. */
    bool noiseBurstActive = false;
    int noiseBurstSamplesRemaining = 0;
    int noiseBurstTotalSamples = 0;
    float noiseBurstGain = 0.03f;

    /**  Fase LFO per modulazioni continue. */
    float lfoPhase = 0.0f;
    /**  Frequenza LFO (Hz). */
    float lfoRate = 3.0f;

    /**  Puntatore alla configurazione generica dello strumento. */
    InstrumentConfig *config = nullptr;

    /**  Invillope ADSR dedicato alla sintesi armonica. */
    juce::ADSR harmonicEnv;
    juce::ADSR::Parameters harmonicEnvParams;

    /**  Modello di chitarra medievale per Karplus-Strong avanzato. */
    /**  Flag per abilitare la modalità chitarra medievale.
     *  \note Se true, usa `guitarString` al posto di `pluckDelay`.
     */
    bool useGuitar = false;

    /**  Collezione di corde addizionali per effetti di chorus. */
    /**  Detune (in semitoni) per ciascuna corda secondaria. */
    std::vector<float> detuneAmounts;

    /**
     *   Configura la chitarra medievale con frequenza e sample rate.
     *   freq               Frequenza di base per la corda.
     *   sampleRate         Frequenza di campionamento (Hz).
     */
    void setupMedievalGuitar(float freq, double sampleRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};