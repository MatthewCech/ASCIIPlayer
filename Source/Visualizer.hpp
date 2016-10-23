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
    Visualizer(AudioDataSize ads, std::string tag) : audioSize_(ads), visualizerTag_(tag) {  }
    virtual ~Visualizer() {  };

    // Member Functions
    virtual bool Update(float* data, AudioDataSize size) DJ_ABSTRACT;
    AudioDataSize GetAudioDataSize() { return audioSize_;  }
    std::string GetTag() { return visualizerTag_; }

  private:
    // Variables
    std::string visualizerTag_;
    AudioDataSize audioSize_;
  };

}


