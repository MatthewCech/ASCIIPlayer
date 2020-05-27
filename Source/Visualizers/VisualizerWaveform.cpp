#include "VisualizerWaveform.hpp"
#include <Core\AudioSystem.hpp>


namespace ASCIIPlayer
{
  // Constructor
  VisualizerWaveform::VisualizerWaveform()
    : ASCIIVisualizer(CONSOLE_WIDTH_FUNC, AUDIODATA_WAVEFORM)
    , startingWidth_(CONSOLE_WIDTH_FUNC)
    , width_(startingWidth_)
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
  bool VisualizerWaveform::Update(float* data, float volume, bool isActive)
  {
    // Before anything else, catch or correct any problematic values.
    if (volume < 0.01f)
      volume = 1;

    // Set up some sweet numbers
    const char symbol = static_cast<unsigned char>(219); // Solid box character on windows. This doesn't really work well on different OSs.
    const int halfHeight = height_ / 2;                  // Half-way point on the console visually
    const float heightScalar = 5.4f;                     // Arbitrary value determining the max vertical offset. Used to be 6, but was scaled for volume of .9 by default.
    const float heightMinimum = 2;                       // Arbitrary value determining smallest vertical offset. Chosen for visual appeal - creates 2 bottom and 1 top line.
    const float volumeScale = 1.0f / volume;             // For volume values smaller than 1, it's good to normalize the data.

    int offset = (width_ - startingWidth_) / 2;
    if (offset < 0)
      offset = 0;

    // For every X position, calculate the Y position based on waveform data and variables above.
    for (int i = 0; i < width_ && i < startingWidth_; ++i)
    {
      for (int j = 0; j < data[i] * heightScalar * volumeScale + heightMinimum; ++j) // Y position
      {
        const int xPos = i;
        RConsole::Canvas::Draw(symbol, i + offset, halfHeight + j, RConsole::BLUE);
        RConsole::Canvas::Draw(symbol, i + offset, halfHeight - j, RConsole::LIGHTBLUE);
      }
    }

    return true;
  }
}

#undef WAVEFORM_SIZE