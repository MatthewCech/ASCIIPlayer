#include "CenterVisualizer.hpp"
#include "ConsoleUtils\console-utils.hpp"
#include <math.h>


#define CONSOLE_WIDTH_FUNC (rlutil::tcols() - 1)
#define CONSOLE_HEIGHT_FUNC (rlutil::trows())


namespace ASCIIPlayer
{
  // Ctor - hide the cursor and set up.
  CenterVisualizer::CenterVisualizer() 
    : Visualizer(64, aSpectrum, "centerVisualizer")
    , lastWidth_(CONSOLE_WIDTH_FUNC)
    , lastHeight_(CONSOLE_HEIGHT_FUNC)
  {  
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Member functions
  bool CenterVisualizer::Update(float* data)
  {
    // Variables
    float scalar{ 800 };               // Amplitude Scalar
    int width{ CONSOLE_WIDTH_FUNC };   // Console Width
    int height{ CONSOLE_HEIGHT_FUNC }; // Console Height

    // Re-init if needed
    if (lastHeight_ != height || lastWidth_ != width)
    {
      RConsole::Canvas::ReInit(width, height);
      rlutil::cls();
      lastHeight_ = height;
      lastWidth_ = width;
      return false;
    }

    // Calculate dependant values and precompute values.
    int audioDataWidth{ static_cast<int>(GetAudioDataSize() / 3 - 1) };                      // Audio data width we use
    int centerOffsetH = (width - audioDataWidth) / 2 < 0 ? 0 : (width - audioDataWidth) / 2; // Horizontal Offset
    int halfHeight{ height / 2 };                                                            // Vertical Offset
    int halfDataWidth{ (audioDataWidth / 2) };                                               // Horizontal Offset

    // Display spectrum bar in center
    for (int i = 0; i < audioDataWidth; ++i)
    {
      // Ignore strange edge case values and negatives.
      if (data[i] < .001) continue;

      // Calculate the log of the input scaled up to make a more smooth visualizer.
      // If it is under 0 for some reason, ignore it.
      float logH{ abs(log(data[i] * scalar)) };
      int actualWidth{ static_cast<int>(logH - (static_cast<float>(i) / logH)) };
      if (actualWidth <= 0) continue;

      // Place 4 mirrored blocks.
      for (int j = 0; j < actualWidth; ++j)
      {
        // Lower Left quadrant
        RConsole::Canvas::Draw(
          static_cast<unsigned char>(219)
          , static_cast<float>((centerOffsetH - i) + halfDataWidth)
          , static_cast<float>(j + halfHeight)
          , RConsole::YELLOW);

        // Lower right quadrant
        RConsole::Canvas::Draw(
          static_cast<unsigned char>(219)
          , static_cast<float>((centerOffsetH + i) + halfDataWidth)
          , static_cast<float>(j + halfHeight)
          , RConsole::YELLOW);

        // Upper left quadrant
        RConsole::Canvas::Draw(
          static_cast<unsigned char>(219)
          , static_cast<float>((centerOffsetH + audioDataWidth - i) - halfDataWidth)
          , static_cast<float>(-j + halfHeight)
          , RConsole::YELLOW);

        // Upper right quadrant
        RConsole::Canvas::Draw(
          static_cast<unsigned char>(219)
          , static_cast<float>((centerOffsetH + audioDataWidth + i) - halfDataWidth)
          , static_cast<float>(-j + halfHeight)
          , RConsole::YELLOW);
      }
    }

    // Update the canvas
    return RConsole::Canvas::Update();
  }
}
