#pragma once
#include "ASCIIVisualizer.hpp"



namespace ASCIIPlayer
{
  class HorizontalWaveformVisualizer : public ASCIIVisualizer
  {
  public:
    // Constructor
    HorizontalWaveformVisualizer();

    // DrawBars
    bool Update(float* data);
  };
}