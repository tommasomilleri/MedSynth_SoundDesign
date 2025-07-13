/*
#include "SynthVoice.h"

void SynthVoice::prepareToPlay(double newSampleRate,
                               int samplesPerBlock,
                               int numChannels)
{
    sampleRate = newSampleRate;

    // Prepara filtro di uscita (passa banda centrato a 12 kHz)
    juce::dsp::ProcessSpec spec {
        sampleRate,
        static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(numChannels)
    };

    outputFilter.reset();
    outputFilter.prepare(spec);
    oscillator.initialise([](float x) { return std::sin(x); });
    oscillator.prepare(spec);
    outputFilter.coefficients =
        juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 12000.0f, 1.0f);
}

void SynthVoice::startNote(int midiNoteNumber,
                           float velocity,
                           juce::SynthesiserSound* ,
                           int )
{
    // Calcola frequenza da nota MIDI e assegna all'oscillatore
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(currentFrequency);

    // Imposta pressione di base (senza inviluppo ADSR)
    basePressure   = velocity;
    noteOn         = true;
    voiceIsActive  = true;
}

void SynthVoice::stopNote(float, bool )
{
    // Termina immediatamente la voice
    noteOn        = false;
    voiceIsActive = false;
    clearCurrentNote();
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                 int startSample,
                                 int numSamples)
{
    if (! voiceIsActive)
        return;

    const int numChannels = outputBuffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i)
    {
        // 1) Oscillatore placeholder (sinusoide) * pressione costante
        float oscValue = oscillator.processSample(0.0f);
        float raw = oscValue * basePressure;

        // 2) Passa attraverso il filtro band-pass
        float filtered = outputFilter.processSample(raw);

        // 3) Somma nel buffer di uscita
        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample(ch, startSample + i, filtered);
    }
}
SynthVoice::SynthVoice()
    : noteOn(false),
      voiceIsActive(false),
      sampleRate(44100.0),
      basePressure(0.0f),
      bowNoiseEnabled(false),
      currentFrequency(440.0),
      config(nullptr) {
    // se vuoi, qui dentro inizializzi anche outputFilter/oscillator allo stato di default
}
bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}
void SynthVoice::setConfig(InstrumentConfig *newConfig) {
    config = newConfig;
}

void SynthVoice::enableBowNoise(bool shouldEnable) {
    bowNoiseEnabled = shouldEnable;
}
// -- implementazioni vuote per i metodi overrides mancanti --

void SynthVoice::pitchWheelMoved(int ) {
    // no-op
}

void SynthVoice::controllerMoved(int , int ) {
    // no-op
}
*/
/*
#include "SynthVoice.h"

void SynthVoice::prepareToPlay(double newSampleRate,
                               int samplesPerBlock,
                               int numChannels) {
    sampleRate = newSampleRate;

    // --- ProcessSpec ---
    juce::dsp::ProcessSpec spec{
        sampleRate,
        (juce::uint32)samplesPerBlock,
        (juce::uint32)numChannels};

    // Oscillatore
    oscillator.initialise([](float x) { return std::sin(x); });
    oscillator.prepare(spec);

    // Filtro di uscita (band-pass di esempio, verrà modulato)
    outputFilter.reset();
    outputFilter.prepare(spec);
    outputFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 12000.0f, 1.0f);

    // Inviluppi
    ampEnv.setSampleRate(sampleRate);
    filterEnv.setSampleRate(sampleRate);

    // Noise burst

    noiseBuffer.setSize(1, (int)(0.02 * sampleRate)); // 20 ms di rumore
    auto *data = noiseBuffer.getWritePointer(0);
    juce::Random r;
    for (int i = 0; i < noiseBuffer.getNumSamples(); ++i)
        data[i] = r.nextFloat() * 2.0f - 1.0f;
    noisePos = 0;
    noiseFilter.reset();
    noiseFilter.prepare(spec);
}

void SynthVoice::startNote(int midiNoteNumber,
                           float velocity,
                           juce::SynthesiserSound * ,
                           int ) {
    // Frequenza
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(currentFrequency);

    // Parametri dal config
    ampEnvParams.attack = config->getAttack();
    ampEnvParams.decay = config->getDecay();
    ampEnvParams.sustain = config->getSustain();
    ampEnvParams.release = config->getRelease();
    ampEnv.setParameters(ampEnvParams);
    ampEnv.noteOn();

    filterEnvParams.attack = config->getAttack();
    filterEnvParams.decay = config->getDecay();
    filterEnvParams.sustain = config->getSustain();
    filterEnvParams.release = config->getRelease();
    filterEnv.setParameters(filterEnvParams);
    filterEnv.noteOn();

    basePressure = velocity;
    noteOn = true;
    voiceIsActive = true;

    noisePos = 0;
    noiseFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, 8000.0f);
}

void SynthVoice::stopNote(float , bool allowTailOff) {
    if (allowTailOff) {
        ampEnv.noteOff();
        filterEnv.noteOff();
    } else {
        ampEnv.reset();
        filterEnv.reset();
        clearCurrentNote();
        voiceIsActive = false;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample,
                                 int numSamples) {
    if (!voiceIsActive)
        return;

    const int numChannels = outputBuffer.getNumChannels();

    for (int i = 0; i < numSamples; ++i) {
        // 1) Oscillatore + inviluppo ampiezza
        float envValue = ampEnv.getNextSample();
        float oscValue = oscillator.processSample(0.0f);
        float toneSample = oscValue * envValue * basePressure;

        // 2) Noise burst filtrato
        float noiseSample = 0.0f;
        if (noisePos < noiseBuffer.getNumSamples()) {
            noiseSample = noiseBuffer.getSample(0, noisePos++);
            noiseSample = noiseFilter.processSample(noiseSample) * (1.0f - envValue);
        }

        // 3) Invio attraverso filtro con cutoff modulato da filterEnv
        float cutoffMod = config->getFilterCutoff() + filterEnv.getNextSample() * (config->getFilterCutoff());
        *outputFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, cutoffMod, config->getFilterResonance());

        float filtered = outputFilter.processSample(toneSample + noiseSample);

        // 4) Scrive nel buffer
        for (int ch = 0; ch < numChannels; ++ch)
            outputBuffer.addSample(ch, startSample + i, filtered);

        // 5) Fine note: se ampEnv ha finito il release, disattiva
        if (!ampEnv.isActive()) {
            clearCurrentNote();
            voiceIsActive = false;
            break;
        }
    }
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound) {
    return dynamic_cast<SynthSound *>(sound) != nullptr;
}
*/
#include "SynthVoice.h"

