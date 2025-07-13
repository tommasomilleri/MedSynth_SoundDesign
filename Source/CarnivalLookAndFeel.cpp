/*
  ==============================================================================

    CarnivalLookAndFeel.cpp
    Created: 24 Apr 2025 6:11:36pm
    Author:  Utente

  ==============================================================================
*/
#include <JuceHeader.h>

#include <juce_gui_basics/juce_gui_basics.h>
#include "CarnivalLookAndFeel.h"

void CarnivalLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                       bool /*isButtonDown*/, int buttonX, int buttonY,
                                       int buttonW, int buttonH, juce::ComboBox& box)
{
    // Disegna l'immagine di sfondo (es. texture di legno)
    //juce::Image img = juce::ImageCache::getFromMemory(BinaryData::sfondo_png, BinaryData::sfondo_pngSize);
    //g.drawImageWithin(img, 0, 0, width, height, juce::RectanglePlacement::stretchToFit);

    // Disegna sfondo colore beige
    g.setColour(juce::Colour::fromRGB(255, 240, 200));  // beige carta pergamena
    g.fillRoundedRectangle(0, 0, width, height, 8.0f);

    // Disegna il bordo
    g.setColour(juce::Colour::fromRGB(160, 100, 50));  // marrone ornamento
    g.drawRoundedRectangle(0, 0, width, height, 8.0f, 2.0f);

    // Disegna il testo (imposta il font rinascimentale)
    //g.setColour(juce::Colour::fromRGB(0, 0, 0));  // colore del testo
    //g.setFont(juce::Font("Cinzel Decorative", 16.0f, juce::Font::bold));
    //g.drawText(box.getText(), 10, 10, width - 20, height - 20, juce::Justification::centred, true);

}
Font CarnivalLookAndFeel::getComboBoxFont(ComboBox &box) {
    return juce::Font("Cinzel Decorative", 16.0f, juce::Font::bold);
}
