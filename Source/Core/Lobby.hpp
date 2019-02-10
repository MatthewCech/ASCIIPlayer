//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include <string>
#include <ArgParser/ArgParser.hpp>  
#include <ASCIIMenus/menu-system.hpp>
#include <ConsoleInput/console-input.hpp>
#include "DJConfig.hpp"
#include "DJ.hpp"


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
    void displayIdle(long long curr_frametime, long long last_frametime);
    void displayExtraMenus();
    void interpretMultiCharInput(const std::string str);
    void interpretChar(char key);
    
    // Private methods - Utility and Management
    int averageFPS(long long start, long long end);
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
    long long times_[200];
    bool showDebug_;
    long long fpsStart_;
    long long fpsPrevStart_;
    long long fpsEnd_;
    long long appStartTime_;
  };
}
