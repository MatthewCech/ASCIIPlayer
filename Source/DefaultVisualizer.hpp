// Default vizualizer if nothing else is defined.
#pragma once
#include "CustomDefines.hpp"
#include "Visualizer.hpp"



namespace ASCIIPlayer
{
  class DefaultVisualizer : public Visualizer
  {
  public:
    // Constructor
    DefaultVisualizer();
    virtual ~DefaultVisualizer();

    // DrawBars
    bool Update(float* data, AudioDataSize size);

  private:
    // Variables
    char oddChar;
    char evenChar;
  };
}
