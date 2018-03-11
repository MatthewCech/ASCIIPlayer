#include "AudioFile.hpp"


namespace ASCIIPlayer
{
  // Static init.
  APUnique AudioFile::uniqueID_STATIC_VAR_ = 0; // ID for lookup in the audio system


  // Constructor and Destructor
  AudioFile::AudioFile(std::string path) 
    : path_(path)
    , fileID_(uniqueID_STATIC_VAR_++)
    , loadedObjects_() // Assigned via lazy init later
  {  }


  // Destructor
  AudioFile::~AudioFile()
  {  }


  // Operator == overload
  bool AudioFile::operator==(const AudioFile &rhs)
  {
    if (fileID_ == rhs.fileID_)
      return true;
    return false;
  }

  // Get the value associated with the Audiosystem with the ID in question
  AudioHandleWrapper *AudioFile::get(APUnique apu)
  {
    return &loadedObjects_[apu];
  }
}
