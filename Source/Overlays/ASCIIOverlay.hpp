#pragma once
#include <string>
#include <ConsoleUtils/console-utils.hpp>
#include "ASCIIOverlay.hpp"
#include "Defines.hpp"



namespace ASCIIPlayer
{
  struct UIInfo
  {
    // Constructors
    UIInfo() : IsRequestedActive(false), Volume(0), IsPlaying(false), IsJumpingPos(false), Song(""), SongPos(0), SongLength(0) { }
    UIInfo(bool requestedActive, bool isPlaying, bool justSkipped, float vol, std::string song, size_t current_pos, size_t length)
      : IsRequestedActive(requestedActive)
      , Volume(vol)
      , IsPlaying(isPlaying)
      , IsJumpingPos(justSkipped)
      , Song(song)
      , SongPos(current_pos)
      , SongLength(length)
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
      else if (SongLength != rhs.SongLength)
        return false;
      else if (IsJumpingPos != rhs.IsJumpingPos)
        return false;
      else if (IsRequestedActive != rhs.IsRequestedActive)
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
    bool IsRequestedActive;
    float Volume;
    bool IsPlaying;
    bool IsJumpingPos;
    std::string Song;
    size_t SongPos;
    size_t SongLength;
  };

  // A class designed to represent UI Overlay.
  class ASCIIOverlay
  {
  public:
    virtual bool Update(const UIInfo uinfo) ABSTRACT;
    virtual bool UpdatePost() { return true; };
  };
}
