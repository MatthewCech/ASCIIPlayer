#pragma once
#include "Visualizer.hpp"



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
    int lastWidth_;
    int lastHeight_;
  };
}



