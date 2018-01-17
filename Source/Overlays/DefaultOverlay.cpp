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

    // Delay all fading and time updates on paused for UI.
    if(info.IsPlaying == false)
      lastFadeTime_ = time_;

    // Determine color index 
    const int fadeDelayMS = 105;
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

    ///////////////// Drawing happens below ///////////////// 

    // Variables
    const int volumeHeightOffset = height - 4;
    const int leftGlobalOffset = 2;
    int volumeWidthOffset = leftGlobalOffset;
    const int volumeWidthTotal = 20;
    const int progressWidthTotal = 20;
    float volumeWidthCurrent = info.Volume * volumeWidthTotal;
    float progressWidthCurrent = static_cast<float>(info.SongPos) / static_cast<float>(info.SongLength) * 20;

    if (info.Volume > 0.99f)
      volumeWidthCurrent = static_cast<float>(volumeWidthTotal);
    if (info.SongPos > info.SongLength - (info.SongLength / 80))
      progressWidthCurrent = static_cast<float>(progressWidthTotal);

    // Label
    // This is split up into each individual character because I wanted to use alt+13, alt+14, and alt+17 at one point for a vol icon.
    RConsole::Canvas::Draw(' ', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('V', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('o', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('l', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw(':', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw(' ', volumeWidthOffset++, volumeHeightOffset, color);

    // Bar
    RConsole::Canvas::Draw('<', volumeWidthOffset++, volumeHeightOffset, color);
    RConsole::Canvas::Draw('>', volumeWidthTotal + volumeWidthOffset, volumeHeightOffset, color);
    for (int i = volumeWidthOffset; i < volumeWidthCurrent + volumeWidthOffset; ++i)
      RConsole::Canvas::Draw(static_cast<unsigned char>(254), i, volumeHeightOffset, color);



    // Draw current song name out: Left aligned
    int titleLeftOffset = leftGlobalOffset;
    const int titleHeightOfset = volumeHeightOffset + 1;
    RConsole::Canvas::Draw('S', titleLeftOffset++, titleHeightOfset, color);
    RConsole::Canvas::Draw('o', titleLeftOffset++, titleHeightOfset, color);
    RConsole::Canvas::Draw('n', titleLeftOffset++, titleHeightOfset, color);
    RConsole::Canvas::Draw('g', titleLeftOffset++, titleHeightOfset, color);
    RConsole::Canvas::Draw(':', titleLeftOffset++, titleHeightOfset, color);
    RConsole::Canvas::Draw(' ', titleLeftOffset++, titleHeightOfset, color);

    // Title
    float titleWidthOffset = (width - info.Song.size()) / 2.0f;
    if (titleWidthOffset < 1)
      titleWidthOffset = 0;
    RConsole::Canvas::DrawString(info.Song.c_str(), static_cast<float>(titleLeftOffset), static_cast<float>(titleHeightOfset), color);



    // Draw song progress
    // Label
    int progressLeftOffset = leftGlobalOffset;
    const int progressHeightOffset = titleHeightOfset + 1;
    RConsole::Canvas::Draw(' ', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw('P', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw('o', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw('s', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw(':', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw(' ', progressLeftOffset++, progressHeightOffset, color);

    // Bar
    RConsole::Canvas::Draw('<', progressLeftOffset++, progressHeightOffset, color);
    RConsole::Canvas::Draw('>', progressWidthTotal + progressLeftOffset, progressHeightOffset, color);
    size_t seconds = info.SongPos / 1000 % 60;
    size_t minutes = seconds / 60 % 60;
    size_t hours = minutes / 60 % 60;
    RConsole::Canvas::DrawString((std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s").c_str()
      , static_cast<float>(progressWidthTotal + progressLeftOffset + 2)
      , static_cast<float>(progressHeightOffset), color);
    for (int i = progressLeftOffset; i < progressWidthCurrent + progressLeftOffset; ++i)
      RConsole::Canvas::Draw(static_cast<unsigned char>(254), i, progressHeightOffset, color);



    // Draw paused icon.
    if (info.IsPlaying == false)
    {
      const float rightOffset = 8;
      const char pChar = static_cast<unsigned char>(219);
      const char pausedLine[] = { pChar, pChar, ' ', ' ', pChar, pChar, '\0' };
      RConsole::Canvas::DrawString(pausedLine, width - rightOffset, volumeHeightOffset - 2.0f, color);
      RConsole::Canvas::DrawString(pausedLine, width - rightOffset, volumeHeightOffset - 1.0f, color);
      RConsole::Canvas::DrawString(pausedLine, width - rightOffset, volumeHeightOffset - 0.0f, color);
      RConsole::Canvas::DrawString(pausedLine, width - rightOffset, volumeHeightOffset + 1.0f, color);
    }

    // Everything was drawn without issue. We're good to go!
    return true;
  }

  // Get a long long of current time in MS
  long long DefaultOverlay::getTime()
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }

}