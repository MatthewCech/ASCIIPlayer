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
    void Run(); // Has while loop and STD::Cin every loop to ensure it's running correctly. Loop sits until DJ starts with proper command.
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
  };
}