#pragma once
#include "ASCIIVisualizer.hpp"
#include <vector>

#define VISUALIZER_SPECTRUM_DATA_SIZE 128



namespace ASCIIPlayer
{
  // Spectrum analyzer
  class VisualizerSpectrum : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerSpectrum();

    // Functions
    bool Update(float* data);
    void OnResize(int newWidth, int newHeight);

    // Internal Variables
    int width_;
    int height_;
    float smoothed_[VISUALIZER_SPECTRUM_DATA_SIZE];

    // Time tracking
    long long lastTimeMS_;
    long long lastTimeSecond_;
    std::vector<long long> lastTimes_;
  };
}

