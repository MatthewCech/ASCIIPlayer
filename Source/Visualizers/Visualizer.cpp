#include "Visualizer.hpp"


namespace ASCIIPlayer
{
  void Visualizer::OnResize(int newWidth, int newHeight)
  {
    // ... In theory, we could track width and height by default
  }
  
  bool Visualizer::UpdatePost()
  {
    return true; 
  }
  
  unsigned int Visualizer::GetAudioDataSize()
  {
    return audioSize_; 
  }
  
  AudioDataStyle Visualizer::GetAudioDataStyle()
  {
    return audioStyle_; 
  }
}

