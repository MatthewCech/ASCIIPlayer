#include <sstream>
#include "Lobby.hpp"
#include "UserStrings.hpp"



namespace ASCIIPlayer
{
  // Given a type of dialog, configure the menus to show.
  void Lobby::showMenu(DialogType dialog_type)
  {
    isDisplayingDialog_ = true;
    displayDialogType_ = dialog_type;
  }

  // Menu callbacks
  void Lobby::callback_close() { exit(0); }
  void Lobby::callback_resetConfig() { }
  void Lobby::callback_goBack() { menuNavBackNextUpdate_ = true; }
  void Lobby::callback_forceClear() { RConsole::Canvas::ForceClearEverything(); }
  void Lobby::callback_hideDialog() { isDisplayingDialog_ = false; }
  void Lobby::callback_visualizerNext() { if (activeDJ_ != nullptr) activeDJ_->VisualizerNext(); }
  void Lobby::callback_visualizerPrev() { if (activeDJ_ != nullptr) activeDJ_->VisualizerPrev(); }
  void Lobby::callback_visualizerSet(int arg) { if (activeDJ_ != nullptr) activeDJ_->VisualizerSet(activeDJ_->GetVisualizerList()[arg].Name); }
  void Lobby::callback_showMenu(int arg) { showMenu(static_cast<DialogType>(arg)); }

  // Creats and configures the menus we can display in the lobby.
  void Lobby::buildMenus()
  {
    // Configure menus
    Container* mainMenu = Container::Create(this, ASCIIMENU_BASE);
    mainMenu->SetOrientation(ASCIIMenus::HORIZONTAL);
    mainMenu->AddItem(" File  ", ASCIIMENU_FILE);
    mainMenu->AddItem(" Edit  ", ASCIIMENU_EDIT);
    mainMenu->AddItem(" Help  ", ASCIIMENU_HELP);

    Container* fileMenu = Container::Create(this, ASCIIMENU_FILE);
    fileMenu->SetOrientation(ASCIIMenus::VERTICAL);
    fileMenu->SetPosition(2, 1);
    fileMenu->AddItem("Open", ASCIIMENU_HELP_INFO_BOX, &Lobby::callback_showMenu, static_cast<int>(DialogType::DIALOG_OPEN));
    //fileMenu->AddItem("Save Settings", ASCIIMENU_NO_CHANGE, []() {}); // TODO(mcech): Allow settings to save off to config
    fileMenu->AddItem("Hide", ASCIIMENU_NO_CHANGE, &Lobby::callback_goBack);
    fileMenu->AddItem("Quit", ASCIIMENU_NO_CHANGE, &Lobby::callback_close); // TODO(mcech): Confirmation of Destructive Action - Dialogue that takes lambda for yes.

    Container* editMenu = Container::Create(this, ASCIIMENU_EDIT);
    editMenu->SetOrientation(ASCIIMenus::VERTICAL);
    editMenu->SetPosition(9, 1);

    editMenu->AddItem("Edit Config", ASCIIMENU_NO_CHANGE, &Lobby::callback_showMenu, static_cast<int>(DialogType::DIALOG_CONFIG));
    editMenu->AddItem("Reset Config", ASCIIMENU_NO_CHANGE, &Lobby::callback_resetConfig);
    editMenu->AddItem("Set Visualizer", ASCIIMENU_SELECT_VISUALIZER, &Lobby::callback_showMenu, static_cast<int>(DialogType::DIALOG_VISUALIZER_LIST));
    editMenu->AddItem("Next Visualizer", ASCIIMENU_NO_CHANGE, &Lobby::callback_visualizerNext);
    editMenu->AddItem("Prev Visualizer", ASCIIMENU_NO_CHANGE, &Lobby::callback_visualizerPrev);
    editMenu->AddItem("Force Clearscreen", ASCIIMENU_NO_CHANGE, &Lobby::callback_forceClear);

    Container* helpMenu = Container::Create(this, ASCIIMENU_HELP);
    helpMenu->SetOrientation(ASCIIMenus::VERTICAL);
    helpMenu->SetPosition(16, 1);
    helpMenu->AddItem("About", ASCIIMENU_HELP_INFO_BOX, &Lobby::callback_showMenu, static_cast<int>(DialogType::HELP_GENERAL));
    helpMenu->AddItem("Keyboard Commands", ASCIIMENU_HELP_INFO_BOX, &Lobby::callback_showMenu, static_cast<int>(DialogType::HELP_COMMANDS));
    helpMenu->AddItem("Config Info", ASCIIMENU_HELP_INFO_BOX, &Lobby::callback_showMenu, static_cast<int>(DialogType::HELP_CONFIG));

    Container* helpMenuPopup = Container::Create(this, ASCIIMENU_HELP_INFO_BOX);
    helpMenuPopup->SetOrientation(ASCIIMenus::HORIZONTAL);
    helpMenuPopup->AddItem("[ Ok ]", ASCIIMENU_BACK, &Lobby::callback_hideDialog);

    Container* visualizerPopup = Container::Create(this, ASCIIMENU_SELECT_VISUALIZER);
    visualizerPopup->SetOrientation(ASCIIMenus::VERTICAL);
    std::vector<DJ::VisualizerInfo> info = activeDJ_->GetVisualizerList();
    for (int i = 0; static_cast<size_t>(i) < info.size(); ++i)
    {
      visualizerPopup->AddItem(info[i].Name, ASCIIMENU_NO_CHANGE, &Lobby::callback_visualizerSet, i);
    }

    // Set colors
    menuSystems_.SetColorSelected(RConsole::LIGHTCYAN);
  }


