#include "VisualizerCirrus.hpp"
#include <Core/AudioSystem.hpp>



namespace ASCIIPlayer
{
  VisualizerCirrus::VisualizerCirrus()
  : ASCIIVisualizer(CIRRUS_DATA_SIZE, AudioDataStyle::AUDIODATA_SPECTRUM)
  , width_(Width())
  , height_(Height())
  , startOffset_(0)
  {
    RConsole::Canvas::SetCursorVisible(false);

    // Initialize Workspace
    memset(workspace_, 0, CIRRUS_DATA_SIZE * sizeof(workspace_[0]));
  }

  /// <summary>
  /// Called when the window is resized. Takes the time to re-initialize everything.
  /// </summary>
  /// <param name="newWidth">new width of the window, post-resize</param>
  /// <param name="newHeight">new height of the window, post-resize</param>
  void VisualizerCirrus::OnResize(int newWidth, int newHeight)
  {
    RConsole::Canvas::ReInit(newWidth, newHeight);
    RConsole::Canvas::ForceClearEverything();
    width_ = newWidth;
    height_ = newHeight;
    RConsole::Canvas::SetCursorVisible(false);
  }

  bool VisualizerCirrus::Update(double dt_raw, float* data, bool isActive)
  {
    const float dt = static_cast<float>(dt_raw);
    const float moveSpeed = 4.0f; // characters per second (DT scaled)
    const float decay = 6; // scalar decay, vibes based.
    const float inputScalar = 80; // Vibes based input scalar
    bool invert = false;

    // intake and decay data. Avoid lowest bucket for consistency reasons.
    const float change = (1 - decay * dt);
    workspace_[0] = workspace_[0] * change;
    workspace_[0] += data[1] * inputScalar * dt;

    for (int i = 1; i < CIRRUS_DATA_SIZE; ++i)
    {
      workspace_[i] = workspace_[i] * change;
      workspace_[i] += data[i] * inputScalar * dt;
    }

    // Walk up to the end and back repeatedly, drawing based on
    // existing data.
    int index = static_cast<int>(startOffset_);
    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        const float cur = workspace_[index];

        if (cur > 3)
        {
          RConsole::Canvas::Draw('X', j, i, RConsole::LIGHTMAGENTA);
        }
        else if (cur > 1)
        {
          RConsole::Canvas::Draw('+', j, i, RConsole::LIGHTMAGENTA);
          RConsole::Canvas::Draw('+', j, i - 1, RConsole::LIGHTMAGENTA);
        }
        else if (cur > 0.6)
        {
          RConsole::Canvas::Draw('=', j, i, RConsole::LIGHTMAGENTA);
          RConsole::Canvas::Draw('=', j, i + 1, RConsole::LIGHTMAGENTA);
        }
        else if (cur > 0.3)
        {
          RConsole::Canvas::Draw('o', j, i, RConsole::MAGENTA);
        }
        else if (cur > 0.2)
        {
          RConsole::Canvas::Draw('-', j, i, RConsole::MAGENTA);
        }
        else if (cur > 0.1)
        {
          RConsole::Canvas::Draw('.', j, i, RConsole::MAGENTA);
        }

        if (invert == false)
        {
          if (index >= CIRRUS_DATA_SIZE - 1)
          {
            invert = true;
          }
          else
          {
            ++index;
          }
        }
        else
        {
          if (index <= 0)
          {
            invert = false;
          }
          else
          {
            --index;
          }
        }
      }
    }

    // 
    startOffset_ += static_cast<float>(dt) * moveSpeed;
    if (startOffset_ >= CIRRUS_DATA_SIZE)
    {
      startOffset_ = 0;
    }

    return true;
  }
}
