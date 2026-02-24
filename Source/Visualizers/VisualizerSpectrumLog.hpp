#pragma once
#include "ASCIIVisualizer.hpp"
#include "Defines.hpp"

namespace ASCIIPlayer
{
  class VisualizerSpectrumLog : public ASCIIVisualizer
  {
  public:
    // Constructor
    VisualizerSpectrumLog();

    // DrawBars
    bool Update(double dt, float* data, bool isActive);
    void OnResize(int newWidth, int newHeight);
    void AnnotateFrequency(const std::pair<int, float>* binLimits, int bin, int binWidth, int leftOffset, int yOffset);

  private:
    int width_;
    int height_;
  };
}