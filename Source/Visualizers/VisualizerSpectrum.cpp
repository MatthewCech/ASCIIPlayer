#include <cmath>
#include "VisualizerSpectrum.hpp"
#include "Defines.hpp"

#define NORMALIZED_TIME_MS 8
#define SPECTRUM_CHAR '^'


namespace ASCIIPlayer
{
  // Constructor
  VisualizerSpectrum::VisualizerSpectrum() 
    : ASCIIVisualizer(VISUALIZER_SPECTRUM_DATA_SIZE, AUDIODATA_SPECTRUM_ALT)
    , width_(RConsole::Canvas::GetConsoleWidth())
    , height_(RConsole::Canvas::GetConsoleHeight())
    , lastTimeMS_(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
    , lastTimeSecond_(lastTimeMS_)
    , lastTimes_(std::vector<std::int64_t>())
  { 
    RConsole::Canvas::SetCursorVisible(false);
    memset(smoothed_, 0, VISUALIZER_SPECTRUM_DATA_SIZE * sizeof(float));
  }

  // Handles window resizing
  void VisualizerSpectrum::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw vertical spectrum based on frequencies
  bool VisualizerSpectrum::Update(float* data, bool isActive)
  {
    const float smoothRate = 0.90f;
    std::int64_t current = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Smooth in new data
    if (current > lastTimeMS_ + NORMALIZED_TIME_MS)
    {
      for (int i = 0; i < VISUALIZER_SPECTRUM_DATA_SIZE; ++i)
      {
        float x = data[i] * 20;
        if (x < 0)
          x = 0;

        smoothed_[i] += x;
      }
    }

    // Draw the lines
    for (int x = 0; x < width_ && x < VISUALIZER_SPECTRUM_DATA_SIZE; ++x)
    {
      float val = static_cast<float>(log((smoothed_[x] * 100.0) - 0.1) * 3.2);//5 * sqrt(smoothed_[x]);

      // Goes a certain distance in either direction!
      for (int y = 0; y < val; ++y)
        RConsole::Canvas::Draw(SPECTRUM_CHAR, x, height_ - 1 - y, RConsole::LIGHTBLUE);
    }

    // Smooth out old data
    if (current > lastTimeMS_ + NORMALIZED_TIME_MS)
    {
      for (int i = 0; i < VISUALIZER_SPECTRUM_DATA_SIZE; ++i)
        smoothed_[i] *= smoothRate;

      lastTimes_.insert(lastTimes_.begin(), current - lastTimeMS_);
      lastTimeMS_ = current;
    }

    // Debug
    float total = 0;
    for (std::int64_t val : lastTimes_)
      total += val;

    total /= lastTimes_.size();

    const int displayFormatWidth = 50;
    float updateCycles = (1000.0f / total);
    char toDisplay[displayFormatWidth];
    memset(toDisplay, 0, sizeof(char) * displayFormatWidth);

#ifdef OS_WINDOWS
    sprintf_s(toDisplay, displayFormatWidth, "%.2f", updateCycles);
#else
    sprintf(toDisplay, "%.2f", updateCycles);
#endif

    RConsole::Canvas::DrawString(toDisplay, 0, 0, RConsole::WHITE);

    // FPS tracking
    if (current > lastTimeSecond_ + 1000)
      lastTimeSecond_ = current;

    if (lastTimes_.size() > 100)
      lastTimes_.pop_back();

    return true;
  }
}

// Cleanup
#undef VISUALIZER_SPECTRUM_DATA_SIZE

