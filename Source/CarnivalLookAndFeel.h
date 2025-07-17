
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginEditor.h"
#include "InstrumentType.h"
#include "CarnivalLookAndFeel.h"


class CarnivalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawComboBox (juce::Graphics& g, int width, int height,
                       bool isButtonDown, int buttonX, int buttonY,
                       int buttonW, int buttonH, juce::ComboBox& box) override;
  juce::Font getComboBoxFont(juce::ComboBox &box) override;
};

