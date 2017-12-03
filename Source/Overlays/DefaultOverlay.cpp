#include "DefaultOverlay.hpp"


namespace ASCIIPlayer
{
  // Constructor
  DefaultOverlay::DefaultOverlay() 
    : ASCIIOverlay() 
  {  }


  // Updates the screen with an overlay
  bool DefaultOverlay::Update(const UIInfo info)
  {
    if (info != lastInfo_)
    {

    }

    return true;
  }
}