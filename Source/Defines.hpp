#pragma once
#include <iostream>      // Debug printing
#include <chrono>        // MS_SINCE_EPOCH
#include "FMOD/fmod.hpp" // Audio types


  //////////////////////////////
 // OS and Compiler-Specific //
//////////////////////////////
#if defined (DEBUG) | (_DEBUG)
  // #define AP_DEBUG /* As in, ascii-player debug */
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
using APUnique      = std::int64_t;       // Type used for unique IDs related to audio files, etc

// Defines
#define TRACKED_TIMES 120
#define ABSTRACT =0
#define MS_PER_SECOND_FLOAT 1000.0f
#define MS_PER_SECOND_INT 1000
#define MS_SINCE_EPOCH std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define CONFIG_FILE_NAME ".asciiplayer"
#define MUTE_THRESHOLD 0.001f

  ///////////
 // Enums //
///////////
enum class AudioDataStyle
{
  AUDIODATA_NO_STYLE = 0,

  AUDIODATA_WAVEFORM = 1, // Raw data collected from the file
  AUDIODATA_SPECTRUM = 2, // Waveform with FFT and windowing
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
