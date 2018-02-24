//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include "DJConfig.hpp"
#include "DJ.hpp"
#include <string>
#include <ArgParser/ArgParser.hpp>
#include <ASCIIMenus/menu-system.hpp>
#include <ConsoleInput/console-input.h>


namespace ASCIIPlayer
{
  class Lobby
  {
  public:
    // Constructor and Destructor
    Lobby(int argc, char** argv);
    ~Lobby();

    // Member Functions
    void Run(); 

  private:
    // Private methods
    std::string cleanCommand(std::string input);
    DJConfig readConfigFile();
    void displayIdle(long long curr_frametime, long long last_frametime);
    void displayExtraMenus();
    void interpretPath(const std::string str);
    void interpretChar(char key);
    void startDJ();
    void stopDj();
    void setDJ(DJ *newDJ);
    bool menuMoveCheckRight();
    bool menuMoveCheckLeft();

    // Variables
    InputParser keyParser_;
    ArgParser argParser_;
    DJ *activeDJ_;
    MenuSystem menuSystems_;
    bool lobbyHosting_;
    bool menuVisible_;
    double idleIndex_; 

    // Debug Related
    int averageFPS(long long start, long long end);
    size_t timesIndex_;
    long long times_[200];
    bool showDebug_;
    long long fpsStart_;
    long long fpsPrevStart_;
    long long fpsEnd_;
    long long appStartTime_;
  };
}