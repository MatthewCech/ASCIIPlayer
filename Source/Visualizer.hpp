// A base class for visualizer objects.
#pragma once
#include "CustomDefines.hpp"



namespace ASCIIPlayer
{
  class Visualizer
  {
  public:
    // Constructor
    Visualizer();
    virtual ~Visualizer();

    // Member Functions
    virtual bool Update(float* data, AudioDataSize size);

  private:
    // Variables
    static unsigned long long visualizerUniqueID_;
  };
}
