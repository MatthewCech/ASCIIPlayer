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
    bool Update(float* data, float volume, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    // Variables
    int width_;
    int height_;
    float radius_;
    float smoothed_[PINEAPPLE_SIZE];

    // Frame time normalization / tracking
    std::int64_t lastTimeMS_;
    std::int64_t lastTimeSecond_;
    std::vector<std::int64_t> lastTimes_;
  };
}

