#pragma once
#include "Visualizers/ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerWaveform : public ASCIIVisualizer
  {
  public:
    // Constructor
		VisualizerWaveform();// : Visualizer(32, aWaveform, "colorDefault") { };

    // DrawBars
		bool Update(float* data);
    void OnResize(int newWidth, int newHeight);
    int width_;
    int height_;
  };
}
