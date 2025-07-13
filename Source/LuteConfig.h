
#pragma once
#include "InstrumentConfig.h"

class LuteConfig : public InstrumentConfig {
  public:
    // －－－ ADSR －－－
    float getAttack() const override { return 0.002f; } // Attack leggermente più lento per non suonare troppo “click”
    float getDecay() const override { return 0.18f; }   // Decay un po’ più rapido per dare più snap
    float getSustain() const override { return 0.05f; } // Poco sustain, giusto un filo per corpo
    float getRelease() const override { return 0.30f; } // Release un po’ più lungo per far sciogliere le armoniche

    // －－－ Oscillatori －－－
    int getOsc1Waveform() const override { return 3; }       // Triangle rimane ok per morbidezza
    int getOsc2Waveform() const override { return 2; }       // Square al posto di saw per armoniche meno “taglienti”
    float getOsc2Blend() const override { return 0.15f; }    // Riduci ancora la componente Osc2
    float getOsc2Transpose() const override { return 0.5f; } // Ottava sotto confermata

    // －－－ Filtro －－－
    float getFilterCutoff() const override { return 3500.0f; }       // Apri un po’ di più per più brillantezza
    float getFilterResonance() const override { return 0.35f; }      // Risonanza più morbida
    float getVelocityToCutoffMod() const override { return 800.0f; } // Più modulazione da velocity sui transienti

    // －－－ Pitch Bend －－－
    float getPitchBendUp() const override { return 1.0f; }
    float getPitchBendDown() const override { return 1.0f; }

    // －－－ Master －－－
    float getMasterGain() const override { return 1.1f; } // Alza leggermente il livello globale
};

/*

LUTE SYNTH CONFIG - SUGGERIMENTI E MIGLIORAMENTI

=========================
 ADSR – Inviluppo
=========================
- Attack: 0.005f → ottimo per pizzicato secco e preciso
- Decay: 0.25f → decadimento naturale e realistico
- Sustain: 0.1f → mantiene un leggero corpo dopo il pizzico
- Release: 0.3f → sfuma dolcemente, buona scelta
✔ Nessun cambiamento necessario.

=========================
Oscillatori
=========================
- Osc1Waveform: Triangle (3) → suono morbido e musicale
- Osc2Waveform: Saw (1) → armonici e corpo extra
- Osc2Blend: 0.2f → perfetto equilibrio
- Osc2Transpose: 0.5f → ottava sotto, aggiunge pienezza

Suggerimento:
➤ Prova a rendere dinamico l’Osc2Blend (es. in base alla velocity)
➤ Alternativa: Square come Osc2 per più brillantezza se desiderato

=========================
Filtro
=========================
- FilterCutoff: 1800.0f → taglia bene gli acuti, tono dolce
- FilterResonance: 0.7f → definisce il bordo ma senza squillare

Suggerimenti:
➤ Se suono troppo ovattato → prova 2200–2400 Hz
➤ Se vuoi più dolcezza → scendi a 1500 Hz con resonance 0.5

=========================
Pitch Bend
=========================
- PitchBendUp/Down: ±1.0f → giusto per strumenti antichi intonati

=========================
 Velocity Modulation
=========================
- Attualmente disattivata (0.0f)
➤ Suggerimento: attiva con 600–800f per brillanza realistica su note forti

=========================
 Miglioramenti Avanzati
=========================
- Noise burst all’attacco:
  ➤ Aggiungi un brevissimo rumore filtrato per simulare il pizzico delle dita
- Envelope sul cutoff:
  ➤ Attacco più brillante → filtro leggermente più aperto nei primi 100–200 ms
- Micro randomizzazione:
  ➤ Pitch o velocity leggermente variabili per suono più umano
  */
