#pragma once
#include "ASCIIOverlay.hpp"



namespace ASCIIPlayer
{
  class DefaultOverlay : public ASCIIOverlay
  {
  public:
    DefaultOverlay();
    virtual bool Update(const UIInfo uinfo);

  private:
    UIInfo lastInfo_;
  };
}