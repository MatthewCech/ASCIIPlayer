#include "VisualizerSpotted.hpp"

#define SPOTTED_SIZE 128


namespace ASCIIPlayer
{
  // Constructor
  VisaulizerSpotted::VisaulizerSpotted()
    : ASCIIVisualizer(SPOTTED_SIZE, AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
  {

  }

  // Called when the console is resized
  void VisaulizerSpotted::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Establish cell-style spotted format.
  bool VisaulizerSpotted::Update(float* data)
  {
    for (int i = 0; i < width_; ++i)
    {
      for (int j = 0; j < height_; ++j)
      {
        RConsole::Canvas::Draw('=', i, j, RConsole::LIGHTMAGENTA);
      }
    }

    return true;
  }
}

