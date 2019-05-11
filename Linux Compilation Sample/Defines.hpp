#pragma once
#include <iostream> // Debug printing
#include <chrono>   // MS_SINCE_EPOCH
#include "fmod.hpp" // Audio types


  //////////////////////////////
 // OS and Compiler-Specific //
//////////////////////////////
#if defined (DEBUG) | (_DEBUG)
  //#define AP_DEBUG // As in, ascii-player debug
#endif

#ifdef _WIN32
  #define OS_WINDOWS
#endif


  ///////////////////////////////////
 // Types, "Keywords", and Values //
///////////////////////////////////
// Using
using ChannelHandle = FMOD::Channel*;     // Channel handle for FMOD
using AudioHandle   = FMOD::Sound;        // Sound object handle for FMOD
using Channelgroup  = FMOD::ChannelGroup; // Channel object handle for FMOD
using APUnique      = unsigned long long;

// Defines
#define ABSTRACT =0
#define SONG_TIME_SCALE_FOR_SECONDS 1000
#define MS_SINCE_EPOCH (std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count())


  ///////////
 // Enums //
///////////
enum AudioDataStyle
{
  AUDIODATA_NO_STYLE = 0,

  AUDIODATA_SPECTRUM = 2,
  AUDIODATA_SPECTRUM_ALT = 3,

  AUDIODATA_WAVEFORM = 1,
};


  //////////////////////////
 // Debug printing, etc. //
//////////////////////////
// Debug Printing
#ifdef AP_DEBUG
  #define DEBUG_PRINT(x) do{ std::cout << x << std::endl; } while (0)
#else
  #define DEBUG_PRINT(x) do{ } while (0)
#endif
