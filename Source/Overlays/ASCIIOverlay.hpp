#pragma once
#include "ASCIIOverlay.hpp"
#include "Defines.hpp"
#include <string>
#include <ConsoleUtils\console-utils.hpp>



namespace ASCIIPlayer
{
  struct UIInfo
  {
    // Constructors
    UIInfo() : Volume(0), IsPlaying(false), Song("") {  }
    UIInfo(float vol, std::string song, bool isPlaying)
      : Volume(vol)
      , IsPlaying(isPlaying)
      , Song(song)
    { }

    // Operator equals
    // Note: Do string comparison last in an attempt to make this less expensive.
    bool operator==(const UIInfo &rhs) const
    {
      if (&rhs == this)
        return true;
      else if (Volume + .0004f <= rhs.Volume || Volume - .0004f >= rhs.Volume)
        return false;
      else if (IsPlaying != rhs.IsPlaying)
        return false;
      else if (Song != rhs.Song)
        return false;

      return true;
    }

    // Operator not equals
    // Just makes use of the comparison operator
    bool operator!=(const UIInfo &rhs) const
    {
      return !(*this == rhs);
    }  

    // Variables
    float Volume;
    bool IsPlaying;
    std::string Song;
  };

  // A class designed to represent UI Overlay.
  class ASCIIOverlay
  {
  public:
    virtual bool Update(const UIInfo uinfo) ABSTRACT;
    virtual bool UpdatePost() { return true; };
  };
}
