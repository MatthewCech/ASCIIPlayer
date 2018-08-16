#include "AudioSystem.hpp"
#include <FMOD/fmod_errors.h>
#include <RTest/RTest.hpp>


namespace ASCIIPlayer
{
  // Static Variable Init
  APUnique AudioSystem::AudioSystemIDIncrement = 0;


  // Constructor
  AudioSystem::AudioSystem(int numChannels, float defaultVolume) 
    : fmodSystem_(nullptr)
    , masterChannel_(nullptr)
    , numdrivers_(0)
    , ID_(AudioSystemIDIncrement++)
  {
    // Generate system and get version
    FCheck(FMOD::System_Create(&fmodSystem_));

    // Check version.
    FCheck(fmodSystem_->getVersion(&version_));
    if (version_ < FMOD_VERSION)
    {
      printf("You are using an old version of FMOD, %08x. This program requires %08x\n",
        version_, FMOD_VERSION);
      throw RTest::RException("Oudated FMOD Version Error!");
    }

    // Check for drivers
    FCheck(fmodSystem_->getNumDrivers(&numdrivers_));
    if (numdrivers_ == 0)
      // Turn off output.
      FCheck(fmodSystem_->setOutput(FMOD_OUTPUTTYPE_NOSOUND)); 
    else
    {
      // Get the driver capabilities.
      FCheck(fmodSystem_->getDriverCaps(0, &caps_, 0, &speakermode_));

      // Set the user selected speaker mode.
      FCheck(fmodSystem_->setSpeakerMode(speakermode_));

      if (caps_ & FMOD_CAPS_HARDWARE_EMULATED)
        // The user has the 'Acceleration' slider set to off! This is really bad
        // for latency! You might want to warn the user about this.
        FCheck(fmodSystem_->setDSPBufferSize(1024, 10));

      // Get the info for the sound device in place, handle ones that don't like things.
      FCheck(fmodSystem_->getDriverInfo(0, name_, 256, 0));
      if (strstr(name_, "SigmaTel"))
        // Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
        // PCM floating point output seems to solve it.
        FCheck(fmodSystem_->setSoftwareFormat(
          48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR));
    }

    // Run formal init, and make sure we could create the buffers.
    FMOD_RESULT res = fmodSystem_->init(numChannels, FMOD_INIT_NORMAL, 0);
    if (res == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
      // Ok, the speaker mode selected isn't supported by this soundcard. Switch it
      // back to stereo...
      FCheck(fmodSystem_->setSpeakerMode(FMOD_SPEAKERMODE_STEREO));

      // ... and re-init.
      res = fmodSystem_->init(numChannels, FMOD_INIT_NORMAL, 0);
    }

    // Regardless, check the result.
    FCheck(res);

    // Set default volume
    fmodSystem_->createChannelGroup("Master", &masterChannel_);
    SetMasterVolume(defaultVolume);
  }


  // Destructor for the audio system.
  AudioSystem::~AudioSystem()
  {
    // Releases all assests.
    FCheck(fmodSystem_->release());
  }


  // Update the audio system. Call as much as possible.
  bool AudioSystem::Update()
  {
    FCheck(fmodSystem_->update());
    return true;
  }


  // Preloads based on a string.
  AudioFile *AudioSystem::PreloadFile(const std::string filepath)
  {
    AudioFile *af = new AudioFile(filepath);
    PreloadFile(*af);
    return af;
  }


  // Loads into memory the audiofile in question.
  // If it sees an object loaded, it assumes the object is valid.
  bool AudioSystem::PreloadFile(AudioFile &audioFile)
  {
    if (!audioFile.get(ID_)->LoadedObject)
    {
      FMOD_RESULT res = fmodSystem_->createStream(
        audioFile.path_.c_str(), FMOD_DEFAULT, 0, &audioFile.get(ID_)->LoadedObject);

      if (res != FMOD_OK)
      {
        DEBUG_PRINT("FAILED to preload:\n  " << audioFile.path_);
        return false;
      }
      else
        DEBUG_PRINT("Preloaded:\n  " << audioFile.path_);
    }
    return true;
  }


  // Unload a file from memory.
  void AudioSystem::UnloadFile(AudioFile &audioFile)
  {
    FCheck(audioFile.get(ID_)->LoadedObject->release());
  }


  // Play a file and sort it into the correct channel set. 
  void AudioSystem::PlayFile(AudioFile &audioFile, bool playing)
  {
    if (audioFile.get(ID_)->LoadedObject == nullptr)
      PreloadFile(audioFile);

    // Play and assign to master channel group.
    ChannelHandle channel;
    FCheck(fmodSystem_->playSound(
      FMOD_CHANNEL_FREE, audioFile.get(ID_)->LoadedObject, !playing, &channel));
    FCheck(channel->setChannelGroup(masterChannel_));

    // @ToDo: Channel override may result in same song playing multiple times.
    // Test this and confirm if it is an issue that needs to be solved.
    channelHandles_[audioFile.fileID_] = channel;
    audioFile.get(ID_)->BoundToChannel = true;
  }


