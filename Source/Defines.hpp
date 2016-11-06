#pragma once
#include "FMOD/fmod.hpp"
#include <iostream>



  //////////////////////////////
 // OS and Compiler-Specific //
//////////////////////////////
#if defined (DEBUG) | (_DEBUG)
  #define AP_DEBUG // As in, ascii-player debug
#endif


  //////////////////////////
 // Types and "Keywords" //
//////////////////////////
// Using
using ChannelHandle = FMOD::Channel*;     // Channel handle for FMOD
using AudioHandle   = FMOD::Sound;        // Sound object handle for FMOD
using Channelgroup  = FMOD::ChannelGroup; // Channel object handle for FMOD
using APUnique      = unsigned long long;

// Defines
#define DJ_ABSTRACT =0


  ///////////
 // Enums //
///////////
// The size of the spectrum to get- confines it to custom size.
enum AudioDataSize
{
  a1 = 1,
  a4 = 4,
  a16 = 16,
  a32 = 32,
  a64 = 64,
  a80 = 80,
  a128 = 128,
  a256 = 256,
  a512 = 512,
  a1024 = 1024,

  aNO_SIZE
};

enum AudioDataStyle
{
  aSpectrum = 0,
  aWaveform = 1,
  
  aNO_STYLE
};

  /////////////////////////////////
 // Fun witchcr-- I mean macros //
/////////////////////////////////
// Debug Printing
#ifdef AP_DEBUG
  #define AP_DEBUG_PRINT_VAL 1
#else
  #define AP_DEBUG_PRINT_VAL 0
#endif
#define DEBUG_PRINT(x) do{ if(AP_DEBUG_PRINT_VAL) { std::cout << x << std::endl; } } while (0)

