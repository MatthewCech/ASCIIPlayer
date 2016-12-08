#pragma once
#include "Visualizer.hpp"
#include <chrono>


namespace ASCIIPlayer
{
  class CenterVisualizer : public Visualizer
  {
  public:
    // Ctor
    CenterVisualizer();

    // Member functions
    bool Update(float* data);

  private:
    // Variables
    int lastWidth_;
    int lastHeight_;
    
    float *prev1_;
    float *prev2_;
    float *prev3_;
    int prevSize_;
    int frameDeleay_;
    int frameDelayMax_;
  };
}



