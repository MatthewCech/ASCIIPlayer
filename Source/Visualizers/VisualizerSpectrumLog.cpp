#include "VisualizerSpectrumLog.hpp"
#include <cmath>

#define NYQUIST (44100.0f / 2.0f)
#define ROUND_INT(f) (static_cast<int>(f < 0.0f ? (f - 0.5f) : (f + 0.5f)))
#define FREQ_TO_BIN(freq, size) std::pair<int, float>(ROUND_INT(freq), freq / NYQUIST * size)

namespace ASCIIPlayer
{
  VisualizerSpectrumLog::VisualizerSpectrumLog()
    : ASCIIVisualizer(FMOD_DATA_SPECTRUM_SIZE, AudioDataStyle::AUDIODATA_SPECTRUM)
    , width_(CONSOLE_WIDTH_FUNC)
    , height_(CONSOLE_HEIGHT_FUNC)
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  /// <summary>
  /// Called when the window is resized. Takes the time to re-initialize everything.
  /// </summary>
  /// <param name="newWidth">new width of the window, post-resize</param>
  /// <param name="newHeight">new height of the window, post-resize</param>
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
  bool VisualizerSpectrumLog::Update(double dt, float* spectrum_data, bool isActive)
  {
    const int combinedDataSize = FMOD_DATA_SPECTRUM_SIZE / 2;
    const float binSizeHz = NYQUIST / FMOD_DATA_SPECTRUM_SIZE;

    // I'm basically fudging 1/3rd octaves in order to make sure they're relevant.
    // May be worth trying 1/12 and just... idk, seeing what has to get dropped?
    // octave bands https://en.wikipedia.org/wiki/Octave_band
    // https://www.engineeringtoolbox.com/octave-bands-frequency-limits-d_1602.html
    const std::pair<int, float> binLimits[] =
    {
      FREQ_TO_BIN(0.0f, combinedDataSize), // Used in place of smallest bin to catch all.
      //FREQ_TO_BIN(11.2f, combinedDataSize), // not used as it's the smallest bin, also granularity issue with next bin.
      FREQ_TO_BIN(14.1f, combinedDataSize), 
      //FREQ_TO_BIN(17.8f, combinedDataSize), // data isn't granular enough to support this
      FREQ_TO_BIN(22.4f, combinedDataSize),
      //FREQ_TO_BIN(28.2f, combinedDataSize), // data isn't granular enough to support this
      FREQ_TO_BIN(35.5f, combinedDataSize),
      FREQ_TO_BIN(44.7f, combinedDataSize),
      FREQ_TO_BIN(56.2f, combinedDataSize),
      FREQ_TO_BIN(70.8f, combinedDataSize),
      FREQ_TO_BIN(89.1f, combinedDataSize),
      FREQ_TO_BIN(112.0f, combinedDataSize),
      FREQ_TO_BIN(141.0f, combinedDataSize),
      FREQ_TO_BIN(178.0f, combinedDataSize),
      FREQ_TO_BIN(224.0f, combinedDataSize),
      FREQ_TO_BIN(282.0f, combinedDataSize),
      FREQ_TO_BIN(355.0f, combinedDataSize),
      FREQ_TO_BIN(447.0f, combinedDataSize),
      FREQ_TO_BIN(562.0f, combinedDataSize),
      FREQ_TO_BIN(708.0f, combinedDataSize),
      FREQ_TO_BIN(891.0f, combinedDataSize),
      FREQ_TO_BIN(1122.0f, combinedDataSize),
      FREQ_TO_BIN(1413.0f, combinedDataSize),
      FREQ_TO_BIN(1778.0f, combinedDataSize),
      FREQ_TO_BIN(2239.0f, combinedDataSize),
      FREQ_TO_BIN(2818.0f, combinedDataSize),
      FREQ_TO_BIN(3548.0f, combinedDataSize),
      FREQ_TO_BIN(4467.0f, combinedDataSize),
      FREQ_TO_BIN(5623.0f, combinedDataSize),
      FREQ_TO_BIN(7079.0f, combinedDataSize),
      FREQ_TO_BIN(8913.0f, combinedDataSize),
      FREQ_TO_BIN(11220.0f, combinedDataSize),
      FREQ_TO_BIN(14130.0f, combinedDataSize),
      FREQ_TO_BIN(17780.0f, combinedDataSize),
      FREQ_TO_BIN(NYQUIST, combinedDataSize),
      FREQ_TO_BIN(NYQUIST * 2, combinedDataSize) // Catch odd remains or file noise, this won't contain valid data but it's still data.
    };

    const int binCount = sizeof(binLimits) / sizeof(std::pair<int, float>) - 1;

    // Sort data into bins
    float data[binCount];
    int pos = 0;

    // Collect from linear bins into log bins
    for (int i = 0; i < binCount; ++i)
    {
      const int lower = ROUND_INT(binLimits[i].second);
      const int upper = ROUND_INT(binLimits[i + 1].second);

      float sum = 0;
      for (int j = lower; j < upper; ++j)
      {
        sum += spectrum_data[j];
      }

      //data[i] = sum;
      data[i] = sum / static_cast<float>(upper - lower);
    }

    const int minHeight = 55;         // Pure magic number for "GoodFeel" minimum height (where detail actually matters)
    const float overallScalar = 1.0f; // Pure magic number. Probably good to not make it greater than 2.

    const int height = max(minHeight, height_);
    const int binWidth = 3;

    const float mainScalar = 100.0f; // Applied to a log
    const float shadowScalar = 0.15f;

    const int startHeight = static_cast<int>((height_ / 5.0f) * 3 * 1.1f); // Location of reflection line. 3/5ths down from the top plus tweaks.. 

    const RConsole::Color base = RConsole::Color::LIGHTMAGENTA;
    const RConsole::Color mid = RConsole::Color::LIGHTRED;
    const RConsole::Color top = RConsole::Color::GREY;

    const int leftOffset = ((width_ - (binCount * binWidth)) / 2);

    for (int bin = 0; bin < binCount; ++bin)
    {
      for (int x = bin * binWidth; x < bin * binWidth + binWidth; ++x)
      {
        float binValue = log2f(data[bin] * height * mainScalar + 1);
        char asciiToUse = 'o';
        char shadowToUse = '-';

        if (x % 3 == 0)
        {
          asciiToUse = static_cast<char>(219);
          shadowToUse = static_cast<char>(177);
        }
        else if (x % 3 == 1)
        {
          asciiToUse = static_cast<char>(178);
          shadowToUse = static_cast<char>(176);
        }
        else
        {
          asciiToUse = static_cast<char>(176);
          shadowToUse = ' ';
        }

        int topVal = static_cast<int>(std::rintf(binValue * overallScalar));
        int bottomVal = static_cast<int>(std::rintf(binValue * shadowScalar)) + 1;

        // Curve top
        if (x >= bin * binWidth && x < bin * binWidth + binWidth - 1)
        {
          topVal += 1;
          bottomVal += 1;
        }

        // Draw 'shadow' (first so top part overlaps)
        for (int y = 1; y < bottomVal; ++y)
        {
          RConsole::Canvas::Draw(shadowToUse, leftOffset + x, startHeight + y, RConsole::Color::DARKGREY);
        }

        // Draw top part
        for (int y = 0; y < topVal; ++y)
        {
          float zeroToOne = y / static_cast<float>(topVal);
          RConsole::Color color = base;

          if (zeroToOne > 0.4f)
            color = mid;
          if (zeroToOne > 0.7f)
            color = top;

          int offset = 0;

          RConsole::Canvas::Draw(asciiToUse, leftOffset + x, startHeight - y - offset, color);
        }

      }

    }
    
    AnnotateFrequency(binLimits, 4, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 8, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 12, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 16, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 20, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 24, binWidth, leftOffset, startHeight + 4);
    AnnotateFrequency(binLimits, 28, binWidth, leftOffset, startHeight + 4);

    return true;
  }

  void VisualizerSpectrumLog::AnnotateFrequency(const std::pair<int, float>* binLimits, int bin, int binWidth, int xOffset, int yOffset)
  {
    int x = xOffset + bin * binWidth - 1;
    int y = yOffset;
    RConsole::Color color = RConsole::Color::CYAN;

    RConsole::Canvas::DrawString((std::string("^") + std::to_string(binLimits[bin].first)).c_str(), x, y, color);
    RConsole::Canvas::Draw('|', x, y - 1, color);
    RConsole::Canvas::Draw('|', x, y - 2, color);
    RConsole::Canvas::Draw('|', x, y - 3, color);
  }
}