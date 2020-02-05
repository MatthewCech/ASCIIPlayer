#include <FMOD/fmod_errors.h>
#include <RTest/RTest.hpp>
#include "AudioSystem.hpp"
#include <ConsoleUtils/console-utils.hpp>

#define DATA_SIZE 512

namespace ASCIIPlayer
{
  typedef struct
  {
    float *buffer;
    float volume_linear;
    int   length_samples;
    int   channels;
  } mydsp_data_t;

  unsigned int __wave_data_size = 10;
  float __wave_data[DATA_SIZE];
  FMOD::DSP *__wave_data_dsp;


  FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
  {
    mydsp_data_t *data = (mydsp_data_t *)dsp_state->plugindata;
    __wave_data_size = length;

    /*
        This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0'
        as the number of channels in FMOD_DSP_DESCRIPTION.
        Specifying an actual channel count will mean you have to take care of any number of channels coming in,
        but outputting the number of channels specified. Generally it is best to keep the channel
        count at 0 for maximum compatibility.
    */
    
    //Input is not modified, but is copied over to the outbuffer.
    for (unsigned int samp = 0; samp < length; samp++)
    {
      const int numChannels = *outchannels;
      for (int chan = 0; chan < numChannels; chan++)
      {
        float inVal = inbuffer[(samp * inchannels) + chan];
        data->buffer[(samp * numChannels) + chan] = outbuffer[(samp * inchannels) + chan] = inVal;
        
        if (samp < __wave_data_size && samp < DATA_SIZE)
        {
          __wave_data[(samp * numChannels) + chan] = inVal;
        }
      }
    }
    
    data->channels = inchannels;

    return FMOD_OK;
  }

