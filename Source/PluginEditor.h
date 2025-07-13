
/*
#pragma once

#include <JuceHeader.h>               // deve essere per primo
#include "PluginProcessor.h"
#include "CarnivalLookAndFeel.h"      
#include "InstrumentType.h"
#include <map>
struct DraggableImageComponent : public juce::ImageComponent {
    juce::ComponentDragger dragger;

    void mouseDown(const juce::MouseEvent &e) override {
        dragger.startDraggingComponent(this, e);
    }

    void mouseDrag(const juce::MouseEvent &e) override {
        dragger.dragComponent(this, e, nullptr);

        // stampa i bounds di *questo* componente, non di luteImageComp
        DBG(getBounds().toString());
    }
};



class JuceSynthFrameworkAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::ComboBox::Listener {
  public:
    JuceSynthFrameworkAudioProcessorEditor(JuceSynthFrameworkAudioProcessor &);
    ~JuceSynthFrameworkAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    void comboBoxChanged(juce::ComboBox *box) override;

    JuceSynthFrameworkAudioProcessor &processor;

    // --- Componenti UI ---
    juce::ComboBox instrumentSelector;             // unica ComboBox
    juce::MidiKeyboardComponent keyboardComponent; // unica tastiera
    juce::Image background;                        // fascia decorativa
    juce::ImageComponent luteImageComp;
    // DraggableImageComponent luteImageComp;

    std::map<InstrumentType, juce::Image> imagesMap;
    std::map<InstrumentType, juce::Rectangle<int>> boundsMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessorEditor)
};
*/
#pragma once

#include <JuceHeader.h> // deve essere per primo
#include "PluginProcessor.h"
#include "CarnivalLookAndFeel.h"
#include "InstrumentType.h"
#include <map>

class JuceSynthFrameworkAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::ComboBox::Listener {
  public:
    JuceSynthFrameworkAudioProcessorEditor(JuceSynthFrameworkAudioProcessor &);
    ~JuceSynthFrameworkAudioProcessorEditor() override;
    //JuceSynthFrameworkAudioProcessorEditor(const JuceSynthFrameworkAudioProcessorEditor &) = delete;
    //JuceSynthFrameworkAudioProcessorEditor& operator=(const JuceSynthFrameworkAudioProcessorEditor &) = delete;

    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    void comboBoxChanged(juce::ComboBox *box) override;

    // Oggetto che gestisce il posizionamento e la rotazione dell'immagine
    void layoutInstrumentImage();

    JuceSynthFrameworkAudioProcessor &processor;

    // --- Componenti UI ---
    juce::ComboBox instrumentSelector;             // unica ComboBox
    juce::MidiKeyboardComponent keyboardComponent; // unica tastiera
    juce::Image background;                        // fascia decorativa
    juce::ImageComponent luteImageComp;            // componente per lo strumento

    // Mappe per immagini e bounds (caricate nel costruttore)
    std::map<InstrumentType, juce::Image> imagesMap;
    std::map<InstrumentType, juce::Rectangle<int>> boundsMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessorEditor)
};
