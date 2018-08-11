// A base class for visualizer objects.
#pragma once
#include "Defines.hpp"
#include <string>
#include <ConsoleUtils\console-utils.hpp>


namespace ASCIIPlayer
{
  class ASCIIVisualizer
  {
  public:
    // Constructor
    ASCIIVisualizer(unsigned int ads, AudioDataStyle style) 
      : audioSize_(ads)
      , audioStyle_(style)
    {  }

    // Destructor
    virtual ~ASCIIVisualizer() {  };

    // Member Functions
    virtual bool Update(float* data) ABSTRACT;
    virtual void OnResize(int newWidth, int newHeight) {  } // Implement this some day.

    // Pre-defined, but virtual functions. 
    // Can be fiddled with if you want, but defaults are reasonable.
    virtual bool UpdatePost() { return true; };
    virtual unsigned int GetAudioDataSize() { return audioSize_; }
    virtual AudioDataStyle GetAudioDataStyle() { return audioStyle_; }

  protected:
    // Variables
    unsigned int audioSize_;
    AudioDataStyle audioStyle_;
  };

}


