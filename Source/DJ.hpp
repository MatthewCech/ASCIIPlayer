//Custom DJ class for managing the Playlist, SoundSystem, and Visualizer
#pragma once
#include "AudioSystem.hpp"
#include "Defines.hpp"
#include "Playlist.hpp"
#include "Visualizer.hpp"
#include "DefaultVisualizer.hpp"
#include "DJConfig.hpp"



namespace ASCIIPlayer
{
  class DJ
  {
  public:
    // Constructor
    DJ(DJConfig config = DJConfig(), bool startPaused = false);
    ~DJ();

    // Member Functons
    bool Update();
    void Play();
    void Pause();
    void Shutdown();
    void ShufflePlaylist();
    void AddSong(AudioFile *toAdd);
    void FillSongSpectrum(float* toFill, AudioDataSize size, FMOD_DSP_FFT_WINDOW window);

  private:
    // Callback
    void playlistUpdatedCallback();

    // Variables
    Playlist<DJ> playlist_;         // Contains the AudioFile objects.
    AudioSystem audioSystem_;       // The sound system to play the audio files.
    Visualizer *visualizer_;        // The visualizer to display the playing data.
    DJConfig config_;               // The proivded config for the DJ.

    // Internal Tracking
    bool hasShutdown_;  // Whether or not we have shut down.
    bool paused_;
    AudioFile *currSong_;
  };
}
