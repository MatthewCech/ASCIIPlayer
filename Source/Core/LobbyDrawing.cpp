#include "Lobby.hpp"
#include "UserStrings.hpp"


namespace ASCIIPlayer
{
  // Static initialization
  bool Lobby::__menu_navigate_back_next_update = false; // For exiting the menu
  bool Lobby::__is_displaying_help_menu = false;        // For displaying the help menu.
  DJ* Lobby::__current_dj = nullptr;                    // A pointer to the current DJ exposed.
  DialogType Lobby::__dialogue_type = DialogType::NONE; // For indicating what type of dialog we're displaying.


  // Creats and configures the menus we can display in the lobby.
  void Lobby::buildMenus()
  {
    // Configure menus
    Container* mainMenu = Container::Create(ASCIIMENU_BASE);
    mainMenu->SetOrientation(ASCIIMenus::HORIZONTAL);
    mainMenu->AddItem(" File  ", ASCIIMENU_FILE);
    mainMenu->AddItem(" Edit  ", ASCIIMENU_EDIT);
    mainMenu->AddItem(" Help  ", ASCIIMENU_HELP);

    Container* fileMenu = Container::Create(ASCIIMENU_FILE);
    fileMenu->SetOrientation(ASCIIMenus::VERTICAL);
    fileMenu->SetPosition(2, 1);
    fileMenu->AddItem("Open", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = DialogType::OPEN; });
    fileMenu->AddItem("Save Settings", ASCIIMENU_NO_CHANGE, []() {}); // TODO(mcech): Allow settings to save off to config
    fileMenu->AddItem("Info", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = DialogType::GENERAL; });
    fileMenu->AddItem("Hide", ASCIIMENU_NO_CHANGE, []() { __menu_navigate_back_next_update = true; });
    fileMenu->AddItem("Quit", ASCIIMENU_NO_CHANGE, []() { exit(0); }); // TODO(mcech): Confirmation of Destructive Action - Dialogue that takes lambda for yes.

    Container* editMenu = Container::Create(ASCIIMENU_EDIT);
    editMenu->SetOrientation(ASCIIMenus::VERTICAL);
    editMenu->SetPosition(9, 1);

    // TODO(mcech): Templatize the AddItem function in order to allow it to access member function content!
    // Why? Well, this will let this function use 'this' as a capture group, and from there the
    // contents of this class can be used! This will be awesome for live-reloading ASCIIPlayer
    // if the config file changed.
    editMenu->AddItem("Edit Config", ASCIIMENU_NO_CHANGE, []() { system(".\\ASCIIPlayer.conf"); });
    editMenu->AddItem("Reset Config", ASCIIMENU_NO_CHANGE, []() { system("del .\\ASCIIPlayer.conf"); });
    //TODO(mcech): editMenu->AddItem("Set Visualizer", ASCIIMENU_VISUALIZER, []() {  }); // Lets you set the visualizer from a list of them!
    editMenu->AddItem("Next Visualizer", ASCIIMENU_NO_CHANGE, []() { if (__current_dj != nullptr) __current_dj->VisualizerNext(); });
    editMenu->AddItem("Prev Visualizer", ASCIIMENU_NO_CHANGE, []() { if (__current_dj != nullptr) __current_dj->VisualizerPrev(); });
    editMenu->AddItem("Force Clearscreen", ASCIIMENU_NO_CHANGE, []() { RConsole::Canvas::ForceClearEverything(); });

    Container* helpMenu = Container::Create(ASCIIMENU_HELP);
    helpMenu->SetOrientation(ASCIIMenus::VERTICAL);
    helpMenu->SetPosition(16, 1);
    helpMenu->AddItem("About", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = DialogType::GENERAL; });
    helpMenu->AddItem("Keyboard Commands", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = DialogType::COMMANDS; });
    helpMenu->AddItem("Config Info", ASCIIMENU_HELP_INFO_BOX, []() { __is_displaying_help_menu = true; __dialogue_type = DialogType::CONFIG; });

    Container* helpMenuPopup = Container::Create(ASCIIMENU_HELP_INFO_BOX);
    helpMenuPopup->SetOrientation(ASCIIMenus::HORIZONTAL);
    helpMenuPopup->AddItem("[ Ok ]", "back", []() { __is_displaying_help_menu = false; });

    // Set colors
    menuSystems_.SetColorSelected(RConsole::LIGHTCYAN);
  }


  // Displays a splash screen with little bouncing image in the center of the console and some idle text.
  void Lobby::drawSplash(std::int64_t curr_frametime, std::int64_t last_frametime)
  {
    // Advance index value
    const double numIndexesPerSecond = 18; // fps of this little bounce graphic
    std::int64_t difference = (curr_frametime)-last_frametime;
    idleIndex_ += static_cast<int>(difference) / 1000.0f * numIndexesPerSecond;

    // Calculate index in array and mod value.
    const int osc[] = { '.', ',', '/', '^', '`', '*', '+', '_', ',', '.', '.', '.', '.', '.', '.' };
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


  // Displays a debug overlay on the player
  void Lobby::drawDebug()
  {
    float loc = 0;
    const size_t seconds = static_cast<size_t>((fpsStart_ - appStartTime_) / 1000);
    const size_t minutes = seconds / 60;
    const size_t hours = minutes / 60;
    const RConsole::Color color = RConsole::DARKGREY;

    RConsole::Canvas::DrawString(("[ argv0: " + argParser_[0]).c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ utime: " + std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m " + std::to_string(seconds % 60) + "s").c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ c/sec: " + std::to_string(calculateUpdateRate(fpsPrevStart_, fpsEnd_)) + " " + Strings::DEBUG_PER_SECOND).c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ vname: " + activeDJ_->VisualizerName()).c_str(), 0.0f, loc++, color);
  }


  // Displays additional menus with the specified string
  void Lobby::drawExtraMenus()
  {
    if (__is_displaying_help_menu)
    {
      std::string message = Strings::MODAL_HELP_DEFAULT;

      if (__dialogue_type == DialogType::GENERAL)
        message = Strings::MODAL_HELP_GENERAL;
      else if (__dialogue_type == DialogType::COMMANDS)
        message = Strings::MODAL_HELP_COMMANDS;
      else if (__dialogue_type == DialogType::CONFIG)
        message = Strings::MODAL_HELP_CONFIG;
      else if (__dialogue_type == DialogType::OPEN)
        message = Strings::MODAL_HELP_OPEN;

      displayInfobox(40, ASCIIMENU_HELP_INFO_BOX, message);
    }
  }

  // Global popups using drawing system
  void Lobby::displayInfobox(size_t maxWidth, std::string containerName, std::string str, size_t stringBuffer)
  {
    // Determine two potential heights...
    // Note that when we make str_height later, we check 
    // to see if there were any newlines and favor them.
    int newlines = 0;
    int longest = 0;
    int current = 0;

    for (char c : str)
    {
      if (++current > longest)
        ++longest;

      if (c == '\n')
      {
        ++newlines;
        current = 0;
      }
    }

    const bool manualFormat = newlines != 0;

    if (manualFormat)
    {
      if (longest + stringBuffer < maxWidth)
        maxWidth = longest + stringBuffer;
    }

    // Define sizes
    const unsigned int half_w = static_cast<unsigned int>(maxWidth / 2);
    const unsigned int str_width = static_cast<unsigned int>(maxWidth - stringBuffer);
    const unsigned int str_height = manualFormat ? newlines + 1 : static_cast<int>(str.size() / static_cast<float>(str_width) + 0.99);
    const unsigned int half_h = (str_height + 5) / 2;
    const unsigned int half_screen_width = RConsole::Canvas::GetConsoleWidth() / 2;
    const unsigned int half_screen_height = RConsole::Canvas::GetConsoleHeight() / 2;

    // Side calculations
    const float left = static_cast<float>(half_screen_width - half_w);
    const float right = static_cast<float>(half_screen_width + half_w);
    const float top = static_cast<float>(half_screen_height - half_h) - 1;
    const float bottom = static_cast<float>(half_screen_height + half_h);

    // Move menu options accordingly
    Container* c = MenuRegistry::GetContainer(containerName);
    c->SetPosition(static_cast<size_t>(right - c->GetSelected().Label.size() - 1), static_cast<size_t>(bottom - 2));

    // Draw box background
    for (int i = static_cast<int>(left); i < right; ++i)
    {
      for (int j = static_cast<int>(top); j < bottom; ++j)
        RConsole::Canvas::Draw(' ', i, j, RConsole::DARKGREY);
    }

    // Draw the string message
    unsigned int offset = 0;
    unsigned int cycles = 0;
    while (offset < str.size())
    {
      if (!manualFormat)
      {
        if (*(str.c_str() + offset) == ' ')
          offset += 1;
      }

      std::string toWrite = std::string(str.c_str() + offset, str_width);
      size_t loc = toWrite.find_first_of('\n');
      if (loc != std::string::npos)
      {
        toWrite = toWrite.substr(0, loc);
        ++offset; // Accounts for newlines
      }

      RConsole::Canvas::DrawString(toWrite.c_str()
        , static_cast<int>(left + 2)
        , static_cast<int>(top + 2 + cycles)
        , RConsole::WHITE);

      offset += toWrite.length();// str_width;
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
}