#include <chrono>
#include <vector>
#include <typeindex>
#include "DJ.hpp"
#include "Defines.hpp"
#include "UserStrings.hpp"
#include "Overlays/DefaultOverlay.hpp"
#include "Visualizers/VisualizerWaveform.hpp"
#include "Visualizers/VisualizerWaveformLite.hpp"
#include "Visualizers/VisualizerSpectrum.hpp"
#include "Visualizers/VisualizerParticle.hpp"

// Constructs a VisualizerInfo object containing the visualizer name and a 'set' callback, adding it to the visualizer list.
#define REGISTER_VISUALIZER(n, t) do{ visualizers_.push_back({ n, [](DJ& dj) { dj.setVisualizer<t>(); } }); } while(0)


namespace ASCIIPlayer
{
  // Constructor
  DJ::DJ(DJConfig config, bool startPaused)
    : playlist_(&DJ::playlistUpdatedCallback, this)
    , audioSystem_(config.DJChannelCount)
    , visualizer_(nullptr)
    , overlay_(nullptr)
    , visaulizerDataSize_(128) // Not magic number, just default width
    , visualizerDataStyle_(AudioDataStyle::AUDIODATA_NO_STYLE)
    , visualizerDataArray_(nullptr)
    , config_(config)
    , hasShutdown_(false)
    , currSong_(nullptr)
    , paused_(true)
    , isJumpingPos_(false)
    , lastVolumeChange_(0)
    , requestUIActive_(false)
    , windowWidth_()
    , windowHeight_()
  {
    // TODO(mcech): HANDLE OVERLAY CONFIRGUATION
    if (config_.DJOverlayID == "some name or something")
      overlay_ = new DefaultOverlay();
    else // default
      overlay_ = new DefaultOverlay();

    // Register Visualizers. Order of list determines cycle order, but not the default.
    REGISTER_VISUALIZER("spectrum", VisualizerSpectrum);
    REGISTER_VISUALIZER("waveform", VisualizerWaveform);
    REGISTER_VISUALIZER("waveform lite", VisualizerWaveformLite);
    REGISTER_VISUALIZER("particle", VisualizerParticle);

    // Set visualizer. This applies the DJVisualizerID as the default.
    VisualizerSet(config_.DJVisualizerID);

    // Looping?
    playlist_.SetLooping(config_.DJLooping);

    // Set volume
    VolumeSet(config_.VolumeDefault);

    // Now that it's all done, get the width and height
    if (visualizer_ != nullptr)
    {
      windowWidth_ = visualizer_->Width();
      windowHeight_ = visualizer_->Height();
    }

    DEBUG_PRINT("DJ done with setup and ready to accept song requests");
  }


  // Destructor, calls shutdown if user did not.
  DJ::~DJ()
  {
    if (!hasShutdown_)
    {
      Shutdown();
    }
  }

