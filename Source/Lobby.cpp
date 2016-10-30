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
  { 
    // Read for config file using FIleIO
    DJConfig config;

    // Make DJ, don't autoplay.
    DJ *Dj = new DJ(config, false);

    // Parse arguments using arg parser
    AudioFile *test1 = new ASCIIPlayer::AudioFile("Test_Audio.mp3");
    Dj->AddSong(test1);

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
    while (lobbyHosting_)
    {
      if (activeDJ_)
        activeDJ_->Update();
    }
  }


  bool Lobby::ParseCommand(std::string command)
  {
    command = cleanCommand(command);

    // ...

    return true;
  }

    /////////////
   // Private //
  /////////////
  // Private methods
  std::string Lobby::cleanCommand(std::string input)
  {
    return "";
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