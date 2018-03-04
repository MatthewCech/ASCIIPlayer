#include "VisualizerSpectrum.hpp"



namespace ASCIIPlayer
{
  VisualizerSpectrum::VisualizerSpectrum() 
    : ASCIIVisualizer(64, AUDIODATA_SPECTRUM, "visualizerSpectrum")
  {

  }

  // Draw vertical spectrum based on frequencies
  bool VisualizerSpectrum::Update(float* data)
  {
    return true;
  }
}
