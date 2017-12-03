#pragma once
#include "ASCIIOverlay.hpp"
#include <chrono>


namespace ASCIIPlayer
{
  class DefaultOverlay : public ASCIIOverlay
  {
  public:
    DefaultOverlay();
    virtual bool Update(const UIInfo uinfo);

  private:
    long long getTime();

    UIInfo lastInfo_;
    long long lastFadeTime_;
    long long lastTime_;
    long long time_;
    long long times_[100] = { 0 };
    size_t timesIndex_ = 0;
  };
}