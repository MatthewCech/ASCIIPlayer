#pragma once
#include "FMOD/fmod.hpp"


// Defines
using ChannelHandle = FMOD::Channel*;      // Channel handle for FMOD
using AudioHandle   = FMOD::Sound;        // Sound object handle for FMOD
using Channelgroup  = FMOD::ChannelGroup; // Channel object handle for FMOD


// The size of the spectrum to get- confines it to custom size.
enum AudioDataSize 
{
  a64 = 64,
  a128 = 128,
  a256 = 256,
  a512 = 512,
  a1024 = 1024
};
