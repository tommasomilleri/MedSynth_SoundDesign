#pragma once
#include "InstrumentConfig.h"

class LuteConfig : public InstrumentConfig {
  public:
    // －－－ ADSR －－－
    
    float getAttack() const override { return 1.2f; }   // ms
    float getDecay() const override { return 700.0f; }   // ms
    float getSustain() const override { return 1.0f; } // quasi nullo
    float getRelease() const override { return 3.0f; } // ms



    // －－－ Oscillatori －－－ (lasciamo inalterati)
    
    int getOsc1Waveform() const override { return 3; }    
    int getOsc2Waveform() const override { return 2; }
    float getOsc2Blend() const override { return 0.0f; }
    float getOsc2Transpose() const override { return 0.0f; }

    // －－－ Filtro －－－
    
    float getFilterCutoff() const override { return 650.0f; }  // Hz
    float getFilterResonance() const override { return 0.4f; } // Q
    float getVelocityToCutoffMod() const override { return 120.0f; }

    // －－－ Pitch Bend －－－
    float getPitchBendUp() const override { return 1.0f; }
    float getPitchBendDown() const override { return 1.0f; }

    // －－－ Master －－－
    float getMasterGain() const override { return 0.85f; }
};