  // Displays a splash screen with little bouncing image in the center of the console and some idle text.
  void Lobby::drawSplash(double dt)
  {
    // Advance index value
    const double splashFPS = 18; 
    double indexOffset = splashFPS * dt;
    idleIndex_ += indexOffset;

    // Calculate index in array and mod value.
    const int osc[] = { '.', ',', '/', '^', '`', '*', '+', '_', ',', '.', '.', '.', '.', '.', '.' };
    const size_t index = static_cast<size_t>(idleIndex_);
    const size_t mod = (sizeof(osc) / sizeof(*osc));
    const RConsole::Color color = RConsole::WHITE;
    const RConsole::Color colorSecondary = RConsole::DARKGREY;

    // Wrap the index
    if (idleIndex_ > (sizeof(osc) / sizeof(int)))
    {
      idleIndex_ = 0;
    }

    // Calcualte and wrap offsets for idle bar
    int verticalOffset = static_cast<int>(RConsole::Canvas::GetConsoleHeight() / 2 - 2);
    std::string msg = Strings::STARTUP_PRIMARY_TEXT + " ";
    RConsole::Canvas::DrawString((msg
      + static_cast<char>(osc[(index) % mod])
      + static_cast<char>(osc[(index + 1) % mod])
      + static_cast<char>(osc[(index + 2) % mod])
      + static_cast<char>(osc[(index + 3) % mod])
      + static_cast<char>(osc[(index + 4) % mod])).c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>((msg.size() + 5)) / 2)
      , verticalOffset
      , color);

