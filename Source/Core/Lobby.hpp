//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include <string>
#include <ArgParser/ArgParser.hpp>  
#include <ASCIIMenus/menu-system.hpp>
#include <ConsoleInput/console-input.hpp>
#include "DJConfig.hpp"
#include "DJ.hpp"

// Number of frame time entries tracked
#define TRACKED_TIMES 200

// Menu No-option
#define ASCIIMENU_NO_CHANGE ""

// Menu heiarchy overview and defines
#define ASCIIMENU_BASE "menuDefualt"
  #define ASCIIMENU_FILE "menuFile"
  #define ASCIIMENU_EDIT "menuEdit"
    #define ASCIIMENU_VISUALIZER "menuVisualizerSelection"
  #define ASCIIMENU_HELP "menuHelp"
    #define ASCIIMENU_HELP_INFO_BOX "menuHelpInfoBox"

// Shared memory class defintiion
namespace shoom
{
  class Shm;
}

// Information on the type of dialog we're using
enum class DialogType
{
  NONE,
  HELP_GENERAL,
  HELP_COMMANDS,
  HELP_CONFIG,
  DIALOG_OPEN,
  DIALOG_CONFIG
};

struct Rect
{
  int left;
  int right;
  int top;
  int bottom;

  int leftPadded;
  int rightPadded;
  int topPadded;
  int bottomPadded;
};

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

    // NOTE(mcech): Refactor the menu system so we can capture a class with the menu lambdas
    // Static local variables for this class only.
    static bool __menu_navigate_back_next_update; // For exiting the menu
    static bool __is_displaying_dialog;        // For displaying the help menu.
    static DJ* __current_dj;                      // A pointer to the current DJ exposed.
    static DialogType __dialog_type;            // The current type of dialog being displayed

  private:
    // Private methods
    void interpretMultiCharInput(const std::string str);
    void interpretChar(char key);
    void handleApplicationOpen(int argc, char** argv);
    void startDJ();
    void stopDj();
    void setDJ(DJ *newDJ);
    int calculateUpdateRate(std::int64_t start, std::int64_t end);

    // Private methods - Menus
    void drawDebug();
    void buildMenus();
    void displayInfobox(size_t maxWidth, std::string containerName, std::string str);
    Rect drawCenteredBox(size_t width, size_t height, size_t margin_height = 2, size_t margin_width = 2, RConsole::Color color = RConsole::WHITE);
    bool menuMoveCheckRight();
    bool menuMoveCheckLeft();
    void drawSplash(std::int64_t curr_frametime, std::int64_t last_frametime);
    void drawDialog();

    // Core Variables
    InputParser keyParser_;
    ArgParser argParser_;
    DJ *activeDJ_;
    MenuSystem menuSystems_;
    bool lobbyHosting_;
    bool menuVisible_;

    // Menu/Visual Related
    double idleIndex_; 
    size_t timesIndex_;
    std::int64_t times_[TRACKED_TIMES];
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
