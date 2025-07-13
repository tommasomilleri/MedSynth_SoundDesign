#pragma once
#include "InstrumentConfig.h"

class RebecConfig : public InstrumentConfig
{
  public:
    float getAttack() const override
    {
        return 0.025f;
    }
    float getDecay() const override
    {
        return 0.2f;
    }
    float getSustain() const override
    {
        return 0.6f;
    }
    float getRelease() const override
    {
        return 0.6f;
    }

    int getOsc1Waveform() const override
    {
        return 1;
    }
    int getOsc2Waveform() const override
    {
        return 2;
    }
    float getOsc2Blend() const override
    {
        return 0.4f;
    }
    float getOsc2Transpose() const override
    {
        return 1.01f;
    }

    float getFilterCutoff() const override
    {
        return 2600.0f;
    }
    float getFilterResonance() const override
    {
        return 0.65f;
    }

    float getPitchBendUp() const override
    {
        return 2.0f;
    }
    float getPitchBendDown() const override
    {
        return 2.0f;
    }

    float getVelocityToCutoffMod() const override
    {
        return 1000.0f;
    }
    float getMasterGain() const override
    {
        return 0.75f;
    }
};
/*

REBEC SYNTH CONFIG - SUGGERIMENTI E MIGLIORAMENTI

=========================
ADSR – Inviluppo
=========================
- Attack: 0.025f
  ➤ Suggerimento: aggiungere modulazione in base alla velocity.
    Esempio: attack = baseAttack + (1.0f - velocity) * 0.015f

- Decay: 0.2f → OK
- Sustain: 0.6f → OK, rende l’effetto di nota mantenuta
- Release: 0.6f → OK, rilascio realistico

=========================
Oscillatori
=========================
- Osc1Waveform: Saw → OK, ricco di armonici
- Osc2Waveform: Square → OK, aggiunge nasale/legno
- Osc2Blend: 0.4f
  ➤ Suggerimento: renderlo dinamico in base a velocity o inviluppo
- Osc2Transpose: 1.01f → OK, simulazione di doppie corde leggermente disallineate
- Alternativa: usare 2 saw leggermente detunati per un timbro più grezzo

=========================
 Filtro
=========================
- FilterCutoff: 2600.0f → OK, abbastanza aperto
- FilterResonance: 0.65f → OK
- Suggerimento extra: modula il cutoff con inviluppo o LFO per più realismo

=========================
Pitch Bend e Vibrato
=========================
- PitchBendUp: 2.0f → OK
- PitchBendDown: 2.0f → OK
- Suggerimento: aggiungere vibrato automatico con LFO (5–7 Hz, profondità crescente)

=========================
Modulazioni
=========================
- VelocityToCutoffMod: 1000.0f → OK, più brillantezza con colpi forti
- Suggerimento: aggiungere anche VelocityToGain, es: 0.25f per maggiore dinamica

=========================
Master Gain
=========================
- MasterGain: 0.75f → OK, bilanciato

=========================
Estensioni avanzate (opzionali)
=========================
- Bow noise: burst di rumore bianco filtrato all’attacco
- Formanti / EQ dinamico: variazioni EQ nel tempo per simulare il corpo dello strumento
- Stereo spread: leggero spread stereo sulle armoniche
- Modal resonator: filtri risonanti su 500–800 Hz per imitare risonanze del corpo
- Envelope → Filter: usa envelope del volume anche per modulare il cutoff
*/
