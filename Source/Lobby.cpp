#include "Lobby.hpp"
#include <FileIO/FileIO.hpp>
#include <ConsoleInput/console-input.h>
#include <exception>


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
    // Make DJ, don't autoplay.
    DJ *Dj = new DJ(readConfigFile(), false);
    
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
    DEBUG_PRINT("Lobby looking spiffy!");

    if (activeDJ_)
    {
      DEBUG_PRINT("DJ Has prepped " << activeDJ_->GetPlaylistSize() << " songs!");
    }

    if (activeDJ_->GetPlaylistSize() > 0)
      activeDJ_->Play();

    // While we're hosting stuff in the lobby
    while (lobbyHosting_)
    {
      if (activeDJ_)
        activeDJ_->Update();

      if (int num = KeyHit())
        while (num-- > 0)
        {
          char input = static_cast<char>(GetChar());
          interpretChar(input);
        }
    }
  }


  // Parse commands as we see fit. Assumes command has been consumed already.
  bool Lobby::ParseCommand(std::string command)
  {
    command = cleanCommand(command);
    
    if (command.size() == 0)
      return false;

    return interpretString(command);
  }


    /////////////
   // Private //
  /////////////
  // Private methods
  std::string Lobby::cleanCommand(std::string input)
  {
    //DEBUG_PRINT("Cleaning " << input);
    //Strip whitespace
    //Stirp command indicator if present
    //Return
    //input.substr(1, input.size() - 1);
    return input;
  }


  bool Lobby::interpretString(const std::string command)
  {
    if(command.size() <= 0)
      return false;

    return false;
    //if(command == whatever)
    //  doThing();
    //...
    // return true;
  }


  bool Lobby::interpretChar(const char c)
  {
    if (activeDJ_ == nullptr)
      throw "You should have an active DJ to issue character commands";

    switch (c)
    {
    case KEY_RIGHT:
    case KEY_NUM_6:
      activeDJ_->SkipForward();
      break;
    case KEY_LEFT:
    case KEY_NUM_4:
      activeDJ_->SkipBackward();
      break;
    case '-':
      activeDJ_->VolumeDown();
      break;
    case '+':
    case '=':
      activeDJ_->VolumeUp();
      break;
    case '0':
    default:
      return false;
    }

    return true;
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

    FileUtil::File f(filepath + "ASCIIPlayer.conf");
    if (f.GetLineCount() == 0)
    {
      DJConfig def;
      f.Write(def.ToString());
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
}