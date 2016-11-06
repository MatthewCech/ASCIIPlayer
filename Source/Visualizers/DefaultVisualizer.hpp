// Default vizualizer if nothing else is defined.
#pragma once
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

