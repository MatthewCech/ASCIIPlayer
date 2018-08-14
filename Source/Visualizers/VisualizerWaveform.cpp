#include "VisualizerWaveform.hpp"


namespace ASCIIPlayer
{
  // Constructor
  VisualizerWaveform::VisualizerWaveform()
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleWidth(), AUDIODATA_WAVEFORM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
  {
    RConsole::Canvas::SetCursorVisible(false); 
  }

  // Called when the window is resized
  void VisualizerWaveform::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw Bars
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
