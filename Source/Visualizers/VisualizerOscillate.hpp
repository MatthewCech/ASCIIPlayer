#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerOscillate : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerOscillate();

    // Functions
    bool Update(float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

    // Internal Variables
    int width_;
    int height_;
    int side_offset_;
    float pos_1_x_;
    float pos_1_y_;
    float pos_2_x_;
    float pos_2_y_;
    double oscillation_location_;
    std::int64_t lastTime_;
  };
}

