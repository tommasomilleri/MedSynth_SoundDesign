#include "SynthVoice.h"

bool SynthVoice::canPlaySound (SynthesiserSound* s)
{
    return dynamic_cast<SynthSound*> (s) != nullptr;
}

void SynthVoice::prepareToPlay (double newSampleRate, int /*spb*/, int /*nc*/)
{
    juce::dsp::ProcessSpec spec{sampleRate, 512, 1};

    sampleRate = newSampleRate;


    // Risonanze della cassa: circa 450 Hz (Q=3) e 1200 Hz (Q=2)
    auto b1 = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 450.0f, 3.0f);
    auto b2 = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 1200.0f, 2.0f);
    auto b3 = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 275.0f, 3.0f);
    
    bodyFilter3.coefficients = *b3;
    bodyFilter1.coefficients = *b1;
    bodyFilter2.coefficients = *b2;

    lowShelfFilter.reset();
    lowShelfFilter.prepare(spec);
    lowShelfFilter.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, 400.0f, 1.0f, juce::Decibels::decibelsToGain(8.0f));
    ksDelay.reset();
    ksDelay.prepare ({ sampleRate, 512, 1 });

    // Prepara inviluppo ampiezza
    ampEnv.setSampleRate (sampleRate);

    // Genera un breve burst di rumore
    int noiseLen = static_cast<int> (sampleRate * 0.05); // 50ms
    noiseBuffer.setSize (1, noiseLen);
    auto* w = noiseBuffer.getWritePointer (0);
    for (int i = 0; i < noiseLen; ++i)
        w[i] = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int)
{
    // Calcola il delay per la corda
    auto freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    int delaySamples = static_cast<int> (sampleRate / freq);
    ksDelay.setDelay (delaySamples);
    ksDelay.reset();
    for (int i = 0; i < delaySamples; ++i) {
        float n = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
        ksDelay.pushSample(0, n);
    }
    // Inviluppo ampiezza preso da config LuteConfig
    ampEnvParams.attack  = config->getAttack();
    ampEnvParams.decay   = config->getDecay();
    ampEnvParams.sustain = config->getSustain();
    ampEnvParams.release = config->getRelease();
    ampEnv.setParameters (ampEnvParams);
    ampEnv.reset();
    ampEnv.noteOn();
    voiceFilter.reset();
    voiceFilter.prepare({sampleRate, 512, 1});

    filterEnvParams = ampEnvParams; // stessa forma per il cutoff
    filterEnv.setParameters(filterEnvParams);
    filterEnv.reset();
    filterEnv.noteOn();

    noisePos      = 0;
    basePressure  = velocity;
    noteOn        = true;
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
        ampEnv.noteOff();
    else
        clearCurrentNote();

    noteOn = false;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample, int numSamples) {
    if (!ampEnv.isActive())
        return;

    auto *outL = outputBuffer.getWritePointer(0, startSample);
    auto *outR = outputBuffer.getNumChannels() > 1
                     ? outputBuffer.getWritePointer(1, startSample)
                     : nullptr;

    static float lpState = 0.0f;
    const float feedbackCoef = 0.995f;

    for (int i = 0; i < numSamples; ++i) {
        // 1) Envelope ampiezza
        float ampVal = ampEnv.getNextSample();

        // 2) Pluck noise filtrato
        float in = 0.0f;
        float y = ksDelay.popSample(0);
        float feedback = y * 0.97f;
        lpState = (feedback + lpState) * 0.5f;
        float ksIn = lpState;
        ksDelay.pushSample(0, ksIn);
        float filtEnvVal = filterEnv.getNextSample();                   // da 1→0
        float baseCut = config->getFilterCutoff();                      // es. 2400 Hz
        float velMod = basePressure * config->getVelocityToCutoffMod(); // fino a 800 Hz
        float cutoffNow = baseCut + filtEnvVal * baseCut + velMod;
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, cutoffNow, config->getFilterResonance());
        *voiceFilter.coefficients = *coeffs;
        float filteredKS = voiceFilter.processSample(ksIn);
        if ((i % 100) == 0)
            ksDelay.setDelay(delaySamples + (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 2);

        // 4) Corpo (body-resonance) sul segnale filtrato
        float b1 = bodyFilter1.processSample(filteredKS);
        float b2 = bodyFilter2.processSample(filteredKS);
        float b3 = bodyFilter3.processSample(filteredKS);
        float bodyMix = (currentFrequency < 250.0f ? 0.5f : 0.35f);
        // per C3 (≈130 Hz) vogliamo più peso sulle basse:
        float freq = static_cast<float>(currentFrequency);

        float bodyMixRatio = (freq < 200.0f) ? 0.6f  // note basse: 60% body
                                             : 0.3f; // note medie/alte: 30% body
        //float mixed = filteredKS * (1.0f - bodyMix) + (b1 + b2 + b3) * (bodyMix / 3.0f);
        float mixed = filteredKS * (1.0f - bodyMixRatio) + (b1 + b2 + b3) * bodyMixRatio / 3.0f;
        float withShelf = lowShelfFilter.processSample(mixed);

        // 4) Applica body-filters in parallelo
        float body1 = bodyFilter1.processSample(ksIn);
        float body2 = bodyFilter2.processSample(ksIn);
        //float mixed = ksIn * 0.6f + (body1 + body2) * 0.4f; // mix KS + risonanze
        float out = withShelf * ampVal * config->getMasterGain() * basePressure;
        outL[i] = out;
        if (outR) outR[i] = out;

        // 5) Output finale
        /* float out = mixed * ampVal * config->getMasterGain() * basePressure;
        outL[i] = out;
        if (outR) outR[i] = out;*/

        // 6) Stop
        if (!ampEnv.isActive()) {
            clearCurrentNote();
            break;
        }
    }
}
