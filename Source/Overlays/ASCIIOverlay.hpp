#pragma once
#include "ASCIIOverlay.hpp"
#include "Defines.hpp"
#include <string>



namespace ASCIIPlayer
{
  struct UIInfo
  {
    UIInfo(float vol, std::string song, bool isPlaying)
      : Volume(vol)
      , Song(song)
      , IsPlaying(isPlaying)
    { }

    // Variables
    float Volume;
    std::string Song;
    bool IsPlaying;
  };

  // A class designed to represent UI Overlay.
  class ASCIIOverlay
  {
  public:
    virtual bool Update(const UIInfo uinfo) ABSTRACT;
    virtual bool UpdatePost() { };
  };
}
