//The main program loop, for configuring the DJ. Low-usage blocking IO.
#pragma once
#include <string>
#include <ArgParser/ArgParser.hpp>  
#include <ConsoleInput/console-input.hpp>
#include "DJConfig.hpp"
#include "DJ.hpp"
#include "MenuSystem.hpp"

// Menu No-option or relative nav
#define ASCIIMENU_NO_CHANGE ""
#define ASCIIMENU_BACK "back"

// Menu heiarchy overview and defines
#define ASCIIMENU_BASE "menuDefualt"
  #define ASCIIMENU_FILE "menuFile"
  #define ASCIIMENU_EDIT "menuEdit"
    #define ASCIIMENU_SELECT_VISUALIZER "menuVisualizerSelection"
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
  DIALOG_CONFIG,
  DIALOG_VISUALIZER_LIST
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
    int calculateUpdateRate(double dt);

    // Private methods - Menus
    void drawDebug(double dt);
    void buildMenus();
    void displayInfobox(size_t maxWidth, std::string containerName, std::string str);
    void displayVisualizerList();
    Rect drawCenteredBox(size_t width, size_t height, size_t margin_height = 2, size_t margin_width = 2, RConsole::Color color = RConsole::WHITE);
    bool menuMoveCheckRight();
    bool menuMoveCheckLeft();
    void drawSplash(double dt);
    void drawDialog();
    void showMenu(DialogType dialog_type);

    // Menu associated
    void callback_close();
    void callback_resetConfig();
    void callback_goBack();
    void callback_hideDialog();
    void callback_forceClear();
    void callback_visualizerNext();
    void callback_visualizerPrev();
    void callback_visualizerSet(int arg);
    void callback_showMenu(int arg);

    // Core Variables
    InputParser keyParser_;
    ArgParser argParser_;
    DJ *activeDJ_;
    DJConfig activeDJConfig_;
    MenuSystem menuSystems_;
    bool lobbyHosting_;
    bool menuVisible_;

    // Menu/Visual Related
    double idleIndex_;
    int timesIndex_;
    int timesSoFar_;
    double dtHistory_[TRACKED_TIMES];
    bool showDebug_;
    std::chrono::steady_clock::time_point frameStart_;
    std::chrono::steady_clock::time_point previousFrameStart_;
    std::chrono::steady_clock::time_point frameEnd_;
    std::chrono::steady_clock::time_point appStartTime_;

    // Menu management
    bool menuNavBackNextUpdate_;   // For exiting the menu
    bool isDisplayingDialog_;      // For displaying the help menu.
    DialogType displayDialogType_; // For indicating what type of dialog we're displaying.

    // Shared memory locations
    shoom::Shm *sharedStatus;
    shoom::Shm *sharedArguments;
  };
}
