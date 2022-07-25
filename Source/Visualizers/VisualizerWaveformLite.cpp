#include "VisualizerWaveformLite.hpp"
#include <Core\AudioSystem.hpp>


namespace ASCIIPlayer
{
  // Constructor
  VisualizerWaveformLite::VisualizerWaveformLite()
    : ASCIIVisualizer(CONSOLE_WIDTH_FUNC * SCALE_FACTOR, AudioDataStyle::AUDIODATA_WAVEFORM)
    , startingWidth_(CONSOLE_WIDTH_FUNC * SCALE_FACTOR)
    , width_(startingWidth_)
    , height_(CONSOLE_HEIGHT_FUNC)
    , workspace_(nullptr)
  {
    RConsole::Canvas::SetCursorVisible(false);
    initializeWorkspace();
  }

  /// <summary>
  /// Clean up allocated memory and perform any necessary shutdown
  /// </summary>
  VisualizerWaveformLite::~VisualizerWaveformLite()
  {
    if (workspace_ != nullptr)
    {
      delete[] workspace_;
    }
  }

  /// <summary>
  /// Deletes any existing workspace, allocating a new one based on the width and zeroing it out.
  /// </summary>
  void VisualizerWaveformLite::initializeWorkspace()
  {
    if (workspace_ != nullptr)
    {
      delete[] workspace_;
    }

    workspaceCount_ = FMOD_DATA_SIZE / SCALE_FACTOR;
    workspace_ = new float[workspaceCount_];
    memset(workspace_, 0, workspaceCount_ * sizeof(workspace_[0]));
  }

  /// <summary>
  /// Called when the window is resized. Takes the time to re-initialize everything.
  /// </summary>
  /// <param name="newWidth">new width of the window, post-resize</param>
  /// <param name="newHeight">new height of the window, post-resize</param>
  void VisualizerWaveformLite::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
    initializeWorkspace();
  }

  // Draw Bars in traditional waveform style.
  // Returns if it succeeded or not. Note that when isActive is true, we aren't 
  // paused. If isActive is false, we're using the data from the previous run
  // and we don't see that scale by volume.
  bool VisualizerWaveformLite::Update(float* normalizedData, bool isActive)
  {
    // Set up some sweet numbers
    const int halfHeight = height_ / 2;             // Half-way point on the console visually
    const float heightScalar = 5.0f * SCALE_FACTOR; // Arbitrary value determining the max vertical offset. Used to be 6, but was scaled for volume of .9 by default.
    const float heightMinimum = 0;                  // Arbitrary value determining smallest vertical offset. Chosen for visual appeal - creates 2 bottom and 1 top line.
    const char symbol = 'o';

    // clear the workspace
    memset(workspace_, 0, workspaceCount_ * sizeof(workspace_[0]));

    int offset = (width_ - startingWidth_) / 2;
    if (offset < 0)
      offset = 0;

    // Only using half the workspace really
    for (int i = 0; i < width_ && i < startingWidth_; ++i)
    {
      int bin = i / SCALE_FACTOR;

      const float yHeight = normalizedData[i] * heightScalar + heightMinimum;
      workspace_[bin] = yHeight / static_cast<float>(SCALE_FACTOR);
    }

    // For every X position, calculate the Y position based on waveform data and variables above.
    for (int i = 0; i < workspaceCount_; ++i)
    {
      const float binVal = workspace_[i];
      const float yPos = halfHeight - binVal;
      const int xPos = i + offset;

      RConsole::Canvas::Draw(symbol, xPos, static_cast<int>(yPos), RConsole::LIGHTBLUE);
    }

    return true;
  }
}

#undef SCALE_FACTOR
#undef WAVEFORM_SIZE