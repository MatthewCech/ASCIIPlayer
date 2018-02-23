#include "Lobby.hpp"
#include <FileIO/FileIO.hpp>
#include <exception>
#include <thread>

// General Defines
#define MS_SINCE_EPOCH std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count()

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
  // Static local variables for help menu
  // ...not a brilliant solution, should use member function callback support.
  static bool __is_displaying_help_menu = false;
  static enum WhichDialogueEnum 
  { 
    GENERAL, 
    CONFIG 
  } __dialogue_type;

  // Global popups using drawing system
  static void DisplayInfobox(size_t maxWidth, std::string containerName, std::string str)
  {
    // Define widths
    const unsigned int half_w             = maxWidth / 2;
    const unsigned int str_width          = maxWidth - 3;
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

    // Configure menus
    Container *mainMenu = Container::Create(ASCIIMENU_BASE);
    mainMenu->SetOrientation(ASCIIMenus::HORIZONTAL);
    mainMenu->AddItem(" File  ", ASCIIMENU_FILE);
    mainMenu->AddItem(" Edit  ", ASCIIMENU_EDIT);
    mainMenu->AddItem(" Help  ", ASCIIMENU_HELP);

    Container *fileMenu = Container::Create(ASCIIMENU_FILE);
    fileMenu->SetOrientation(ASCIIMenus::VERTICAL);
    fileMenu->SetPosition(2, 1);
    fileMenu->AddItem("Open", "");
    fileMenu->AddItem("Save Settings", "");
    fileMenu->AddItem("Info", ""); // Provides some info on ASCIIPlayer!
    fileMenu->AddItem("Hide", "back");
    fileMenu->AddItem("Quit", "", []() { exit(0); });

    Container *editMenu = Container::Create(ASCIIMENU_EDIT);
    editMenu->SetOrientation(ASCIIMenus::VERTICAL);
    editMenu->SetPosition(9, 1);
    editMenu->AddItem("Edit Config", "", []() { system(".\\ASCIIPlayer.conf"); });
    editMenu->AddItem("Reset Config", "");
    editMenu->AddItem("Set Visualizer", ASCIIMENU_VISUALIZER); 

    Container *helpMenu = Container::Create(ASCIIMENU_HELP);
    helpMenu->SetOrientation(ASCIIMenus::VERTICAL);
    helpMenu->SetPosition(16, 1);
    helpMenu->AddItem("General Info", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = GENERAL; });
    helpMenu->AddItem("Config Info", ASCIIMENU_HELP_INFO_BOX,  []() { __is_displaying_help_menu = true; __dialogue_type = CONFIG; });

    Container *helpMenuPopup = Container::Create(ASCIIMENU_HELP_INFO_BOX);
    helpMenuPopup->SetOrientation(ASCIIMenus::HORIZONTAL);
    helpMenuPopup->AddItem("[ Ok ]", "back", []() { __is_displaying_help_menu = false; });
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
        displayIdle(fpsStart_, fpsPrevStart_);

      // Actively run DJ
      if (activeDJ_)
        activeDJ_->Update();

      // Parse input
      keyParser_.HandleInput(this, &Lobby::interpretChar, &Lobby::interpretPath);

      // Finalize drawing for debug
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

      // Draw menus and finalize drawing for menu overlay.
      displayExtraMenus();
      menuSystems_.Draw(0, 0, true);

      // Write out and display all drawing
      RConsole::Canvas::Update();

      // Smol sleep. This makes most OSs extremely happy and reduces CPU load by like 30%.
      std::this_thread::sleep_for(std::chrono::microseconds(500)); 


      // ============================ End primary loop ============================
      fpsEnd_ = MS_SINCE_EPOCH;
      ++loops;
    }
  }


  // Parse commands as we see fit. Assumes command has been consumed already.
  void Lobby::ParseCommand(std::string command)
  {
    command = cleanCommand(command);
    
    if (command.size() == 0)
      return;

    interpretString(command);
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

  void Lobby::interpretPath(const std::string str)
  {
    std::string input = str;
    if (str[0] == '"' && str[str.size() - 1] == '"')
      input = str.substr(1, str.size() - 2);
    if (str[0] == '\\' && str[str.size() - 1] == '"')
      input = str.substr(2, str.size() - 4);

    AudioFile *test1 = new ASCIIPlayer::AudioFile(input);
    activeDJ_->AddSong(test1);
  }

  void Lobby::interpretString(const std::string command)
  {
    if (command.size() <= 0)
      return;
  }


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
      if(menuVisible_)
        if (!menuMoveCheckLeft())
          menuSystems_.Up();
      break;
    case KEY_NUM_4:
    case KEY_LEFT:
      if(!menuVisible_)
        activeDJ_->MoveBackward();
      else
        if(!menuMoveCheckLeft())
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
      if(!menuVisible_)
        activeDJ_->MoveForward();
      else
        if(!menuMoveCheckRight())
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
      if(!menuVisible_)
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
    default:
      return;
    }
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
    std::string msg = "Waiting for songs ";
    RConsole::Canvas::DrawString((msg
      + static_cast<char>(osc[(index) % mod])
      + static_cast<char>(osc[(index + 1) % mod])
      + static_cast<char>(osc[(index + 2) % mod])
      + static_cast<char>(osc[(index + 3) % mod])
      + static_cast<char>(osc[(index + 4) % mod])).c_str()
      // Positoning and color...
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - ((msg.size() + 5) / 2)
      , static_cast<int>(RConsole::Canvas::GetConsoleHeight() / 2 - 2)
      , RConsole::WHITE);

    std::string submsg = "(press ESC for menu)";
    RConsole::Canvas::DrawString(submsg.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (submsg.size() / 2)
      , static_cast<int>(RConsole::Canvas::GetConsoleHeight() / 2));
  }


  // Displays additional menus
  void Lobby::displayExtraMenus()
  {
    if (__is_displaying_help_menu)
    {
      std::string message = "Hmm, no message was set for this menu. You shouldn't see this!";

      if (__dialogue_type == GENERAL)
        message = "ASCIIPlayer is a command-line program navigated exclusively by keyboard. If you want to play a song, you can drag it in to add it to the playlist, or select multiple and pass them as command line aguments!";
      else if (__dialogue_type == CONFIG)
        message = "Not all configurations can be edited by the menu. ASCIIPlayer.conf is located next to the program executable, and can be opened with your default text editor via the menu 'EDIT' menu or by hand in the program folder.";

      DisplayInfobox(40, ASCIIMENU_HELP_INFO_BOX, message);

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

  // Move to the right in the menu
  bool Lobby::menuMoveCheckRight()
  {
    if (menuSystems_.MenuDepth() == 2)
    {
      menuSystems_.Back();
      menuSystems_.Down();
      menuSystems_.Select();
      return true;
    }
    return false;
  }

  // Move to the left in the menu, wrapps the actions assoicated with it
  bool Lobby::menuMoveCheckLeft()
  {
    if (menuSystems_.MenuDepth() == 2)
    {
      menuSystems_.Back();
      menuSystems_.Up();
      menuSystems_.Select();
      return true;
    }
    return false;
  }

  // Interprets a single-character piece of input
  void Lobby::interpretChar(char c)
  {
    if (activeDJ_ == nullptr)
      throw "You should have an active DJ to issue character commands";

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
      break;


      // Menu Navigation: Up/Left
    case KEY_NUM_4:
    case KEY_LEFT:
    case 'a':
      if (!menuVisible_)
        activeDJ_->MoveBackward();
      else
        if (!menuMoveCheckLeft())
          menuSystems_.Up();
      break;
    case 'w':
      if (menuVisible_)
        menuSystems_.Up();
      break;


      // Menu Navigation: Down/Right. Special case D to handle menu movement and debug.
    case KEY_NUM_6:
    case KEY_RIGHT:
      if (!menuVisible_)
        activeDJ_->MoveForward();
      else
      {
        if (!menuMoveCheckRight())
          menuSystems_.Down();
      }
    case 's':
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
    default:
      return;
    }
  }
}
