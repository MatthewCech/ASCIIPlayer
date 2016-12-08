#include "CenterVisualizer.hpp"
#include "ConsoleUtils\console-utils.hpp"
#include <math.h>


#define CONSOLE_WIDTH_FUNC (rlutil::tcols() - 1)
#define CONSOLE_HEIGHT_FUNC (rlutil::trows())


namespace ASCIIPlayer
{
  void DrawSplit(int dataSize, float *data, int width, int height, unsigned char drawChar, int scalarVal = 1)
  {    // Calculate dependant values and precompute values.
    const int audioDataWidth{ static_cast<int>(dataSize / 2.5 - 1) };                              // Audio data width we use
    const int centerOffsetH = (width - audioDataWidth) / 2 < 0 ? 0 : (width - audioDataWidth) / 2; // Horizontal Offset
    const int halfHeight{ height / 2 };                                                            // Vertical Offset
    const int halfDataWidth{ (audioDataWidth / 2) };                                               // Horizontal Offset
    const int scalar{ 800 };

    // Display spectrum bar in center
    for (int i = 0; i < audioDataWidth; ++i)
    {
      // Ignore strange edge case values and negatives.
      if (data[i] < .001) continue;

      // Calculate the log of the input scaled up to make a more smooth visualizer.
      // If it is under 0 for some reason, ignore it.
      const float logH{ abs(log(data[i])) }; // data[i] * scalar
      const int actualWidth{ static_cast<int>(logH - (static_cast<float>(i) / logH)) };
      if (actualWidth <= 0) continue;

      // Place 4 mirrored blocks.
      for (int j = 0; j < actualWidth * scalarVal; ++j)
      {
        // Lower Left quadrant
        RConsole::Canvas::Draw(
            drawChar
          , static_cast<float>((centerOffsetH - i) + halfDataWidth)
          , static_cast<float>(j + halfHeight)
          , RConsole::BLUE);

        // Lower right quadrant
        RConsole::Canvas::Draw(
            drawChar
          , static_cast<float>((centerOffsetH + i) + halfDataWidth)
          , static_cast<float>(j + halfHeight)
          , RConsole::BLUE);

        // Upper left quadrant
        RConsole::Canvas::Draw(
            drawChar
          , static_cast<float>((centerOffsetH + audioDataWidth - i) - halfDataWidth)
          , static_cast<float>(-j + halfHeight)
          , RConsole::LIGHTBLUE);

        // Upper right quadrant
        RConsole::Canvas::Draw(
            drawChar
          , static_cast<float>((centerOffsetH + audioDataWidth + i) - halfDataWidth)
          , static_cast<float>(-j + halfHeight)
          , RConsole::LIGHTBLUE);
      }
    }
  }

  // Ctor - hide the cursor and set up.
  CenterVisualizer::CenterVisualizer()
    : Visualizer(64, aSpectrum, "centerVisualizer")
    , lastWidth_(CONSOLE_WIDTH_FUNC)
    , lastHeight_(CONSOLE_HEIGHT_FUNC)
    , frameDelayMax_(90)
    , frameDeleay_(0)
  {  
    RConsole::Canvas::SetCursorVisible(false);
    prevSize_ = GetAudioDataSize();
    prev_ = new float[prevSize_]{ 0 };
    prev2_ = new float[prevSize_] { 0 };

  }

  // Member functions
  bool CenterVisualizer::Update(float* data)
  {
    // Variables
    const int width{ CONSOLE_WIDTH_FUNC };                      // Console Width
    const int height{ CONSOLE_HEIGHT_FUNC };                    // Console Height
    const int dataSize{ static_cast<int>(GetAudioDataSize()) }; // Width of data for this loop

    // Re-init if needed
    if (lastHeight_ != height || lastWidth_ != width)
    {
      RConsole::Canvas::ReInit(width, height);
      rlutil::cls();
      lastHeight_ = height;
      lastWidth_ = width;
      return false;
    }

    // Draw primary shape
    DrawSplit(dataSize, prev2_, width, height, static_cast<unsigned char>(176), 2);
    DrawSplit(dataSize, prev_, width, height, static_cast<unsigned char>(177), 2);
    DrawSplit(dataSize, data, width, height, static_cast<unsigned char>(219), 2);

    // Update the canvas
    if (frameDeleay_ > frameDelayMax_)
    {
      prev2_ = static_cast<float *>(memcpy(prev2_, prev_, prevSize_));
      prev_ = static_cast<float *>(memcpy(prev_, data, prevSize_));
      frameDeleay_ = { 0 };
    }
    else
      ++frameDeleay_;
    return RConsole::Canvas::Update();
  }
}
