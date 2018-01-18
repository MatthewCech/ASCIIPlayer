#include "DJ.hpp"
#include "Visualizers/DefaultVisualizer.hpp"
#include "Visualizers/HorizontalWaveformVisualizer.hpp"
#include "Visualizers/ColorDefaultVisualizer.hpp"
#include "Visualizers/CenterVisualizer.hpp"
#include "Overlays/DefaultOverlay.hpp"
#include <chrono>


namespace ASCIIPlayer
{
  // Constructor
  DJ::DJ(DJConfig config, bool startPaused)
    : playlist_(&DJ::playlistUpdatedCallback, this)
    , audioSystem_(config.DJChannelCount)
    , visualizer_(nullptr)
    , overlay_(nullptr)
    , visaulizerDataSize_(64) // Not magic number, just default width
    , visualizerDataStyle_(aNO_STYLE)
    , config_(config)
    , hasShutdown_(false)
    , currSong_(false)
    , paused_(true)
    , isJumpingPos_(false)
	  , lastVolumeChange_(0)
    , requestUIActive_(false)
  {
    //!TODO: HANDLE OVERLAY CONFIRGUATION
    if (config_.DJVisualizerID == "some name or something")
      overlay_ = new DefaultOverlay();
    else // default
      overlay_ = new DefaultOverlay();

    //!TODO: HANDLE VISUALIZER CONFIGURATION
    if (config_.DJVisualizerID == "horizontalWaveform")
      visualizer_ = new HorizontalWaveformVisualizer();
    else if (config_.DJVisualizerID == "colorDefault")
      visualizer_ = new ColorDefaultVisualizer();
    else if (config_.DJVisualizerID == "centerVisualizer")
      visualizer_ = new CenterVisualizer();
    else // default
    {
      visualizer_ = new DefaultVisualizer();
    }
    visaulizerDataSize_ = visualizer_->GetAudioDataSize();
    visualizerDataStyle_ = visualizer_->GetAudioDataStyle();
    visualizerDataArray_ = new float[visaulizerDataSize_];
    memset(visualizerDataArray_, 0, visaulizerDataSize_);

    // Looping?
    if (config_.DJLooping)
      playlist_.SetLooping(true);
    else
      playlist_.SetLooping(false);

    // Set volume
    VolumeSet(config_.VolumeDefault);

    // Done!
    DEBUG_PRINT("== DJ done with setup- Ready to accept song requests! ==");
  }


  // Destructor, calls shutdown if user did not.
  DJ::~DJ()
  {
    if (!hasShutdown_)
      Shutdown();
  }


  // Member Functons
  bool DJ::Update()
  {
    if (!hasShutdown_)
    {
      // Update the song and proceed if necessary
      if (!audioSystem_.IsActive(*currSong_))
      {
        playlist_.Next();

        // if playlist is not looping, gets called.
        if (playlist_.GetPlaylistPos() == playlist_.GetPlaylistLength())
          return false;
      }
      else
      {
        if (visualizer_)
        {
          //!TODO: Make this more efficient, don't allocate it every time.

          // Only fill if not paused
          if(!paused_)
            FillSongData(visualizerDataArray_, visaulizerDataSize_, FMOD_DSP_FFT_WINDOW_RECT);

          visualizer_->Update(visualizerDataArray_);
          visualizer_->UpdatePost();
        }

        // Draw overlay after visualizer so it's "On top"
        if (overlay_)
        {
          overlay_->Update(
            UIInfo(requestUIActive_
              , !audioSystem_.IsPaused(*currSong_)
              , isJumpingPos_
              , audioSystem_.GetMasterVolume()
              , audioSystem_.GetFilename(*currSong_)
              , audioSystem_.GetCurrentPosition(*currSong_)
              , audioSystem_.GetLength(*currSong_)));

          overlay_->UpdatePost();
        }
      }

      // Update our audio system at the very end.
      isJumpingPos_ = false;
      audioSystem_.Update();
      return true;
    }
    else
      return false;
  }
  

  // Plays the current song, or starts if there's nothing.
  void DJ::Play()
  {
    if (currSong_)
    {
      if (paused_)
      {
        audioSystem_.SetPaused(*currSong_, false);
        paused_ = false;
      }
    }
  }


