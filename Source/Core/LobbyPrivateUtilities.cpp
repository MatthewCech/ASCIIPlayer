#include "Lobby.hpp"


namespace ASCIIPlayer
{
    //////////////////////////////////////
   // Private - Utility and Management //
  //////////////////////////////////////
  // Calculates average framerate based on the start and stop provided. 
  int Lobby::averageFPS(std::int64_t start, std::int64_t end)
  {
    // Doing this is clever and all, but this used to say sizeof(times_) / sizeof(std::int64_t), which is 2x the amount.
    //size_t size = sizeof(times_) / sizeof(std::int64_t);
    size_t size = TRACKED_TIMES;
    times_[timesIndex_++] = end - start;
    if (timesIndex_ >= size)
      timesIndex_ = 0;

    float total = 0;
    for (size_t i = 0; i < size; ++i)
      total += times_[i];

    return static_cast<int>(1000.0f / (total / size));
  }


  // Begins playing the DJ.
  void Lobby::startDJ()
  {
    activeDJ_->Play();
  }


  // Stops playing the DJ
  void Lobby::stopDj()
  {
    activeDJ_->Pause();
  }


  // Sets current DJ
  void Lobby::setDJ(DJ *newDJ)
  {
    if (activeDJ_)
    {
      activeDJ_->Pause();
      activeDJ_->Shutdown();
      delete activeDJ_;
    }

    activeDJ_ = newDJ;
  }


  // Move to the right in the menu
  bool Lobby::menuMoveCheckRight()
  {
    if (menuSystems_.MenuDepth() == 2)
    {
      menuSystems_.Back();
      menuSystems_.Down();
      menuSystems_.Select();

      return true;
    }

    return false;
  }


  // Move to the left in the menu, wrapps the actions assoicated with it
  bool Lobby::menuMoveCheckLeft()
  {
    if (menuSystems_.MenuDepth() == 2)
    {
      menuSystems_.Back();
      menuSystems_.Up();
      menuSystems_.Select();

      return true;
    }

    return false;
  }
}
