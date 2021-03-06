#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisaulizerSpotted : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisaulizerSpotted();

    // Member functions
    bool Update(float* data, float volume, bool isActive);
    void OnResize(int newWidth, int newHeight);

    // Variables
    int width_;
    int height_;
    float runningStart_;
    bool runningInvert_;
  };
}

