#include "DJ.hpp"
#include "Visualizers\DefaultVisualizer.hpp"
#include "Visualizers\HorizontalWaveformVisualizer.hpp"
#include "Visualizers\ColorDefaultVisualizer.hpp"
#include "Visualizers\CenterVisualizer.hpp"


namespace ASCIIPlayer
{
  // Constructor
  DJ::DJ(DJConfig config, bool startPaused)
    : playlist_(&DJ::playlistUpdatedCallback, this)
    , audioSystem_(config.ChannelCount)
    , visualizer_(nullptr)
    , visaulizerDataSize_(64) // Not magic number, just default width
    , visualizerDataStyle_(aNO_STYLE)
    , config_(config)
    , hasShutdown_(false)
    , currSong_(false)
    , paused_(true)
  {
    //!TODO: Handle visualizer configuration
    if (config.VisualizerID == "horizontalWaveform")
      visualizer_ = new HorizontalWaveformVisualizer();
    else if (config.VisualizerID == "colorDefault")
      visualizer_ = new ColorDefaultVisualizer();
    else if (config.VisualizerID == "centerVisualizer")
      visualizer_ = new CenterVisualizer();
    else
    {
      // config.VisualizerID == "default"
      visualizer_ = new DefaultVisualizer();
    }
    visaulizerDataSize_ = visualizer_->GetAudioDataSize();
    visualizerDataStyle_ = visualizer_->GetAudioDataStyle();
    visualizerDataArray_ = new float[visaulizerDataSize_];

    // Looping?
    if (config.Looping)
      playlist_.SetLooping(true);
    else
      playlist_.SetLooping(false);

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
      if (!audioSystem_.IsPlaying(*currSong_))
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
          FillSongData(visualizerDataArray_, visaulizerDataSize_, FMOD_DSP_FFT_WINDOW_RECT); //blackman windooooowwww yaaaaaaaaaaassss (change that, rect is def)
          visualizer_->Update(visualizerDataArray_);
        }
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
}