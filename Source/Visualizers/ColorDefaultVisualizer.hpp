#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class ColorDefaultVisualizer : public ASCIIVisualizer
  {
  public:
    // Constructor
		ColorDefaultVisualizer();// : Visualizer(32, aWaveform, "colorDefault") { };

    // DrawBars
		bool Update(float* data);// { return false; };
    int width_;
    int height_;
  };
}