  // Pauses the file in question to be resumed later.
  void AudioSystem::SetPaused(AudioFile &audioFile, bool pausedState)
  {
    if (audioFile.get(ID_)->LoadedObject == nullptr)
      return;
    
    if (audioFile.get(ID_)->BoundToChannel == false)
    {
      PlayFile(audioFile, !pausedState);
      return;
    }

    // Toggle paused using !
    bool pausedStatus;
    ChannelHandle ch = channelHandles_[audioFile.fileID_];
    FCheck(ch->getPaused(&pausedStatus));

    if (pausedState)
      if (pausedStatus == true)
        return;
    
    if (!pausedState)
      if (pausedStatus == false)
        return;

    FCheck(ch->setPaused(!pausedStatus));
  }
  

  // Stops the audio file completely, removing the channel.
  void AudioSystem::StopFile(AudioFile &audioFile)
  {
    ChannelHandle *ch = &channelHandles_[audioFile.fileID_];
    bool playing;
    (*ch)->isPlaying(&playing);
    if(playing)
      FCheck((*ch)->stop());
  }
  

  // Sets the volume of the master channel.
  // Returns what we set the volume to.
  float AudioSystem::SetMasterVolume(float f)
  {
    if (f > 1) f = 1;
    if (f < 0) f = 0;
    FCheck(masterChannel_->setVolume(f));
    return f;
  }


  // Returns volume of master channel.
  float AudioSystem::GetMasterVolume() const
  {
    float vol;
    FCheck(masterChannel_->getVolume(&vol));
    return vol;
  }


  // Fills a provided array of floats with the spectrum in question.
  void AudioSystem::FillWithAudioData(float *arr, int numVals, int channelOffset, FMOD_DSP_FFT_WINDOW window, AudioDataStyle style)
  {
    if(style == AUDIODATA_WAVEFORM)
      FCheck(fmodSystem_->getWaveData(arr, numVals, channelOffset));
    else
      FCheck(fmodSystem_->getSpectrum(arr, numVals, channelOffset, window));
  }



    //////////////////////////
   // Private member funcs //
  //////////////////////////
  // Checks the result of the FMOD operation.
  void AudioSystem::FCheck(const FMOD_RESULT &res) const
  {
    if (res != FMOD_OK)
      throw RTest::RException(std::string("FMOD Error: ") + FMOD_ErrorString(res));
  }


  // Gets if the audiofile is playing
  bool AudioSystem::IsActive(AudioFile &audioFile)
  {
    bool status;
    channelHandles_[audioFile.fileID_]->isPlaying(&status);
    return status;
  }


  // Returns wether or not the audio file is playing.
  bool AudioSystem::IsPaused(AudioFile &audioFile)
  {
    bool status;
    channelHandles_[audioFile.fileID_]->getPaused(&status);
    return status;
  }


  // Returns the length of the file in question in Milliseconds.
  unsigned int AudioSystem::GetLength(AudioFile &audioFile) const
  {
    unsigned int tu;
    FCheck(audioFile.get(ID_)->LoadedObject->getLength(&tu, FMOD_TIMEUNIT_MS));
    return tu;
  }


  // Returns our current location in the file, in milliseconds.
  unsigned int AudioSystem::GetCurrentPosition(AudioFile &audioFile)
  {
    if (audioFile.get(ID_)->LoadedObject == nullptr)
      return 0;

    unsigned int tu;
    FCheck(channelHandles_[audioFile.fileID_]->getPosition(&tu, FMOD_TIMEUNIT_MS));
    return tu;
  }


  // Set the position of the current song in MS
  void AudioSystem::SetCurrentPosition(AudioFile &audioFile, unsigned int pos)
  {
    if (audioFile.get(ID_)->LoadedObject == nullptr)
      return;

    unsigned int length = GetLength(audioFile);
    if (length <= pos)
      pos = length - 1;

    FCheck(channelHandles_[audioFile.fileID_]->setPosition(pos, FMOD_TIMEUNIT_MS));
  }


  // Get the filename
  std::string AudioSystem::GetFilename(const AudioFile &audioFile) const
  {
    std::string name = audioFile.path_;
    std::size_t location = name.find_last_of("/\\");
    if (location != std::string::npos)
      name = name.substr(location + 1);
    return name;
  }


  // Get the filepath
  std::string AudioSystem::GetFilepath(const AudioFile &audioFile) const
  {
    std::string path = audioFile.path_;
    std::size_t location = path.find_last_of("/\\");
    if (location != std::string::npos)
      path = path.substr(0, location + 1);
    return path;
  }
}