bool SynthVoice::canPlaySound (SynthesiserSound* s)
{
    return dynamic_cast<SynthSound*> (s) != nullptr;
}

void SynthVoice::prepareToPlay (double newSampleRate, int /*spb*/, int /*nc*/)
{
    sampleRate = newSampleRate;




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

    // Inviluppo ampiezza preso da config LuteConfig
    ampEnvParams.attack  = config->getAttack();
    ampEnvParams.decay   = config->getDecay();
    ampEnvParams.sustain = config->getSustain();
    ampEnvParams.release = config->getRelease();
    ampEnv.setParameters (ampEnvParams);
    ampEnv.reset();
    ampEnv.noteOn();

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

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                  int startSample,
                                  int numSamples)
{
    if (! ampEnv.isActive())
        return;

    auto* outL = outputBuffer.getWritePointer (0, startSample);
    auto* outR = outputBuffer.getNumChannels() > 1
               ? outputBuffer.getWritePointer (1, startSample)
               : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        float env = ampEnv.getNextSample();

        // Burst di rumore solo all’inizio
        float in = 0.0f;
        if (noisePos < noiseBuffer.getNumSamples())
            in = noiseBuffer.getSample (0, noisePos++);

        // Karplus-Strong: feed-forward del delay + feedback attenuato
        float y = ksDelay.popSample (0);
        float newSample = in + y * 0.998f;  // feedback ~= 0.998
        ksDelay.pushSample (0, newSample);

        float out = newSample * env * config->getMasterGain() * basePressure;

        outL[i] = out;
        if (outR) outR[i] = out;
    }

    if (! ampEnv.isActive())
        clearCurrentNote();
}
