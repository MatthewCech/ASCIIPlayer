#pragma once
#include "ASCIIVisualizer.hpp"

#define SCALE_FACTOR 8

namespace ASCIIPlayer
{
  class VisualizerWaveformLite : public ASCIIVisualizer
  {
  public:
    // Ctor/Dtor
    VisualizerWaveformLite();
    ~VisualizerWaveformLite();

    void OnResize(int newWidth, int newHeight);
		bool Update(float* data, bool isActive);

  private:
    void initializeWorkspace();
    int startingWidth_;
    int width_;
    int height_;

    float* workspace_;
    int workspaceCount_;
  };
}
