#include <math.h>
#include "VisualizerDefault.hpp"

#define VERTICAL_PADDING 3
#define DATA_SIZE 128


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
    for (int i = 0; i < 20; ++i)
    {
      float height = data[i];
      if (height < 0)
        height *= -1;

      height *= 50;

      for (int j = 0; j < height && j < 20; ++j)
          RConsole::Canvas::Draw('>', j, i, RConsole::LIGHTBLUE);
    }

    return true;
  }
}

#undef DATA_SIZE
