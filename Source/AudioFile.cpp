#include "AudioFile.hpp"


namespace ASCIIPlayer
{
  // Static init.
  std::size_t AudioFile::uniqueID_ = 0; // ID for lookup in the audio system


  // Constructor and Destructor
  AudioFile::AudioFile(std::string path) 
    : path_(path)
    , fileID_(uniqueID_++)
    , loadedObject_(nullptr) // Assigned via lazy init later
  {  }


  // Destructor
  AudioFile::~AudioFile()
  {  }
}
