#include <thread>
#include <exception>
#include "Lobby.hpp"
#include "UserStrings.hpp"

// Menu heiarchy overview and defines
#define ASCIIMENU_BASE "menuDefualt"
  #define ASCIIMENU_FILE "menuFile"
  #define ASCIIMENU_EDIT "menuEdit"
    #define ASCIIMENU_VISUALIZER "menuVisualizerSelection"
  #define ASCIIMENU_HELP "menuHelp"
    #define ASCIIMENU_HELP_INFO_BOX "menuHelpInfoBox"


namespace ASCIIPlayer
{
    ///////////////////////////
   // File-specific Globals //
  ///////////////////////////
  // TODO(mcech): Make this entire section not a thing.
  //
  // ...not a brilliant solution, should use member function callback support.
  // Do not keep adding to this, just rework the menu system to include member functions,
  // like it honestly should do anyways.
  //

  // Static local variables for this file only.
  static bool __menu_navigate_back_next_update = false; // For exiting the menu
  static bool __is_displaying_help_menu = false;        // For displaying the help menu.
  static DJ* __current_dj = nullptr;                    // A pointer to the current DJ exposed.

  // Enums
  static enum WhichDialogueEnum 
  { 
    GENERAL, 
    CONFIG,
    OPEN
  } __dialogue_type;

  // Global popups using drawing system
  static void __Display_Infobox(size_t maxWidth, std::string containerName, std::string str)
  {
    // Define widths
    const unsigned int half_w             = static_cast<unsigned int>(maxWidth / 2);
    const unsigned int str_width          = static_cast<unsigned int>(maxWidth - 3);
    const unsigned int str_height         = static_cast<int>(str.size() / static_cast<float>(str_width) + 0.99);
    const unsigned int half_h             = (str_height + 5) / 2;
    const unsigned int half_screen_width  = RConsole::Canvas::GetConsoleWidth() / 2;
    const unsigned int half_screen_height = RConsole::Canvas::GetConsoleHeight() / 2;

    // Side calculations
    const float left   = static_cast<float>(half_screen_width - half_w);
    const float right  = static_cast<float>(half_screen_width + half_w);
    const float top    = static_cast<float>(half_screen_height - half_h) - 1;
    const float bottom = static_cast<float>(half_screen_height + half_h);

    // Move menu options accordingly
    Container *c = MenuRegistry::GetContainer(containerName);
    c->SetPosition(static_cast<size_t>(right - c->GetSelected().Label.size() - 1)
      , static_cast<size_t>(bottom - 2));

    // Draw box background
    for (int i = static_cast<int>(left); i < right; ++i)
      for (int j = static_cast<int>(top); j < bottom; ++j)
        RConsole::Canvas::Draw(' ', i, j, RConsole::DARKGREY);

    // Draw the string message
    unsigned int offset = 0;
    unsigned int cycles = 0;
    while (offset < str.size())
    {
      if (*(str.c_str() + offset) == ' ')
        offset += 1;

      RConsole::Canvas::DrawString(std::string(str.c_str() + offset, str_width).c_str()
        , static_cast<int>(left + 2)
        , static_cast<int>(top + 2 + cycles)
        , RConsole::WHITE);

      offset += str_width;
      ++cycles;
    }

    // Draw sides of the box and add corners
    for (int i = static_cast<int>(left); i < right; ++i)
    {
      RConsole::Canvas::Draw(static_cast<unsigned char>(205), i, static_cast<int>(top), RConsole::WHITE);
      RConsole::Canvas::Draw(static_cast<unsigned char>(205), i, static_cast<int>(bottom), RConsole::WHITE);
    }
    for (int i = static_cast<int>(top); i < bottom; ++i)
    {
      RConsole::Canvas::Draw(static_cast<unsigned char>(186), static_cast<int>(left), i, RConsole::WHITE);
      RConsole::Canvas::Draw(static_cast<unsigned char>(186), static_cast<int>(right), i, RConsole::WHITE);
    }

    // Draw corners
    RConsole::Canvas::Draw(static_cast<unsigned char>(187), right, top, RConsole::WHITE);
    RConsole::Canvas::Draw(static_cast<unsigned char>(188), right, bottom, RConsole::WHITE);
    RConsole::Canvas::Draw(static_cast<unsigned char>(201), left, top, RConsole::WHITE);
    RConsole::Canvas::Draw(static_cast<unsigned char>(200), left, bottom, RConsole::WHITE); 
  }



