#include "DJ.hpp"

namespace ASCIIPlayer
{
  // Constructor
  DJ::DJ(DJConfig config)
    : hasShutdown_(false)
  {

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

  }
  
  // Shuts down the DJ, closing out of all the audio that has been loaded and it has
  // prepped for.
  void DJ::Shutdown()
  {

    hasShutdown_ = true;
  }

  void DJ::ShufflePlaylist()
  {

  }

  // Adds a song to the DJ playlist.
  // All songs must be in a playlist.
  void DJ::AddSong(AudioFile toAdd)
  {

  }

  // Fills the array provided with the active spectrum.
  void DJ::FillSongSpectrum(float* toFill, AudioDataSize size, FMOD_DSP_FFT_WINDOW window)
  {
    audioSystem_.FillSpectrum(toFill, size, 0, window)
  }
}