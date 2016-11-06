// A base class for visualizer objects.
#pragma once
#include "Defines.hpp"
#include <string>


namespace ASCIIPlayer
{
  class Visualizer
  {
  public:
    // Constructor
    Visualizer(AudioDataSize ads, AudioDataStyle style, std::string tag) : visualizerTag_(tag), audioSize_(ads), audioStyle_(style){  }
    virtual ~Visualizer() {  };

    // Member Functions
    virtual bool Update(float* data) DJ_ABSTRACT;
    AudioDataSize GetAudioDataSize() { return audioSize_; }
    AudioDataStyle GetAudioDataStyle() { return audioStyle_; }
    std::string GetTag() { return visualizerTag_; }

  protected:
    // Variables
    std::string visualizerTag_;
    AudioDataSize audioSize_;
    AudioDataStyle audioStyle_;
  };

}


