#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerWaveform : public ASCIIVisualizer
  {
  public:
    // Constructor
		VisualizerWaveform();

    // DrawBars
		bool Update(double dt, float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    int startingWidth_;
    int width_;
    int height_;
  };
}
