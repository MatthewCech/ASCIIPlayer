#include <math.h>
#include "VisualizerWisp.hpp"


namespace ASCIIPlayer
{
  // Ctor - hide the cursor and set up.
  VisualizerWisp::VisualizerWisp()
    : ASCIIVisualizer(64, AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
    , frameDelayMax_(5)
    , frameDeleay_(0)
    , offsetX_(0)
    , offsetY_(0)
    , offsetX1_(0)
    , offsetY1_(0)
    , offsetX2_(0)
    , offsetY2_(0)
    , offsetX3_(0)
    , offsetY3_(0)
    , moveDelayMax_(30)
    , moveDelay_(0)
    , rand1_(0)
    , rand2_(0)
  {
    RConsole::Canvas::SetCursorVisible(false);
    prevSize_ = GetAudioDataSize();
    prev1_ = new float[prevSize_] { 0 };
    prev2_ = new float[prevSize_] { 0 };
    prev3_ = new float[prevSize_] { 0 };
  }

  // Draw the split center formation
  void DrawSplit(int dataSize, float *data, int width, int height, unsigned char drawChar, float offsetX = 0, float offsetY = 0, float SCALAR_TO_CHANGE = 1.5, float horizontalScalar = 1.0f)
  { 
	  // Calculate dependant values and precompute values.
    const int audioDataWidth{ static_cast<int>(dataSize / 2.5 - 1) };                              // Audio data width we use
    const int centerOffsetH = (width - audioDataWidth) / 2 < 0 ? 0 : (width - audioDataWidth) / 2; // Horizontal Offset
    const int halfHeight{ height / 2 };                                                            // Vertical Offset
    const int halfDataWidth{ (audioDataWidth / 2) };                                               // Horizontal Offset
    const int scalar{ 800 };

    // Display spectrum bar in center
    for (int i = 0; i < audioDataWidth * horizontalScalar; ++i)
    {
      // Ignore strange edge case values and negatives.
      if (data[i] < .001) continue;

      // Calculate the log of the input scaled up to make a more smooth visualizer.
      // If it is under 0 for some reason, ignore it.
      const float logH{ abs(log(data[i])) }; // data[i] * scalar
      const int actualHeight{ static_cast<int>(logH - (static_cast<float>(i) / logH)) };
      if (actualHeight <= 0) continue;

      // Place 4 mirrored blocks.
      for (int j = 0; j < actualHeight * SCALAR_TO_CHANGE; ++j)
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

  // Called when resizing
  void VisualizerWisp::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Updates the visualizer w/ new audiodata
  bool VisualizerWisp::Update(float* data)
  {
    // Variables
    const int dataSize{ static_cast<int>(GetAudioDataSize()) }; // Width of data for this loop

    // Draw primary shape with 3 frames of fade, fade drawn from most to least faded.
	  const float lowBinValues { (data[0] + data[1]) / 2 };
	  offsetX_ += rand1_ * (lowBinValues) * .5f;
	  offsetY_ += rand2_ * (lowBinValues) * .2f;
	  offsetX_ *= .985f;
	  offsetY_ *= .985f;
    DrawSplit(dataSize, prev3_, width_, height_, static_cast<unsigned char>(176), offsetX3_, offsetY3_, 1.4f, 1.0f); // most faded
    DrawSplit(dataSize, prev2_, width_, height_, static_cast<unsigned char>(177), offsetX2_, offsetY2_, 1.1f, .8f); // mid faded
    DrawSplit(dataSize, prev1_, width_, height_, static_cast<unsigned char>(178), offsetX1_, offsetY1_, .8f, .6f); // least faded
    DrawSplit(dataSize, data, width_, height_, static_cast<unsigned char>(219), offsetX_, offsetY_, .5f, .4f);   // current

    // Delayed updating 
	  if (++moveDelay_ > moveDelayMax_)
	  {
		  rand1_ = static_cast<float>(rand() % 20 - 10);
		  rand2_ = static_cast<float>(rand() % 20 - 10);
		  moveDelay_ = 0;
	  }

    // If we are past frame delay, update.
    if (++frameDeleay_ > frameDelayMax_ - 3)
    {
      // Stagger memcpy calls to reduce visibility
		  if (frameDeleay_ == frameDelayMax_ - 2)
		  {
			  prev3_ = static_cast<float *>(memcpy(prev3_, prev2_, prevSize_));
			  offsetX3_ = offsetX2_;
			  offsetY3_ = offsetY2_;
		  }
		  else if (frameDeleay_ == frameDelayMax_ - 1)
		  {
			  prev2_ = static_cast<float *>(memcpy(prev2_, prev1_, prevSize_));
			  offsetX2_ = offsetX1_;
			  offsetY2_ = offsetY1_;
		  }
		  else if (frameDeleay_ == frameDelayMax_)
		  {
		    prev1_ = static_cast<float *>(memcpy(prev1_, data, prevSize_));
		    offsetX1_ = offsetX_;
			  offsetY1_ = offsetY_;
		    frameDeleay_ = 0;
		  }
    }

    return true;
  }
}
