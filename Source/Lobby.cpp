#include "Lobby.hpp"
#include <FileIO/FileIO.hpp>
#include <ConsoleInput/console-input.h>
#include <exception>
#include <thread>

#define MS_SINCE_EPOCH std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()


namespace ASCIIPlayer
{
    ////////////
   // Public //
  ////////////
  // Constructor and Destructor
  Lobby::Lobby(int argc, char** argv) 
    : argParser_(argc, argv)
    , activeDJ_(nullptr)
    , menuSystems_("")
    , lobbyHosting_(true)
    , menuVisible_(false)
    , timesIndex_(0)
    , times_()
    , showDebug_(false)
    , fpsStart_(0)
    , fpsPrevStart_(0)
    , fpsEnd_(0)
    , appStartTime_(MS_SINCE_EPOCH)
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


  // Has while loop and STD::Cin every loop to ensure it's running correctly. 
  // Loop sits until DJ starts with proper command, at which point we get to "lobbyHosting".
  void Lobby::Run()
  {
    DEBUG_PRINT("Lobby looking spiffy!");

    if (activeDJ_)
      DEBUG_PRINT("DJ Has prepped " << activeDJ_->GetPlaylistSize() << " songs!");
    

    // Set up to start entering the primary loop
    RConsole::Canvas::ForceClearEverything();
    if (activeDJ_->GetPlaylistSize() > 0)
      activeDJ_->Play();

    // While we're hosting stuff in the lobby
    size_t loops = 0;
    while (lobbyHosting_)
    {
      fpsPrevStart_ = fpsStart_;
      fpsStart_ = MS_SINCE_EPOCH;
      // ============================ Start primary loop ============================

      // Idle screen if necessary
      if (activeDJ_->GetPlaylistSize() == 0)
      {
        const int osc[] = {'`', '*', '+', '_', ',', '.', '.', '.', '.', ',', '/', '^'};
        const size_t index = loops / 30;
        const size_t mod = (sizeof(osc) / sizeof(*osc));

        RConsole::Canvas::DrawString((std::string("Waiting for songs ") 
          + static_cast<char>(osc[(index) % mod])
          + static_cast<char>(osc[(index + 1) % mod])
          + static_cast<char>(osc[(index + 2) % mod])
          + static_cast<char>(osc[(index + 3) % mod])
          + static_cast<char>(osc[(index + 4) % mod])
          ).c_str(), 3, 3, RConsole::WHITE);
      }

      // Actively run DJ
      if (activeDJ_)
        activeDJ_->Update();

      if (int num = KeyHit())
        while (num-- > 0)
        {
          char input = static_cast<char>(GetChar());
          interpretChar(input);
        }

      // Finalize drawing
      if (showDebug_)
      {
        float loc = 0;
        const size_t seconds = static_cast<size_t>((fpsStart_ - appStartTime_) / 1000);
        const size_t minutes = seconds / 60;
        const size_t hours = minutes / 60;

        RConsole::Canvas::DrawString(("[ argv0: " + argParser_[0]).c_str(), 0.0f, loc++, RConsole::DARKGREY);
        RConsole::Canvas::DrawString(("[ utime: " + std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m " + std::to_string(seconds % 60) + "s").c_str(), 0.0f, loc++, RConsole::DARKGREY);
        RConsole::Canvas::DrawString(("[ c/sec: " + std::to_string(averageFPS(fpsPrevStart_, fpsEnd_)) + " per second").c_str(), 0.0f, loc++, RConsole::DARKGREY);
      }

      // Finalize all drawing
      RConsole::Canvas::Update();

      // Smol sleep. This makes most OSs extremely happy and reduces CPU load by like 30%.
      std::this_thread::sleep_for(std::chrono::microseconds(500)); 


      // ============================ End primary loop ============================
      fpsEnd_ = MS_SINCE_EPOCH;
      ++loops;
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
    case 'w':
    case 'W':
      //if (menuVisible_)

      break;
    case ']':
    case '}':
    case '>':
    case '.':
    case 'e':
    //case 'E':
    case KEY_TAB: 
    case KEY_END: // Also capital O
      activeDJ_->SongNext();
      break;
    case '[':
    case '{':
    case '<':
    case ',':
    case 'q':
    //case 'Q':
    case KEY_HOME: // also capital G
    case KEY_BACKSPACE: 
      activeDJ_->SongPrev();
      break;
    case KEY_RIGHT:
    case KEY_NUM_6:
      activeDJ_->MoveForward();
      break;
    case KEY_LEFT:
    case KEY_NUM_4:
      activeDJ_->MoveBackward();
      break;
    case '-':
    case KEY_PAGEDOWN: // also capital Q
      activeDJ_->VolumeDown();
      break;
    case '+':
    case '=':
    case KEY_PAGEUP: // also capital I
      activeDJ_->VolumeUp();
      break;
    case 'p':
    case 'P':
    case KEY_SPACE: // Pauses. Not another key afaik.
      activeDJ_->TogglePause();
      break;
    case 'd':
    case 'D':
      showDebug_ = !showDebug_;
      break;
    case 'u':
    case 'U':
    case 'i':
      activeDJ_->ToggleRequestUIActive();
      break;
    case '0': // Make it so the UI is requested.
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
}