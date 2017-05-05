//Custom DJ class for managing the Playlist, SoundSystem, and Visualizer
#pragma once
#include "AudioSystem.hpp"
#include "Defines.hpp"
#include "Playlist.hpp"
#include "Visualizer.hpp"
#include "Visualizers\DefaultVisualizer.hpp"
#include "DJConfig.hpp"



namespace ASCIIPlayer
{
  class DJ
  {
  public:
    // Constructor
    DJ(DJConfig config = DJConfig(), bool startPaused = true);
    ~DJ();

    // Member Functons
    bool Update();
    void Play();
    void Pause();
    void Shutdown();
    void ShufflePlaylist();
    void AddSong(AudioFile *toAdd);
    void SkipForward();
    void SkipBackward();
    void VolumeUp();
    void VolumeDown();
    void VolumeSet(const float newVolume);
    unsigned int GetPlaylistSize();
    void FillSongData(float* toFill, unsigned int size, FMOD_DSP_FFT_WINDOW window);

  private:
    // Callback
    void playlistUpdatedCallback();

    // Variables
    Playlist<DJ> playlist_;   // Contains the AudioFile objects.
    AudioSystem audioSystem_; // The sound system to play the audio files.
    Visualizer *visualizer_;  // The visualizer to display the playing data.
    DJConfig config_;         // The proivded config for the DJ.

    // Internal Tracking
    bool hasShutdown_;                   // Whether or not we have shut down.
    bool paused_;                        // Is the JD paused or not?
    AudioFile *currSong_;                // What is the current song?
    unsigned int visaulizerDataSize_;   // The size of the array for the audio visualizer.
    AudioDataStyle visualizerDataStyle_; // The style of data for the audio visualizer
    float *visualizerDataArray_;         // Data used for the visualizer;
  };
}
