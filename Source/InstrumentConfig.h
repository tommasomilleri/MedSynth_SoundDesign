#pragma once

class InstrumentConfig
{
public:
    virtual ~InstrumentConfig() = default;

    // Parametri inviluppo
    virtual float getAttack() const = 0;
    virtual float getDecay() const = 0;
    virtual float getSustain() const = 0;
    virtual float getRelease() const = 0;

    // Oscillatori
    virtual int getOsc1Waveform() const = 0;
    virtual int getOsc2Waveform() const = 0;
    virtual float getOsc2Blend() const { return 0.0f; }      // 0.0 = off, 1.0 = massimo
    virtual float getOsc2Transpose() const { return 1.0f; }  // 0.5 = un'ottava sotto, 1.0 = stessa ottava

    // Filtro
    virtual float getFilterCutoff() const = 0;
    virtual float getFilterResonance() const = 0;

    // Pitch bend range
    virtual float getPitchBendUp() const = 0;
    virtual float getPitchBendDown() const = 0;

    // Modulatori opzionali
    virtual float getVelocityToCutoffMod() const { return 0.0f; }
    virtual float getMasterGain() const = 0;

};
