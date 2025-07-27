#pragma once
#include "InstrumentConfig.h"

class LuteConfig : public InstrumentConfig {
  public:
    // －－－ ADSR －－－
    float getAttack() const override { return 30.0f; }   
    float getDecay() const override { return 600.0f; }
    float getSustain() const override { return 1.2f; }
    float getRelease() const override { return 180.0f; }
    // －－－ Oscillatori －－－
    int getOsc1Waveform() const override { return 3; }    
    int getOsc2Waveform() const override { return 2; }
    float getOsc2Blend() const override { return 0.15f; }
    float getOsc2Transpose() const override { return 0.0f; }
    // －－－ Filtro －－－
    float getFilterCutoff() const override { return 900.0f; }
    float getFilterResonance() const override { return 0.10f; }
    float getVelocityToCutoffMod() const override { return 80.0f; }
    // －－－ Pitch Bend －－－
    float getPitchBendUp() const override { return 1.0f; }
    float getPitchBendDown() const override { return 1.0f; }
    // －－－ Master －－－
    float getMasterGain() const override { return 3.0f; }
};

/*
1. Quando arriva un evento MIDI di tipo “note on”, il metodo `startNote()` calcola la frequenza della nota (con una trasposizione di ottava), 
    determina la lunghezza della delay line (`pluckDelay`) e la riempie con un breve burst di rumore bianco per simulare il pizzico. 
    Contemporaneamente vengono configurati e avviati gli inviluppi ADSR (`ampEnv` e `filterEnv`) usando i valori provenienti da `currentConfig` e si chiama `noteOn()`
    per iniziare il ciclo di vita della voce.
2. All’interno di `renderNextBlock()`, per ogni campione audio si estrae un valore dalla delay line di Karplus-Strong, 
    lo si smorza con un semplice passa-basso, lo si re-inietta nella delay line per mantenere la risonanza e lo si scala con l’inviluppo di ampiezza e la velocity. 
    Il segnale così ottenuto passa poi attraverso tre filtri passa-banda fissi che simulano le formanti del corpo del liuto e 
    infine è filtrato nuovamente da un filtro passa-basso dinamico (`voiceFilter`) la cui frequenza di taglio è modulata dal filtro ADSR e dalla velocity.
3. Il sintetizzatore polifonico è gestito da `mySynth.renderNextBlock()`, che miscela l’output di tutte le voci attive in un unico buffer audio.
4. Subito dopo la sintesi, il buffer viene passato a `luteReverb.process()` per applicare la convoluzione con l’impulse response del liuto registrato, restituendo le risonanze caratteristiche del corpo dello strumento.
5. Senza interrompere il flusso, il buffer viene poi sottoposto a un riverbero algoritmico con `reverb.processStereo()`, che aggiunge ulteriore spazialità e profondità al suono.
6. A seguire si applica il chorus: dopo aver chiamato `chorus.reset()` e `chorus.prepare(spec)`, il metodo di processing del chorus introduce sottili modulazioni di delay multiple, 
    arricchendo lo spettro armonico e creando un effetto di leggera ampiezza stereofonica.
7. Per ridurre le distorsioni non lineari nei filtri e negli effetti, il flusso audio viene temporaneamente sovracampionamento:
    `upsampler.reset(); upsampler.initProcessing(samplesPerBlock)` eleva il sample rate prima dei processori critici, e `downsampler.reset(); 
    downsampler.initProcessing(samplesPerBlock)` lo riporta al valore originale al termine della catena.
8. Infine, si calcola il guadagno finale chiamando `masterGainSmoothed.setTargetValue()` con il valore desiderato e 
    si applica al buffer tramite `masterGainSmoothed.getNextValue()`, assicurando uno smoothing del volume che previene click o pop quando l’utente modifica il parametro.
*/