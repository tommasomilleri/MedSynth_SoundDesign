# 🏰 MedSynth: Joglar 

[![en](https://img.shields.io/badge/lang-English-blue.svg)](#english-version) 
[![it](https://img.shields.io/badge/lang-Italiano-green.svg)](#versione-italiana)
![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![JUCE](https://img.shields.io/badge/JUCE-Framework-4caf50?style=flat)

---

## <a name="english-version"></a> 🇬🇧 English Version

**MedSynth: Joglar** is a custom software synthesizer developed in **C++** using the **JUCE Framework**. It bridges the gap between ancient acoustics and modern digital signal processing (DSP), designed specifically to emulate and explore the unique soundscapes of medieval instruments. It is an ideal tool for composers and sound designers crafting audio for historical interactive media, cinematic scores, and game audio.

### ✨ Key Features
* **Medieval Sound Engines**: Custom configurations tailored for historical realism:
  * 🪕 **Lute** (Configured via `LuteConfig.h`)
  * 🎻 **Rebec** (Configured via `RebecConfig.h`)
* **Robust OOP Architecture**: A highly modular, object-oriented C++ codebase ensuring scalable and efficient DSP.
* **Custom DSP Engine & Audio Routing**: 
  * Dual Oscillator architecture (`Oscillator.h` / `Oscillator2.h`) for complex, rich wave generation.
  * Integrated multi-mode Filter (`Filter.h`) for precise frequency sculpting.
  * Fully polyphonic synth voice management (`SynthVoice`, `SynthSound`).
  * Clean signal chain routing natively integrated within the JUCE `PluginProcessor` audio block.
* **Immersive UI/UX**: A bespoke graphic interface built with `CarnivalLookAndFeel`, incorporating high-quality thematic assets (lute and rebec PNGs, custom backgrounds) for an engaging user experience.
* **Versatile Formats**: Can be built as a Standalone application or as a Plugin (VST3/AU) for major Digital Audio Workstations (DAWs).

### 📂 Repository Structure
* `/Source`: Contains all C++ headers (`.h`) and source (`.cpp`) files, encapsulating DSP logic, UI components (`BannerComponent`, `PluginEditor`), and graphical assets.
* `Joglar.jucer`: The Projucer project file to automatically generate IDE-specific build files (Visual Studio, Xcode, etc.).
* `Joglar.exe`: A pre-compiled standalone executable for immediate use on Windows.
* `Documentazione.html`: The official, in-depth documentation detailing the project's technical specifications.

### 🚀 Getting Started

#### Prerequisites
To build this project from source, you will need:
* [JUCE Framework](https://juce.com/) (specifically the Projucer application).
* A C++ compiler (Visual Studio for Windows, Xcode for macOS).

#### Installation & Build
1. Clone the repository:
   ```bash
   git clone [https://github.com/tommasomilleri/medsynth_sounddesign.git](https://github.com/tommasomilleri/medsynth_sounddesign.git)
   ```
2. Open `Joglar.jucer` with **Projucer**.
3. Select your target exporter (e.g., Visual Studio 2022) and click **"Save Project and Open in IDE"**.
4. Build the solution in your IDE (target Standalone or VST3).
5. *Alternatively (Windows only)*: Run the pre-compiled `Joglar.exe` to test the standalone synthesizer instantly without compiling.

### 📚 Documentation
For a deep dive into the software architecture, custom DSP algorithms, and UI state management, please refer to the included [`Documentazione.html`](./Documentazione.html) file.

### 👤 Author
**Tommaso Milleri** *Developed as part of an advanced Audio Development & Sound Design project.*

---
---

## <a name="versione-italiana"></a> 🇮🇹 Versione Italiana

**MedSynth: Joglar** è un sintetizzatore software personalizzato sviluppato in **C++** tramite il **Framework JUCE**. Unisce il fascino delle acustiche antiche con il moderno Digital Signal Processing (DSP), ed è progettato per emulare i paesaggi sonori unici degli strumenti medievali. È uno strumento ideale per compositori e sound designer che lavorano su media interattivi a tema storico, colonne sonore o audio per videogiochi.

### ✨ Funzionalità Principali
* **Motori Sonori Medievali**: Configurazioni personalizzate mirate al realismo storico:
  * 🪕 **Liuto** (Configurato tramite `LuteConfig.h`)
  * 🎻 **Ribeca** (Configurata tramite `RebecConfig.h`)
* **Solida Architettura OOP**: Una base di codice C++ modulare e orientata agli oggetti che garantisce un DSP efficiente e scalabile.
* **Motore DSP Custom e Routing Audio**: 
  * Architettura a doppio oscillatore (`Oscillator.h` / `Oscillator2.h`) per la generazione di onde complesse e ricche.
  * Filtro multi-modale integrato (`Filter.h`) per un controllo preciso delle frequenze.
  * Gestione della voce di sintesi completamente polifonica (`SynthVoice`, `SynthSound`).
  * Signal chain pulita e lineare, instradata nativamente all'interno del blocco audio del `PluginProcessor` di JUCE.
* **UI/UX Immersiva**: Interfaccia grafica su misura realizzata con `CarnivalLookAndFeel`, che incorpora asset tematici di alta qualità (sfondi e render PNG del liuto e della ribeca) per un'esperienza utente coinvolgente.
* **Formati Versatili**: Può essere compilato come applicazione Standalone o come Plugin (VST3/AU) per le principali Digital Audio Workstation (DAW).

### 📂 Struttura del Repository
* `/Source`: Contiene tutti i file sorgente (`.cpp`) e gli header (`.h`), inclusa la logica DSP, i componenti UI (`BannerComponent`, `PluginEditor`) e le risorse grafiche.
* `Joglar.jucer`: Il file di progetto Projucer necessario per generare automaticamente le build per IDE specifici (Visual Studio, Xcode, ecc.).
* `Joglar.exe`: L'eseguibile standalone precompilato, pronto all'uso immediato per sistemi Windows.
* `Documentazione.html`: La documentazione ufficiale e dettagliata con le specifiche tecniche del progetto.

### 🚀 Come Iniziare

#### Prerequisiti
Per compilare questo progetto dal codice sorgente, avrai bisogno di:
* [Framework JUCE](https://juce.com/) (in particolare l'applicazione Projucer).
* Un compilatore C++ (Visual Studio per Windows, Xcode per macOS).

#### Installazione e Compilazione
1. Clona il repository:
   ```bash
   git clone [https://github.com/tommasomilleri/medsynth_sounddesign.git](https://github.com/tommasomilleri/medsynth_sounddesign.git)
   ```
2. Apri il file `Joglar.jucer` con **Projucer**.
3. Seleziona il tuo target exporter (es. Visual Studio 2022) e clicca su **"Save Project and Open in IDE"**.
4. Compila la soluzione nel tuo IDE (scegliendo come target Standalone o VST3).
5. *In alternativa (solo per Windows)*: Avvia direttamente il file precompilato `Joglar.exe` per testare subito il sintetizzatore in versione standalone senza dover compilare nulla.

### 📚 Documentazione
Per un'analisi approfondita dell'architettura del software, degli algoritmi DSP personalizzati e della gestione degli stati della UI, si prega di consultare il file [`Documentazione.html`](./Documentazione.html) incluso nel repository.

### 👤 Autore
**Tommaso Milleri** *Sviluppato come parte di un progetto avanzato di Audio Development e Sound Design.*
