#include "DefaultOverlay.hpp"



namespace ASCIIPlayer
{
  // Constructor
  DefaultOverlay::DefaultOverlay() 
    : ASCIIOverlay() 
    , lastTime_(getTime())
  {  }


  // Updates the screen with an overlay
  bool DefaultOverlay::Update(const UIInfo info)
  {
    //std::cout << (end - start) << std::endl;
    const int width{ CONSOLE_WIDTH_FUNC };
    const int height{ CONSOLE_HEIGHT_FUNC };

    if (info != lastInfo_)
    {
      lastFadeTime_ = getTime();
      lastInfo_ = info;
    }

    // Update times
    lastTime_ = time_;
    time_ = getTime();

    // Determine color index 
    const int fadeDelayMS = 100;
    long long color_index = (time_ - lastFadeTime_) / fadeDelayMS;
    const RConsole::Color colors[] = { RConsole::WHITE, RConsole::WHITE, RConsole::WHITE, RConsole::WHITE 
                                     , RConsole::WHITE, RConsole::WHITE, RConsole::WHITE, RConsole::WHITE 
                                     , RConsole::WHITE, RConsole::WHITE, RConsole::WHITE, RConsole::WHITE
                                     , RConsole::WHITE, RConsole::WHITE, RConsole::WHITE, RConsole::WHITE
                                     , RConsole::GREY , RConsole::DARKGREY };
     
    // Leave if we have no more colors to go through
    if (color_index >= sizeof(colors) / sizeof(RConsole::Color))
      return false;

    // Set up variables for colors, then use it to draw the UI.
    RConsole::Color color = colors[color_index];

    ///////////////// Draw code below ///////////////// 

    // Left aligned volume bar
    const int volumeHeightOffset = height - 5;
    int volumeWidthOffset = 2;
    const int volumeWidthTotal = 20;
    float volumeWidthCurrent = info.Volume * volumeWidthTotal;

    if (info.Volume > 0.99f)
      volumeWidthCurrent = static_cast<float>(volumeWidthTotal);

    RConsole::Canvas::Draw('V', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('o', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('l', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('.', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw(' ', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('[', static_cast<float>(volumeWidthOffset++), static_cast<float>(volumeHeightOffset), color);
    RConsole::Canvas::Draw(']', static_cast<float>(volumeWidthTotal + volumeWidthOffset), static_cast<float>(volumeHeightOffset), color);

    for (int i = volumeWidthOffset; i < volumeWidthCurrent + volumeWidthOffset; ++i)
      RConsole::Canvas::Draw('-', static_cast<float>(i), static_cast<float>(volumeHeightOffset), color);

    /*
    // Draw volume bar: Centered.
    const int volumeHeightOffset = 2;
    const int volumeWidthOffset = 2;
    const int volumeWidthTotal = (width - volumeWidthOffset + 1);
    float volumeWidthCurrent = info.Volume * volumeWidthTotal;

    if (info.Volume > 0.99f)
      volumeWidthCurrent = static_cast<float>(volumeWidthTotal);

    RConsole::Canvas::Draw('[', static_cast<float>(volumeWidthOffset - 1), static_cast<float>(volumeHeightOffset), color);
    RConsole::Canvas::Draw(']', static_cast<float>(volumeWidthTotal), static_cast<float>(volumeHeightOffset), color);

    for (int i = volumeWidthOffset; i < volumeWidthCurrent; ++i)
      RConsole::Canvas::Draw('-', static_cast<float>(i), static_cast<float>(volumeHeightOffset), color);
    */

    // Draw current song name out: Centered
    const int titleHeightOfset = volumeHeightOffset + 2;
    float titleWidthOffset = (width - info.Song.size()) / 2.0f;
    if (titleWidthOffset < 1)
      titleWidthOffset = 0;

    RConsole::Canvas::DrawString(info.Song.c_str(), titleWidthOffset, static_cast<float>(titleHeightOfset), color);

    // Everything was drawn without issue. We're good to go!
    return true;
  }

  // Get a long long of current time in MS
  long long DefaultOverlay::getTime()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

}