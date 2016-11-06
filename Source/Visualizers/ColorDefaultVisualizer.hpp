#pragma once
#include "Visualizer.hpp"


namespace ASCIIPlayer
{
  class ColorDefaultVisualizer : public Visualizer
  {
  public:
    // Constructor
    ColorDefaultVisualizer();

    // DrawBars
    bool Update(float* data);
    int width_;
    int height_;
  };
}
