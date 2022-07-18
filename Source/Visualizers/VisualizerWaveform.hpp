#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerWaveform : public ASCIIVisualizer
  {
  public:
    // Constructor
		VisualizerWaveform();// : Visualizer(32, aWaveform, "colorDefault") { };

    // DrawBars
		bool Update(float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);
    int startingWidth_;
    int width_;
    int height_;
  };
}
