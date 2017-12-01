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
    ASCIIVisualizer(unsigned int ads, AudioDataStyle style, std::string tag) 
      : visualizerTag_(tag)
      , audioSize_(ads)
      , audioStyle_(style)
    {  }

    // Destructor
    virtual ~ASCIIVisualizer() {  };

    // Member Functions
    virtual bool Update(float* data) ABSTRACT;

    // Pre-defined, but virtual functions. 
    // Can be fiddled with if you want, but defaults are reasonable.
    virtual bool UpdatePost() { return RConsole::Canvas::Update(); };
    virtual unsigned int GetAudioDataSize() { return audioSize_; }
    virtual AudioDataStyle GetAudioDataStyle() { return audioStyle_; }
    virtual std::string GetTag() { return visualizerTag_; }

  protected:
    // Variables
    std::string visualizerTag_;
    unsigned int audioSize_;
    AudioDataStyle audioStyle_;
  };

}


