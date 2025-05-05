#pragma once
#include "ASCIIVisualizer.hpp"
#include "Defines.hpp"

namespace ASCIIPlayer
{
  class VisualizerSpectrum : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerSpectrum();

    // DrawBars
    bool Update(double dt, float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    int width_;
    int height_;
  };
}