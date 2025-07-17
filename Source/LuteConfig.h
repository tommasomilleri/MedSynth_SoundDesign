
#pragma once
#include "InstrumentConfig.h"

class LuteConfig : public InstrumentConfig {
  public:
    // －－－ ADSR －－－
    /*
    float getAttack() const override { return 0.003f; } 
    float getDecay() const override { return 0.3f; }   
    float getSustain() const override { return 0.08f; } 
    float getRelease() const override { return 0.40f; } 
    */
    float getAttack() const override { return 3.0f; }   // ms
    float getDecay() const override { return 250.0f; }   // ms
    float getSustain() const override { return 0.1f; } // ms (level 0→1, ma per simplicity lo manteniamo in ms)
    float getRelease() const override { return 180.0f; }  // ms

    // －－－ Oscillatori －－－ (lasciamo inalterati)
    int getOsc1Waveform() const override { return 1; }
    int getOsc2Waveform() const override { return 2; }
    float getOsc2Blend() const override { return 0.0f; }
    float getOsc2Transpose() const override { return 0.0f; }

    // －－－ Filtro －－－
    float getFilterCutoff() const override { return 800.0f; }
    float getFilterResonance() const override { return 5.0f; }
    float getVelocityToCutoffMod() const override { return 300.0f; }

    // －－－ Pitch Bend －－－
    float getPitchBendUp() const override { return 1.0f; }
    float getPitchBendDown() const override { return 1.0f; }

    // －－－ Master －－－
    float getMasterGain() const override { return 0.9f; }
};