#pragma once
#include "ASCIIVisualizer.hpp"


namespace ASCIIPlayer
{
  class VisualizerSpectrum : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerSpectrum();

    // Functions
    bool Update(float* data);
    
    // Internal Variables
    int width_;
    int height_;
    int side_offset_;
    float pos_1_x_;
    float pos_1_y_;
    float pos_2_x_;
    float pos_2_y_;
    double oscilation_location_;
    long long lastTime_;
  };
}

