#pragma once
#include <chrono>
#include "ASCIIVisualizer.hpp"
#include "Overlays/ASCIIOverlay.hpp"


namespace ASCIIPlayer
{
  class VisualizerWisp : public ASCIIVisualizer
  {
  public:
    // Ctor
    VisualizerWisp();

    // Member functions
    bool Update(float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);

  private:
    // Variables
    int width_;
    int height_;
    
	  // Previous info
    float *prev1_;
    float *prev2_;
    float *prev3_;
    int prevSize_;
    int frameDeleay_;
    int frameDelayMax_;
	
    // Delay for movement
    int moveDelayMax_;
    int moveDelay_;
    float rand1_;
    float rand2_;

    // Visual offsets of backing
    float offsetX_;
    float offsetY_;
    float offsetX1_;
    float offsetY1_;
    float offsetX2_;
    float offsetY2_;
    float offsetX3_;
    float offsetY3_;
  };
}



