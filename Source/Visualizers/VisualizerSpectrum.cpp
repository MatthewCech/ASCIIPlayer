#include "VisualizerSpectrum.hpp"


namespace ASCIIPlayer
{
  VisualizerSpectrum::VisualizerSpectrum()
    : ASCIIVisualizer(CONSOLE_WIDTH_FUNC, AudioDataStyle::AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  void VisualizerSpectrum::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  // The maximum data size for a spectrum is going to be 1 since it represents a 0 to 1 occurrence of the frequency (% appearing at.)
  // No negatives present. Not currently scaling buckets down for width.
  bool VisualizerSpectrum::Update(double dt, float* data, bool isActive)
  {
    const int minHeight = 55;         // Pure magic number for "GoodFeel" minimum height (where detail actually matters)
    const float overallScalar = 1.5f; // Pure magic number. Probably good to not make it greater than 2.

    const int height = max(minHeight, height_);
    const int width = width_;

    const float dataScalar = (height / 5.0f) * 2 * overallScalar;          // Max height: 2/5th * overallScalar
    const float dataScalarHalf = dataScalar / 3 * overallScalar;           // Max height of shadow: 2/15th * overallScalar
    const int startHeight = static_cast<int>((height_ / 5.0f) * 3 * 1.1f); // Location of reflection line. 3/5ths down from the top plus tweaks.. 

    const RConsole::Color base = RConsole::Color::BLUE;
    const RConsole::Color mid = RConsole::Color::LIGHTCYAN;
    const RConsole::Color top = RConsole::Color::GREY;

    for (int x = 0; x < width; ++x)
    {
      float binValue = data[x];
      int topVal = static_cast<int>(std::rintf(binValue * dataScalar));
      int bottomVal = static_cast<int>(std::rintf(binValue * dataScalarHalf));

      char asciiToUse = (x % 2 == 0) ? static_cast<char>(174) : static_cast<char>(175);

      // Draw bottom part (start height down, shifted by 1 so that it doesn't overlap)
      for (int y = 0; y < bottomVal; ++y)
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

    return true;
  }

}