    ////////////
   // Public //
  ////////////
  // Constructor and Destructor
  Lobby::Lobby(int argc, char** argv)
    
    // Operation related
    : keyParser_()
    , argParser_(argc, argv)
    , activeDJ_(nullptr)
    , menuSystems_("")
    , lobbyHosting_(true)
    , menuVisible_(false)
    , idleIndex_(0)

    // Debug
    , timesIndex_(0)
    , times_()
    , showDebug_(false)
    , fpsStart_(0)
    , fpsPrevStart_(0)
    , fpsEnd_(0)
    , appStartTime_(MS_SINCE_EPOCH)
  { 
    // Make DJ, don't autoplay. Read in the argument!
    DJ *Dj = new DJ(DJConfig::Read(argParser_[0]), false);

    // Just loop through and see if anything happens to be a song to load.
    std::vector<std::string> args = argParser_.GetAllArgs();
    for (unsigned int i = 0; i < args.size(); ++i)
    {
      AudioFile *test1 = new ASCIIPlayer::AudioFile(args[i]);
      Dj->AddSong(test1);
    }

    // Set activeDJ to our DJ
    activeDJ_ = Dj;

    // Configure menus
    Container *mainMenu = Container::Create(ASCIIMENU_BASE);
    mainMenu->SetOrientation(ASCIIMenus::HORIZONTAL);
    mainMenu->AddItem(" File  ", ASCIIMENU_FILE);
    mainMenu->AddItem(" Edit  ", ASCIIMENU_EDIT);
    mainMenu->AddItem(" Help  ", ASCIIMENU_HELP);

    Container *fileMenu = Container::Create(ASCIIMENU_FILE);
    fileMenu->SetOrientation(ASCIIMenus::VERTICAL);
    fileMenu->SetPosition(2, 1);
    fileMenu->AddItem("Open", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = OPEN; });
    fileMenu->AddItem("Save Settings", "");
    fileMenu->AddItem("Info", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = GENERAL; });
    fileMenu->AddItem("Hide", "", []() { __menu_navigate_back_next_update = true; }); // TODO(mcech): Convert to template 
    fileMenu->AddItem("Quit", "", []() { exit(0); });                                 // TODO(mcech): Confirmation of Destructive Action - Dialogue that takes lambda for yes.

    Container *editMenu = Container::Create(ASCIIMENU_EDIT);
    editMenu->SetOrientation(ASCIIMenus::VERTICAL);
    editMenu->SetPosition(9, 1);

    // TODO(mcech): Templatize the AddItem function in order to allow it to access member function content!
    // Why? Well, this will let this function use 'this' as a capture group, and from there the
    // contents of this class can be used! This will be awesome for live-reloading ASCIIPlayer
    // if the config file changed.
    editMenu->AddItem("Edit Config", "", []() { system(".\\ASCIIPlayer.conf"); }); 
    // TODO(mcech): editMenu->AddItem("Reset Config", "");//, []() { system("del .\\ASCIIPlayer.conf"); }); // Deletes / resets existing config.
    // TODO(mcech): editMenu->AddItem("Set Visualizer", ASCIIMENU_VISUALIZER); // Lets you set the visualizer from a list of them!
    editMenu->AddItem("Next Visualizer", "", []() { if (__current_dj != nullptr) __current_dj->VisualizerNext(); });
    editMenu->AddItem("Prev Visualizer", "", []() { if (__current_dj != nullptr) __current_dj->VisualizerPrev(); });
    editMenu->AddItem("Force Clearscreen", "", []() { RConsole::Canvas::ForceClearEverything(); });

    Container *helpMenu = Container::Create(ASCIIMENU_HELP);
    helpMenu->SetOrientation(ASCIIMenus::VERTICAL);
    helpMenu->SetPosition(16, 1);
    helpMenu->AddItem("General Info", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = GENERAL; });
    helpMenu->AddItem("Config Info", ASCIIMENU_HELP_INFO_BOX,  []() { __is_displaying_help_menu = true; __dialogue_type = CONFIG; });

    Container *helpMenuPopup = Container::Create(ASCIIMENU_HELP_INFO_BOX);
    helpMenuPopup->SetOrientation(ASCIIMenus::HORIZONTAL);
    helpMenuPopup->AddItem("[ Ok ]", "back", []() { __is_displaying_help_menu = false; });

    // Set colors
    menuSystems_.SetColorSelected(RConsole::LIGHTCYAN);
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
      // TODO(mcech): Make this line not a thing.
      __current_dj = activeDJ_; 

      // Loop tracking
      fpsPrevStart_ = fpsStart_;
      fpsStart_ = MS_SINCE_EPOCH;

      // ============================ Start primary loop ============================

      // TODO(mcech): Make this line not a thing.
      if (__menu_navigate_back_next_update)
      {
        while (menuSystems_.Back()) {  }
        __menu_navigate_back_next_update = false;
        menuVisible_ = menuSystems_.IsVisible();
      }

      // Idle screen if necessary
      if (activeDJ_->GetPlaylistSize() == 0)
        displayIdle(fpsStart_, fpsPrevStart_);

      // Actively run DJ
      if (activeDJ_)
        activeDJ_->Update();

      // Parse input
      keyParser_.HandleInput(this, &Lobby::interpretChar, &Lobby::interpretMultiCharInput);

      // Finalize drawing for debug
      if (showDebug_)
      {
        float loc = 0;
        const size_t seconds = static_cast<size_t>((fpsStart_ - appStartTime_) / 1000);
        const size_t minutes = seconds / 60;
        const size_t hours = minutes / 60;

        RConsole::Canvas::DrawString(("[ argv0: " + argParser_[0]).c_str(), 0.0f, loc++, RConsole::DARKGREY);
        RConsole::Canvas::DrawString(("[ utime: " + std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m " + std::to_string(seconds % 60) + "s").c_str(), 0.0f, loc++, RConsole::DARKGREY);
        RConsole::Canvas::DrawString(("[ c/sec: " + std::to_string(averageFPS(fpsPrevStart_, fpsEnd_)) + " " + Strings::DEBUG_PER_SECOND).c_str(), 0.0f, loc++, RConsole::DARKGREY);
      }

      // Draw menus and finalize drawing for menu overlay.
      displayExtraMenus();
      menuSystems_.Draw(0, 0, true);

      // Write out and display all drawing
      RConsole::Canvas::Update();

      // ============================ End primary loop ============================
      fpsEnd_ = MS_SINCE_EPOCH;
      ++loops;
    }
  }


    /////////////
   // Private //
  /////////////
  // Displays a little bouncing image in the last 
  void Lobby::displayIdle(long long curr_frametime, long long last_frametime)
  {
    // Advance index value
    const double numIndexesPerSecond = 12.5;
    idleIndex_ += (static_cast<double>(curr_frametime + 1) - static_cast<double>(last_frametime)) / 1000 * numIndexesPerSecond;

    // Calculate index in array and mod value.
    const int osc[] = { '`', '*', '+', '_', ',', '.', '.', '.', '.', '.', '.', '.', ',', '/', '^' };
    const size_t index = static_cast<size_t>(idleIndex_);
    const size_t mod = (sizeof(osc) / sizeof(*osc));

    // Calcualte and wrap offsets for idle bar
    int verticalOffset = static_cast<int>(RConsole::Canvas::GetConsoleHeight() / 2 - 2);
    std::string msg = Strings::STARTUP_PRIMARY_TEXT + " ";
    RConsole::Canvas::DrawString((msg
      + static_cast<char>(osc[(index) % mod])
      + static_cast<char>(osc[(index + 1) % mod])
      + static_cast<char>(osc[(index + 2) % mod])
      + static_cast<char>(osc[(index + 3) % mod])
      + static_cast<char>(osc[(index + 4) % mod])).c_str()
      // Positioning and color...
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>((msg.size() + 5)) / 2)
      , verticalOffset++
      , RConsole::WHITE);

    std::string submsg = Strings::STARTUP_SUBTEXT_LINE_1;
    RConsole::Canvas::DrawString(submsg.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(submsg.size()) / 2)
      , ++verticalOffset
      , RConsole::WHITE);

    std::string escMenu = Strings::STARTUP_SUBTEXT_LINE_2;
    RConsole::Canvas::DrawString(escMenu.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(escMenu.size()) / 2)
      , ++verticalOffset
      , RConsole::WHITE);
  }


  // Displays additional menus
  void Lobby::displayExtraMenus()
  {
    if (__is_displaying_help_menu)
    {
      std::string message = Strings::MODAL_HELP_DEFAULT;

      if (__dialogue_type == GENERAL)
        message = Strings::MODAL_HELP_GENERAL;
      else if (__dialogue_type == CONFIG)
        message = Strings::MODAL_HELP_CONFIG;
      else if (__dialogue_type == OPEN)
        message = Strings::MODAL_HELP_OPEN;

      __Display_Infobox(40, ASCIIMENU_HELP_INFO_BOX, message);
    }
  }


  // Interpret specific paths
  void Lobby::interpretMultiCharInput(const std::string str)
  {
    std::string input = str;
    if (str[0] == '"' && str[str.size() - 1] == '"')
      input = str.substr(1, str.size() - 2);
    if (str[0] == '\\' && str[str.size() - 1] == '"')
      input = str.substr(2, str.size() - 4);

#pragma warning (disable: 4309)
    if (str[0] == static_cast<signed char>(224))
      interpretChar(str[1]);

    AudioFile *new_song = new ASCIIPlayer::AudioFile(input);
    activeDJ_->AddSong(new_song);
  }


  // Interprets a single-character piece of input
  void Lobby::interpretChar(char c)
  {
    // Ensure we have a DJ active before any character commands are parsed!
    if (activeDJ_ == nullptr)
      throw Strings::ERROR_INPUT_NO_DJ;

    switch (c)
    {
      // Menu Show/Hide
    case KEY_ESCAPE:
      //case KEY_ALT_LEFT:
      if (menuVisible_)
        menuSystems_.Back();
      else
        menuSystems_.Select(ASCIIMENU_BASE);

      menuVisible_ = menuSystems_.IsVisible();
      if (__is_displaying_help_menu)
        __is_displaying_help_menu = false;
      break;

      // Menu Navigation: Up/Left
    case 'a':
      if (menuVisible_)
        if (!menuMoveCheckLeft())
          menuSystems_.Up();
      break;
    case KEY_NUM_4:
    case KEY_LEFT:
      if (!menuVisible_)
        activeDJ_->MoveBackward();
      else
        if (!menuMoveCheckLeft())
          menuSystems_.Up();
      break;
    case 'w':
    case KEY_UP:
      if (menuVisible_)
        menuSystems_.Up();
      break;

      // Menu Navigation: Down/Right. Special case D to handle menu movement and debug.
    case KEY_NUM_6:
    case KEY_RIGHT:
      if (!menuVisible_)
        activeDJ_->MoveForward();
      else
        if (!menuMoveCheckRight())
          menuSystems_.Down();
      break;
    case 's':
    case KEY_DOWN:
      if (menuVisible_)
        menuSystems_.Down();
      break;
    case 'd':
      if (menuVisible_)
      {
        if (!menuMoveCheckRight())
          menuSystems_.Down();
      }
      else
        showDebug_ = !showDebug_;
      break;

      // Song Skipping
    case ']':
    case '}':
    case '>':
    case '.':
    case 'e':
    case KEY_TAB:
      activeDJ_->SongNext();
      break;
    case '[':
    case '{':
    case '<':
    case ',':
    case 'q':
    case KEY_BACKSPACE:
      activeDJ_->SongPrev();
      break;

      // Volume Adjustments
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
      activeDJ_->TogglePause();
      break;
    case KEY_SPACE: // Pauses. Not another key afaik.
      if (!menuVisible_)
        activeDJ_->TogglePause();
    case KEY_ENTER:
      if (menuVisible_)
      {
        menuSystems_.Select();
        menuVisible_ = menuSystems_.IsVisible();
      }
      break;

      // UI or Info
    case 'u':
    case 'U':
    case 'i':
      activeDJ_->ToggleRequestUIActive();
      break;
    case '0': // Make it so the UI is requested.

      // Visualizer stepping
    case 'v':
    case 'V':
      activeDJ_->VisualizerNext();

    default:
      return;
    }
  }
}
