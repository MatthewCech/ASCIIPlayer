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
    bool Update(float* data);
    void OnResize(int newWidth, int newHeight);

    // Variables
    int width_;
    int height_;
  };
}

