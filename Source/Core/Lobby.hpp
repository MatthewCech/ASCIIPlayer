//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include <string>
#include <ArgParser/ArgParser.hpp>  
#include <ASCIIMenus/menu-system.hpp>
#include <ConsoleInput/console-input.hpp>
#include "DJConfig.hpp"
#include "DJ.hpp"

namespace shoom
{
  class Shm;
}

namespace ASCIIPlayer
{
  class Lobby
  {
  public:
    // Constructor and Destructor
    Lobby(int argc, char** argv);
    ~Lobby();

    // Public Member Functions
    void Run(); 

  private:
    // Private methods - Parsing and Overlays
    void displayIdle(std::int64_t curr_frametime, std::int64_t last_frametime);
    void displayExtraMenus();
    void interpretMultiCharInput(const std::string str);
    void interpretChar(char key);
    void HandleApplicationOpen(int argc, char** argv);

    // Private methods - Utility and Management
    int averageFPS(std::int64_t start, std::int64_t end);
    void startDJ();
    void stopDj();
    void setDJ(DJ *newDJ);
    bool menuMoveCheckRight();
    bool menuMoveCheckLeft();

    // Core Variables
    InputParser keyParser_;
    ArgParser argParser_;
    DJ *activeDJ_;
    MenuSystem menuSystems_;
    bool lobbyHosting_;
    bool menuVisible_;

    // Debug or Idle Related
    double idleIndex_; 
    size_t timesIndex_;
    std::int64_t times_[200];
    bool showDebug_;
    std::int64_t fpsStart_;
    std::int64_t fpsPrevStart_;
    std::int64_t fpsEnd_;
    std::int64_t appStartTime_;

    // Shared memory locations
    shoom::Shm *sharedStatus;
    shoom::Shm *sharedArguments;
  };
}
