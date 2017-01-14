#include "CenterVisualizer.hpp"
#include "ConsoleUtils\console-utils.hpp"
#include <math.h>


#define CONSOLE_WIDTH_FUNC (rlutil::tcols() - 1)
#define CONSOLE_HEIGHT_FUNC (rlutil::trows())


namespace ASCIIPlayer
{
  void DrawSplit(int dataSize, float *data, int width, int height, unsigned char drawChar, float offsetX = 0, float offsetY = 0, float SCALAR_TO_CHANGE = 1.5)
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
      for (int j = 0; j < actualWidth * SCALAR_TO_CHANGE; ++j)
      {
        // Lower Left quadrant
        RConsole::Canvas::Draw(
            drawChar
          , offsetX + static_cast<float>((centerOffsetH - i) + halfDataWidth)
          , offsetY + static_cast<float>(j + halfHeight)
          , RConsole::BLUE);

        // Lower right quadrant
        RConsole::Canvas::Draw(
            drawChar
          , offsetX + static_cast<float>((centerOffsetH + i) + halfDataWidth)
          , offsetY + static_cast<float>(j + halfHeight)
          , RConsole::BLUE);

        // Upper left quadrant
        RConsole::Canvas::Draw(
            drawChar
          , offsetX + static_cast<float>((centerOffsetH + audioDataWidth - i) - halfDataWidth)
          , offsetY + static_cast<float>(-j + halfHeight)
          , RConsole::LIGHTBLUE);

        // Upper right quadrant
        RConsole::Canvas::Draw(
            drawChar
          , offsetX + static_cast<float>((centerOffsetH + audioDataWidth + i) - halfDataWidth)
          , offsetY + static_cast<float>(-j + halfHeight)
          , RConsole::LIGHTBLUE);
      }
    }
  }

  // Ctor - hide the cursor and set up.
  CenterVisualizer::CenterVisualizer()
    : Visualizer(64, aSpectrum, "centerVisualizer")
    , lastWidth_(CONSOLE_WIDTH_FUNC)
    , lastHeight_(CONSOLE_HEIGHT_FUNC)
    , frameDelayMax_(70) //!TODO: Tune, current 60
    , frameDeleay_(0)
  {  
    RConsole::Canvas::SetCursorVisible(false);
    prevSize_ = GetAudioDataSize();
    prev1_ = new float[prevSize_] { 0 };
    prev2_ = new float[prevSize_] { 0 };
    prev3_ = new float[prevSize_] { 0 };
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

    // Draw primary shape with 3 frames of fade, fade drawn from most to least faded.
		float offsetX = (rand() % 20 - 10) * ((data[5] + data[6] + data[7]) / 3);
		float offsetY = (rand() % 20 - 10) * ((data[0] + data[1]) / 2);
    DrawSplit(dataSize, prev2_, width, height, static_cast<unsigned char>(176), offsetX, offsetY); // most faded
    DrawSplit(dataSize, prev2_, width, height, static_cast<unsigned char>(177), offsetX, offsetY); // mid faded
    DrawSplit(dataSize, prev1_, width, height, static_cast<unsigned char>(178)), offsetX, offsetY; // least faded
    DrawSplit(dataSize, data, width, height, static_cast<unsigned char>(219), offsetX, offsetY);   // current

    // If we are past frame delay, update.
    if (++frameDeleay_ > frameDelayMax_ - 3)
    {
      // Stagger memcpy calls.
      if(frameDeleay_ == frameDelayMax_ - 2)
        prev3_ = static_cast<float *>(memcpy(prev3_, prev2_, prevSize_));
      else if(frameDeleay_ == frameDelayMax_ - 1)
        prev2_ = static_cast<float *>(memcpy(prev2_, prev1_, prevSize_));
      else if (frameDeleay_ == frameDelayMax_)
      {
        prev1_ = static_cast<float *>(memcpy(prev1_, data, prevSize_));
        frameDeleay_ = { 0 };
      }
    }

    // Update the canvas
    return RConsole::Canvas::Update();
  }
}
