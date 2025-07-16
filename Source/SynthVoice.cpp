#include "SynthVoice.h"
#include "SynthSound.h" // Aggiunto per risolvere l'errore "identificatore non definito"

bool SynthVoice::canPlaySound(juce::SynthesiserSound *s) {
    return dynamic_cast<SynthSound *>(s) != nullptr;
}

void SynthVoice::prepareToPlay(double newSampleRate, int /*spb*/, int /*nc*/) {
    currentSampleRate = newSampleRate;

    // Filtro passa-alto generico per pulizia
    highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(newSampleRate, 70.0f));

    juce::dsp::ProcessSpec spec{currentSampleRate, 512, 1};

    // Oscillatori per la sintesi sottrattiva (usati per dettaglio timbrico)
    osc1.initialise([this](float x) {
        switch (config->getOsc1Waveform()) {
        case 1:
            return x / float_Pi; // saw
        case 2:
            return (x < 0 ? -1.0f : 1.0f); // square
        default:
            return std::sin(x); // sine (modificato per coerenza)
        }
    });
    osc2.initialise([this](float x) {
        switch (config->getOsc2Waveform()) {
        case 1:
            return x / float_Pi;
        case 2:
            return (x < 0 ? -1.0f : 1.0f);
        default:
            return std::sin(x); // sine (modificato per coerenza)
        }
    });

    // Parziali per la sintesi additiva
    for (int k = 0; k < numPartials; ++k)
        partialAmps[k] = std::pow(0.5f, float(k));

    osc1.prepare(spec);
    osc2.prepare(spec);
    for (int i = 0; i < numPartials; ++i) {
        partialOscs[i].initialise([](float x) { return std::sin(x); });
        partialOscs[i].prepare(spec);
        partialOscs[i].reset();
    }

    // Preparazione Karplus-Strong
    pluckDelay.reset();
    pluckDelay.prepare(spec);

    // Preparazione filtri e inviluppi
    noiseFilter.reset();
    noiseFilter.prepare(spec);
    harmonicEnv.reset();
    ampEnv.reset();
    filterEnv.reset();

    // === MODIFICHE PER CHITARRA MEDIEVALE: FILTRI DI RISONANZA ===
    // Configurazione dei filtri passa-banda per simulare la risonanza del corpo
    bodyFilter1.reset();
    bodyFilter1.prepare(spec);
    bodyFilter1.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 300.0f, 1.1f); // Q da 1.5 a 1.1

    bodyFilter2.reset();
    bodyFilter2.prepare(spec);
    bodyFilter2.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 800.0f, 0.9f); // Q da 1.2 a 0.9

    bodyFilter3.reset();
    bodyFilter3.prepare(spec);
    bodyFilter3.coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate, 2200.0f, 0.8f); // Q da 1.0 a 0.8
    // =============================================================

    voiceFilter.reset();
    voiceFilter.prepare(spec);
    lowShelfFilter.reset();
    lowShelfFilter.prepare(spec);
    lowShelfFilter.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate,
            200.0f,
            0.8f,
            juce::Decibels::decibelsToGain(6.0f));

    ampEnv.setSampleRate(currentSampleRate);
    filterEnv.setSampleRate(currentSampleRate);
    harmonicEnv.setSampleRate(currentSampleRate);
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *, int) {

    // Burst di rumore per l'attacco del pizzico
    noiseBurstTotalSamples = static_cast<int>(currentSampleRate * 0.005f);
    noiseBurstSamplesRemaining = noiseBurstTotalSamples;
    noiseBurstGain = 0.03f;
    noiseBurstActive = true;

    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    velocityLevel = velocity;
    noteIsActive = true;

    // Preparazione Karplus-Strong: riempie il buffer di ritardo con rumore bianco
    delaySamples = int(currentSampleRate / noteFrequency);
    pluckDelay.setDelay(delaySamples);
    pluckDelay.reset();
    for (int i = 0; i < delaySamples; ++i)
        pluckDelay.pushSample(0, juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);

    for (int k = 0; k < numPartials; ++k)
        partialOscs[k].setFrequency(noteFrequency * (k + 1));

    // Inviluppo armonico
    harmonicEnvParams = {0.001f, 0.20f, 0.0f, 0.10f};
    harmonicEnv.setParameters(harmonicEnvParams);
    harmonicEnv.noteOn();

    // === MODIFICHE PER CHITARRA MEDIEVALE: INVILUPPI ADSR ===
    // Inviluppo di ampiezza con attacco rapido e sustain nullo
    ampEnvParams = {0.005f, 0.40f, 0.0f, 0.15f}; // Attack, Decay, Sustain, Release
    ampEnv.setParameters(ampEnvParams);
    ampEnv.noteOn();

    // Inviluppo del filtro
    filterEnvParams = ampEnvParams;
    filterEnvParams.release = 1.0f; // Rilascio piÃ¹ lungo per il filtro
    filterEnv.setParameters(filterEnvParams);
    filterEnv.noteOn();
    // ========================================================
}

