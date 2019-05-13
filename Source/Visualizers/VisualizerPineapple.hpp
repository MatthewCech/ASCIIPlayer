#pragma once
#include <vector>
#include "ASCIIVisualizer.hpp"

#define PINEAPPLE_SIZE 64

namespace ASCIIPlayer
{
  class VisualizerPineapple : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerPineapple();

    // Member functions
    bool Update(float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    // Variables
    int width_;
    int height_;
    float radius_;
    float smoothed_[PINEAPPLE_SIZE];

    // Frame time normalization / tracking
    long long lastTimeMS_;
    long long lastTimeSecond_;
    std::vector<long long> lastTimes_;
  };
}

