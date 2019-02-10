// Default vizualizer if nothing else is defined.
#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerDefault : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerDefault();

    // Member functions
    bool Update(float* data);
    void OnResize(int newWidth, int newHeight);

  private:
    int height_;
    int width_;
  };
}

