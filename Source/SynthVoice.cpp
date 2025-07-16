#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound *s) {
    return dynamic_cast<SynthSound *>(s) != nullptr;
}

void SynthVoice::prepareToPlay(double newSampleRate, int /*spb*/, int /*nc*/) {
    currentSampleRate = newSampleRate;


    highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(newSampleRate, 70.0f));



    juce::dsp::ProcessSpec spec{currentSampleRate, 512, 1};
    osc1.initialise([this](float x) {
        switch (config->getOsc1Waveform()) {
        case 1:
            return x / float_Pi; // saw
        case 2:
            return (x < 0 ? -1.0f : 1.0f); // square
        default:
            return std::asin(std::sin(x)) * (2.0f / float_Pi); // triangle
        }
    });
    osc2.initialise([this](float x) {
        switch (config->getOsc2Waveform()) {
        case 1:
            return x / float_Pi;
        case 2:
            return (x < 0 ? -1.0f : 1.0f);
        default:
            return std::asin(std::sin(x)) * (2.0f / float_Pi);
        }
    });
    for (int k = 0; k < numPartials; ++k)
        partialAmps[k] = std::pow(0.5f, float(k)); // ampiezza dimezza a ogni armonica

    // Prepare oscillators
    osc1.prepare(spec);
    osc2.prepare(spec);
    for (int i = 0; i < numPartials; ++i) {
        partialOscs[i].initialise([](float x) { return std::sin(x); });
        partialOscs[i].prepare(spec);
        partialOscs[i].reset();
    }
    // Prepare pluck delay
    pluckDelay.reset();
    pluckDelay.prepare(spec);

    // Prepare filters
    noiseFilter.reset();
    noiseFilter.prepare(spec);
    bodyFilter1.reset();
    bodyFilter1.prepare(spec);
    bodyFilter2.reset();
    bodyFilter2.prepare(spec);
    bodyFilter3.reset();
    harmonicEnv.reset();
    ampEnv.reset();
    filterEnv.reset();

    bodyFilter3.prepare(spec);

    bodyFilter1.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate,
                                                           200.0f, // centro 200 Hz
                                                           1.2f);  // Q = 1.2

    bodyFilter2.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate,
                                                           700.0f, // centro 700 Hz
                                                           1.0f);  // Q = 1.0

    bodyFilter3.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeBandPass(currentSampleRate,
                                                           1800.0f, // centro 1.8 kHz
                                                           1.0f);   // Q = 1.0



    voiceFilter.reset();
    voiceFilter.prepare(spec);
    lowShelfFilter.reset();
    lowShelfFilter.prepare(spec);
    lowShelfFilter.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate,
            200.0f, // frequenza di taglio shelf
            0.8f,   // Q
            juce::Decibels::decibelsToGain(6.0f));

    // Prepare envelopes
    ampEnv.setSampleRate(currentSampleRate);
    filterEnv.setSampleRate(currentSampleRate);
    harmonicEnv.setSampleRate(currentSampleRate);
}

void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound *, int) {



    noiseBurstTotalSamples = static_cast<int>(currentSampleRate * 0.005f); // 8 ms
    noiseBurstSamplesRemaining = noiseBurstTotalSamples;
    noiseBurstGain = 0.03f; // livello del burst
    noiseBurstActive = true;




    noteFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    velocityLevel = velocity;
    noteIsActive = true;
    //osc1.setFrequency(float(noteFrequency));
    //osc2.setFrequency(float(noteFrequency));
    // Set delay for KS
    delaySamples = int(currentSampleRate / noteFrequency);
    pluckDelay.setDelay(delaySamples);
    pluckDelay.reset();
    for (int i = 0; i < delaySamples; ++i)
        pluckDelay.pushSample(0, juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f);

    // Set partial frequencies
    for (int k = 0; k < numPartials; ++k)
        partialOscs[k].setFrequency(noteFrequency * (k + 1));

    // Start harmonic envelope
    harmonicEnvParams = {0.001f, 0.20f, 0.0f, 0.10f};
    harmonicEnv.setParameters(harmonicEnvParams);
    harmonicEnv.noteOn();

    // Start amplitude envelope
    ampEnvParams = {0.025f, 0.350f, 0.150f, 0.050f};
    ampEnv.setParameters(ampEnvParams);
    ampEnv.noteOn();

    // Filter envelope
    filterEnvParams = ampEnvParams;
    filterEnvParams.release = 1.200f;

    filterEnv.setParameters(filterEnvParams);
    filterEnv.noteOn();
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

    float lpState = 0.0f;
    for (int i = 0; i < numSamples; ++i) {
        // A) Envelope
        float envVal = ampEnv.getNextSample();
        //float lfo = std::sin(2.0f * float_Pi * lfoPhase);
        //float tunedFreq = noteFrequency * (1.0f + 0.0005f * lfo); // ±0.05%
        osc1.setFrequency(noteFrequency);
        osc2.setFrequency(float(noteFrequency));
        /*
        lfoPhase += lfoRate / currentSampleRate;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;*/
        // B) Additive partials
        float sum = 0.0f;
        for (int k = 0; k < numPartials; ++k)
            sum += partialOscs[k].processSample(0.0f) * partialAmps[k];
        float addOut = sum * harmonicEnv.getNextSample() * velocityLevel;
        //currentSample = highPassFilter.processSingleSampleRaw(currentSample);

        // C) Subtractive
        float s1 = osc1.processSample(0.0f);
        float s2 = osc2.processSample(0.0f);


        


        float blend = config->getOsc2Blend();
        float subOut = (s1 * (1.0f - blend) + s2 * blend) * envVal * velocityLevel;

        // D) Karplus-Strong
        float ks = pluckDelay.popSample(0);
        float fb = ks * (noteFrequency < 250.0f ? 0.998f : 0.995f);
        //lpState = 0.5f * (fb + lpState);
        float ksFilt = 0.5f * (fb + lpState);
        lpState = 0.98f * ksFilt + 0.02f * lpState; // smorza le alte
        pluckDelay.pushSample(0, lpState);
        float ksOut = lpState * envVal * velocityLevel;

        // E) Mix engines
        //float mixedIn = 0.4f * addOut + 0.4f * ksOut + 0.2f * subOut;
        float mixedIn = 0.7f * ksOut     // corpo principale
                        + 0.2f * addOut  // armoniche estratte
                        + 0.1f * subOut; // dettaglio timbrico

        if (noiseBurstActive && noiseBurstSamplesRemaining > 0) {
            // genera rumore [-1,1]
            float noiseSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * noiseBurstGain;

            // envelope lineare per evitare click
            float env = float(noiseBurstSamplesRemaining) / float(noiseBurstTotalSamples);
            mixedIn += noiseSample * env;

            --noiseBurstSamplesRemaining;
            if (noiseBurstSamplesRemaining <= 0)
                noiseBurstActive = false;
        }
        // F) Body resonance
        float b1 = bodyFilter1.processSample(mixedIn)*1.2f;
        float b2 = bodyFilter2.processSample(mixedIn)*0.8f;
        float b3 = bodyFilter3.processSample(mixedIn)*0.5f;
        float bodyMix = noteFrequency < 250.0f ? 0.6f : 0.4f;
        float mixed = mixedIn * (1.0f - bodyMix) + (b1 + b2 + b3) * (bodyMix / 3.0f);

        // G) Voice filter + shelving
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
