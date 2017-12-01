// Default vizualizer if nothing else is defined.
#pragma once
#include "ASCIIVisualizer.hpp"



namespace ASCIIPlayer
{
  class DefaultVisualizer : public ASCIIVisualizer
  {
  public:
    // Constructor
    DefaultVisualizer();

    // DrawBars
    bool Update(float* data);
  };
}

