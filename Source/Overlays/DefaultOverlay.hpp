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
    std::int64_t getTime();

    // Private variables
    UIInfo lastInfo_;
    std::int64_t lastFadeTime_;
    std::int64_t lastTime_;
    std::int64_t time_;
    std::int64_t times_[100] = { 0 };
    size_t timesIndex_ = 0;
  };
}