# JUCE Synth Framework (Aggiornato)

Il **JUCE Synth Framework** è un’architettura modulare per lo sviluppo di sintetizzatori software come plugin VST/AU. Offre una base estensibile e ottimizzata, con supporto per motori di sintesi multipli, gestione avanzata di parametri e un’interfaccia grafica flessibile.

---

## Indice

1. [Panoramica](#panoramica)
2. [Caratteristiche Principali](#caratteristiche-principali)
3. [Prerequisiti](#prerequisiti)
4. [Configurazione e Build](#configurazione-e-build)
5. [Struttura del Progetto](#struttura-del-progetto)
6. [Utilizzo](#utilizzo)
7. [Licenza](#licenza)

---

## <a name="panoramica"></a>Panoramica

Questo framework fornisce:

* Un **AudioProcessor** configurabile via parametri ADSR, filtri, LFO ed effetti.
* Una classe **SynthVoice** estensibile per definire la generazione audio di ogni voce.
* Un sistema di gestione centralizzata dei parametri tramite **AudioProcessorValueTreeState**.
* Integrazione opzionale con la libreria di sintesi **Maximilian**.

---

## <a name="caratteristiche-principali"></a>Caratteristiche Principali

* **Multipiattaforma**: compatibile con macOS e Windows.
* **Modularità**: separazione netta di logica audio, interfaccia e configurazione.
* **Espressività**: supporto a pitch-bend, modulazioni, LFO e inviluppi complessi.
* **GUI**: componenti pronte all’uso per slider, knob, visualizzazioni spettrali e tastiera MIDI.
* **Performance**: ottimizzato per bassa latenza, stabilità e utilizzo efficiente della CPU.

---

## <a name="prerequisiti"></a>Prerequisiti

* **JUCE** versione 5.4 o successiva
* **C++17**
* **Maximilian** (opzionale) nella stessa cartella principale del framework
* IDE di sviluppo:

  * **Windows**: Visual Studio 2017 o successivo
  * **macOS**: Xcode

---

## <a name="configurazione-e-build"></a>Configurazione e Build

1. Posiziona le cartelle del framework e di Maximilian come directory "sibling" (stesso livello).
2. Apri il file `.jucer` con Projucer.
3. Genera il progetto per il tuo ambiente:

   * Xcode (macOS)
   * Visual Studio (Windows)
4. Compila e avvia in modalità Debug o Release.

---

## <a name="struttura-del-progetto"></a>Struttura del Progetto

```
juceSynthFramework/
├── include/           # Header (*.h)
├── src/               # Codice sorgente (*.cpp)
├── juceSynthFramework.jucer
├── Resources/         # Asset audio e grafici
└── examples/          # Progetti demo e template

Maximilian/           # Libreria di sintesi opzionale
```

---

## <a name="utilizzo"></a>Utilizzo

* **Nuovo Strumento**: estendi `InstrumentConfig` e definisci inviluppo, oscillatori e filtri.
* **Sintesi per Voce**: implementa `SynthVoice::renderNextBlock` con la logica di generazione.
* **Parametri**: registra automazioni e salvataggio in `AudioProcessorValueTreeState`.
* **Interfaccia**: crea pannelli con componenti JUCE (`Slider`, `ComboBox`, `MidiKeyboardComponent`).

---

## <a name="licenza"></a>Licenza

Rilasciato sotto licenza **MIT**. Consultare il file LICENSE per i dettagli.
