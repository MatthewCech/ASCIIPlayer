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

    // DrawBars
    bool Update(float* data);

  private:
    size_t height_;
  };
}

