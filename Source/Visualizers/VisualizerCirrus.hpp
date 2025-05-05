#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  #define CIRRUS_DATA_SIZE 256

  class VisualizerCirrus : public ASCIIVisualizer
  {
  public:
    VisualizerCirrus();

    void OnResize(int newWidth, int newHeight);
    bool Update(double dt, float* data, bool isActive);

  private:
    // Current width and height of console
    int width_;
    int height_;

    float startOffset_;
    float workspace_[CIRRUS_DATA_SIZE];
  };
}
