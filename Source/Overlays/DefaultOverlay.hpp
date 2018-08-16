#pragma once
#include <chrono>
#include "ASCIIOverlay.hpp"


namespace ASCIIPlayer
{
  class DefaultOverlay : public ASCIIOverlay
  {
  public:
    DefaultOverlay();
    virtual bool Update(const UIInfo uinfo);

  private:
    // Private methods
    long long getTime();

    // Private variables
    UIInfo lastInfo_;
    long long lastFadeTime_;
    long long lastTime_;
    long long time_;
    long long times_[100] = { 0 };
    size_t timesIndex_ = 0;
  };
}