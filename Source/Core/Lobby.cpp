#include <thread>
#include <exception>
#include <shoom/shoom.hpp>
#include "Lobby.hpp"
#include "UserStrings.hpp"
#include "MenuSystem.hpp"


namespace ASCIIPlayer
{
    ////////////
   // Public //
  ////////////
  // Constructor and Destructor
  Lobby::Lobby(int argc, char** argv)
    // Operation related
    : keyParser_()
    , argParser_(argc, argv)
    , activeDJ_(nullptr)
    , activeDJConfig_()
    , menuSystems_("")
    , lobbyHosting_(true)
    , menuVisible_(false)
    , idleIndex_(0)
    , isOneshot_(false)
    , updateLoopCount_(0)

    // Menu
    , displayDialogType_(DialogType::NONE)
    , menuNavBackNextUpdate_(false)
    , isDisplayingDialog_(false)

    // Debug
    , timesIndex_(0)
    , timesSoFar_(0)
    , dtHistory_()
    , showDebug_(false)
    , frameStart_()
    , previousFrameStart_()
    , frameEnd_()
    , appStartTime_(std::chrono::high_resolution_clock::now())
  { 
    // Update FPS start
    frameStart_ = appStartTime_;
    previousFrameStart_ = appStartTime_;

    // Begin by handling the application opening
    handleApplicationOpen(argc, argv);

    // Make DJ, don't auto-play. Read in the argument!
    activeDJConfig_ = DJConfig::ReadFile(argParser_[0]);
    DJ *Dj = new DJ(activeDJConfig_, false);

    // Just loop through and see if anything happens to be a song to load.
    std::vector<std::string> args = argParser_.GetAllArgs();
    for (unsigned int i = 1; i < args.size(); ++i)
    {
      AudioFile *test1 = new ASCIIPlayer::AudioFile(args[i]);
      Dj->AddSong(test1);
    }

    // Set activeDJ to our DJ
    activeDJ_ = Dj;

    // Set up our menus
    buildMenus();
  }

  // This is designed to ensure we're not running multiple instances
  // of ASCIIPlayer. If we are, hand off the arguments to the already
  // running instance and close this one.
  void Lobby::handleApplicationOpen(int argc, char** argv)
  {
    // Initialize shared memory and stuff
    sharedStatus = new shoom::Shm("ASCIIPlayer_status", 1); 
    sharedStatus->Create();

    sharedArguments = new shoom::Shm("ASCIIPlayer_arguments", 512);
    sharedArguments->Create();

    char val = sharedStatus->Data()[0];
    if (val == 1)
    {
      if (argc >= 2)
      {
        size_t maxlen = 512;
        size_t arglen = strlen(argv[1]);
        if (arglen < maxlen)
          maxlen = arglen;

        memcpy(sharedArguments->Data(), argv[1], maxlen);

        delete sharedStatus;
        delete sharedArguments;
      }

      printf("ASCIIPlayer is already running - Handing off your request to the running instance.");
      exit(0);
      return;
    }

    // Indicate that we're the only running item.
    sharedStatus->Data()[0] = 1;
  }


  // Destructor
  Lobby::~Lobby()
  {
    if (activeDJ_)
    {
      activeDJ_->Shutdown();

      delete activeDJ_;
    }

    delete sharedStatus;
    delete sharedArguments;
  }


  // Flag that we want to play one sound once, and then leave.
  // Should be called before run along with adding a song via input
  void Lobby::EnableOneshotPlayback()
  {
    isOneshot_ = true;
  }


  DJ* Lobby::GetDJ()
  {
    return activeDJ_;
  }
  

  std::uint64_t Lobby::GetUpdateLoopCount()
  {
    return updateLoopCount_;
  }


