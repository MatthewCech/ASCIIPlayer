#include <cmath>
#include "VisualizerSpectrum.hpp"
#include "Defines.hpp"

#define DATA_SIZE 256
#define OSCILATION_CAP 2048.0f



namespace ASCIIPlayer
{
  // Constructor
  VisualizerSpectrum::VisualizerSpectrum() 
    : ASCIIVisualizer(DATA_SIZE, AUDIODATA_SPECTRUM)
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
    , lastTime_(MS_SINCE_EPOCH)
  { 
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Handles window resizing
  void VisualizerSpectrum::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw vertical spectrum based on frequencies
  bool VisualizerSpectrum::Update(float* data)
  {
    return true;
  }
}


// Cleanup
#undef DATA_SIZE
#undef OSCILATION_CAP

