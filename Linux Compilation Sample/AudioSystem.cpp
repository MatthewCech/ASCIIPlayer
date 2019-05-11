#include "fmod_errors.h"
#include "AudioSystem.hpp"
#include <cstring>

namespace ASCIIPlayer
{
  // Static Variable Init
  APUnique AudioSystem::AudioSystemIDIncrement = 0;


  // Constructor
  AudioSystem::AudioSystem(int numChannels, float defaultVolume) 
    : fmodSystem_(nullptr)
    , masterChannel_(nullptr)
    , numdrivers_(0)
    , ID_(AudioSystemIDIncrement++)
  {
    // Generate system and get version
    FCheck(FMOD::System_Create(&fmodSystem_));

    // Check version.
    FCheck(fmodSystem_->getVersion(&version_));
    if (version_ < FMOD_VERSION)
    {
      printf("You're using an old version of FMOD, %08x. This program requires %08x\n", version_, FMOD_VERSION);
      throw "Oudated FMOD Version Error!";
    }

    // Check for drivers
    FCheck(fmodSystem_->getNumDrivers(&numdrivers_));
    if (numdrivers_ == 0)
    {
      // Turn off output.
      FCheck(fmodSystem_->setOutput(FMOD_OUTPUTTYPE_NOSOUND)); 
    }
    else
    {
      // Get the driver capabilities.
      FCheck(fmodSystem_->getDriverCaps(0, &caps_, 0, &speakermode_));

      // Set the user selected speaker mode.
      FCheck(fmodSystem_->setSpeakerMode(speakermode_));

      if (caps_ & FMOD_CAPS_HARDWARE_EMULATED)
      {
        // The user has the 'Acceleration' slider set to off! This is really bad
        // for latency! You might want to warn the user about this.
        FCheck(fmodSystem_->setDSPBufferSize(1024, 10));
      }

      // Get the info for the sound device in place, handle ones that don't like things.
      FCheck(fmodSystem_->getDriverInfo(0, name_, 256, 0));
      if (strstr(name_, "SigmaTel"))
      {
        // Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
        // PCM floating point output seems to solve it.
        FCheck(fmodSystem_->setSoftwareFormat(
          48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR));
      }
    }

    // Run formal init, and make sure we could create the buffers.
    FMOD_RESULT res = fmodSystem_->init(numChannels, FMOD_INIT_NORMAL, 0);
    if (res == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
      // Ok, the speaker mode selected isn't supported by this soundcard. Switch it
      // back to stereo...
      FCheck(fmodSystem_->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));

      // ... and re-init.
      res = fmodSystem_->init(numChannels, FMOD_INIT_NORMAL, 0);
    }

    // Regardless, check the result.
    FCheck(res);

    // Set default volume
    fmodSystem_->createChannelGroup("Master", &masterChannel_);
  }

  // Destructor for the audio system.
  AudioSystem::~AudioSystem()
  {
    // Releases all assests.
    FCheck(fmodSystem_->release());
  }

    //////////////////////////
   // Private member funcs //
  //////////////////////////
  // Checks the result of the FMOD operation.
  void AudioSystem::FCheck(const FMOD_RESULT &res) const
  {
    if (res != FMOD_OK)
      throw "FMOD Error: ";
  }
}
