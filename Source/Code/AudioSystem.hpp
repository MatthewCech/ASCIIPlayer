// Handles the backend, playing, loading, and unloading files.
#pragma once
#include "AudioFile.hpp"
#include <unordered_map>
#include "CustomDefines.hpp"



namespace ASCIIPlayer
{
  class AudioSystem
  {
  public:
    // Constructor
    AudioSystem(int numChannels = 10);
    ~AudioSystem();

    // Member Functions
    // General Upkeep
    bool Update();

    // Loading
    void PreloadFile(const AudioFile &audioFile);
    void UnloadFile(const AudioFile &audioFile);

    // Basic Play/Pause
    void PlayFile(const AudioFile &audioFile);
    void PauseFile(const AudioFile &audioFile);
    void StopFile(const AudioFile &audioFile);
    void SetMasterVolume(float f);
    float GetMasterVolume();

    //Info about song
    unsigned long long GetLength(const AudioFile &audioFile);
    unsigned long long GetCurrentPosition(const AudioFile &audioFile);
    std::string GetFilename(const AudioFile &audioFile);
    std::string GetFilepath(const AudioFile &audioFile);

  private:
    //Active Handles map:
    std::unordered_map<unsigned long long, ChannelHandle> handles_;
    Channelgroup masterChannel_;
  };
}