  // Has while loop and std::cin every loop to ensure it's running correctly. 
  // Loop sits until DJ starts with proper command, at which point we get to "lobbyHosting".
  void Lobby::Run()
  {
    DEBUG_PRINT("Lobby looking spiffy!");

    if (activeDJ_)
      DEBUG_PRINT("DJ Has prepped " << activeDJ_->GetPlaylistSize() << " songs!");

    // Configure one-shot
    if (isOneshot_)
      activeDJ_->SetLooping(false);

    // Set up to start entering the primary loop
    RConsole::Canvas::ForceClearEverything();
    if (activeDJ_->GetPlaylistSize() > 0)
      activeDJ_->Play();

    // While we're hosting stuff in the lobby
    while (lobbyHosting_)
    {
      // Loop tracking
      previousFrameStart_ = frameStart_;
      frameStart_ = std::chrono::high_resolution_clock::now();
      double dtSeconds = std::chrono::duration_cast<std::chrono::microseconds>(frameStart_ - previousFrameStart_).count() / 1000000.0f;

      // ============================ Start primary loop ============================

      // Attempt to view and if necessary respond to shared memory arguments
      if (sharedArguments->Data()[0] != 0)
      {
        sharedArguments->Data()[sharedArguments->Size() - 1] = 0;
        std::string str = std::string(reinterpret_cast<char*>(sharedArguments->Data()));
        memset(sharedArguments->Data(), 0, sharedArguments->Size());
        InterpretMultiCharInput(str);
      }
      
      // Sub-menu navigation and updating
      if (menuNavBackNextUpdate_)
      {
        while (menuSystems_.Back()) {  }

        menuNavBackNextUpdate_ = false;
        menuVisible_ = menuSystems_.IsVisible();
      }

      // Draw the splash/idle screen if we have nothing to play
      if (activeDJ_->GetPlaylistSize() == 0)
        drawSplash(dtSeconds);

      // Actively run the current DJ
      if (activeDJ_)
        activeDJ_->Update(dtSeconds);

      // Parse input
      keyParser_.HandleInput(this, &Lobby::InterpretCharInput, &Lobby::InterpretMultiCharInput);

      // Finalize drawing for debug
      if (showDebug_)
        drawDebug(dtSeconds);

      // Draw menus and finalize drawing for menu overlay.
      drawDialog();
      menuSystems_.Draw(0, 0, true);

      // Write out and display all drawing
      RConsole::Canvas::Update();
      
      // Exit if we're done playing our one-shot
      if (isOneshot_ && !activeDJ_->IsPlaying())
        lobbyHosting_ = false;

      // ============================ End primary loop ============================
      frameEnd_ = std::chrono::high_resolution_clock::now();
      ++updateLoopCount_;
    }
  }

    /////////////
   // Private //
  /////////////
  // Calculates average framerate based on the start and stop provided. 
  int Lobby::calculateUpdateRate(double dt)
  {
    int size = TRACKED_TIMES < timesSoFar_ ? TRACKED_TIMES : timesSoFar_;
    dtHistory_[timesIndex_++] = dt;
    if (timesIndex_ >= size)
    {
      timesIndex_ = 0;
    }

    double total = 0;
    for (int i = 0; i < size; ++i)
    {
      total += dtHistory_[i];
    }

    if (timesSoFar_ < TRACKED_TIMES)
    {
      ++timesSoFar_;
    }

    double avgFrametimeSeconds = total / size;

    return static_cast<int>(1.0 / avgFrametimeSeconds);
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
  void Lobby::setDJ(DJ* newDJ)
  {
    if (activeDJ_)
    {
      activeDJ_->Pause();
      activeDJ_->Shutdown();
      delete activeDJ_;
    }

    activeDJ_ = newDJ;
  }


  // Interpret specific paths
  void Lobby::InterpretMultiCharInput(const std::string str)
  {
    std::string input = str;
    if (str[0] == '"' && str[str.size() - 1] == '"')
      input = str.substr(1, str.size() - 2);
    if (str[0] == '\\' && str[str.size() - 1] == '"')
      input = str.substr(2, str.size() - 4);

#pragma warning (disable: 4309)
    if (str[0] == static_cast<signed char>(224))
      InterpretCharInput(str[1]);

    AudioFile *new_song = new ASCIIPlayer::AudioFile(input);
    activeDJ_->AddSong(new_song);
  }


  // Interprets a single-character piece of input
  void Lobby::InterpretCharInput(char c)
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
      if (isDisplayingDialog_)
        isDisplayingDialog_ = false;
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
