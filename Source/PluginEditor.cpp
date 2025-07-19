
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "PluginEditor.h"
#include "CarnivalLookAndFeel.h"
#include "InstrumentType.h"
#include <JuceHeader.h>

static CarnivalLookAndFeel carnivalLookAndFeel;


JuceSynthFrameworkAudioProcessorEditor::JuceSynthFrameworkAudioProcessorEditor(JuceSynthFrameworkAudioProcessor &p)
    : AudioProcessorEditor(&p), processor(p), keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard) {
    {

        juce::Image src = juce::ImageCache::getFromMemory(BinaryData::sfondo1_jpg, BinaryData::sfondo1_jpgSize);
        constexpr float scale = 0.5f;
        int scaledW = static_cast<int>(std::round(src.getWidth() * scale));
        int scaledH = static_cast<int>(std::round(src.getHeight() * scale));
        setSize(scaledW, scaledH);
        setResizable(false, false);

        background = juce::Image(src.getFormat(), src.getWidth(), src.getHeight(), true);

     
        juce::Graphics g(background);
        g.drawImageTransformed(src,
                               juce::AffineTransform::identity);
        imagesMap[InstrumentType::Lute] = juce::ImageFileFormat::loadFrom(
            BinaryData::LUTE_noBG_png,
            BinaryData::LUTE_noBG_pngSize);
addAndMakeVisible(luteImageComp);
    auto img = juce::ImageFileFormat::loadFrom(
        BinaryData::LUTE_noBG_png,
        (size_t)BinaryData::LUTE_noBG_pngSize);
    luteImageComp.setImage(img, juce::RectanglePlacement::centred);
    imagesMap[InstrumentType::Rebec] = juce::ImageFileFormat::loadFrom(
        BinaryData::rebecfrontPhotoroom_png,
        BinaryData::rebecfrontPhotoroom_pngSize);
    addAndMakeVisible(luteImageComp);
    luteImageComp.setImage(imagesMap[processor.getCurrentInstrument()],
                           juce::RectanglePlacement::centred);
    }
    
    instrumentSelector.onChange = [this] {
        auto sel = static_cast<InstrumentType>(instrumentSelector.getSelectedId());
        processor.setInstrument(sel);
        luteImageComp.setImage(imagesMap[sel], juce::RectanglePlacement::centred);
        luteImageComp.repaint();
    };
    instrumentSelector.setLookAndFeel(&carnivalLookAndFeel);
    instrumentSelector.setJustificationType(juce::Justification::centred);
    instrumentSelector.setColour(juce::ComboBox::textColourId,
                                 juce::Colours::black);
    instrumentSelector.addItem("Lute", static_cast<int>(InstrumentType::Lute));
    instrumentSelector.addItem("Rebec", static_cast<int>(InstrumentType::Rebec));
    instrumentSelector.onChange = [this] {
        auto sel = static_cast<InstrumentType>(instrumentSelector.getSelectedId());
        processor.setInstrument(sel);

        luteImageComp.setImage(imagesMap[sel],
                               juce::RectanglePlacement::centred);
        luteImageComp.repaint();
    };
    keyboardComponent.setLookAndFeel(&carnivalLookAndFeel);
    keyboardComponent.setKeyWidth(24.0f);
    keyboardComponent.setAvailableRange(0, 127);
    keyboardComponent.setOpaque(true);          
    keyboardComponent.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour::fromRGB(255, 255, 240));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour::fromRGB(30, 30, 30));
    addAndMakeVisible(instrumentSelector);
    
    instrumentSelector.setJustificationType(juce::Justification::centred);

    instrumentSelector.setColour(
        juce::ComboBox::textColourId,
        juce::Colours::black);
    instrumentSelector.setColour(
        juce::ComboBox::backgroundColourId,
        juce::Colour::fromRGB(255, 240, 200));

    instrumentSelector.setColour(
        juce::ComboBox::arrowColourId,
        juce::Colours::black);
    addAndMakeVisible(keyboardComponent);
}

JuceSynthFrameworkAudioProcessorEditor::~JuceSynthFrameworkAudioProcessorEditor() {
    instrumentSelector.setLookAndFeel(nullptr);
    keyboardComponent.setLookAndFeel(nullptr);
}

void JuceSynthFrameworkAudioProcessorEditor::resized() {
    auto area = getLocalBounds().reduced(6);

    const int keyboardH = 100; 
    const int comboH = 30;

    keyboardComponent.setBounds(area.removeFromBottom(keyboardH));

    auto w = getWidth();
    auto h = getHeight();
    int comboW = int(w * 0.3f);
    int comboX = int(w * 0.468f);
    int comboY = int(h * 0.1855f);

    // 4) Imposta la comboBox
    instrumentSelector.setBounds(comboX, comboY,
                                 comboW, comboH);
    luteImageComp.setBounds(-60, 144, 550, 550);
    
}

void JuceSynthFrameworkAudioProcessorEditor::paint(juce::Graphics &g) {
    // 1) Sfondo tipo pergamena chiara
    g.fillAll(juce::Colour::fromRGB(250, 244, 230));

    // 2) Cornice decorativa in cima (sopra la tastiera e ComboBox)
    g.drawImageWithin(background,
                      0, 0, getWidth(), getHeight(),
                      juce::RectanglePlacement::fillDestination);
}

void JuceSynthFrameworkAudioProcessorEditor::comboBoxChanged(juce::ComboBox *box) {
    if (box == &instrumentSelector)
        processor.setInstrument(static_cast<InstrumentType>(box->getSelectedId()));
}