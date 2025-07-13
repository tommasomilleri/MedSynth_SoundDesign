#pragma once
#include <JuceHeader.h>

class BannerComponent : public juce::Component
{
  public:
    BannerComponent();
    void paint(juce::Graphics &) override;

  private:
    juce::Image rotated;
};
