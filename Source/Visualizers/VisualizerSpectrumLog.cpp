#include "VisualizerSpectrumLog.hpp"


namespace ASCIIPlayer
{
  VisualizerSpectrumLog::VisualizerSpectrumLog()
    : ASCIIVisualizer(FMOD_DATA_SIZE, AudioDataStyle::AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  void VisualizerSpectrumLog::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // The maximum data size for a spectrum is going to be 1 since it represents a 0 to 1 occurrence of the frequency (% appearing at.)
  // No negatives present. Not currently scaling buckets down for width.
  bool VisualizerSpectrumLog::Update(double dt, float* data_both_channels, bool isActive)
  {
    const int binLimits[] =
    {
      0,
      1,
      2,
      4,
      8,
      16,
      32,
      64,
      128,
      256,
      512
    };
    const int binCount = sizeof(binLimits) / sizeof(int);

    // Combine channels
    float combined[FMOD_DATA_SIZE / 2];
    float data[binCount];
    int pos = 0;
    for (int i = 0; i < FMOD_DATA_SIZE; i += 2)
    {
      float avg = (data_both_channels[i] + data_both_channels[i + 1]) / 2.0f;
      combined[pos] = avg;
      ++pos;
    }

    for (int i = 0; i < binCount - 1; ++i)
    {
      const int lower = binLimits[i];
      const int upper = binLimits[i + 1];
      float sum = 0;
      for (int j = lower; j < upper; ++j)
      {
        sum += combined[j];
      }
      data[i] = sum;// / static_cast<float>(binLimits[i + 1] - binLimits[i]);
    }

    const int minHeight = 55;         // Pure magic number for "GoodFeel" minimum height (where detail actually matters)
    const float overallScalar = 1.0f; // Pure magic number. Probably good to not make it greater than 2.

    const int height = max(minHeight, height_);
    const int binWidth = 5;

    const float dataScalar = 1;// (height / 5.0f) * 2;          // Max height: 2/5th * overallScalar
    const float dataScalarHalf = 1;// dataScalar / 3;           // Max height of shadow: 2/15th * overallScalar
    const int startHeight = static_cast<int>((height_ / 5.0f) * 3 * 1.1f); // Location of reflection line. 3/5ths down from the top plus tweaks.. 

    const RConsole::Color base = RConsole::Color::LIGHTMAGENTA;
    const RConsole::Color mid = RConsole::Color::LIGHTRED;
    const RConsole::Color top = RConsole::Color::GREY;

    for (int bin = 0; bin < binCount; ++bin)
    {
      for (int x = bin * binWidth; x < bin * binWidth + binWidth; ++x)
      {
        float binValue = log2f(data[bin] + 1) * 20;
        int topVal = static_cast<int>(std::rintf(binValue * dataScalar * overallScalar));

        char asciiToUse = static_cast<char>(177);// (x % 2 == 0) ? static_cast<char>(174) : static_cast<char>(175);

        // Draw bottom part (start height down, shifted by 1 so that it doesn't overlap)
        //int bottomVal = static_cast<int>(std::rintf(binValue * dataScalarHalf * overallScalar)) + 1;
        for (int y = 0; y < 1; ++y)
        {
          RConsole::Canvas::Draw(asciiToUse, x, startHeight + y, RConsole::Color::DARKGREY);
        }

        // Draw top part (start height up)
        for (int y = 0; y < topVal; ++y)
        {
          float zeroToOne = y / static_cast<float>(topVal);
          RConsole::Color color = base;

          if (zeroToOne > 0.4f)
            color = mid;
          if (zeroToOne > 0.7f)
            color = top;

          RConsole::Canvas::Draw(asciiToUse, x, startHeight - y, color);
        }
      }
    }

    return true;
  }

}