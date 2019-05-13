// Handles the backend, playing, loading, and unloading files.
#pragma once
#include <unordered_map>
#include "AudioFile.hpp"
#include "Defines.hpp"


namespace ASCIIPlayer
{
  class AudioSystem
  {
  public:
    // Constructor
    AudioSystem(int numChannels = 10, float defaultVolume = .8f);
    ~AudioSystem();

    // Member Functions
    // General Upkeep
    bool Update();

    // Loading
    bool PreloadFile(AudioFile &audioFile);
    void UnloadFile(AudioFile &audioFile);

    // Basic Play/Pause
    AudioFile *PreloadFile(const std::string filepath);
    void PlayFile(AudioFile &audioFile, bool playing = false);
    void SetPaused(AudioFile &audioFile, bool pausedState);
    void StopFile(AudioFile &audioFile);
    float SetMasterVolume(float f);
    float GetMasterVolume() const;

    // Info about song
    bool IsActive(AudioFile &audioFile);
    bool IsPaused(AudioFile &audioFile);
    unsigned int GetLength(AudioFile &audioFile) const;
    unsigned int GetCurrentPosition(AudioFile &audioFile);
    void SetCurrentPosition(AudioFile &audioFile, unsigned int pos);
    std::string GetFilename(const AudioFile &audioFile) const;
    std::string GetFilepath(const AudioFile &audioFile) const;
    void FillWithAudioData(float *arr, int numVals, int channelOffset, AudioDataStyle style);

    static void FCheck(const FMOD_RESULT &res);
  private:
      //////////////////////////
     // Private member funcs //
    //////////////////////////

      ///////////////
     // Variables //
    ///////////////
    // Audio System
    FMOD::System *fmodSystem_;   // Handle to the system we use to initialize and play sounds.
    Channelgroup *masterChannel_; // Master channel to shove things into for volume reasons.
    unsigned int version_;

    // Active Handles map:
    std::unordered_map<APUnique, ChannelHandle> channelHandles_;

    // ID Tracking
    APUnique ID_;
    static APUnique AudioSystemIDIncrement;
  };
}
