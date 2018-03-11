#include "VisualizerParticle.hpp"
#include <cmath>

#define DATA_SIZE 128


namespace ASCIIPlayer
{
  // Constructor
  VisualizerParticle::VisualizerParticle()
    : ASCIIVisualizer(DATA_SIZE, AUDIODATA_SPECTRUM, "visualizerParticle")
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
  {  }


  // Primary update loop for particle
  bool VisualizerParticle::Update(float* data)
  {
    return true;
  }
}
