#include "DJ.hpp"

namespace ASCIIPlayer
{
  // Constructor
  DJ::DJ(DJConfig config, bool startPaused)
    : playlist_(&DJ::playlistUpdatedCallback, this)
    , audioSystem_(config.ChannelCount)
    , visualizer_(nullptr)
    , config_(config)
    , hasShutdown_(false)
    , currSong_(false)
    , paused_(startPaused)
  {
    //!TODO: Handle visualizer configuration
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
      if (visualizer_)
      {
        //!TODO: Make this more efficient, don't allocate it every time.
        float vals[a128];
        FillSongSpectrum(vals, a128, FMOD_DSP_FFT_WINDOW_BLACKMAN); //blackman windooooowwww yaaaaaaaaaaassss (change that, rect is def)
        visualizer_->Update(vals, a128);
      }

      // Update our audio system at the very end.
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
        audioSystem_.TogglePause(*currSong_);
        paused_ = false;
      }
    }
  }

  void DJ::Pause()
  {
    if (currSong_)
    {
      if (!paused_)
      {
        audioSystem_.TogglePause(*currSong_);
        paused_ = true;
      }
    }
  }

  // Shuts down the DJ, closing out of all the audio that has been loaded and it has
  // prepped for.
  void DJ::Shutdown()
  {
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
    audioSystem_.PreloadFile(*toAdd);
    playlist_.Add(toAdd);
  }


  // Fills the array provided with the active spectrum.
  void DJ::FillSongSpectrum(float* toFill, AudioDataSize size, FMOD_DSP_FFT_WINDOW window)
  {
    audioSystem_.FillSpectrum(toFill, size, 0, window);
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
    audioSystem_.PlayFile(*currSong_);
  }
}