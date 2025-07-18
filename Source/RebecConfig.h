#pragma once
#include "InstrumentConfig.h"

class RebecConfig : public InstrumentConfig {
  public:
    float getAttack() const override {
        return 0.04f;
    }
    float getDecay() const override {
        return 0.2f;
    }
    float getSustain() const override {
        return 0.8f;
    }
    float getRelease() const override {
        return 0.5f;
    }

    int getOsc1Waveform() const override {
        return 1;
    }
    int getOsc2Waveform() const override {
        return 2;
    }
    float getOsc2Blend() const override {
        return 0.35f;
    }
    float getOsc2Transpose() const override {
        return 1.0029f;
    }
    float getFilterCutoff() const override {
        return 4500.0f;
    }
    float getFilterResonance() const override {
        return 0.30f;
    }
    float getPitchBendUp() const override {
        return 2.0f;
    }
    float getPitchBendDown() const override {
        return 2.0f;
    }
    float getVelocityToCutoffMod() const override {
        return 1200.0f;
    }
    float getMasterGain() const override {
        return 0.75f;
    }
};
