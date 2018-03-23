#include "VisualizerDefault.hpp"
#include <math.h>
#include <thread>

#define VERTICAL_PADDING 4
#ifdef OS_WINDOWS
  #pragma warning(disable : 4996)
#endif

namespace ASCIIPlayer
{
  // Constructor
  VisualizerDefault::VisualizerDefault()
    : ASCIIVisualizer(RConsole::Canvas::GetConsoleHeight() - (VERTICAL_PADDING * 2), AUDIODATA_WAVEFORM, "visualizerDefault")
    , height_(RConsole::Canvas::GetConsoleHeight())
  {
    RConsole::Canvas::SetCursorVisible(false);
  }

  // Draw waveform based on updating
  bool VisualizerDefault::Update(float* data)
  {
    // Construct appropriate buffers, one for input and one for output.
    char buffer[80];
    char buffer_bar[80];
    std::fill_n(buffer_bar, sizeof(buffer_bar) / sizeof(char), '>');

    // Handle drawing
    for (size_t i = 0; i < GetAudioDataSize() && i < height_ - VERTICAL_PADDING; ++i)
    {
      // Calculate the distance of the string to display, and display dv of it.
      int dv = static_cast<int>(data[i] * 90);
      
      // Clear buffers
      std::fill_n(buffer, sizeof(buffer) / sizeof(char), 0);

      // Cap values
      const int min = 0;
      if (dv < 0)
        dv = 0;
      const int cap = sizeof(buffer) / sizeof(char) - 1;
      if (dv > cap) 
        dv = cap;

      // Format and print buffer. Use sprintf
      sprintf(buffer, "%.*s\n", dv, buffer_bar);
      RConsole::Canvas::DrawString(buffer, 0, i + VERTICAL_PADDING, RConsole::LIGHTCYAN);
    }

    // We're good! Return true!
    return true;
  }
}


#ifdef OS_WINDOWS
  #pragma warning(default : 4996)
#endif
