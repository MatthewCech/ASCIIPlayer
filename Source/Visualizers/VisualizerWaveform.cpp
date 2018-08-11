#include "VisualizerWaveform.hpp"


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


  // Draw Bars
  // NOTE(mcech): Until there is a callback of sorts for the change of bars, I don't 
  // intend to add any resize stuff on the grounds that this one should be as fast as possible.
  bool VisualizerWaveform::Update(float* data)
  {
    const char symbol = static_cast<unsigned char>(219); // Solid box character on windows. This doesn't really work well on different OSs.
    const int halfHeight = height_ / 2;                  // Half-way point on the console visually
    const int heightScalar = 6;                          // Arbitrary value determining the max vertical offset
    const int heightMinimum = 2;                         // Arbitrary value determining smallest vertical offset

    // For every X position, calculate the Y position based on waveform data and variables above.
    for (int i = 0; i < width_; ++i)
    {
      for (int j = 0; j < data[i] * heightScalar + heightMinimum; ++j) // Y position
      {
        const int xPos = i;
        RConsole::Canvas::Draw(symbol, i, halfHeight + j, RConsole::BLUE);
        RConsole::Canvas::Draw(symbol, i, halfHeight - j, RConsole::LIGHTBLUE);
      }
    }

    return true;
  }
}
