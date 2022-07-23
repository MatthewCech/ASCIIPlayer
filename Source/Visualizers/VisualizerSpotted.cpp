#include "VisualizerSpotted.hpp"
#include <chrono>
#define SPOTTED_SIZE 512


namespace ASCIIPlayer
{
  // Constructor
  VisaulizerSpotted::VisaulizerSpotted()
    : ASCIIVisualizer(SPOTTED_SIZE, AudioDataStyle::AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
    , runningStart_(0)
    , runningInvert_(false)
  {  }

  // Called when the console is resized
  void VisaulizerSpotted::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    runningStart_ = 0;
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Establish cell-style spotted format.
  bool VisaulizerSpotted::Update(float* data, float volume, bool isActive)
  {
    int loop = static_cast<int>(runningStart_); // give it some initial offset
    bool invert = false;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        float val = data[loop] * 8;
        bool show = val > 0.1;

        if (show)
        {
          if(val > 0.4)
            RConsole::Canvas::Draw('0', j, i, RConsole::DARKGREY);
          else if (val > 0.3)
            RConsole::Canvas::Draw('O', j, i, RConsole::MAGENTA);
          else if (val > 0.2)
            RConsole::Canvas::Draw('o', j, i, RConsole::LIGHTMAGENTA);
          else
            RConsole::Canvas::Draw(',', j, i, RConsole::LIGHTMAGENTA);
        }

        if (invert == false)
        {
          if (loop >= SPOTTED_SIZE) // Consider halving?
            invert = true;
          else
            ++loop;
        }
        else
        {
          if (loop <= 0)
            invert = false;
          else
            --loop;
        }
      }
    }

    const float changeAmt = (data[0] * 6 + data[1] * 4) / 2;
    if (runningInvert_ == false)
    {
      if (runningStart_ > 64)
        runningInvert_ = true;
      else
        runningStart_ += changeAmt;
    }
    else
    {
      if (runningStart_ < 0)
        runningInvert_ = false;
      else
        runningStart_ -= changeAmt;
    }

    return true;
  }
}

