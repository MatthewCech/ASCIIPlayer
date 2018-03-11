#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerParticle : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerParticle();

    // DrawBars
    bool Update(float* data);
    int width_;
    int height_;
  };
}
