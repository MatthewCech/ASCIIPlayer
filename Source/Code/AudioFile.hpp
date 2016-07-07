// Audio file object for path and associated data required for streaming.
#pragma once
#include <string>
#include "CustomDefines.hpp"



namespace ASCIIPlayer
{
	class AudioFile
	{
	public:
    // Constructor and Destructor
    AudioFile(std::string Path);
    ~AudioFile();

	private:
    // Variables
    std::string path_;                   // Filepath of actual song
    AudioHandle loadedObject_;           // Handle to the loaded object.
    static unsigned long long uniqueID_; // ID for lookup in the audio system
	};
}
