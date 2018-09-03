#include <math.h>
#include "VisualizerDefault.hpp"

#define VERTICAL_PADDING 3
#define DATA_SIZE 64


namespace ASCIIPlayer
{
  // Constructor
  VisualizerDefault::VisualizerDefault()
    : ASCIIVisualizer(DATA_SIZE, AUDIODATA_SPECTRUM)
    , height_(RConsole::Canvas::GetConsoleHeight())
    , width_(RConsole::Canvas::GetConsoleWidth())
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Is called when the window is resized.
  void VisualizerDefault::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw waveform based on updating
  bool VisualizerDefault::Update(float* data)
  {
    // Handle drawing
    for (size_t i = 0; i < GetAudioDataSize() && i < static_cast<size_t>(height_); ++i)
    {
      // Calculate the distance of the string to display, and display dv of it.
      int dv = static_cast<int>(data[i] * 90);

      // enforce positive value.
      if (dv < 0)
        dv = 0;

      RConsole::Canvas::DrawString(std::string(dv, '>').c_str(), 0, static_cast<unsigned int>(i), RConsole::LIGHTCYAN);
    }

    // We're good! Return true!
    return true;
  }
}

#undef DATA_SIZE
