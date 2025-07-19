
#include <JuceHeader.h>

#include <juce_gui_basics/juce_gui_basics.h>
#include "CarnivalLookAndFeel.h"

void CarnivalLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                       bool /*isButtonDown*/, int buttonX, int buttonY,
                                       int buttonW, int buttonH, juce::ComboBox& box)
{
    
    // Disegna sfondo colore beige
    g.setColour(juce::Colour::fromRGB(255, 240, 200));
    g.fillRoundedRectangle(0, 0, width, height, 8.0f);

    // Disegna il bordo
    g.setColour(juce::Colour::fromRGB(160, 100, 50));
    g.drawRoundedRectangle(0, 0, width, height, 8.0f, 2.0f);

}
Font CarnivalLookAndFeel::getComboBoxFont(ComboBox &box) {
    return juce::Font("Cinzel Decorative", 16.0f, juce::Font::bold);
}
