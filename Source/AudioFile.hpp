// Audio file object for path and associated data required for streaming.
#pragma once
#include <string>
#include "CustomDefines.hpp"



namespace ASCIIPlayer
{
  class AudioSystem;
	class AudioFile
	{
    // Mark as friend to manage handles.
    friend AudioSystem;

	public:
    // Constructor and Destructor
    AudioFile(std::string Path);
    ~AudioFile();

	private:
    // Variables
    std::string path_;          // Filepath of actual song
    std::size_t fileID_;        // Unique song ID.
    AudioHandle *loadedObject_; // Handle to the loaded object.

    // Static Variables
    static std::size_t uniqueID_; // ID for lookup in the audio system
	};
}
