#pragma once
#include "ASCIIVisualizer.hpp"


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
    long long lastTime_;
  };
}

