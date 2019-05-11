// Handles the backend, playing, loading, and unloading files.
#pragma once
#include <unordered_map>
#include "Defines.hpp"


namespace ASCIIPlayer
{
  class AudioSystem
  {
  public:
    // Constructor
    AudioSystem(int numChannels = 10, float defaultVolume = .8f);
    ~AudioSystem();

  private:
      //////////////////////////
     // Private member funcs //
    //////////////////////////
    void FCheck(const FMOD_RESULT &res) const;

      ///////////////
     // Variables //
    ///////////////
    // Audio System
    FMOD::System *fmodSystem_;   // Handle to the system we use to initialize and play sounds.
    Channelgroup *masterChannel_; // Master channel to shove things into for volume reasons.

    // Additional FMOD info
    int              numdrivers_;  // Number of drivers in the system (audio cards)
    char             name_[256];   // The name of the driver.
    FMOD_CAPS        caps_;        // The capabilities of the soundcard.
    unsigned int     version_;     // Version number.
    FMOD_SPEAKERMODE speakermode_; // The speaker mode- 5.1, 7.1, etc.

    // Active Handles map:
    std::unordered_map<APUnique, ChannelHandle> channelHandles_;

    // ID Tracking
    APUnique ID_;
    static APUnique AudioSystemIDIncrement;
  };
}
