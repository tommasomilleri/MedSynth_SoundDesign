#pragma once
#include <vector>

class GuitarString {
  public:
    GuitarString(double frequency, double sampleRate);
    void pluck();
    float process();
    void setFrequency(double frequency);

  private:
    std::vector<float> buffer;
    int bufferIndex;
    double sampleRate;
    double frequency;
    float lastSample = 0.0f;
};