#include "VisualizerSpectrum.hpp"



namespace ASCIIPlayer
{
  VisualizerSpectrum::VisualizerSpectrum() 
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleWidth(), AUDIODATA_SPECTRUM, "visualizerSpectrum")
  {

  }

  // DrawBars
  bool VisualizerSpectrum::Update(float* data)
  {
    return true;
  }
}
