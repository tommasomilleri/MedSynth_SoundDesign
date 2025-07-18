#pragma once
#include "InstrumentConfig.h"

class RebecConfig : public InstrumentConfig {
  public:
    // Attacco un po' più morbido per simulare il bowing medievale
    float getAttack() const override {
        return 0.04f; // 40 ms
    }
    // Decay rimane sui 200 ms, già nella fascia ideale
    float getDecay() const override {
        return 0.2f;
    }
    // Sustain più alto per mantenere corpo al suono
    float getSustain() const override {
        return 0.8f;
    }
    // Release leggermente più corto per evitare code troppo lunghe
    float getRelease() const override {
        return 0.5f; // 500 ms
    }

    // Sawtooth sul primo oscillatore
    int getOsc1Waveform() const override {
        return 1; // 1 = saw
    }
    // Square sul secondo oscillatore
    int getOsc2Waveform() const override {
        return 2; // 2 = square
    }
    // Mix moderato del secondo oscillatore
    float getOsc2Blend() const override {
        return 0.35f;
    }
    // Detune sottile di circa 5 centesimi
    float getOsc2Transpose() const override {
        return 1.0029f; // ~+5 cents
    }

    // Cutoff più alto per far passare gli armonici nasali
    float getFilterCutoff() const override {
        return 4500.0f;
    }
    // Risonanza moderata, senza auto-oscillazione
    float getFilterResonance() const override {
        return 0.30f;
    }

    // Mantieni ±2 semitoni di pitch bend
    float getPitchBendUp() const override {
        return 2.0f;
    }
    float getPitchBendDown() const override {
        return 2.0f;
    }

    // Modula il cutoff in base alla velocity (consigliato intorno a 1000–1500 Hz)
    float getVelocityToCutoffMod() const override {
        return 1200.0f;
    }
    // Gain generale, da regolare in mix
    float getMasterGain() const override {
        return 0.75f;
    }
};
