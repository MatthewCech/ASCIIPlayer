#include "VisualizerSpotted.hpp"

#define SPOTTED_SIZE 128


namespace ASCIIPlayer
{
  // Constructor
  VisaulizerSpotted::VisaulizerSpotted()
    : ASCIIVisualizer(SPOTTED_SIZE, AUDIODATA_SPECTRUM)
  {

  }

  // Called when the console is resized
  void VisaulizerSpotted::OnResize(int newWidth, int newHeight)
  {

  }

  // Establish cell-style spotted format.
  bool VisaulizerSpotted::Update(float* data)
  {
    RConsole::Canvas::Draw('=', 2, 2, RConsole::LIGHTMAGENTA);
    return true;
  }
}