void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff)
        ampEnv.noteOff();
    else {
        noteIsActive = false;
        clearCurrentNote();
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &buffer,
                                 int startSample, int numSamples) {
    if (!noteIsActive) return;

    auto *outL = buffer.getWritePointer(0, startSample);
    auto *outR = buffer.getNumChannels() > 1
                     ? buffer.getWritePointer(1, startSample)
                     : nullptr;

    float lpState = 0.0f; // Stato per il filtro passa-basso nel loop Karplus-Strong

    for (int i = 0; i < numSamples; ++i) {
        float envVal = ampEnv.getNextSample();

        osc1.setFrequency(noteFrequency);
        osc2.setFrequency(float(noteFrequency) * config->getOsc2Transpose());

        // B) Sintesi additiva (per arricchire armonicamente)
        float sum = 0.0f;
        for (int k = 0; k < numPartials; ++k)
            sum += partialOscs[k].processSample(0.0f) * partialAmps[k];
        float addOut = sum * harmonicEnv.getNextSample() * velocityLevel;

        // C) Sintesi sottrattiva (per dettaglio e corpo)
        float s1 = osc1.processSample(0.0f);
        float s2 = osc2.processSample(0.0f);
        float blend = config->getOsc2Blend();
        float subOut = (s1 * (1.0f - blend) + s2 * blend) * envVal * velocityLevel;

        // === MODIFICHE PER CHITARRA MEDIEVALE: KARPLUS-STRONG CON FILTRO ===
        // D) Karplus-Strong per il suono della corda pizzicata
        float ks = pluckDelay.popSample(0);
        float fb = ks * (noteFrequency < 250.0f ? 0.992f : 0.988f); // Feedback
        // Filtro passa-basso per smorzare le alte frequenze nel tempo
        float ksFilt = 0.5f * (fb + lpState);
        lpState = 0.9f * ksFilt + 0.1f * lpState;

        pluckDelay.pushSample(0, lpState);
        float ksOut = lpState * envVal * velocityLevel;
        // =================================================================

        // === MODIFICHE PER CHITARRA MEDIEVALE: MIXING DEI SEGNALI ===
        // E) Miscelazione delle sorgenti sonore
        float mixedIn = ksOut;

        // ============================================================

        // Aggiunge il burst di rumore iniziale
        if (noiseBurstActive && noiseBurstSamplesRemaining > 0) {
            float noiseSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * noiseBurstGain;
            float env = float(noiseBurstSamplesRemaining) / float(noiseBurstTotalSamples);
            mixedIn += noiseSample * env;
            --noiseBurstSamplesRemaining;
            if (noiseBurstSamplesRemaining <= 0)
                noiseBurstActive = false;
        }

        // F) Risonanza del corpo
        float b1 = bodyFilter1.processSample(mixedIn) * 1.2f;
        float b2 = bodyFilter2.processSample(mixedIn) * 0.8f;
        float b3 = bodyFilter3.processSample(mixedIn) * 0.5f;
        float bodyMix = noteFrequency < 250.0f ? 0.5f : 0.35f;
        float mixed = mixedIn * (1.0f - bodyMix) + (b1 + b2 + b3) * (bodyMix / 3.0f);

        // G) Filtro della voce e shelving finale
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