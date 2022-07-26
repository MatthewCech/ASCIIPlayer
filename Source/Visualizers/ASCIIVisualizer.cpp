#include "ASCIIVisualizer.hpp"

namespace ASCIIPlayer
{
  /// <summary>
  /// Constructor. Nothing specific for this, but we want to 
  /// </summary>
  /// <param name="audioDataSize">Starting size of the data. Can be updated with sizing callbacks later.</param>
  /// <param name="style">The type of data to collect for this visualizer.</param>
  ASCIIVisualizer::ASCIIVisualizer(unsigned int audioDataSize, AudioDataStyle style)
    : Visualizer(audioDataSize, style)
  {  }

  /// <summary>
  /// Virtual destructor
  /// </summary>
  ASCIIVisualizer::~ASCIIVisualizer()
  {  }

  /// <summary>
  /// Get a realistic width of the console window. Has internal guards to prevent negatives.
  /// </summary>
  /// <returns>A positive int representing a window width</returns>
  int ASCIIVisualizer::Width()
  {
    const int minWidth = 0;
    const int width = CONSOLE_WIDTH_FUNC;

    if (width < minWidth)
    {
      return minWidth;
    }

    return width;
  }
  
  /// <summary>
/// Get a realistic height of the console window. Has internal guards to prevent negatives.
/// </summary>
/// <returns>A positive int representing a window height</returns>
  int ASCIIVisualizer::Height()
  {
    const int minHeight = 0;
    const int height = CONSOLE_HEIGHT_FUNC;

    if (height < minHeight)
    {
      return minHeight;
    }

    return height;
  }
}
