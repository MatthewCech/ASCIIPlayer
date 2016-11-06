#pragma once
#include "Visualizer.hpp"



namespace ASCIIPlayer
{
  class HorizontalWaveformVisualizer : public Visualizer
  {
  public:
    // Constructor
    HorizontalWaveformVisualizer();

    // DrawBars
    bool Update(float* data);
  };
}