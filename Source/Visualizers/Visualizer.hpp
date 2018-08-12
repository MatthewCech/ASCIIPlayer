#pragma once
#include "Defines.hpp"


namespace ASCIIPlayer
{
  // NOTE(mcech): This is intended to be a base class for any type of 
  // visualizer, be it displayed with ConsoleUtils, OpenGL, or any other system.
  //
  // To see how this is used to create other base classes for a specific 
  // visualization style, check out ASCIIVisualizer.hpp.
  class Visualizer
  {
  public:
    // Constructor
    Visualizer(unsigned int audioDataSize, AudioDataStyle style) 
      : audioSize_(audioDataSize)
      , audioStyle_(style)
    {  }

    // Destructor
    virtual ~Visualizer() {  };

    // Member Functions
    virtual bool Update(float* data) ABSTRACT;
    virtual int Width() ABSTRACT;
    virtual int Height() ABSTRACT;

    // Pre-defined, but virtual functions. 
    // Can be fiddled with if you want, but defaults are reasonable.
    virtual void OnResize(int newWidth, int newHeight) {  }
    virtual bool UpdatePost() { return true; };
    virtual unsigned int GetAudioDataSize() { return audioSize_; }
    virtual AudioDataStyle GetAudioDataStyle() { return audioStyle_; }

  protected:
    // Variables
    unsigned int audioSize_;
    AudioDataStyle audioStyle_;
  };
}


