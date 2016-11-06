// Default vizualizer if nothing else is defined.
#pragma once
#include "Defines.hpp"
#include "Visualizer.hpp"



namespace ASCIIPlayer
{
  class DefaultVisualizer : public Visualizer
  {
  public:
    // Constructor
    DefaultVisualizer();

    // DrawBars
    bool Update(float* data);
  };
}

namespace ASCIIPlayer
{
  class LargeWaveformVisualizer : public Visualizer
  {
  public:
    // Constructor
    LargeWaveformVisualizer();

    // DrawBars
    bool Update(float* data);
  };
}
