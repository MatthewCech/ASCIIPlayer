#include "CenterVisualizer.hpp"
#include "ConsoleUtils\console-utils.hpp"
#include <math.h>


#define CONSOLE_WIDTH_FUNC (rlutil::tcols() - 1)
#define CONSOLE_HEIGHT_FUNC (rlutil::trows())


namespace ASCIIPlayer
{
  // Ctor
  CenterVisualizer::CenterVisualizer() 
    : Visualizer(128, aSpectrum, "CenterVisualizer")
    , lastWidth_(CONSOLE_WIDTH_FUNC)
    , lastHeight_(CONSOLE_HEIGHT_FUNC)
  {  
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Member functions
  bool CenterVisualizer::Update(float* data)
  {
    // Variables
    float scalar = 800;                              // Amplitude Scalar
    int audioDataWidth = GetAudioDataSize() / 2 - 1; // Audio data width we use
    int width = CONSOLE_WIDTH_FUNC;                  // Console Width
    int height = CONSOLE_HEIGHT_FUNC;                // Console Height
    int halfHeight = height / 2;                     // Vertical Offset
    int centerOffset = (width - audioDataWidth) / 2; // Horizontal Offset
    if (centerOffset < 0)
      centerOffset = 0;

    // Re-init if needed
    if (lastHeight_ != height || lastWidth_ != width)
    {
      RConsole::Canvas::ReInit(width, height);
      rlutil::cls();
    }

    // Display spectrum bar in center
    for (int i = 0; i < audioDataWidth; ++i)
    {
      for (unsigned int j = 0; j < log(data[i] * scalar); ++j)
      {
        RConsole::Canvas::Draw(
          static_cast<unsigned char>(219)
          , static_cast<float>(centerOffset + i)
          , static_cast<float>(j)
          , RConsole::YELLOW);
      }
    }


    lastHeight_ = height;
    lastWidth_ = width;
    return RConsole::Canvas::Update();
  }
}