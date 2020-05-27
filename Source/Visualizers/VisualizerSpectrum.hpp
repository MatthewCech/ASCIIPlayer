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
    bool Update(float* data, float volume, bool isActive);
    void OnResize(int newWidth, int newHeight);

    // Internal Variables
    int width_;
    int height_;
    float smoothed_[VISUALIZER_SPECTRUM_DATA_SIZE];

    // Time tracking
    std::int64_t lastTimeMS_;
    std::int64_t lastTimeSecond_;
    std::vector<std::int64_t> lastTimes_;
  };
}

