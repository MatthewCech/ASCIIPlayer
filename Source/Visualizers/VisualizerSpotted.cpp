#include "VisualizerSpotted.hpp"


namespace ASCIIPlayer
{
  // Constructor
  VisaulizerSpotted::VisaulizerSpotted()
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleWidth(), AUDIODATA_SPECTRUM)
  {

  }

  // Called when the console is resized
  void VisaulizerSpotted::OnResize(int newWidth, int newHeight)
  {

  }

  // Establish cell-style spotted format.
  bool VisaulizerSpotted::Update(float* data)
  {
    return true;
  }
}

