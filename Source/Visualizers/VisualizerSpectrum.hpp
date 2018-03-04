#pragma once
#include "ASCIIVisualizer.hpp"



namespace ASCIIPlayer
{
  class VisualizerSpectrum : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerSpectrum();

    // DrawBars
    bool Update(float* data);
  };
}

