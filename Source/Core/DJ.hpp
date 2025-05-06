//Custom DJ class for managing the Playlist, SoundSystem, and Visualizer
#pragma once
#include <vector>
#include <string>
#include <functional>
#include "AudioSystem.hpp"
#include "Defines.hpp"
#include "Playlist.hpp"
#include "Visualizers/Visualizer.hpp"
#include "DJConfig.hpp"
#include "Overlays/ASCIIOverlay.hpp"



namespace ASCIIPlayer
{
  class DJ
  {
  public:
    // Keeps track of a given visualizer
    struct VisualizerInfo
    {
      std::string Name;
      std::function<void(DJ&)> Func;
    };

    // Constructor
    DJ(DJConfig config = DJConfig(), bool startPaused = true);
    ~DJ();

    // Member Functons
    bool Update(double dt);
    void Play();
    void Pause();
    bool IsPaused();
    bool IsPlaying();
    void TogglePause();
    void ToggleRequestUIActive();
    void Shutdown();
    void SetLooping(bool isLooping);
    void ShufflePlaylist();
    void AddSong(AudioFile *toAdd);
    void SongNext();
    void SongPrev();
    void MoveForward();
    void MoveBackward();
    void VolumeUp();
    void VolumeDown();
    void VolumeSet(float newVolume);
    void VisualizerSet(const std::string &name);
    void VisualizerNext();
    void VisualizerPrev();
    std::string VisualizerName();
    std::int64_t GetLastVolumeChange();
    unsigned int GetPlaylistSize();
    std::vector<VisualizerInfo> GetVisualizerList();
    void FillSongData(float* toFill, unsigned int size);

  private:

    // Callback
    void playlistUpdatedCallback();
    void updateLastVolumeChange();

    // Variables
    Playlist<DJ> playlist_;       // Contains the AudioFile objects.
    AudioSystem audioSystem_;     // The sound system to play the audio files.
    std::string visualizerName_;  // The name of the current visualizer.
    Visualizer *visualizer_;      // The visualizer to display the playing data.
    ASCIIOverlay *overlay_;       // The overlay for the visualizer
    DJConfig config_;             // The provided config for the DJ.

    // Internal Tracking
    bool hasShutdown_;                        // Whether or not we have shut down.
    bool paused_;                             // Is the JD paused or not?
    bool requestUIActive_;                    // Whether or not the user requested the UI be visible by default.
    bool isJumpingPos_;                       // Are we using this update loop to update our song position with a manual jump?
    AudioFile *currSong_;                     // What is the current song?
    unsigned int visaulizerDataSize_;         // The size of the array for the audio visualizer.
    AudioDataStyle visualizerDataStyle_;      // The style of data for the audio visualizer
    float *visualizerDataArray_;              // Data used for the visualizer;
    std::int64_t lastVolumeChange_;           // The last time the volume was changed, in ms
    int windowWidth_;                         // Width of the display window the DJ is operating in
    int windowHeight_;                        // Height of the display window the DJ is operating in
    std::vector<VisualizerInfo> visualizers_; // Holds visualizer information
	  
    // Visualizer Switching w/ templates
    template <typename T>
    void setVisualizer()
    {
      // Clean up as necessary
      if (visualizer_)
        delete visualizer_;
      if (visualizerDataArray_)
        delete[] visualizerDataArray_;

      visualizer_ = new T();

      // Set some internal variables
      visaulizerDataSize_ = visualizer_->GetAudioDataSize();
      visualizerDataStyle_ = visualizer_->GetAudioDataStyle();
      visualizerDataArray_ = new float[visaulizerDataSize_];
      memset(visualizerDataArray_, 0, visaulizerDataSize_);
    }
  };
}
