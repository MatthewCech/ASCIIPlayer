#pragma once

// Temp Defines
#define ChannelHandle int* // Channel handle for FMOD -- PLACEHOLDER
#define AudioHandle  int*  // Sound object handle for FMOD -- PLACEHOLDER
#define Channelgroup int   // Channel object handle for FMOD -- PLACEHOLDER

// The size of the spectrum to get- confines it to custom size.
enum AudioDataSize 
{
  a64 = 64,
  a128 = 128,
  a256 = 256,
  a512 = 512,
  a1024 = 1024
};