    std::string spacer = Strings::STARTUP_SUBTEXT_LINE_SPACER;
    RConsole::Canvas::DrawString(spacer.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(spacer.size()) / 2)
      , ++verticalOffset
      , color);

    std::string submsg = Strings::STARTUP_SUBTEXT_LINE_1;
    RConsole::Canvas::DrawString(submsg.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(submsg.size()) / 2)
      , ++verticalOffset
      , color);

    std::string escMenu = Strings::STARTUP_SUBTEXT_LINE_2;
    RConsole::Canvas::DrawString(escMenu.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(escMenu.size()) / 2)
      , ++verticalOffset
      , color);

    // Footer
    std::string fmodCredit = Strings::STARTUP_FMOD_ATTRIBUTION;
    RConsole::Canvas::DrawString(fmodCredit.c_str()
      , static_cast<int>(RConsole::Canvas::GetConsoleWidth() / 2) - (static_cast<unsigned int>(fmodCredit.size()) / 2)
      , (verticalOffset + 3)
      , colorSecondary);
  }


  // Displays a debug overlay on the player
  void Lobby::drawDebug(double dt)
  {
    float loc = 0;
    long long seconds = std::chrono::duration_cast<std::chrono::seconds>(frameStart_ - appStartTime_).count();
    long long minutes = seconds / 60;
    long long hours = minutes / 60;
    RConsole::Color color = RConsole::DARKGREY;
    long long perFrameMSDelay = activeDJConfig_.DJPerLoopSleepMS;
    std::string delayText = perFrameMSDelay > 0 ? std::to_string(perFrameMSDelay) + "ms" : std::string(Strings::DEBUG_YIELDING);

    RConsole::Canvas::DrawString(("[ argv0: " + argParser_[0]).c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ utime: " + std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m " + std::to_string(seconds % 60) + "s").c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ c/sec: " + std::to_string(calculateUpdateRate(dt)) + " " + Strings::DEBUG_PER_SECOND + " (" + Strings::DEBUG_DELAY + " " + delayText + "/c)").c_str(), 0.0f, loc++, color);
    RConsole::Canvas::DrawString(("[ vname: " + activeDJ_->VisualizerName()).c_str(), 0.0f, loc++, color);
  }


  // Displays additional menus with the specified string
  void Lobby::drawDialog()
  {
    if (isDisplayingDialog_)
    {
      const size_t width = 40;

      switch (displayDialogType_)
      {
        case DialogType::HELP_GENERAL:
          displayInfobox(width, ASCIIMENU_HELP_INFO_BOX, Strings::MODAL_HELP_GENERAL);
          break;
        case DialogType::HELP_COMMANDS:
          displayInfobox(width, ASCIIMENU_HELP_INFO_BOX, Strings::MODAL_HELP_COMMANDS);
          break;
        case DialogType::HELP_CONFIG:
          displayInfobox(width, ASCIIMENU_HELP_INFO_BOX, Strings::MODAL_HELP_CONFIG);
          break;
        case DialogType::DIALOG_OPEN:
          displayInfobox(width, ASCIIMENU_HELP_INFO_BOX, Strings::MODAL_HELP_OPEN);
          break;
        case DialogType::DIALOG_VISUALIZER_LIST:
          displayVisualizerList();
          break;

        default:
          displayInfobox(width, ASCIIMENU_HELP_INFO_BOX, Strings::MODAL_HELP_DEFAULT);
          break;
      }
    }
  }

  // Draws a box with the specified size and returns the box size in a Rect struct.
  // The information about the caluclated rect is returned in a Rect struct.
  Rect Lobby::drawCenteredBox(size_t width, size_t height, size_t margin_height, size_t margin_width, RConsole::Color color)
  {
    // Define sizes
    const unsigned int half_w = static_cast<unsigned int>((width) / 2);
    const unsigned int half_h = static_cast<unsigned int>((height) / 2);
    const unsigned int half_screen_width = RConsole::Canvas::GetConsoleWidth() / 2;
    const unsigned int half_screen_height = RConsole::Canvas::GetConsoleHeight() / 2;
    const unsigned int margin_top = margin_height;
    const unsigned int margin_bottom = margin_height;
    const unsigned int margin_left = margin_width;
    const unsigned int margin_right = margin_width;

    // Side calculations
    Rect rect;
    rect.left = half_screen_width - half_w;
    rect.right = half_screen_width + half_w;
    if (width % 2 == 0)
    {
      rect.right = rect.right - 1;
    }
    rect.top = half_screen_height - half_h;
    rect.bottom = half_screen_height + half_h;
    if (height % 2 == 0)
    {
      rect.bottom = rect.bottom - 1;
    }

    rect.leftPadded = rect.left - margin_left;
    rect.rightPadded = rect.right + margin_right;
    rect.topPadded = rect.top - margin_top;
    rect.bottomPadded = rect.bottom + margin_bottom;


    // Draw box background
    for (int i = rect.leftPadded; i < rect.rightPadded; ++i)
    {
      for (int j = rect.topPadded; j < rect.bottomPadded; ++j)
        RConsole::Canvas::Draw(' ', i, j, RConsole::DARKGREY);
    }

    // Draw sides of the box - horizontal sides
    for (int i = rect.leftPadded; i < rect.rightPadded; ++i)
    {
      RConsole::Canvas::Draw(static_cast<unsigned char>(205), i, static_cast<int>(rect.topPadded), color);
      RConsole::Canvas::Draw(static_cast<unsigned char>(205), i, static_cast<int>(rect.bottomPadded), color);
    }

    // Draw sides of the box - vertical sides
    for (int i = rect.topPadded; i < rect.bottomPadded; ++i)
    {
      RConsole::Canvas::Draw(static_cast<unsigned char>(186), static_cast<int>(rect.leftPadded), i, color);
      RConsole::Canvas::Draw(static_cast<unsigned char>(186), static_cast<int>(rect.rightPadded), i, color);
    }

    // Draw corners
    RConsole::Canvas::Draw(static_cast<unsigned char>(187), rect.rightPadded, rect.topPadded, color);
    RConsole::Canvas::Draw(static_cast<unsigned char>(188), rect.rightPadded, rect.bottomPadded, color);
    RConsole::Canvas::Draw(static_cast<unsigned char>(201), rect.leftPadded, rect.topPadded, color);
    RConsole::Canvas::Draw(static_cast<unsigned char>(200), rect.leftPadded, rect.bottomPadded, color);

    // Generate return structure
    return rect;
  }


  // Global popups using drawing system
  void Lobby::displayInfobox(size_t max_width, std::string containerName, std::string str)
  {
    // Determine two potential heights...
    // Note that when we make str_height later, we check 
    // to see if there were any newlines and favor them.
    int newlines = 0;
    int longest = 0;
    int current = 0;

    for (char c : str)
    {
      ++current;
      if (current > longest)
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
      if (longest < static_cast<int>(max_width))
        max_width = longest;
    }

    // Determine expected box size
    const size_t str_width = static_cast<unsigned int>(max_width);
    const size_t str_height = manualFormat ? newlines + 1: static_cast<unsigned int>(std::ceilf(str.size() / static_cast<float>(str_width)));

    // Draw the box. Use returned rect as buffer.
    Rect rect = drawCenteredBox(str_width, str_height + 2);

    // Move menu options accordingly
    Container* c = MenuRegistry::GetContainer(containerName);
    c->SetPosition(static_cast<size_t>(rect.rightPadded - c->GetSelected().Label.size() - 1), static_cast<size_t>(rect.bottomPadded - 2));
    
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
        , static_cast<int>(rect.left)
        , static_cast<int>(rect.top + cycles)
        , RConsole::WHITE);

      offset += toWrite.length();;
      ++cycles;
    }
  }

  // Draws the visualizer list menu
  void Lobby::displayVisualizerList()
  {
    // Draw the box. Use returned rect as buffer.
    Rect rect = drawCenteredBox(12, 12 + 2);
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