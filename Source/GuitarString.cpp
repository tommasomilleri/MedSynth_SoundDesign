#include "GuitarString.h"
#include <random>

GuitarString::GuitarString(double frequency, double sampleRate)
    : frequency(frequency), sampleRate(sampleRate), bufferIndex(0), lastSample(0.0f) {
    int bufferSize = static_cast<int>(sampleRate / frequency + 0.5);
    buffer.resize(bufferSize, 0.0f);
}

void GuitarString::pluck() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    for (auto &sample : buffer)
        sample = dist(gen);
    bufferIndex = 0;
}

float GuitarString::process() {
    float currentSample = buffer[bufferIndex];
    int nextIndex = (bufferIndex + 1) % buffer.size();
    float nextSample = buffer[nextIndex];
    float newSample = 0.9985f * 0.5f * (currentSample + nextSample);
    buffer[bufferIndex] = newSample;
    bufferIndex = nextIndex;
    // Filtro passa basso semplice
    float filtered = 0.5f * (lastSample + currentSample);
    lastSample = currentSample;
    return filtered;
}

void GuitarString::setFrequency(double newFrequency) {
    frequency = newFrequency;
    int bufferSize = static_cast<int>(sampleRate / frequency + 0.5);
    buffer.resize(bufferSize, 0.0f);
    bufferIndex = 0;
}