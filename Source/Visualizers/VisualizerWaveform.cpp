#include "VisualizerWaveform.hpp"
#include <Core\AudioSystem.hpp>


namespace ASCIIPlayer
{
  // Constructor
  VisualizerWaveform::VisualizerWaveform()
    : ASCIIVisualizer(CONSOLE_WIDTH_FUNC, AudioDataStyle::AUDIODATA_WAVEFORM)
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

  // Draw Bars in traditional waveform style.
  // Returns if it succeeded or not. Note that when isActive is true, we aren't 
  // paused. If isActive is false, we're using the data from the previous run
  // and we don't see that scale by volume.
  bool VisualizerWaveform::Update(float* normalizedData, bool isActive)
  {
    // Set up some sweet numbers
    const char symbol = static_cast<unsigned char>(219);      // Solid box character on windows. This doesn't really work well on different OSs.
    const int halfHeight = height_ / 2;                       // Half-way point on the console visually
    const float heightScalar = 5.4f;                          // Arbitrary value determining the max vertical offset. Used to be 6, but was scaled for volume of .9 by default.
    const float heightMinimum = 2;                            // Arbitrary value determining smallest vertical offset. Chosen for visual appeal - creates 2 bottom and 1 top line.

    int offset = (width_ - startingWidth_) / 2;
    if (offset < 0)
      offset = 0;

    // For every X position, calculate the Y position based on waveform data and variables above.
    for (int i = 0; i < width_ && i < startingWidth_; ++i)
    {
      const float yHeight = normalizedData[i] * heightScalar + heightMinimum;

      for (int j = 0; j < yHeight; ++j) // Y position
      {
        const int xPos = i + offset;
        const int yPosDown = halfHeight + j;
        const int yPosUp = halfHeight - j;

        // We draw the up over the down so that the 'shadow' of the down is smaller than up.
        RConsole::Canvas::Draw(symbol, xPos, yPosDown, RConsole::BLUE);
        RConsole::Canvas::Draw(symbol, xPos, yPosUp, RConsole::LIGHTBLUE);
      }
    }

    return true;
  }
}

#undef WAVEFORM_SIZE