  /*
      Callback called when DSP is created.   This implementation creates a structure which is attached to the dsp state's 'plugindata' member.
  */
  FMOD_RESULT F_CALLBACK myDSPCreateCallback(FMOD_DSP_STATE *dsp_state)
  {
    unsigned int blocksize;
    FMOD_RESULT result;

    result = dsp_state->functions->getblocksize(dsp_state, &blocksize);
    AudioSystem::FCheck(result);

    mydsp_data_t *data = (mydsp_data_t *)calloc(sizeof(mydsp_data_t), 1);
    if (!data)
    {
      return FMOD_ERR_MEMORY;
    }
    dsp_state->plugindata = data;
    data->volume_linear = 1.0f;
    data->length_samples = blocksize;

    data->buffer = (float *)malloc(blocksize * 8 * sizeof(float));      // *8 = maximum size allowing room for 7.1.   Could ask dsp_state->functions->getspeakermode for the right speakermode to get real speaker count.
    if (!data->buffer)
    {
      return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
  }

  /*
      Callback called when DSP is destroyed.   The memory allocated in the create callback can be freed here.
  */
  FMOD_RESULT F_CALLBACK myDSPReleaseCallback(FMOD_DSP_STATE *dsp_state)
  {
    if (dsp_state->plugindata)
    {
      mydsp_data_t *data = (mydsp_data_t *)dsp_state->plugindata;

      if (data->buffer)
      {
        free(data->buffer);
      }

      free(data);
    }

    return FMOD_OK;
  }

  /*
      Callback called when DSP::getParameterData is called.   This returns a pointer to the raw floating point PCM data.
      We have set up 'parameter 0' to be the data parameter, so it checks to make sure the passed in index is 0, and nothing else.
  */
  FMOD_RESULT F_CALLBACK myDSPGetParameterDataCallback(FMOD_DSP_STATE *dsp_state, int index, void **data, unsigned int *length, char *)
  {
    if (index == 0)
    {
      unsigned int blocksize;
      FMOD_RESULT result;
      mydsp_data_t *mydata = (mydsp_data_t *)dsp_state->plugindata;

      result = dsp_state->functions->getblocksize(dsp_state, &blocksize);
      AudioSystem::FCheck(result);

      *data = (void *)mydata;
      *length = blocksize * 2 * sizeof(float);

      return FMOD_OK;
    }

    return FMOD_ERR_INVALID_PARAM;
  }

  /*
      Callback called when DSP::setParameterFloat is called.   This accepts a floating point 0 to 1 volume value, and stores it.
      We have set up 'parameter 1' to be the volume parameter, so it checks to make sure the passed in index is 1, and nothing else.
  */
  FMOD_RESULT F_CALLBACK myDSPSetParameterFloatCallback(FMOD_DSP_STATE *dsp_state, int index, float value)
  {
    if (index == 1)
    {
      mydsp_data_t *mydata = (mydsp_data_t *)dsp_state->plugindata;

      mydata->volume_linear = value;

      return FMOD_OK;
    }

    return FMOD_ERR_INVALID_PARAM;
  }

  /*
      Callback called when DSP::getParameterFloat is called.   This returns a floating point 0 to 1 volume value.
      We have set up 'parameter 1' to be the volume parameter, so it checks to make sure the passed in index is 1, and nothing else.
      An alternate way of displaying the data is provided, as a string, so the main app can use it.
  */
  FMOD_RESULT F_CALLBACK myDSPGetParameterFloatCallback(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valstr)
  {
    if (index == 1)
    {
      mydsp_data_t *mydata = (mydsp_data_t *)dsp_state->plugindata;

      *value = mydata->volume_linear;
      if (valstr)
      {
#pragma warning (disable : 4996)
        sprintf(valstr, "%d", (int)((*value * 100.0f) + 0.5f));
      }

      return FMOD_OK;
    }

    return FMOD_ERR_INVALID_PARAM;
  }



  // Static Variable Init
  APUnique AudioSystem::AudioSystemIDIncrement = 0;


  // Constructor
  AudioSystem::AudioSystem(int numChannels, float defaultVolume) 
    : fmodSystem_(nullptr)
    , masterChannel_(nullptr)
    , ID_(AudioSystemIDIncrement++)
  {
    // Initialize spectrum structure and set values to 0. Lock is required.
    //__wave_data_mutex.lock();
    //__wave_data = new float[DATA_SIZE];
    //memset(__wave_data, 0, DATA_SIZE);
    //__wave_data_mutex.unlock();

    FCheck(FMOD::System_Create(&fmodSystem_));
    FCheck(fmodSystem_->getVersion(&version_));

    if (version_ < FMOD_VERSION)
      throw "FMOD lib version %08x doesn't match header version %08x", version_, FMOD_VERSION;

    FCheck(fmodSystem_->init(numChannels, FMOD_INIT_NORMAL, nullptr));

    // Set default volume
    FCheck(fmodSystem_->createChannelGroup("Master", &masterChannel_));
    SetMasterVolume(defaultVolume);

    FMOD_DSP_DESCRIPTION dspdesc;
    memset(&dspdesc, 0, sizeof(dspdesc));
    FMOD_DSP_PARAMETER_DESC wavedata_desc;
    FMOD_DSP_PARAMETER_DESC volume_desc;
    FMOD_DSP_PARAMETER_DESC *paramdesc[2] =
    {
        &wavedata_desc,
        &volume_desc
    };

    FMOD_DSP_INIT_PARAMDESC_DATA(wavedata_desc, "wave data", "", "wave data", FMOD_DSP_PARAMETER_DATA_TYPE_USER);
    FMOD_DSP_INIT_PARAMDESC_FLOAT(volume_desc, "volume", "%", "linear volume in percent", 0, 1, 1);

    strncpy(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
    dspdesc.version = 0x00010000;
    dspdesc.numinputbuffers = 1;
    dspdesc.numoutputbuffers = 1;
    dspdesc.read = myDSPCallback;
    dspdesc.create = myDSPCreateCallback;
    dspdesc.release = myDSPReleaseCallback;
    dspdesc.getparameterdata = myDSPGetParameterDataCallback;
    dspdesc.setparameterfloat = myDSPSetParameterFloatCallback;
    dspdesc.getparameterfloat = myDSPGetParameterFloatCallback;
    dspdesc.numparameters = 2;
    dspdesc.paramdesc = paramdesc;

    FCheck(fmodSystem_->createDSP(&dspdesc, &__wave_data_dsp));
    FCheck(masterChannel_->addDSP(0, __wave_data_dsp));
  }


  // Destructor for the audio system.
  AudioSystem::~AudioSystem()
  {
    FCheck(masterChannel_->removeDSP(__wave_data_dsp));

    // Close the system
    FCheck(fmodSystem_->close());

    // Releases all assests.
    FCheck(fmodSystem_->release());

    //delete[] __wave_data;
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
      {
        DEBUG_PRINT("Preloaded:\n  " << audioFile.path_);
      }
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
    FCheck(fmodSystem_->playSound(audioFile.get(ID_)->LoadedObject, FMOD_DEFAULT, !playing, &channel));
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
    {
      if (pausedStatus == true)
        return;
    }
    
    if (!pausedState)
    {
      if (pausedStatus == false)
        return;
    }

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
    if (f > 1) 
      f = 1;
    else if (f < 0) 
      f = 0;
    
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
  void AudioSystem::FillWithAudioData(float *arr, int numVals, int channelOffset, AudioDataStyle style)
  {
    for(unsigned int i = 0; i < __wave_data_size && i < static_cast<unsigned int>(numVals) && i < DATA_SIZE; ++i)
      arr[i] = __wave_data[i];
    
    switch (style)
    {
      case AUDIODATA_WAVEFORM:
        //arr = spectrum;
        //numVals = 1024;
        //FCheck(fmodSystem_->get(arr, numVals, channelOffset));
        break;

      case AUDIODATA_SPECTRUM:
        //FCheck(fmodSystem_->getSpectrum(arr, numVals, channelOffset, FMOD_DSP_FFT_WINDOW_RECT));
        break;
       
      case AUDIODATA_SPECTRUM_ALT:
        //FCheck(fmodSystem_->getSpectrum(arr, numVals, channelOffset, FMOD_DSP_FFT_WINDOW_BLACKMAN));
        break;

      default:
        throw "Unsupported Style";
    }
  }



    //////////////////////////
   // Private member funcs //
  //////////////////////////
  // Checks the result of the FMOD operation.
  void AudioSystem::FCheck(const FMOD_RESULT &res)
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


  // Get the filename of an audio file 
  std::string AudioSystem::GetFilename(const AudioFile &audioFile) const
  {
    std::string name = audioFile.path_;
    std::size_t location = name.find_last_of("/\\");
    
    if (location != std::string::npos)
      name = name.substr(location + 1);
    
    return name;
  }


  // Get the filepath of.an audio file. 
  std::string AudioSystem::GetFilepath(const AudioFile &audioFile) const
  {
    std::string path = audioFile.path_;
    std::size_t location = path.find_last_of("/\\");
    
    if (location != std::string::npos)
      path = path.substr(0, location + 1);
    
    return path;
  }
}
