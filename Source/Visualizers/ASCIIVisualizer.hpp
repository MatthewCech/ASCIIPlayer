#pragma once
#include "Defines.hpp"
#include <ConsoleUtils/console-utils.hpp>
#include <Visualizers/Visualizer.hpp>


namespace ASCIIPlayer
{
  // NOTE(mcech): This is just one of several different ways to do ASCII display stuff
  // with the console. This is the abstract class that handles a lot of the first-pass
  // ascii visuals the application uses.
  class ASCIIVisualizer : public Visualizer
  {
  public:
    // Constructor
    ASCIIVisualizer(unsigned int audioDataSize, AudioDataStyle style)
      : Visualizer(audioDataSize, style)
    {  }

    // Destructor
    virtual ~ASCIIVisualizer() {  };

    int Width() { return CONSOLE_WIDTH_FUNC; }
    int Height() { return CONSOLE_HEIGHT_FUNC; }
  };
}