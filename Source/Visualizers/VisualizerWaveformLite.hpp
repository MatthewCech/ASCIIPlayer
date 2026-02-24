#pragma once
#include "ASCIIVisualizer.hpp"

#define CUSTOM_SIZE 1024

// So if this is "8" this means "Collapse the input waveform data 8:1, where 8 entries are averaged to 1 bucket". 
// Useful for fitting things on screen.
#define SCALE_FACTOR 8


namespace ASCIIPlayer
{
  class VisualizerWaveformLite : public ASCIIVisualizer
  {
  public:
    // Ctor/Dtor
    VisualizerWaveformLite();
    ~VisualizerWaveformLite();

    void OnResize(int newWidth, int newHeight);
		bool Update(double dt, float* data, bool isActive);

  private:
    void initializeWorkspace();

    // Current width and height of console
    int width_;
    int height_;

    float* workspace_;
    int workspaceCount_;
  };
}
