#include "Lobby.hpp"
#include <FileIO/FileIO.hpp>


namespace ASCIIPlayer
{
    ////////////
   // Public //
  ////////////
  // Constructor and Destructor
  Lobby::Lobby(int argc, char** argv) 
    : argParser_(argc, argv)
    , activeDJ_(nullptr)
    , lobbyHosting_(true)
  { 
    // Read for config file using FIleIO
    DJConfig config;
    //FileUtil::File f("config.cfg")

    // Make DJ, don't autoplay.
    DJ *Dj = new DJ(config, false);
    
    // Handle all of the flags
    std::vector<std::string> cmds = argParser_.StartsWith("-");
    for (unsigned int i = 0; i < cmds.size(); ++i)
      ParseCommand(cmds[i]);

    // Just loop through and see if anything happens to be a song to load.
    std::vector<std::string> args = argParser_.GetAllArgs();
    for (unsigned int i = 0; i < args.size(); ++i)
    {
      AudioFile *test1 = new ASCIIPlayer::AudioFile(args[i]);
      Dj->AddSong(test1);
    }

    // Set activeDJ to our DJ
    activeDJ_ = Dj;
  }


  // Destructor
  Lobby::~Lobby()
  {
    if (activeDJ_)
    {
      activeDJ_->Pause();
      activeDJ_->Shutdown();
      delete activeDJ_;
    }
  }


  // Runs the primary lobby loop
  void Lobby::Run()
  {
    DEBUG_PRINT("== Lobby looking spiffy! == ");

    if (AP_DEBUG_PRINT_VAL)
      if (activeDJ_)
        DEBUG_PRINT("== DJ Has prepped " << activeDJ_->GetPlaylistSize() << " songs! ==");

    if (activeDJ_->GetPlaylistSize() > 0)
      activeDJ_->Play();

    while (lobbyHosting_)
    {
      if (activeDJ_)
        activeDJ_->Update();
    }
  }


  // Parse commands as we see fit.
  bool Lobby::ParseCommand(std::string command)
  {
    command = cleanCommand(command);
    bool update = false;
    //if (command == "loop")
      //activeDJ_->

    return true;
  }


    /////////////
   // Private //
  /////////////
  // Private methods
  std::string Lobby::cleanCommand(std::string input)
  {
    return input.substr(1, input.size() - 1);
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
}