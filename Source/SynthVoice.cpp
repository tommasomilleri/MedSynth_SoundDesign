// SynthVoice.cpp — implementazione di SynthVoice senza juce::dsp::ADSR
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
                           juce::SynthesiserSound* /*sound*/,
                           int /*pitchWheelPos*/)
{
    // Calcola frequenza da nota MIDI e assegna all'oscillatore
    currentFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    oscillator.setFrequency(currentFrequency);

    // Imposta pressione di base (senza inviluppo ADSR)
    basePressure   = velocity;
    noteOn         = true;
    voiceIsActive  = true;
}

void SynthVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
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

void SynthVoice::pitchWheelMoved(int /*newValue*/) {
    // no-op
}

void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/) {
    // no-op
}
