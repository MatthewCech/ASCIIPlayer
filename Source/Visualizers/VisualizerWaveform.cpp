#include "VisualizerWaveform.hpp"
//#include <ConsoleUtils\console-utils.hpp>


namespace ASCIIPlayer
{
  // Constructor
  VisualizerWaveform::VisualizerWaveform()
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleWidth(), AUDIODATA_WAVEFORM)
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
  {
    RConsole::Canvas::SetCursorVisible(false); 
  }

  // DrawBars
  bool VisualizerWaveform::Update(float* data)
  {
    int h2 = height_ / 2;
    for (int i = 0; i < width_; ++i)
      for (int j = 0; j < data[i] * 6 + 2; ++j)
      {
        RConsole::Canvas::Draw(static_cast<unsigned char>(219), static_cast<float>(i), static_cast<float>(h2 + j), RConsole::BLUE);
        RConsole::Canvas::Draw(static_cast<unsigned char>(219), static_cast<float>(i), static_cast<float>(h2 - j), RConsole::LIGHTBLUE);
      }

    return true;
  }
}
