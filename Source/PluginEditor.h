

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
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    void comboBoxChanged(juce::ComboBox *box) override;
    void layoutInstrumentImage();

    JuceSynthFrameworkAudioProcessor &processor;

    // --- Componenti UI ---
    juce::ComboBox instrumentSelector;             // unica ComboBox
    juce::MidiKeyboardComponent keyboardComponent; // unica tastiera
    juce::Image background;
    juce::ImageComponent luteImageComp;

    // Mappe per immagini e bounds (caricate nel costruttore)
    std::map<InstrumentType, juce::Image> imagesMap;
    std::map<InstrumentType, juce::Rectangle<int>> boundsMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceSynthFrameworkAudioProcessorEditor)
};
