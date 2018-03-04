#include "VisualizerSpotted.hpp"



namespace ASCIIPlayer
{
  // Constructor
  VisaulizerSpotted::VisaulizerSpotted()
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleWidth(), AUDIODATA_SPECTRUM, "visualizerSpotted")
  {

  }

  // Establish cell-style spotted format.
  bool VisaulizerSpotted::Update(float* data)
  {
    return true;
  }
}

