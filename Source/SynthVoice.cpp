/*! \file SynthVoice.cpp
 *  \brief Implementazione dei metodi di SynthVoice per la sintesi audio.
 */
#include "SynthVoice.h"
#include "SynthSound.h"
/**
 *  \brief Verifica se la voce può riprodurre il suono passato.
 *  \param s Puntatore al SynthesiserSound da controllare.
 *  \return true se il suono è un SynthSound, false altrimenti.
 */
bool SynthVoice::canPlaySound(juce::SynthesiserSound *s) {
    return dynamic_cast<SynthSound *>(s) != nullptr;
}
/**
 *  \brief Prepara la voce per la riproduzione impostando sample rate e filtri.
 *  \param newSampleRate Frequenza di campionamento in Hz.
 *  \param spb            Numero di campioni per blocco (non utilizzato).
 *  \param nc             Numero di canali (non utilizzato).
 */
void SynthVoice::prepareToPlay(double newSampleRate, int /*spb*/, int /*nc*/) {
    currentSampleRate = newSampleRate;
    highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(newSampleRate, 70.0f));

    juce::dsp::ProcessSpec spec{currentSampleRate, 512, 1};

    osc1.initialise([](float x) { return std::sin(x); });
    osc2.initialise([](float x) { return std::sin(x); });

    osc1.prepare(spec);
    osc2.prepare(spec);

    pluckDelay.reset();
    pluckDelay.prepare(spec);
    noiseFilter.reset();
    noiseFilter.prepare(spec);
    harmonicEnv.reset();
    ampEnv.reset();
    filterEnv.reset();

    bodyFilter1.reset();
    bodyFilter1.prepare(spec);
    bodyFilter1.coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 300.0f, 1.1f);

    bodyFilter2.reset();
    bodyFilter2.prepare(spec);
    bodyFilter2.coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 800.0f, 0.9f);

    bodyFilter3.reset();
    bodyFilter3.prepare(spec);
    bodyFilter3.coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 2200.0f, 0.8f);

    voiceFilter.reset();
    voiceFilter.prepare(spec);
    lowShelfFilter.reset();
    lowShelfFilter.prepare(spec);
    lowShelfFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, 200.0f, 0.8f, juce::Decibels::decibelsToGain(6.0f));

    ampEnv.setSampleRate(currentSampleRate);
    filterEnv.setSampleRate(currentSampleRate);
    harmonicEnv.setSampleRate(currentSampleRate);
}
/**
 *  \brief Inizia la riproduzione di una nota MIDI.
 *  \param midiNoteNumber Numero della nota MIDI.
 *  \param velocity       Velocità di attacco [0.0, 1.0].
 *  \param sound          Puntatore al SynthesiserSound (non utilizzato).
 *  \param currentPitchWheelPosition Posizione della ruota del pitch (non utilizzata).
 */

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *, int) {
    noiseBurstTotalSamples = static_cast<int>(currentSampleRate * 0.005f);
    noiseBurstSamplesRemaining = noiseBurstTotalSamples;
    noiseBurstGain = 0.03f;
    noiseBurstActive = true;

    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) * 2.0f; // un'ottava sopra
    velocityLevel = velocity;
    noteIsActive = true;

    delaySamples = int(currentSampleRate / noteFrequency);
    pluckDelay.setDelay(delaySamples);
    pluckDelay.reset();
    for (int i = 0; i < delaySamples; ++i)
        pluckDelay.pushSample(0, juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);

    ampEnvParams = {
        config->getAttack() / 1000.0f,   /**< Attack in secondi */
        config->getDecay() / 1000.0f,    /**< Decay in secondi  */
        config->getSustain(),            /**< Sustain level     */
        config->getRelease() / 1000.0f   /**< Release in secondi*/
    };
    ampEnv.setParameters(ampEnvParams);
    ampEnv.noteOn();

    filterEnvParams = ampEnvParams;
    filterEnvParams.release = 1.0f;
    filterEnv.setParameters(filterEnvParams);
    filterEnv.noteOn();

    float osc1Freq = noteFrequency * pow(2.0f, osc1Transpose);
}
/**
 *  \brief Ferma la nota corrente, con o senza tail-off.
 *  \param velocity               Velocità di rilascio (non utilizzata).
 *  \param allowTailOff           true per permettere il rilascio, false per interrompere immediatamente.
 */
void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff)
        ampEnv.noteOff();
    else {
        noteIsActive = false;
        clearCurrentNote();
    }
}
/**
 *  \brief Rende il blocco audio successivo scrivendolo nel buffer.
 *  \param buffer AudioBuffer di destinazione.
 *  \param startSample Indice del primo campione da scrivere.
 *  \param numSamples Numero di campioni da processare.
 */
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer, int startSample, int numSamples) {
    if (!noteIsActive) return;

    auto *outL = buffer.getWritePointer(0, startSample);
    auto *outR = buffer.getNumChannels() > 1
                     ? buffer.getWritePointer(1, startSample)
                     : nullptr;

    float lpState = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        float envVal = ampEnv.getNextSample();

        float ks = pluckDelay.popSample(0);
        float fb = ks * (noteFrequency < 250.0f ? 0.970f : 0.965f);

        float dampedSample = 0.5f * (fb + lpState);
        lpState = dampedSample;

        pluckDelay.pushSample(0, lpState);
        float ksOut = lpState * envVal * velocityLevel;

        float mixedIn = ksOut;
        if (noiseBurstActive && noiseBurstSamplesRemaining > 0) {
            float noiseSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * noiseBurstGain;
            float env = float(noiseBurstSamplesRemaining) / float(noiseBurstTotalSamples);
            mixedIn += noiseSample * env;
            --noiseBurstSamplesRemaining;
            if (noiseBurstSamplesRemaining <= 0)
                noiseBurstActive = false;
        }

        float b1 = bodyFilter1.processSample(mixedIn) * 1.2f;
        float b2 = bodyFilter2.processSample(mixedIn) * 0.8f;
        float b3 = bodyFilter3.processSample(mixedIn) * 0.5f;
        float bodyMix = noteFrequency < 250.0f ? 0.5f : 0.35f;
        float mixed = mixedIn * (1.0f - bodyMix) + (b1 + b2 + b3) * (bodyMix / 3.0f);

        float fEnv = filterEnv.getNextSample();
        float baseCut = config->getFilterCutoff();
        float cutoff = baseCut + fEnv * baseCut + velocityLevel * config->getVelocityToCutoffMod();
        *voiceFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            currentSampleRate, cutoff, config->getFilterResonance());
        float vf = voiceFilter.processSample(mixed);
        float out = lowShelfFilter.processSample(vf) * config->getMasterGain();
        out = highPassFilter.processSingleSampleRaw(out);

        outL[i] = out;
        if (outR) outR[i] = out;

        if (!ampEnv.isActive()) {
            noteIsActive = false;
            clearCurrentNote();
            break;
        }
    }
}