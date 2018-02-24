#include <Lobby.hpp>
#include <FileIO/FileIO.hpp>



namespace ASCIIPlayer
{
    /////////////
   // Private //
  /////////////
  // Calculates average framerate based on the start and stop provided. 
  int Lobby::averageFPS(long long start, long long end)
  {
    size_t size = sizeof(times_) / sizeof(long long);
    times_[timesIndex_++] = end - start;
    if (timesIndex_ >= size)
      timesIndex_ = 0;

    float total = 0;
    for (size_t i = 0; i < size; ++i)
      total += times_[i];

    return static_cast<int>(1000.0f / (total / size));
  }


  // Tries to open config file for the visualizer, generates one otherwise.
  DJConfig Lobby::readConfigFile()
  {
    std::string arg0 = argParser_[0];
    size_t loc = arg0.find_last_of('\\');
    std::string filepath = "";
    if (loc != std::string::npos)
      filepath = arg0.substr(0, loc);

    DEBUG_PRINT("This is our arg0: " << arg0);
    DEBUG_PRINT("This is our path: " << filepath);

    FileUtil::File f(filepath + "\\ASCIIPlayer.conf");
    if (f.GetLineCount() == 0)
    {
      DJConfig def;
      std::string str = def.ToString();
      f.Write(str);
      f.Save();
      return def;
    }
    else
    {
      DJConfig newConf;
      for (unsigned int i = 0; i < f.GetLineCount(); ++i)
        newConf.ParseLine(f[i]);
      return newConf;
    }
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
