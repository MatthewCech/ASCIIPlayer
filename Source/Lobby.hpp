//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include "DJConfig.hpp"
#include "DJ.hpp"
#include <string>
#include <ArgParser/ArgParser.hpp>



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
    bool ParseCommand(std::string command); // Returns if it was a valid command after executing. If not, display error message.
    
  private:
    // Private methods
    std::string cleanCommand(std::string input);
    DJConfig readConfigFile();
    bool interpretString(const std::string str);
    bool interpretChar(const char key);
    void startDJ();
    void stopDj();
    void setDJ(DJ *newDJ);

    // Variables
    ArgParser argParser_;
    DJ *activeDJ_;
    bool lobbyHosting_;

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