  // Pauses the current song
  void DJ::Pause()
  {
    if (currSong_)
    {
      if (!paused_)
      {
        audioSystem_.SetPaused(*currSong_, true);
        paused_ = true;
      }
    }
  }


  // Toggles if it's paused or not
  void DJ::TogglePause()
  {
    if (!paused_)
      Pause();
    else
      Play();
  }


  // Toggles if we are requesting to have an acive UI
  void DJ::ToggleRequestUIActive()
  {
    requestUIActive_ = !requestUIActive_;
  }


  // Shuts down the DJ, closing out of all the audio that has been loaded and it has
  // prepped for.
  void DJ::Shutdown()
  {
    if(visualizer_)
      delete visualizer_;
    hasShutdown_ = true;
  }


  // Shuffles all the songs in the playlist.
  void DJ::ShufflePlaylist()
  {
    playlist_.Scramble();
  }


  // Adds a song to the DJ playlist.
  // All songs must be in a playlist, even solo songs. They get added to the DJ playlist.
  // This will be allocated when passed. new AudioFile("string");
  // The DJ does NOT handle deallocation for you, in case you wanted to add this to other DJs.
  void DJ::AddSong(AudioFile *toAdd)
  {
    if(audioSystem_.PreloadFile(*toAdd))
      playlist_.Add(toAdd);
  }


  // Next song in playlist
  void DJ::SongNext()
  {
    playlist_.Next();
  }


  // Previous song in playlist
  void DJ::SongPrev()
  {
    playlist_.Back();
  }


  // Convert config skip to MS and add it to the current position
  void DJ::MoveForward()
  {
    if (!currSong_)
      return;

    const unsigned int posMS = audioSystem_.GetCurrentPosition(*currSong_);
    audioSystem_.SetCurrentPosition(*currSong_, posMS + config_.SkipForwardSeconds * SONG_TIME_SCALE_FOR_SECONDS);
    isJumpingPos_ = true;
  }


  // Convert config skip to MS and subtract it from the current position
  void DJ::MoveBackward()
  {
    if (!currSong_)
      return;

    const unsigned int posMS = audioSystem_.GetCurrentPosition(*currSong_);
    audioSystem_.SetCurrentPosition(*currSong_, posMS - config_.SkipForwardSeconds * SONG_TIME_SCALE_FOR_SECONDS);
    isJumpingPos_ = true;
  }


  // Increases by volume change amount
  void DJ::VolumeUp()
  {
    const float vol = audioSystem_.GetMasterVolume();
    audioSystem_.SetMasterVolume(vol + config_.VolumeChangeAmount);
	  updateLastVolumeChange();
  }


  // Decreases by volume change amount
  void DJ::VolumeDown()
  {
    const float vol = audioSystem_.GetMasterVolume();
    audioSystem_.SetMasterVolume(vol - config_.VolumeChangeAmount);
	  updateLastVolumeChange();
  }
    

  // Sets the volume, capped between 0 and 1.
  void DJ::VolumeSet(const float newVolume)
  {
    //config_ = newVolume
    audioSystem_.SetMasterVolume(newVolume);
	updateLastVolumeChange();
  }


  // Gets the last time the volume changed.
  long long DJ::GetLastVolumeChange()
  {
	  return lastVolumeChange_;
  }


  // Returns the number of songs in the playlist.
  unsigned int DJ::GetPlaylistSize()
  {
    return playlist_.GetPlaylistLength();
  }


  // Fills the array provided with the active spectrum.
  void DJ::FillSongData(float* toFill, unsigned int size, FMOD_DSP_FFT_WINDOW window)
  {
    if(visualizerDataStyle_ == aWaveform)
      audioSystem_.FillWithAudioData(toFill, size, 0, window, aWaveform);
    else
      audioSystem_.FillWithAudioData(toFill, size, 0, window, aSpectrum);
  }


    //////////////////////////////
   // Private member funcitons //
  //////////////////////////////
  void DJ::playlistUpdatedCallback()
  {
    // Stop the song and play a new one, since we've been updated.
    if(currSong_)
      audioSystem_.StopFile(*currSong_);

    currSong_ = playlist_.GetCurrent();
    if(!paused_)
      audioSystem_.PlayFile(*currSong_, true);
  }

	
  // Called whenever the volume is changed, this sets the internal tracking variables for that.
  void DJ::updateLastVolumeChange()
  {
	  lastVolumeChange_ = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }
}