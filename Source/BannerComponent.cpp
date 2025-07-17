
#include <JuceHeader.h>
#include "BannerComponent.h"

BannerComponent::BannerComponent()
{
    // carica l’immagine
    juce::Image src = juce::ImageCache::getFromMemory (BinaryData::sfondo_png,
                                                       BinaryData::sfondo_pngSize);

    // prendi la banda alta (220 px) e ruotala
    const int sliceH = 220;
    juce::Image slice = src.getClippedImage ({ 0, 0, src.getWidth(), sliceH });

    rotated = juce::Image (slice.getFormat(), sliceH, src.getWidth(), true);
    juce::Graphics g (rotated);
    g.drawImageTransformed (slice,
                            juce::AffineTransform::rotation (-juce::MathConstants<float>::halfPi)
                                                  .translated (0, static_cast<float> (src.getWidth())));
}

void BannerComponent::paint (juce::Graphics& g)
{
    g.drawImageWithin (rotated, 0, 0, getWidth(), getHeight(),
                       juce::RectanglePlacement::stretchToFit);
}
