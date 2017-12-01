#include "HorizontalWaveformVisualizer.hpp"


namespace ASCIIPlayer
{
  // Constructor
  HorizontalWaveformVisualizer::HorizontalWaveformVisualizer()
    : ASCIIVisualizer(128, aWaveform, "horizontalWaveform")
  {  }

  // Draw waveform based on updating
  bool HorizontalWaveformVisualizer::Update(float* data)
  {
    for (unsigned int i = 0; i < GetAudioDataSize(); ++i)
    {
      int dv = static_cast<int>(data[i] * 90);
      if (dv < 0) dv *= -1;
      printf("%.*s\n", dv, "#######X##XX#X##XXXXXXXxXXxXxxXxxxxxxxx=xx=x==x============-==-=--=-----------");
    }

    return true;
  }
}