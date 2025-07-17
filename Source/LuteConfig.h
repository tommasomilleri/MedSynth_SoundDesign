#pragma once
#include "InstrumentConfig.h"

class LuteConfig : public InstrumentConfig {
  public:
    // －－－ ADSR －－－
    
    float getAttack() const override { return 4.2f; }   
    float getDecay() const override { return 250.0f; }
    float getSustain() const override { return 3.0f; }
    float getRelease() const override { return 3.0f; }
    // －－－ Oscillatori －－－
    int getOsc1Waveform() const override { return 3; }    
    int getOsc2Waveform() const override { return 2; }
    float getOsc2Blend() const override { return 0.0f; }
    float getOsc2Transpose() const override { return 0.0f; }
    // －－－ Filtro －－－
    float getFilterCutoff() const override { return 250.0f; }
    float getFilterResonance() const override { return 0.30f; }
    float getVelocityToCutoffMod() const override { return 80.0f; }
    // －－－ Pitch Bend －－－
    float getPitchBendUp() const override { return 1.0f; }
    float getPitchBendDown() const override { return 1.0f; }
    // －－－ Master －－－
    float getMasterGain() const override { return 1.0f; }
};