  // Primary update function that's tasked with handling playlist management
  // along with drawing the current visualizer + overlay.
  bool DJ::Update(double dt)
  {
    if (hasShutdown_)
    {
      return false;
    }

    // We want to honor any user specified sleep times so there's CPU control. 
    // If nothing is specified though, at minimum we want to give up our current time slice
    if (config_.DJPerLoopSleepMS > 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.DJPerLoopSleepMS));
    }
    else
    {
      std::this_thread::yield();
    }

    // Update the song and proceed if necessary
    if (currSong_ && !audioSystem_.IsActive(*currSong_))
    {
      playlist_.Next();

      // if playlist is not looping, gets called.
      if (playlist_.GetPlaylistPos() == playlist_.GetPlaylistLength())
        return false;
    }
    else
    {
      // If we've got songs to visualize and a visualizer to do it, then lets show some data!
      if (visualizer_ && playlist_.GetPlaylistLength() > 0)
      {
        // Update song data appropriately 
        FillSongData(visualizerDataArray_, visaulizerDataSize_);
 
        // Determine if the window size changed at all.
        int width = visualizer_->Width();
        int height = visualizer_->Height();
        if (windowWidth_ != width || windowHeight_ != height)
        {
          windowWidth_ = width;
          windowHeight_ = height;
          visualizer_->OnResize(width, height);
        }

        // Update and post-update functions
        bool status = !paused_;
        if (playlist_.GetPlaylistLength() <= 0)
        {
          status = false;
        }

        visualizer_->Update(visualizerDataArray_, status);
        visualizer_->UpdatePost();
      }

      // Draw overlay after visualizer so it's "On top"
      if (currSong_ && overlay_)
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

    // Everything's runnin', It's all gravy~
    return true;
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


  // Returns if the DJ is paused or not
  bool DJ::IsPaused()
  {
    return paused_;
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
    bool start = false;
	  
    if (playlist_.GetPlaylistLength() == 0)
      start = true;

    if(audioSystem_.PreloadFile(*toAdd))
      playlist_.Add(toAdd);

    if (start)
      Play();

    // Cycling play and pause if it is currently paused in order to acquire a valid 
    // piece of inital data for spectrum purposes. This is not ideal, but works well enough.
    if (paused_)
    {
      Play();
      Pause();
    }
  }


  // Next song in playlist
  // NOTE(mcech): Play enforced for smooth transition. Resumes being paused if it was previously.
  void DJ::SongNext()
  {
    bool wasPaused = IsPaused();

    Play();
    playlist_.Next();
    Play();

    if (wasPaused)
      Pause();
  }


  // Previous song in playlist.
  // NOTE(mcech): Play enforced for smooth transition. Resumes being paused if it was previously.
  void DJ::SongPrev()
  {
    bool wasPaused = IsPaused();

    Play();
    playlist_.Back(audioSystem_);
    Play();

    if (wasPaused)
      Pause();
  }


  // Convert config skip to MS and add it to the current position
  void DJ::MoveForward()
  {
    if (!currSong_)
      return;

    const unsigned int posMS = audioSystem_.GetCurrentPosition(*currSong_);
    audioSystem_.SetCurrentPosition(*currSong_, posMS + config_.SkipForwardSeconds * MS_PER_SECOND_INT);
    isJumpingPos_ = true;
  }


  // Convert config skip to MS and subtract it from the current position
  void DJ::MoveBackward()
  {
    if (!currSong_)
      return;

    const unsigned int posMS = audioSystem_.GetCurrentPosition(*currSong_);

    int location = static_cast<int>(posMS) - config_.SkipForwardSeconds * MS_PER_SECOND_INT;
    if (location < 0)
      location = 0;

    audioSystem_.SetCurrentPosition(*currSong_, location);
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


  // Returns if it succeeded in finding the specified visualizer.
  void DJ::VisualizerSet(const std::string &name)
  {
    for (VisualizerInfo &v : visualizers_)
    {
      if (name == v.Name)
      {
        v.Func(*this);
        visualizerName_ = name;
        return;
      }
    }

    // Recursively set the visualizer to default. There should always be a default.
    VisualizerSet("default");

    // TODO(mcech): Replace with logging system for errors.
    throw(Strings::ERROR_VISUALIZER_CONFIG_INVALID);
  }


  // Attempts to find the current visualizer then go to the next that's in the array.
  void DJ::VisualizerNext()
  {
    if (visualizers_.size() <= 1)
      return;

    for (size_t i = 0; i < visualizers_.size(); ++i)
    {
      if (visualizers_[i].Name == visualizerName_)
      {
        if (i + 1 < visualizers_.size())
        {
          visualizers_[i + 1].Func(*this);
          visualizerName_ = visualizers_[i + 1].Name;
        }
        else
        {
          visualizers_[0].Func(*this);
          visualizerName_ = visualizers_[0].Name;
        }

        return;
      }
    }
  }


  // Attempts to find the current visualizer then go to the previous that's in the array.
  void DJ::VisualizerPrev()
  {
    if (visualizers_.size() <= 1)
      return;

    for (size_t i = 0; i < visualizers_.size(); ++i)
    {
      if (visualizers_[i].Name == visualizerName_)
      {
        if (i > 0)
        {
          visualizers_[i - 1].Func(*this);
          visualizerName_ = visualizers_[i - 1].Name;
        }
        else
        {
          visualizers_[visualizers_.size() - 1].Func(*this);
          visualizerName_ = visualizers_[visualizers_.size() - 1].Name;
        }

        return;
      }
    }
  }


  // Returns the name of the current visualizer
  std::string DJ::VisualizerName()
  {
    return visualizerName_;
  }


  // Gets the last time the volume changed.
  std::int64_t DJ::GetLastVolumeChange()
  {
    return lastVolumeChange_;
  }


  // Returns the number of songs in the playlist.
  unsigned int DJ::GetPlaylistSize()
  {
    return static_cast<unsigned int>(playlist_.GetPlaylistLength());
  }


  // Fills the array provided with the active spectrum.
  // Only fill visualizer data if not paused.
  // Otherwise, '0' is provided.
  void DJ::FillSongData(float* toFill, unsigned int size)
  {
    // Collect the volume for scaling operations
    const float masterVolume = audioSystem_.GetMasterVolume();

    // Make sure we're not paused. If so, skip.
    if (paused_)
      return;

    // Ensure there's some audio data present. If no, skip.
    if (masterVolume < MUTE_THRESHOLD)
      return;

    // Make sure we have a defined visualizer, otherwise there's a problem
    if (visualizerDataStyle_ == AudioDataStyle::AUDIODATA_NO_STYLE)
      throw "A style is required";

    // Collect the data from FMOD
    audioSystem_.FillWithAudioData(toFill, size, visualizerDataStyle_);

    // Normalize as if we've got a volume of 1, since by default FMOD will
    // scale the output of the data by the volume internally.
    for (size_t i = 0; i < visaulizerDataSize_; ++i)
    {
      visualizerDataArray_[i] = visualizerDataArray_[i] / masterVolume;
    }
  }


  // Returns a vector of visualizer lists
  std::vector<DJ::VisualizerInfo> DJ::GetVisualizerList()
  {
    return visualizers_;
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
