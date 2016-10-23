// Audio file object for path and associated data required for streaming.
#pragma once
#include <string>
#include "Defines.hpp"



namespace ASCIIPlayer
{
  class AudioSystem;
	class AudioFile
	{
    // Mark as friend to manage handles.
    friend class AudioSystem;

	public:
    // Constructor and Destructor
    AudioFile(std::string Path);
    ~AudioFile();

    // Operator overloads
    bool operator==(const AudioFile &rhs);

	private:
    // Variables
    std::string path_;          // Filepath of actual song
    unsigned long long fileID_; // Unique song ID.
    AudioHandle *loadedObject_; // Handle to the loaded object.

    // Static Variables
    static unsigned long long uniqueID_; // ID for lookup in the audio system
	};
}
